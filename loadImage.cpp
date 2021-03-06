/*
 * http://github.com/dusty-nv/jetson-inference
 */
 
#include "loadImage.h"
#include "cudaMappedMemory.h"

#include <QImage>


// loadImageRGBA
bool loadImageRGBA( const char* filename, float4** cpu, float4** gpu, int* width, int* height )
{
	if( !filename || !cpu || !gpu || !width || !height )
	{
		printf("loadImageRGB - invalid parameter\n");
		return false;
	}
	
	// load original image
	QImage qImg;

	if( !qImg.load(filename) )
	{
		printf("failed to load image %s\n", filename);
		return false;
	}

	if( *width != 0 && *height != 0 )
		qImg = qImg.scaled(*width, *height, Qt::IgnoreAspectRatio);
	
	const uint32_t imgWidth  = qImg.width();
	const uint32_t imgHeight = qImg.height();
	const uint32_t imgPixels = imgWidth * imgHeight;
	const size_t   imgSize   = imgWidth * imgHeight * sizeof(float) * 4;

	printf("loaded image  %s  (%u x %u)  %zu bytes\n", filename, imgWidth, imgHeight, imgSize);

	// allocate buffer for the image
	if( !cudaAllocMapped((void**)cpu, (void**)gpu, imgSize) )
	{
		printf(LOG_CUDA "failed to allocated %zu bytes for image %s\n", imgSize, filename);
		return false;
	}

	float4* cpuPtr = *cpu;
	
	for( uint32_t y=0; y < imgHeight; y++ )
	{
		for( uint32_t x=0; x < imgWidth; x++ )
		{
			const QRgb rgb  = qImg.pixel(x,y);
			const float4 px = make_float4(float(qRed(rgb)), 
										  float(qGreen(rgb)), 
										  float(qBlue(rgb)), 1.0f);
			
			cpuPtr[y*imgWidth+x] = px;
		}
	}
	
	*width  = imgWidth;
	*height = imgHeight;	
	return true;
}


// loadImageRGB
bool loadImageRGB( const char* filename, float3** cpu, float3** gpu, int* width, int* height, const float3& mean )
{
	if( !filename || !cpu || !gpu || !width || !height )
	{
		printf("loadImageRGB - invalid parameter\n");
		return false;
	}
	
	// load original image
	QImage qImg;

	if( !qImg.load(filename) )
	{
		printf("failed to load image %s\n", filename);
		return false;
	}

	if( *width != 0 && *height != 0 )
		qImg = qImg.scaled(*width, *height, Qt::IgnoreAspectRatio);
	
	const uint32_t imgWidth  = qImg.width();
	const uint32_t imgHeight = qImg.height();
	const uint32_t imgPixels = imgWidth * imgHeight;
	const size_t   imgSize   = imgWidth * imgHeight * sizeof(float) * 3;

	printf("loaded image  %s  (%u x %u)  %zu bytes\n", filename, imgWidth, imgHeight, imgSize);

	// allocate buffer for the image
	if( !cudaAllocMapped((void**)cpu, (void**)gpu, imgSize) )
	{
		printf(LOG_CUDA "failed to allocated %zu bytes for image %s\n", imgSize, filename);
		return false;
	}

	float* cpuPtr = (float*)*cpu;
	
	for( uint32_t y=0; y < imgHeight; y++ )
	{
		for( uint32_t x=0; x < imgWidth; x++ )
		{
			const QRgb rgb  = qImg.pixel(x,y);
			const float mul = 1.0f; 	//1.0f / 255.0f;
			const float3 px = make_float3((float(qRed(rgb))   - mean.x) * mul, 
										  (float(qGreen(rgb)) - mean.y) * mul, 
										  (float(qBlue(rgb))  - mean.z) * mul );
			
			// note:  caffe/GIE is band-sequential (as opposed to the typical Band Interleaved by Pixel)
			cpuPtr[imgPixels * 0 + y * imgWidth + x] = px.x; 
			cpuPtr[imgPixels * 1 + y * imgWidth + x] = px.y; 
			cpuPtr[imgPixels * 2 + y * imgWidth + x] = px.z; 
		}
	}
		
	*width  = imgWidth;
	*height = imgHeight;
	return true;
}


// loadImageBGR
bool loadImageBGR( const char* filename, float3** cpu, float3** gpu, int* width, int* height, const float3& mean )
{
	if( !filename || !cpu || !gpu || !width || !height )
	{
		printf("loadImageRGB - invalid parameter\n");
		return false;
	}
	
	// load original image
	QImage qImg;

	if( !qImg.load(filename) )
	{
		printf("failed to load image %s\n", filename);
		return false;
	}

	if( *width != 0 && *height != 0 )
		qImg = qImg.scaled(*width, *height, Qt::IgnoreAspectRatio);
	
	const uint32_t imgWidth  = qImg.width();
	const uint32_t imgHeight = qImg.height();
	const uint32_t imgPixels = imgWidth * imgHeight;
	const size_t   imgSize   = imgWidth * imgHeight * sizeof(float) * 3;

	printf("loaded image  %s  (%u x %u)  %zu bytes\n", filename, imgWidth, imgHeight, imgSize);

	// allocate buffer for the image
	if( !cudaAllocMapped((void**)cpu, (void**)gpu, imgSize) )
	{
		printf(LOG_CUDA "failed to allocated %zu bytes for image %s\n", imgSize, filename);
		return false;
	}

	float* cpuPtr = (float*)*cpu;
	
	for( uint32_t y=0; y < imgHeight; y++ )
	{
		for( uint32_t x=0; x < imgWidth; x++ )
		{
			const QRgb rgb  = qImg.pixel(x,y);
			const float mul = 1.0f; 	//1.0f / 255.0f;
			const float3 px = make_float3((float(qBlue(rgb))  - mean.x) * mul, 
										  (float(qGreen(rgb)) - mean.y) * mul, 
										  (float(qRed(rgb))   - mean.z) * mul );
			
			// note:  caffe/GIE is band-sequential (as opposed to the typical Band Interleaved by Pixel)
			cpuPtr[imgPixels * 0 + y * imgWidth + x] = px.x; 
			cpuPtr[imgPixels * 1 + y * imgWidth + x] = px.y; 
			cpuPtr[imgPixels * 2 + y * imgWidth + x] = px.z; 
		}
	}
			
	return true;
}