/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

/**
 * This eventually prefixes log records with the current frame number.
 */

#include "System/maindefines.h"
#include "System/SafeCStrings.h"

//#include <cstdio>
#include <cstdarg>
#include <cstring>


#ifdef __cplusplus
extern "C" {
#endif

static int* frameNum = NULL;

void log_framePrefixer_setFrameNumReference(int* frameNumReference)
{
	frameNum = frameNumReference;
}

void log_framePrefixer_prefix(char* result, size_t resultSize,
		const char* record)
{
	SNPRINTF(result, resultSize, "[f=%07d] %s", *frameNum, record);
}

#ifdef __cplusplus
} // extern "C"
#endif

