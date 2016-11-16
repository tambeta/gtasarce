#include <stdlib.h>
#include <glib.h>

#include "gameobjects.h"

GPtrArray *go_match_name(const GameObject go[],  gchar *needle) {
    
    // Matches needle to a GameObject array, returning a GPtrArray
    // consisting of the elements of the passed array wherein name 
    // begins with needle. Case-insensitive. Caller is responsible
    // for destroying GPtrArray.
    
    GPtrArray *ret;    
    gchar *lneedle;
    
    ret = g_ptr_array_new();
    lneedle = g_ascii_strdown(needle, -1);
    
    for (GameObject *i=(GameObject*)go;; i++) {     
        gchar *index;
        gchar *lname;        
        gboolean is_prefix;        
                
        if (i->name == NULL)
            break;        
        lname = g_ascii_strdown(i->name, -1);        
        is_prefix = g_str_has_prefix(lname, lneedle);        
        if (is_prefix)
            g_ptr_array_add(ret, i);
           
        g_free(lname);
    }
    
    g_free(lneedle);
    return ret;
}

GPtrArray *go_match_id(const GameObject go[],  int id) {
    
    // Matches id to a GameObject array, returning a GPtrArray
    // consisting of the elements of the passed array wherein id 
    // equals with passed id.
    
    GPtrArray *ret = g_ptr_array_new();
    
    for (GameObject *i=(GameObject*)go;; i++) {     
        if (i->name == NULL)
            break;        
        if (i->id == id)
            g_ptr_array_add(ret, i);           
    }
    
    return ret;
}
