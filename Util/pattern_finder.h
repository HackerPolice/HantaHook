#pragma once
#pragma once

#include "../HantaHook.h"
#include <cstdint>
#include <psapi.h>

namespace PatternFinder
{
	std::uintptr_t FindPattern(const char* module, const char* target_pattern);
	std::uintptr_t FindPattern(const uintptr_t& start_address, const uintptr_t& end_address, const char* target_pattern);
	std::uintptr_t FindPattern(uintptr_t dwAddress, uintptr_t dwLen, unsigned char* bMask, const char* szMask);
}