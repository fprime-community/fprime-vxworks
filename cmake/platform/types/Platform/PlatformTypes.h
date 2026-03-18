/**
 * \brief PlatformTypes.h C-compatible type definitions for VxWorks
 *
 * PlatformTypes.h is typically published by platform developers to define
 * the standard available arithmetic types for use in fprime. This standard
 * types header is designed to support standard VxWorks distributions using
 * standard compilers shipped with the operating system.
 *
 * In C++ code, users may use std::numeric_limits<PlatformIntType>::min()
 * to reference the min/max limits of a type.
 */
#ifndef PLATFORM_TYPES_H_
#define PLATFORM_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <vxWorks.h>

// Capture current value of VxWorks constants
enum VxWorksConstants {
	VXWORKS_ERROR = ERROR,
	VXWORKS_READ = READ,
	VXWORKS_OK = OK,
	VXWORKS_NO_WAIT = NO_WAIT,
        VXWORKS_NONE = NONE
};

#undef OK
#undef ERROR
#undef READ
#undef NO_WAIT
#undef NONE

// Redefine constants as enumeration
enum {
	ERROR = VXWORKS_ERROR,
	READ = VXWORKS_READ,
	OK = VXWORKS_OK,
	NO_WAIT = VXWORKS_NO_WAIT,
        NONE = VXWORKS_NONE
};

#include <inttypes.h>
#include <stdint.h>

typedef int PlatformIntType;
#define PRI_PlatformIntType "d"

typedef unsigned int PlatformUIntType;
#define PRI_PlatformUIntType "u"

// VxWorks definitions for pointer have various sizes across platforms
// and since these definitions need to be consistent we must ask the size.
// Check for __SIZEOF_POINTER__ or cause error
#ifndef __SIZEOF_POINTER__
#error "Compiler does not support __SIZEOF_POINTER__, cannot use VxWorks types"
#endif

// Pointer sizes are determined by compiler
#if __SIZEOF_POINTER__ == 8
typedef uint64_t PlatformPointerCastType;
#define PRI_PlatformPointerCastType PRIx64
#elif __SIZEOF_POINTER__ == 4
typedef uint32_t PlatformPointerCastType;
#define PRI_PlatformPointerCastType PRIx32
#elif __SIZEOF_POINTER__ == 2
typedef uint16_t PlatformPointerCastType;
#define PRI_PlatformPointerCastType PRIx16
#elif __SIZEOF_POINTER__ == 1
typedef uint8_t PlatformPointerCastType;
#define PRI_PlatformPointerCastType PRIx8
#else
#error "Expected __SIZEOF_POINTER__ to be one of 8, 4, 2, or 1"
#endif

#ifdef __cplusplus
}
#endif

#endif  // PLATFORM_TYPES_H_
