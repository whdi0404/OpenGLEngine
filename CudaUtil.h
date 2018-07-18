#pragma once

// CUDA system and GL includes
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>

// Helper functions
#include <helper_functions.h> // CUDA SDK Helper functions
#include <helper_cuda.h>      // CUDA device initialization helper functions

typedef unsigned int uint;
typedef unsigned char uchar;

//CudaTest
enum eFilterMode
{
	MODE_NEAREST,
	MODE_BILINEAR,
	MODE_BICUBIC,
	MODE_FAST_BICUBIC,
	MODE_CATMULL_ROM,
	NUM_MODES
};

extern "C"
void runTest(void* texData, int width, int height, int textureDataSize);
extern "C"
int RunKernel();