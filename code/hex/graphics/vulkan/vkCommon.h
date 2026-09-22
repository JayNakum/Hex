#pragma once

#include "core/common.h"

#if !defined(VK_NO_PROTOTYPES)
#define VK_NO_PROTOTYPES
#endif

#include <volk.h>

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#endif
#include "profiles.hpp"
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
