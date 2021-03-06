// http://www.jera.com/techinfo/jtns/jtn002.html
#include "console.h"

#define mu_assert(message, test) do { if (!(test)) return message; } while (0)
#define mu_run_test(test) do { char *message = test(); tests_run++; \
                               if (message) {error_test_fail(message); return message;} test_info_end(); } while (0)
#define mu_tag(tag) test_info(tag);
extern int tests_run;