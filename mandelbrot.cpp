#include "mandelbrot.h"

//------------------------------------------------------------------------------
// Constants:
namespace
{
	const uint MAX_ITERATIONS = 50;
}

//------------------------------------------------------------------------------
inline void GenerateColour( PixelToaster::Pixel& out_pixel, uint escape_iterations, uint max_iterations, double z_real, double z_imag )
{
	if( escape_iterations >= max_iterations )
	{
		out_pixel = PixelToaster::Pixel( 0.0f, 0.0f, 0.0f );
		return;
	}

	const uint red_limit = (max_iterations/2)-1;
	if( escape_iterations < red_limit )
	{
		const float val = (float)escape_iterations / (float)red_limit;
		out_pixel = PixelToaster::Pixel( val, 0.0f, 0.0f );
	}
	else
	{
		escape_iterations -= red_limit;
		max_iterations -= red_limit;
		const float val = (float)escape_iterations / (float)red_limit;
		out_pixel = PixelToaster::Pixel( 1.0f, val, val );
	}
}

//------------------------------------------------------------------------------
Mandelbrot::Mandelbrot( double re_min, double re_max, double im_min, double im_max )
{
	SetLimits( re_min, re_max, im_min, im_max );
}

//------------------------------------------------------------------------------
Mandelbrot::~Mandelbrot()
{
	//
}

//------------------------------------------------------------------------------
void Mandelbrot::SetLimits( double re_min, double re_max, double im_min, double im_max )
{
	mReMin = re_min;
	mReMax = re_max;
	mImMin = im_min;
	mImMax = im_max;
}

//------------------------------------------------------------------------------
void Mandelbrot::Draw( PixelToaster::vector<PixelToaster::Pixel>& out_pixels, uint width, uint height ) const
{
	const double re_range = mReMax - mReMin;
	const double re_delta = re_range / (width-1);

	const double im_range = mImMax - mImMin;
	const double im_delta = im_range / (height-1);

	for( uint y = 0; y < height; ++y )
	{
		const double c_imag = mImMax - (y * im_delta);

		for( uint x = 0; x < width; ++x )
		{
			const double c_real = mReMin + (x * re_delta);

			//Is number contained in the mandelbrot set?
			double z_real = c_real;
			double z_imag = c_imag;

			uint escape_iterations = 0;
			while( escape_iterations < MAX_ITERATIONS )
			{
				const double z_real2 = z_real*z_real;
				const double z_imag2 = z_imag*z_imag;

				if( (z_real2 + z_imag2) > 4 )
				{
					break;
				}

				//Note the order of these two calculations!
			    z_imag = 2.0*z_real*z_imag + c_imag;
			    z_real = z_real2 - z_imag2 + c_real;

				++escape_iterations;
			}

			GenerateColour( out_pixels[ x + (y * width) ], escape_iterations, MAX_ITERATIONS, z_real, z_imag );
		}
	}

	//
}
