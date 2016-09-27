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

double logmax;
double dc;

void LogPlotter( uint16_t iterations, void* copyloc )
{
    Uint32* pix = copyloc;
    double logit = log( iterations );
    *pix = SDL_MapRGB( texfmt, 0, 0, logit * dc );
}

void Plotter( uint16_t iterations, void* copyloc )
{
    Uint32* pix = copyloc;
    *pix = SDL_MapRGB( texfmt, 0, 0, iterations );
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
    logmax = log( maxiter );
    dc = 255 / logmax;

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

    double remin = RE_MIN_MANDELBROT;
    double remax = RE_MAX_MANDELBROT;
    double immin = IM_MIN_MANDELBROT;
    double immax = IM_MAX_MANDELBROT;

    int replot = 1;

    double complex c = 0;

    PlotFunction plotter = Plotter;

    // Wait for the user to quit.
    SDL_Event event;
    do
    {
        SDL_WaitEvent( &event );
        if ( event.type == SDL_MOUSEBUTTONDOWN )
        {
            if ( current_mode == MODE_MANDELBROT )
            {
                current_mode = MODE_JULIA;

                double scalex = (remax - remin) / SCREEN_WIDTH;
                double scaley = (immax - immin) / SCREEN_HEIGHT;
                double adjustx = event.button.x * scalex + remin;
                double adjusty = event.button.y * scaley + immin;
                c = adjustx + adjusty * I;

                remin = RE_MIN_JULIA;
                remax = RE_MAX_JULIA;
                immin = IM_MIN_JULIA;
                immax = IM_MAX_JULIA;

                replot = 1;
            }
            else
            {
                current_mode = MODE_MANDELBROT;

                remin = RE_MIN_MANDELBROT;
                remax = RE_MAX_MANDELBROT;
                immin = IM_MIN_MANDELBROT;
                immax = IM_MAX_MANDELBROT;

                replot = 1;
            }
        }
        else if ( event.type == SDL_KEYDOWN )
        {
            int mousex, mousey;
            SDL_GetMouseState( &mousex, &mousey );

            #define ZOOM_RATIO 0.5
            if ( event.key.keysym.sym == SDLK_z )
            {
                double scalex = (remax - remin) / SCREEN_WIDTH;
                double scaley = (immax - immin) / SCREEN_HEIGHT;
                double adjustx = mousex * scalex + remin;
                double adjusty = mousey * scaley + immin;

                remin = remin + (adjustx - remin) * ZOOM_RATIO;
                remax = remax - (remax - adjustx) * ZOOM_RATIO;
                immin = immin + (adjusty - immin) * ZOOM_RATIO;
                immax = immax - (immax - adjusty) * ZOOM_RATIO;
            }
            else if ( event.key.keysym.sym == SDLK_o )
            {
                if ( current_mode == MODE_JULIA )
                {
                    remin = RE_MIN_JULIA;
                    remax = RE_MAX_JULIA;
                    immin = IM_MIN_JULIA;
                    immax = IM_MAX_JULIA;
                }
                else
                {
                    remin = RE_MIN_MANDELBROT;
                    remax = RE_MAX_MANDELBROT;
                    immin = IM_MIN_MANDELBROT;
                    immax = IM_MAX_MANDELBROT;
                }
                replot = 1;
            }
            else if ( event.key.keysym.sym == SDLK_l )
            {
                if ( plotter == LogPlotter )
                {
                    plotter = Plotter;
                }
                else
                {
                    plotter = LogPlotter;
                }
                replot = 1;
            }

            replot = 1;
        }

        if (replot)
        {
            SDL_LockTexture( fractex, NULL, (void**)&pixels, &pitch );
            if ( current_mode == MODE_JULIA )
            {
                PlotJuliaF( c, pixels, 4, pitch, SCREEN_WIDTH, SCREEN_HEIGHT,
                            remin + immin * I, remax + immax * I, &maxiter, plotter );
            }
            else
            {
                PlotMandelbrotF( pixels, 4, pitch, SCREEN_WIDTH, SCREEN_HEIGHT,
                                 remin + immin * I, remax + immax * I, &maxiter, plotter );
            }
            SDL_UnlockTexture( fractex );
            SDL_RenderCopy( winrend, fractex, NULL, NULL );
            SDL_RenderPresent( winrend );
            replot = 0;
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
