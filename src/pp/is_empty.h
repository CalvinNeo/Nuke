#pragma once

#include "base.h"
#include "elem.h"

#define NUKE_EMPTY()
#define NUKE_EAT_BRACE(x) x NUKE_EMPTY


#define NUKE_IS_EMPTY_HELPER() , 0
#define NUKE_IS_EMPTY(x) NUKE_IS_EMPTY_I(x NUKE_IS_EMPTY_HELPER)
#define NUKE_IS_EMPTY_I(contents) NUKE_ELEM(1, NUKE_IS_EMPTY_DEF_ ## contents())
#define NUKE_IS_EMPTY_DEF_NUKE_IS_EMPTY_HELPER _, NUKE_EAT_BRACE(1)

