#include "stdafx.h"
#include "MemoryUtil.h"

void MemoryUtil::float2Bytes(float val, byte * bytes_array)
{
	// Create union of shared memory space
	union {
		float float_variable;
		byte temp_array[4];
	} u;
	// Overite bytes of union with float variable
	u.float_variable = val;
	// Assign bytes to input array
	memcpy(bytes_array, u.temp_array, 4);
}