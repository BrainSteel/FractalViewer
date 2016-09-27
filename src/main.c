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

SDL_PixelFormat texfmt;

void Plotter( uint16_t iterations, void* copyloc )
{
    Uint32* pix = copyloc;
    *pix = SDL_MapRGBA( &texfmt, iterations, iterations, iterations, SDL_ALPHA_OPAQUE );
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

    SDL_QueryTexture( fractex, &texfmt.format, NULL, NULL, NULL );
    int bpp;
    SDL_PixelFormatEnumToMasks( texfmt.format, &bpp, &texfmt.Rmask, &texfmt.Gmask, &texfmt.Bmask, &texfmt.Amask );
    texfmt.BitsPerPixel = bpp;
    texfmt.BytesPerPixel = (bpp + 1) / 4;

    Uint32* pixels;
    int pitch;
    uint16_t maxiter = 255;

    SDL_LockTexture( fractex, NULL, (void**)&pixels, &pitch );
    PlotMandelbrotF( pixels, 4, pitch, SCREEN_WIDTH, SCREEN_HEIGHT, 1.5 - 1.5 * I, -1.5 + 1.5 * I, &maxiter, Plotter );
    SDL_UnlockTexture( fractex );

    SDL_RenderCopy( winrend, fractex, NULL, NULL );

    SDL_RenderPresent( winrend );


    // Wait for the user to quit.
    SDL_Event event;
    do
    {
        SDL_WaitEvent( &event );
    } while ( event.type != SDL_QUIT );

    SDL_DestroyTexture( fractex );

    // Destroy the window and rendering context.
    SDL_DestroyWindow( window );
    SDL_DestroyRenderer( winrend );

    // Quit SDL.
    SDL_Quit( );

    // Success!
    return 0;
}
