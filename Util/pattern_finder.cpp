#pragma once
#include "pattern_finder.h"


// Original function by learn_more -- https://github.com/learn-more/findpattern-bench
#define INRANGE(x, a, b) (x >= a && x <= b) 
#define getBits(x) (INRANGE((x & (~0x20)), 'A', 'F') ? ((x & (~0x20)) - 'A' + 0xA): (INRANGE(x, '0', '9') ? x - '0': 0))
#define getByte(x) (getBits(x[0]) << 4 | getBits(x[1]))

// original code by dom1n1k and Patrick at GameDeception
inline bool Compare(const unsigned char* pData, const unsigned char* bMask, const char* szMask)
{
	for (; *szMask; ++szMask, ++pData, ++bMask)
		if (*szMask == 'x' && *pData != *bMask)
			return false;

	return (*szMask) == 0;
}

std::uintptr_t PatternFinder::FindPattern(uintptr_t dwAddress, uintptr_t dwLen, unsigned char* bMask, const char* szMask)
{
	for (uintptr_t i = 0; i < dwLen; i++)
		if (Compare((unsigned char*)(dwAddress + i), bMask, szMask))
			return (uintptr_t)(dwAddress + i);

	return 0;
}


uintptr_t PatternFinder::FindPattern(const uintptr_t& start_address, const uintptr_t& end_address, const char* target_pattern) {
	const char* pattern = target_pattern;

	uintptr_t first_match = 0;

	for (uintptr_t position = start_address; position < end_address; position++) {
		if (!*pattern)
			return first_match;

		const uint8_t pattern_current = *reinterpret_cast<const uint8_t*>(pattern);
		const uint8_t memory_current = *reinterpret_cast<const uint8_t*>(position);

		if (pattern_current == '\?' || memory_current == getByte(pattern)) {
			if (!first_match)
				first_match = position;

			if (!pattern[2])
				return first_match;

			pattern += pattern_current != '\?' ? 3 : 2;
		}
		else {
			pattern = target_pattern;
			first_match = 0;
		}
	}

	return NULL;
}

uintptr_t PatternFinder::FindPattern(const char* module_name, const char* target_pattern) {
	MODULEINFO module_info = { 0 };

	if (!GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(module_name), &module_info, sizeof(MODULEINFO))) // Cheking if the module is available or not
		return NULL;

	const uintptr_t start_address = uintptr_t(module_info.lpBaseOfDll);
	const uintptr_t end_address = start_address + module_info.SizeOfImage;

	return FindPattern(start_address, end_address, target_pattern);
}