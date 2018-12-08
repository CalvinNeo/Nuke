#pragma once

#include "base.h"
#include "elem.h"
#include "bool.h"

#define NUKE_IF_BOOL(test, t, f) NUKE_CAT(NUKE_IF_BOOL_,test)(t, f)

#define NUKE_IF_BOOL_0(t, f) f
#define NUKE_IF_BOOL_1(t, f) t


#define NUKE_IF(test, t, f) NUKE_IF_BOOL(NUKE_BOOL(test), t, f)
