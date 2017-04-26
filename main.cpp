#include "../pixeltoaster/PixelToaster.h"
#include "types.h"
#include "mandelbrot.h"
#include <utility>
#include <cassert>

using namespace PixelToaster;

//------------------------------------------------------------------------------
// Constants:
namespace
{
	const char* const	APP_NAME		= "Mandelbrot";
	const uint			SCREEN_WIDTH	= 640;
	const uint			SCREEN_HEIGHT	= 480;

	const double		RE_MIN_INITIAL	= -2.0;
	const double		RE_MAX_INITIAL	= 1.0;
	const double		IM_MIN_INITIAL	= -1.2;
	const double		IM_MAX_INITIAL	= IM_MIN_INITIAL+(RE_MAX_INITIAL-RE_MIN_INITIAL)*((double)SCREEN_HEIGHT/(double)SCREEN_WIDTH);

	const Pixel			COLOUR_SELECTION_OUTLINE	= Pixel( 1.0f, 1.0f, 1.0f );
	const Pixel			COLOUR_SELECTION_TINT		= Pixel( 0.3f, 0.3f, 0.3f );
}

//------------------------------------------------------------------------------
class Application : public Listener
{
public:
	Application()
		:	mDisplay( APP_NAME, SCREEN_WIDTH, SCREEN_HEIGHT )
		,	mMandelbrot( RE_MIN_INITIAL, RE_MAX_INITIAL, IM_MIN_INITIAL, IM_MAX_INITIAL )
		,	mSelecting( false )
		,	mReMin( RE_MIN_INITIAL )
		,	mReMax( RE_MAX_INITIAL )
		,	mImMin( IM_MIN_INITIAL )
		,	mImMax( IM_MAX_INITIAL )
		,	mSelectX( 0 )
		,	mSelectY( 0 )
		,	mMouseX( 0 )
		,	mMouseY( 0 )
	{
		mDisplay.listener( this );
		mDisplayPixels.resize( SCREEN_WIDTH * SCREEN_HEIGHT );
		mMandelbrotPixels.resize( SCREEN_WIDTH * SCREEN_HEIGHT );

		mMandelbrot.Draw( mMandelbrotPixels, SCREEN_WIDTH, SCREEN_HEIGHT );
	}

	virtual void onMouseMove( DisplayInterface & display, Mouse mouse )
	{
		mMouseX = (uint) std::max( 0.0f, std::min( mouse.x, (float)SCREEN_WIDTH-1.0f ) );
		mMouseY = (uint) std::max( 0.0f, std::min( mouse.y, (float)SCREEN_HEIGHT-1.0f ) );
	}

	virtual void onMouseButtonDown( DisplayInterface & display, Mouse mouse )
	{
		if( ! mSelecting && mouse.buttons.left )
		{
			//Start select
			mSelecting = true;
			mSelectX = mMouseX;
			mSelectY = mMouseY;
		}

		if( mouse.buttons.right )
		{
			//Reset zoom
			mReMin = RE_MIN_INITIAL;
			mReMax = RE_MAX_INITIAL;
			mImMin = IM_MIN_INITIAL;
			mImMax = IM_MAX_INITIAL;

			mMandelbrot.SetLimits( mReMin, mReMax, mImMin, mImMax );
			mMandelbrot.Draw( mMandelbrotPixels, SCREEN_WIDTH, SCREEN_HEIGHT );

			//Reset select
			mSelecting = false;
			mSelectX = 0;
			mSelectY = 0;
		}
	}

	virtual void onMouseButtonUp( DisplayInterface & display, Mouse mouse )
	{
		if( mSelecting && ! mouse.buttons.left )
		{
			ActivateSelection();
		}
	}

	virtual void onKeyDown( DisplayInterface & display, Key key )
	{
		//
	}

	virtual void onKeyUp( DisplayInterface & display, Key key )
	{
		//
	}

	void DrawSelectionBox()
	{
		assert( mSelecting );

		const uint x_start	= std::min( mSelectX, mMouseX );
		const uint x_end	= std::max( mSelectX, mMouseX );
		const uint y_start	= std::min( mSelectY, mMouseY );
		const uint y_end	= std::max( mSelectY, mMouseY );

		//Draw white outline...

		//Horizontal lines
		for( uint x = x_start; x <= x_end; ++x )
		{
			mDisplayPixels[ x + (y_start * SCREEN_WIDTH) ]	= COLOUR_SELECTION_OUTLINE;
			mDisplayPixels[ x + (y_end * SCREEN_WIDTH) ]	= COLOUR_SELECTION_OUTLINE;
		}

		//Vertical lines
		for( uint y = y_start; y <= y_end; ++y )
		{
			const uint y_pixel = y * SCREEN_WIDTH;
			mDisplayPixels[ x_start + y_pixel ]	= COLOUR_SELECTION_OUTLINE;
			mDisplayPixels[ x_end + y_pixel ]	= COLOUR_SELECTION_OUTLINE;
		}

		//Draw tint...
		for( uint y = y_start + 1; y < y_end; ++y )
		{
			for( uint x = x_start + 1; x < x_end; ++x )
			{
				Pixel& p = mDisplayPixels[ x + (y * SCREEN_WIDTH) ];
				p.r += COLOUR_SELECTION_TINT.r;
				p.g += COLOUR_SELECTION_TINT.g;
				p.b += COLOUR_SELECTION_TINT.b;
			}
		}
	}

	void ActivateSelection()
	{
		assert( mSelecting );

		//Calculate new mandelbrot ranges
		const double x_start	= (double) std::min( mSelectX, mMouseX );
		const double x_end		= (double) std::max( mSelectX, mMouseX );
		const double y_start	= (double) std::min( mSelectY, mMouseY );
		const double y_end		= (double) std::max( mSelectY, mMouseY );

		const double re_range = mReMax - mReMin;
		const double re_delta = re_range / (SCREEN_WIDTH-1);
		const double im_range = mImMax - mImMin;
		const double im_delta = im_range / (SCREEN_HEIGHT-1);

		//NOTE: Order of calculations is important
		mReMax = mReMin + (x_end * re_delta);
		mReMin = mReMin + (x_start * re_delta);
		mImMin = mImMax - (y_end * im_delta);
		mImMax = mImMax - (y_start * im_delta);

		//Update mandelbrot
		mMandelbrot.SetLimits( mReMin, mReMax, mImMin, mImMax );
		mMandelbrot.Draw( mMandelbrotPixels, SCREEN_WIDTH, SCREEN_HEIGHT );

		//Reset selection
		mSelecting = false;
		mSelectX = 0;
		mSelectY = 0;
	}

	void Run()
	{
		while( mDisplay.open() )
		{
			memcpy( &mDisplayPixels[ 0 ], &mMandelbrotPixels[ 0 ], SCREEN_WIDTH * SCREEN_HEIGHT * sizeof( Pixel ) );

			if( mSelecting )
			{
				DrawSelectionBox();
			}

			mDisplay.update( mDisplayPixels );
		}
	}

private:
	Display			mDisplay;
	vector<Pixel>	mDisplayPixels;

	Mandelbrot		mMandelbrot;
	vector<Pixel>	mMandelbrotPixels;

	//Mandelbrot draw range
	double			mReMin;
	double			mReMax;
	double			mImMin;
	double			mImMax;

	//Selection box draw values
	bool			mSelecting;
	uint			mSelectX;
	uint			mSelectY;
	uint			mMouseX;
	uint			mMouseY;
};

//------------------------------------------------------------------------------
int main()
{
	Application the_app;
	the_app.Run();
}
