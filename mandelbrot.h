#ifndef MANDELBROT_H
#define MANDELBROT_H

#include "types.h"
#include "../pixeltoaster/PixelToaster.h"

class Mandelbrot
{
public:
	Mandelbrot( double re_min, double re_max, double im_min, double im_max );
	virtual ~Mandelbrot();

	void SetLimits( double re_min, double re_max, double im_min, double im_max );
	void Draw( PixelToaster::vector<PixelToaster::Pixel>& out_pixels, uint width, uint height ) const;

private:
	//Calculation range - Real = x axis, Imag = y axis
	double mReMin;
	double mReMax;
	double mImMin;
	double mImMax;
};

#endif	//MANDELBROT_H
