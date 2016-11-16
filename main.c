#include <stdio.h>
#include <stdlib.h>

#include <glib.h>
#include "gameobjects.h"
#include "missionscript.h"

int main(int, char*[]);

static gchar **p_vehicle = NULL;
static gchar *p_filename = NULL;
static gchar *p_race = NULL;
static gchar *p_listrace = NULL;
static gboolean p_loopv = FALSE;

static const GOptionEntry entries[] = {
    {"vehicle", 'v', 0, G_OPTION_ARG_STRING_ARRAY, &p_vehicle, 
        "Name of vehicle to set for race; empty string to list. Pass several times to set opponent cars.", "V"},
    {"race", 'r', 0, G_OPTION_ARG_STRING, &p_race, 
        "Name of race to edit; empty string to list.", "R"},
    {"filename", 'f', 0, G_OPTION_ARG_FILENAME, &p_filename, 
        "Name of the GTA:SA compiled mission script (main.scm) to modify.", "F"},
    {"all", 'a', 0, G_OPTION_ARG_NONE, &p_loopv, 
        "Set all contestant vehicles in race based on those passed with -v options.", NULL},
    {"list", 'l', 0, G_OPTION_ARG_STRING, &p_listrace, 
        "List contestant vehicles of given race", "R"},        
    { NULL }
};

void parse_cmdline(int argc, char** argv) {
    GError *error = NULL;
    GOptionContext *context;
    
    context = g_option_context_new("- edit GTA:SA land race mission vehicles");
    g_option_context_add_main_entries (context, entries, NULL);
    
    if (!g_option_context_parse (context, &argc, &argv, &error)) {
        g_print("Option parsing failed: %s\n", error->message);
        exit (1);
    }
}

const GameObject *match_or_die(const GameObject *gos, gchar *name, gchar *go_type_name) {
    
    // Convenience function to match name to a single
    // GameObject or exit with an error message. gos is 
    // a pointer to the first GameObject in an array,
    // go_type_name is the kind of the GameObject (i.e. 
    // "race", "vehicle").
    
    GPtrArray *matched = go_match_name(gos, name);
    const GameObject *ret;
    
    if (matched->len > 1) {
        g_print("Ambiguous %s name - partially mathces the following:\n", go_type_name);
        for (int j=0; j<matched->len; j++)
            g_print("%s\n", ((GameObject*)g_ptr_array_index(matched, j))->name);                
        exit(1);
    }
    else if (matched->len < 1) {
        g_print("No matches found for %s \"%s\"", go_type_name, name);
        exit(1);   
    }
    
    ret = (const GameObject*)g_ptr_array_index(matched, 0);
    g_ptr_array_free(matched, TRUE);
    return ret;
}

int main(int argc, char *argv[]) {    
    GError *error = NULL;
    MissionScript *ms;    
    const GameObject *goss[] = {GO_RACES, GO_VEHICLES};
    const GameObject *race;
    GPtrArray *vehicles = g_ptr_array_new();
       
    parse_cmdline(argc, argv);    
    
    // Cmdline checks
    
    if (p_listrace == NULL) {
        if (p_vehicle == NULL) {
            g_print("No vehicle specified\n");
            exit(1);   
        }
        if (p_race == NULL) {
            g_print("No race specified\n");
            exit(1);   
        }    
    }
    if (p_filename == NULL) {
        g_print("No filename specified\n");
        exit(1);
    }
    
    if (!(
        g_file_test(p_filename, G_FILE_TEST_EXISTS) &&
        g_file_test(p_filename, G_FILE_TEST_IS_REGULAR)
    )) {
        g_print("File %s does not exist or is not a regular file\n", p_filename);
        exit(1);
    }
    
    // Init mission script
    
    if ((ms = scm_new(p_filename, &error)) == NULL) {
        if (error != NULL)
            g_print("Error: %s\n", error->message);
        exit(1);
    }
    
    // List race vehicles and exit, if requested
    
    if (p_listrace != NULL) {
        GPtrArray *rveh;
        
        race = match_or_die(GO_RACES, p_listrace, "race");        
        rveh = scm_get_race_vehicles(ms, race, &error);
                
        if (error != NULL) {
            g_print("Error: %s\n", error->message);
            exit(1);      
        }
        
        g_print("Listing \"%s\" -", race->name);
        
        for (int i=0; i<rveh->len; i++) {
            const GameObject *go = g_ptr_array_index(rveh, i);
            g_print("%s %s", (i == 0 ? "" : ","), go->name);
        }
        
        exit(0);   
    }
    
    // Determine race and vehicle IDs
    
    for (int i=0; i<=1; i++) {
        const GameObject *gos = goss[i];        
        const GameObject *go;
        
        if (gos == GO_VEHICLES) {
            int j=0; for (;; j++) {                
                if (p_vehicle[j] == NULL || j >= MAX_CONTESTANTS)
                    break;
                go = match_or_die(gos, p_vehicle[j], "vehicle");
                g_ptr_array_add(vehicles, (gpointer)go);                
            }
            
            // "Loop" vehicle arguments to fill all 
            // contestant spots if so requested
            
            if (p_loopv) {
                for (int k=0; k<MAX_CONTESTANTS; k++) {
                    if (k < j) continue;
                    g_ptr_array_add(vehicles, g_ptr_array_index(vehicles, k % j));                    
                }
            }
        }
        else {           
            race = match_or_die(gos, p_race, "race");                   
        }
    }
    
    // Set race -> vehicle
    
    if (!scm_set_race_vehicles(ms, race, vehicles, &error)) {    
        if (error != NULL)
            g_print("Error setting vehicles: %s\n", error->message);
        exit(1);
    }
    
    if (!scm_write(ms, &error)) {
        if (error != NULL)
            g_print("Error writing SCM: %s\n", error->message);
        exit(1);
    }
    
    // Print info and exit
    
    g_ptr_array_set_size(vehicles, MAX_CONTESTANTS);
    g_print("Setting \"%s\" -", race->name);
    
    for (int i=0; i<MAX_CONTESTANTS; i++) {
        GameObject *v = g_ptr_array_index(vehicles, i);        
        if (v == NULL)
            break;
        g_print("%s %s", (i == 0 ? "" : ","), v->name);
    }
    
    g_print("\n");
    return 0;
}
