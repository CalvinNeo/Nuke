
#include "../src/pp/is_empty.h"                                                                                                               
#include "../src/pp/size.h"
#include "../src/pp/bool.h"
#include "../src/pp/base.h"
#include "../src/pp/if.h"


#define T222
#define T333 3
NUKE_BOOL_defined(T111) // 0 Not defined
NUKE_BOOL_defined(T222) // 1 Defined to empty
NUKE_BOOL_defined(T333) // 0 Defined to not empty

NUKE_IF(2, a, b)
NUKE_BOOL(2)
