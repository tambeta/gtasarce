#include <stdlib.h>
#include <glib.h>

#include "opcode.h"

#define OC_ERROR oc_error_quark()

G_DEFINE_QUARK("oc-error-quark", oc_error);

Opcode *oc_new(gchar *fp, GError **error) {
    
    // Initialize new opcode. An opcode comprises the raw 
    // operation code (16-bit uint) and the parameter list.
    // All parameter values are stored inside gint32s.
    
    Opcode *oc = g_new0(Opcode, 1);    
    guint16 oc_raw = *(guint16*)fp;
    const OpcodeDescription *ocd;
    GArray *oc_params = g_array_new(FALSE, FALSE, sizeof(OpcodeP));
    
    // Init opcode
    
    oc->pointer = fp;    
    oc->params = oc_params;
    ocd = oc_get_ocd(oc, oc_raw);
    oc->ocd = ocd;
    
    if (ocd == NULL) {
        if (error != NULL)
            *error = g_error_new(OC_ERROR, 1, "An unknown opcode occurred: %04x", oc_raw);
        return NULL;
    }
    
    // Loop over and store opcode parameters
    
    fp += 2; for (int i=0; i<ocd->n_params; i++) {
        OpcodePType ptype = (OpcodePType)(*fp);
        ocpval pvalue;
        OpcodeP param;
        
        fp++;
        param.pointer = fp;
        
        if (ptype == INT32) {
            pvalue = *(ocpval*)fp;
            fp += 4;
        }
        else if (ptype == PTR16A || ptype == PTR16B) {
            pvalue = (ocpval)(*(guint16*)fp);
            fp += 2;
        }
        else if (ptype == INT16) {
            pvalue = (ocpval)(*(gint16*)fp);
            fp += 2;
        }        
        else if (ptype == INT8) {
            pvalue = (ocpval)(*fp);
            fp += 1;
        }
        else if (ptype == FLOAT32) {
            pvalue = *(ocpval*)fp;
            fp += 4;
        }        
        else {
            if (error != NULL)
                *error = g_error_new(OC_ERROR, 2, "An unknown opcode param datatype occurred: %02x", ptype);
            return NULL;                  
        }
        
        param.type = ptype;
        param.value = pvalue;
        g_array_append_val(oc_params, param);
    }
    
    if (ocd->n_params >= 1)
        oc->fst = g_array_index(oc_params, OpcodeP, 0).value;
    if (ocd->n_params >= 2)
        oc->snd = g_array_index(oc_params, OpcodeP, 1).value;
    
    oc->next_pointer = fp;
    return oc;
}

const OpcodeDescription *oc_get_ocd(Opcode *oc, guint16 code) {
    for (int i=0;; i++) {
        const OpcodeDescription *ocd = &KNOWN_OPCODES[i];
        
        if (ocd->code == 0 && ocd->n_params == 0)
            break;
        if (ocd->code == code)
            return ocd;        
    }
    
    return NULL;
}

ocpval oc_get_pval(Opcode *oc, int index) {
    OpcodeP p = g_array_index(oc->params, OpcodeP, index);
    return p.value;
}

gboolean oc_set_pval(Opcode *oc, int index, ocpval value) {
    OpcodeP p = g_array_index(oc->params, OpcodeP, index);
    
    if (p.type != INT16) // no support for setting anything
        return FALSE;    // besides datatype 0x5 for now (int16)
    
    *(gint16*)(p.pointer) = (gint16)value;    
    return TRUE;
}
