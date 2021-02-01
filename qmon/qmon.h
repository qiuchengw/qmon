#pragma once

#include "resource.h"

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifdef min
#undef min
#undef max
#endif

#ifndef WIN32
#define WIN32 1
#endif

#ifndef __cplusplus
#define __cplusplus
#endif

#include <algorithm>
