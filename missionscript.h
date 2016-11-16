#ifndef _MISSIONSCRIPT_H
#define _MISSIONSCRIPT_H 1

#include "gameobjects.h"

static const int MAX_CONTESTANTS = 6;

typedef struct {
    gchar *file;
    gchar *filename;
    gsize length;
    GArray *rsub_offsets;    
} MissionScript;

MissionScript *scm_new(gchar *fn, GError **error);
gboolean scm_write(MissionScript *ms, GError **error);
gboolean scm_set_race_vehicles(MissionScript *ms, const GameObject *race, GPtrArray *vehicles, GError **error);
GPtrArray *scm_get_race_vehicles(MissionScript *ms, const GameObject *race, GError **error);

#endif
