#include "signals.h"
#include <signal.h>

void signals_init(void) {
    signal(SIGINT,  SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
}
