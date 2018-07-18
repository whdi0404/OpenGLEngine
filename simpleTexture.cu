#include "simpleTexture.cuh"

__global__ void transformKernel(float *outputData,
                                int width,
                                int height,
                                float theta)
{
    // calculate normalized texture coordinates
    unsigned int x = blockIdx.x*blockDim.x + threadIdx.x;
    unsigned int y = blockIdx.y*blockDim.y + threadIdx.y;

    float u = (float)x - (float)width/2; 
    float v = (float)y - (float)height/2; 
    float tu = u*cosf(theta) - v*sinf(theta); 
    float tv = v*cosf(theta) + u*sinf(theta); 

    tu /= (float)width; 
    tv /= (float)height; 

    // read from texture and write to global memory
    outputData[y*width + x] = tex2D(tex, tu+0.5f, tv+0.5f);
}
extern "C"
void runTest(void* texData, int width, int height, int textureDataSize)
{
    // Allocate device memory for result
    float *dData = NULL;
    checkCudaErrors(cudaMalloc((void **) &dData, textureDataSize));

    // Allocate array and copy image data
    cudaChannelFormatDesc channelDesc =
        cudaCreateChannelDesc(32, 0, 0, 0, cudaChannelFormatKindFloat);
    cudaArray *cuArray;
    checkCudaErrors(cudaMallocArray(&cuArray,
                                    &channelDesc,
                                    width,
                                    height));
    checkCudaErrors(cudaMemcpyToArray(cuArray,
                                      0,
                                      0,
                                      texData,
									  textureDataSize,
                                      cudaMemcpyHostToDevice));

    // Set texture parameters
    tex.addressMode[0] = cudaAddressModeWrap;
    tex.addressMode[1] = cudaAddressModeWrap;
    tex.filterMode = cudaFilterModeLinear;
    tex.normalized = true;    // access with normalized texture coordinates

    // Bind the array to the texture
    checkCudaErrors(cudaBindTextureToArray(tex, cuArray, channelDesc));

    dim3 dimBlock(8, 8, 1);
    dim3 dimGrid(width / dimBlock.x, height / dimBlock.y, 1);

    // Warmup
    transformKernel<<<dimGrid, dimBlock, 0>>>(dData, width, height, angle);

    checkCudaErrors(cudaDeviceSynchronize());
    StopWatchInterface *timer = NULL;
    sdkCreateTimer(&timer);
    sdkStartTimer(&timer);

    // Execute the kernel
    transformKernel<<<dimGrid, dimBlock, 0>>>(dData, width, height, angle);

    // Check if kernel execution generated an error
    getLastCudaError("Kernel execution failed");

    checkCudaErrors(cudaDeviceSynchronize());
    sdkStopTimer(&timer);
    printf("Processing time: %f (ms)\n", sdkGetTimerValue(&timer));
    printf("%.2f Mpixels/sec\n",
           (width *height / (sdkGetTimerValue(&timer) / 1000.0f)) / 1e6);
    sdkDeleteTimer(&timer);

    // Allocate mem for the result on host side
    char *hOutputData = (char *) malloc(textureDataSize);

    // copy result from device to host
    checkCudaErrors(cudaMemcpy(hOutputData,
                               dData,
							   textureDataSize,
                               cudaMemcpyDeviceToHost));

    checkCudaErrors(cudaFree(dData));
    checkCudaErrors(cudaFreeArray(cuArray));
}

__global__ void increment_kernel(int *g_data, int inc_value)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	g_data[idx] = g_data[idx] + inc_value;
}

extern "C"
int RunKernel()
{
	int n = 16 * 1024 * 1024;
	int nbytes = n * sizeof(int);
	int value = 26;

	// allocate host memory
	int *a = 0;
	checkCudaErrors(cudaMallocHost((void **)&a, nbytes));
	memset(a, 0, nbytes);

	// allocate device memory
	int *d_a = 0;
	checkCudaErrors(cudaMalloc((void **)&d_a, nbytes));
	checkCudaErrors(cudaMemset(d_a, 255, nbytes));

	// set kernel launch configuration
	dim3 threads = dim3(512, 1);
	dim3 blocks = dim3(n / threads.x, 1);

	// create cuda event handles
	cudaEvent_t start, stop;
	checkCudaErrors(cudaEventCreate(&start));
	checkCudaErrors(cudaEventCreate(&stop));

	StopWatchInterface *timer = NULL;
	sdkCreateTimer(&timer);
	sdkResetTimer(&timer);

	checkCudaErrors(cudaDeviceSynchronize());
	float gpu_time = 0.0f;

	// asynchronously issue work to the GPU (all to stream 0)
	sdkStartTimer(&timer);
	cudaEventRecord(start, 0);
	cudaMemcpyAsync(d_a, a, nbytes, cudaMemcpyHostToDevice, 0);
	increment_kernel<<<blocks, threads, 0, 0 >>>(d_a, value);
	cudaMemcpyAsync(a, d_a, nbytes, cudaMemcpyDeviceToHost, 0);
	cudaEventRecord(stop, 0);
	sdkStopTimer(&timer);

	// have CPU do some work while waiting for stage 1 to finish
	unsigned long int counter = 0;

	while (cudaEventQuery(stop) == cudaErrorNotReady)
	{
		counter++;
	}

	checkCudaErrors(cudaEventElapsedTime(&gpu_time, start, stop));

	// print the cpu and gpu times
	printf("time spent executing by the GPU: %.2f\n", gpu_time);
	printf("time spent by CPU in CUDA calls: %.2f\n", sdkGetTimerValue(&timer));
	printf("CPU executed %lu iterations while waiting for GPU to finish\n", counter);

	// check the output for correctness

	// release resources
	checkCudaErrors(cudaEventDestroy(start));
	checkCudaErrors(cudaEventDestroy(stop));
	checkCudaErrors(cudaFreeHost(a));
	checkCudaErrors(cudaFree(d_a));

	return 0;
}
