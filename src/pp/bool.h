#pragma once

#include "is_empty.h"

#define NUKE_BOOL(x) NUKE_BOOL_##x(x)
#define NUKE_BOOL_true(x) 1
#define NUKE_BOOL_false(x) 0
#define NUKE_BOOL_0(x) 0
#define NUKE_BOOL_1(x) 1
#define NUKE_BOOL_2(x) 1
#define NUKE_BOOL_3(x) 1
#define NUKE_BOOL_4(x) 1
#define NUKE_BOOL_defined(x) NUKE_IS_EMPTY(x) 

