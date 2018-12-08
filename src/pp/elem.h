#pragma once

#include "base.h"

#define NUKE_ELEM(n, ...) NUKE_CAT(NUKE_ELEM_, n)(__VA_ARGS__,_)
#define NUKE_ELEM_0(e0, ...) e0
#define NUKE_ELEM_1(e0, e1, ...) e1
#define NUKE_ELEM_2(e0, e1, e2, ...) e2

