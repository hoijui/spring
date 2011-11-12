/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef LOG_FRAME_PREFIXER_H
#define LOG_FRAME_PREFIXER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Injects a reference to the current frame number.
 * This will only possibly be called when we are in an engine binary.
 */
void log_framePrefixer_setFrameNumReference(int* frameNumReference);

/**
 * Prepends a log record with a prefix containing the frame number,
 * if it is available.
 */
void log_framePrefixer_prefix(char* result, size_t resultSize,
		const char* record)

#ifdef __cplusplus
} // extern "C"
#endif

#endif // LOG_FRAME_PREFIXER_H

