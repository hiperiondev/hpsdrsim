#include "hpsdr_debug.h"

#include <stdbool.h>

static int debug_level = 0;
int debug_id = 1;

void dbg_setlevel(int Level) {
    debug_level = Level;
}

int dbg_getlevel() {
    return debug_level;
}

void dbg_printf(int Level, const char *fmt, ...) {
    if (Level <= debug_level) {
        bool debug_id_m = false;
        va_list args;
        va_start(args, fmt);
        if (fmt[0] == '<') {
            --debug_id;
            debug_id_m = true;
            fprintf(stderr, "%*c(%d)", debug_id, ' ', debug_id);
        }
        if (fmt[0] == '>') {
            debug_id_m = true;
            fprintf(stderr, "%*c(%d)", debug_id, ' ', debug_id);
            ++debug_id;
        }
        if (!debug_id_m)
            fprintf(stderr, "%*c", debug_id, ' ');
        vfprintf(stderr, fmt, args);
        va_end(args);
        debug_id_m = false;
    }
}
