#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <glib.h>

#include "missionscript.h"
#include "opcode.h"

#define SCM_ERROR scm_error_quark()

G_DEFINE_QUARK("scm-error-quark", scm_error);

guint32 scm_search(MissionScript *ms, guint32 offset, gchar *buf, int bufsize);
guint32 scm_get_mission_offset(MissionScript *ms, guint32 jtoffset);
gint scm_race_oc_cmp(gconstpointer a, gconstpointer b);
GPtrArray *scm_get_race_setocs(MissionScript *ms, const GameObject *go, GError **error);

MissionScript *scm_new(gchar *fn, GError **error) {
    MissionScript *ms = g_new0(MissionScript, 1);
    GArray *rsubs = g_array_new(FALSE, FALSE, sizeof(guint32));
    guint32 jt_offset;
    guint32 mission_offset;
    gchar * oc_ptr;    
    gchar opcode_jt[] = { 0x71, 0x8 };
    
    // Slurp entire SCM mission bytecode file
    
    if (!g_file_get_contents(fn, &(ms->file), &(ms->length), error))
        return NULL;
    ms->filename = fn;    
    
    // Locate race mission jumptable by searching for 
    // the string "CPRACE", then the second jumptable 
    // opcode (0x0871) from that position.
    
    jt_offset = scm_search(ms, 0, "CPRACE", 6);
    jt_offset = scm_search(ms, jt_offset, opcode_jt, 2);    
    jt_offset = scm_search(ms, jt_offset + 1, opcode_jt, 2);        
    
    if (jt_offset == 0) {
        if (error != NULL)
            *error = g_error_new(SCM_ERROR, 0, "Could not find race mission jumptable in SCM file");
        return NULL;
    }
    
    // Determine mission offset from the SCM header
    
    mission_offset = scm_get_mission_offset(ms, jt_offset);    

    if (mission_offset == 0) {
        if (error != NULL)
            *error = g_error_new(SCM_ERROR, 0, "Could not find race mission offset in SCM file");
        return NULL;
    }    
    
    // Fetch race subroutine offsets from consecutive 
    // jumptables (0x0871, 0x0872)
    
    oc_ptr = ms->file + jt_offset; 
    
    while (TRUE) {
        Opcode *oc = oc_new(oc_ptr, error);
        guint16 code;
        
        if (oc == NULL) {
            if (error != NULL && *error == NULL)
                *error = g_error_new(SCM_ERROR, 0, "Unknown error occurred in oc_new()");
            return NULL;
        }
        
        code = oc->ocd->code;
        
        if (!(code == 0x0871 || code == 0x0872))
            break;
        
        // Loop over jumptable (0x0871, 0x0872) parameters.
        // These are offsets of race subs relative to mission start.
        
        for (int i=1; i<oc->ocd->n_params; i+=2) {
            guint32 rsub_offset;
            int index;
            
            if (code == 0x0871 && i<5) // 0x0871 jumps start from index 5
                continue;
            
            rsub_offset = mission_offset + (0-oc_get_pval(oc, i));            
            index = (int)(oc_get_pval(oc, i-1));
            
            if (index >= 0) {
                g_array_insert_val(rsubs, index, rsub_offset);
                //g_print("race n. %d -> %d\n", index, rsub_offset);
            }
        }
        
        oc_ptr = oc->next_pointer;
    }
    
    ms->rsub_offsets = rsubs;
    return ms;
}

gboolean scm_write(MissionScript *ms, GError **error) {
    
    // Write the modified SCM to disk
    
    if (!g_file_set_contents(ms->filename, ms->file, ms->length, error))
        return FALSE;
    return TRUE;
}

GPtrArray *scm_get_race_vehicles(MissionScript *ms, const GameObject *race, GError **error) {    
    GPtrArray *setocs = scm_get_race_setocs(ms, race, error);
    GPtrArray *ret = g_ptr_array_new();
    
    if (setocs == NULL) {
        if (error != NULL && *error == NULL)
            *error = g_error_new(SCM_ERROR, 0, "Unknown error occurred in scm_get_race_setocs()");
        return NULL;
    }
    
    for (int i=0; i<MAX_CONTESTANTS; i++) {
        Opcode *oc = g_ptr_array_index(setocs, i);
        GPtrArray *matches;
        
        if (oc == NULL)
            break;
        
        matches = go_match_id(GO_VEHICLES, oc->snd);
        
        if (matches->len > 1) {
            if (error != NULL)
                *error = g_error_new(SCM_ERROR, 0, "Multiple matches to vehicle ID %d", oc->snd);
            return NULL;
        }
        else if (matches->len < 1) {
            if (error != NULL)
                *error = g_error_new(SCM_ERROR, 0, "No matches to vehicle ID %d", oc->snd);
            return NULL;
        }
        
        g_ptr_array_add(ret, g_ptr_array_index(matches, 0));
    }
    
    return ret;
}

gboolean scm_set_race_vehicles(
    MissionScript *ms, const GameObject *race,
    GPtrArray *vehicles, GError **error
) {
    GPtrArray *setocs;
    
    g_ptr_array_set_size(vehicles, MAX_CONTESTANTS);
    setocs = scm_get_race_setocs(ms, race, error);
    
    if (setocs == NULL) {
        if (error != NULL && *error == NULL)
            *error = g_error_new(SCM_ERROR, 0, "Unknown error occurred in scm_get_race_setocs()");
        return FALSE;
    }
    
    // Change opcode parameters to new 
    // vehicle IDs
    
    for (int i=0; i<MAX_CONTESTANTS; i++) {
        Opcode *setoc = g_ptr_array_index(setocs, i);
        GameObject *v = g_ptr_array_index(vehicles, i);
        
        if (v == NULL || setoc == NULL)
            return TRUE;
        oc_set_pval(setoc, 1, (ocpval)(v->id));
        //g_print("%04x: %d, %d -> %s\n", setoc->ocd->code, setoc->fst, setoc->snd, v->name);
    }

    return TRUE;
}

guint32 scm_get_mission_offset(MissionScript *ms, guint32 jtoffset) {
    
    // Determine offset of the mission wherein jtpointer
    // lies. See http://www.gtamodding.com/index.php?title=SA_SCM.
    // On error returns NULL (or most likely segfaults).
    
    guint32 offset;    
    guint32 n_miss;
    guint32 *mptr;
    guint32 last_offset;
    
    offset = *(guint32*)(ms->file + 3);             // offset of 2nd table
    offset = *(guint32*)(ms->file + offset + 3);    // offset of 3rd table
    n_miss = *(guint32*)(ms->file + offset + 16);   // n. of missions
    
    mptr = (guint32*)(ms->file + offset + 24);
    offset = 0xFFFFFFFF;
    
    // Loop over table of mission offsets, 
    // determine offset of race mission.
    
    for (guint32 i=0; i<n_miss; i++) {        
        last_offset = offset;
        offset = *(mptr + i);
        
        if (jtoffset >= last_offset && jtoffset <= offset) {
            //g_print("jtpointer @mission %d, offset %u", i, last_offset);
            return last_offset;
            break;    
        }
    }
    
    return 0;   
}

guint32 scm_search(MissionScript *ms, guint32 offset, gchar *buf, int bufsize) {
    
    // Look for the first occurrence of buf in the SCM 
    // file, return an offset into the file or NULL.
    
    gchar *firstpos = ms->file + offset;
    gchar *lastpos = ms->file + ms->length - bufsize;
    
    if (offset < 0)
        return 0;
    
    for (gchar *i=firstpos; i<=lastpos; i++) {
        if (memcmp(i, buf, bufsize) == 0)
            return (i - ms->file);
    }
    
    return 0;
}

GPtrArray *scm_get_race_setocs(MissionScript *ms, const GameObject *race, GError **error) {
    
    // Fetch array of vehicle setting opcodes for a
    // given race. Returns a ptrarray of size MAX_CONTESTANTS
    // or NULL on error.

    guint32 offset = g_array_index(ms->rsub_offsets, guint32, race->id);
    gchar *fp = ms->file + offset;
    gboolean check_block_end = FALSE;    
    GPtrArray *setocs = g_ptr_array_new();
    int n_ocs = 0;
    
    // Loop over opcodes of the race subroutine, find
    // the 0x04AF block (set contestant vehicle IDs).
    
    while (TRUE) {
        Opcode *oc = oc_new(fp, error);
        guint16 code;
        ocpval pval0 = 0;
        
        if (oc == NULL) {
            if (error != NULL && *error == NULL)
                *error = g_error_new(SCM_ERROR, 0, "Unknown error occurred in oc_new()");
            return NULL;
        }
        
        code = oc->ocd->code;
                
        if (oc->ocd->n_params > 0)
            pval0 = oc_get_pval(oc, 0);
        if (code == 0x04AF && pval0 == 716) {
            check_block_end = TRUE;
        }
        
        if (check_block_end) {               
            if (code != 0x04AF) {
                
                // The 0x04AF block has ended. Sanity 
                // check n. of contestants.
                
                if (n_ocs > MAX_CONTESTANTS) {
                    if (error != NULL)
                        *error = g_error_new(
                            SCM_ERROR, 0, 
                            "Race has more than MAX_CONTESTANTS vehicle entries"
                        );
                    return NULL;
                }                
                
                // Sort opcode pointer array in reverse
                // so that player opcode is the first; pad 
                // array so the last element is guaranteed
                // to be NULL
                
                g_ptr_array_sort(setocs, scm_race_oc_cmp);
                g_ptr_array_set_size(setocs, MAX_CONTESTANTS);                   
                return setocs;
            }
            
            g_ptr_array_add(setocs, oc);
            n_ocs++;
        }
                
        fp = oc->next_pointer;        
    }     
    
    assert(0 == 1); // should never be here
}

gint scm_race_oc_cmp(gconstpointer a, gconstpointer b) {
    
    // Sort vehicle setting opcodes in reverse
    // by pointers. Player's vehicle's opcode will
    // subsequently be the first in the ptrarray.
    
    if ((*(Opcode**)a)->pointer < (*(Opcode**)b)->pointer)
        return 1;
    return -1;
}
