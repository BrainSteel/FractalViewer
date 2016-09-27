/*

    FractalViewer main source file.
    Created by Jesse Pritchard.

*/

#include "stdio.h"
#include "SDL.h"
#include "fractals.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define WINDOW_FLAGS SDL_WINDOW_SHOWN

#define RE_MIN_MANDELBROT ((-2.0))
#define RE_MAX_MANDELBROT ((1.0))
#define IM_MIN_MANDELBROT ((-1.5))
#define IM_MAX_MANDELBROT ((1.5))

#define RE_MIN_JULIA ((-1.5))
#define RE_MAX_JULIA ((1.5))
#define IM_MIN_JULIA ((-1.5))
#define IM_MAX_JULIA ((1.5))


SDL_PixelFormat* texfmt;

void Plotter( uint16_t iterations, void* copyloc )
{
    Uint32* pix = copyloc;
    *pix = SDL_MapRGB( texfmt, iterations, iterations, iterations );
}

int main( int argc, char** argv )
{
    // Initialize SDL 2.0.
    if ( SDL_Init( SDL_INIT_EVERYTHING ) < 0 )
    {
        printf( "Failed to initialize SDL.\n" );
        return 1;
    }

    // Create a window and rendering context.
    SDL_Window* window;
    SDL_Renderer* winrend;
    if ( SDL_CreateWindowAndRenderer( SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_FLAGS,
                                      &window, &winrend ) < 0 )
    {
        printf( "Failed window initialization processes.\n" );
        return 1;
    }

    SDL_Texture* fractex = SDL_CreateTexture( winrend, SDL_GetWindowPixelFormat(window),
                           SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT );

    Uint32 enumfmt;
    SDL_QueryTexture( fractex, &enumfmt, NULL, NULL, NULL );
    texfmt = SDL_AllocFormat( enumfmt );

    Uint32* pixels;
    int pitch;
    uint16_t maxiter = 255;

    SDL_LockTexture( fractex, NULL, (void**)&pixels, &pitch );
    PlotMandelbrotF( pixels, 4, pitch, SCREEN_WIDTH, SCREEN_HEIGHT,
                     RE_MIN_MANDELBROT + IM_MIN_MANDELBROT * I,
                     RE_MAX_MANDELBROT + IM_MAX_MANDELBROT * I, &maxiter, Plotter );
    SDL_UnlockTexture( fractex );

    SDL_RenderCopy( winrend, fractex, NULL, NULL );

    SDL_RenderPresent( winrend );

#define MODE_MANDELBROT 0
#define MODE_JULIA 1

    int current_mode = MODE_MANDELBROT;
    // Wait for the user to quit.
    SDL_Event event;
    do
    {
        SDL_WaitEvent( &event );
        if ( event.type == SDL_MOUSEBUTTONDOWN )
        {
            SDL_LockTexture( fractex, NULL, (void**)&pixels, &pitch );
            if ( current_mode == MODE_MANDELBROT )
            {
                double scalex = (RE_MAX_MANDELBROT - RE_MIN_MANDELBROT) / SCREEN_WIDTH;
                double scaley = (IM_MAX_MANDELBROT - IM_MIN_MANDELBROT) / SCREEN_HEIGHT;
                double adjustx = event.button.x * scalex + RE_MIN_MANDELBROT;
                double adjusty = event.button.y * scaley + IM_MIN_MANDELBROT;
                PlotJuliaF( adjustx + adjusty * I, pixels, 4, pitch, SCREEN_WIDTH, SCREEN_HEIGHT,
                            RE_MIN_JULIA + IM_MIN_JULIA * I,
                            RE_MAX_JULIA + IM_MAX_JULIA * I, &maxiter, Plotter );
                current_mode = MODE_JULIA;
            }
            else
            {
                PlotMandelbrotF( pixels, 4, pitch, SCREEN_WIDTH, SCREEN_HEIGHT,
                                 RE_MIN_MANDELBROT + IM_MIN_MANDELBROT * I,
                                 RE_MAX_MANDELBROT + IM_MAX_MANDELBROT * I, &maxiter, Plotter );
                current_mode = MODE_MANDELBROT;
            }
            SDL_UnlockTexture( fractex );
            SDL_RenderCopy( winrend, fractex, NULL, NULL );
            SDL_RenderPresent( winrend );
        }
    } while ( event.type != SDL_QUIT );

    SDL_FreeFormat( texfmt );
    SDL_DestroyTexture( fractex );

    // Destroy the window and rendering context.
    SDL_DestroyWindow( window );
    SDL_DestroyRenderer( winrend );

    // Quit SDL.
    SDL_Quit( );

    // Success!
    return 0;
}
