#pragma once

#ifdef __cplusplus  
extern "C" {
#endif 

#include "inttypes.h"

typedef enum TByteOrder {
	boLittleEndian, boBigEndian
} TByteOrder;

// TByteOrders = set of TByteOrder // a set of flags
// 
// Flags are obtained from the enum values of TByteOrder and get combined by the
// | operator. Values from TByteOrder are converted to flags, by interpreting
// them as powers of two, like in the following expression:
// 1 << Flag
// An example, to set both, the little and big endian flags:
// 1 << boLittleEndian | 1 << boBigEndian
typedef uint8_t TByteOrders; 

typedef enum TDataTypeWidth {
	dtwVariable, dtwFixed
} TDataTypeWidth;

typedef enum TIntegerDisplayOption {
	idoDecimal, idoHexadecimalUpperCase, idoHexadecimalLowerCase
} TIntegerDisplayOption;

typedef enum TBytesToStrError {
	btseNone, btseInvalidBytes, btseBytesTooShort
} TBytesToStrError;

typedef enum TStrToBytesError {
	stbeNone, stbeInvalidString, stbeUnderflow, stbeOverflow,
	stbeOutOfRange // if unclear whether underflow or overflow
} TStrToBytesError;

typedef void* TConverterClassID;
typedef TConverterClassID* PConverterClassID;

#ifdef __cplusplus  
}
#endif