#ifndef _OPCODE_H
#define _OPCODE_H 1

typedef gint32 ocpval;

typedef enum {        
    INT32 = 1,
    PTR16A = 2,
    PTR16B = 3,
    INT8 = 4,
    INT16 = 5,
    FLOAT32 = 6
} OpcodePType;

typedef struct {
    OpcodePType type;
    ocpval value;
    gchar *pointer;
} OpcodeP;

typedef struct {
    guint16 code;
    guint32 n_params;
} OpcodeDescription;

typedef struct {    
    gchar *pointer;
    gchar *next_pointer;
    GArray *params;
    const OpcodeDescription *ocd;
    ocpval fst;
    ocpval snd;
} Opcode;

static const OpcodeDescription KNOWN_OPCODES[] = {
    { 0x0871, 18 },
    { 0x0872, 18 },
    { 0x0006, 2 },
    { 0x0007, 2 },
    { 0x04AF, 2 },
    { 0x0087, 2 },
    { 0x0050, 2 },
    { 0x0002, 2 },
    { 0, 0 }
};

Opcode *oc_new(gchar *fp, GError **error);
const OpcodeDescription *oc_get_ocd(Opcode *oc, guint16 code);
ocpval oc_get_pval(Opcode *oc, int index);
gboolean oc_set_pval(Opcode *oc, int index, ocpval value);

#endif
