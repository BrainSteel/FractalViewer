/*

    FractalViewer main source file.
    Created by Jesse Pritchard.

*/

#include "stdio.h"
#include "SDL.h"
#include "Font.h"
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

#define ZOOM_RATIO 0.5

#define OVERLAY_SIZE 20

static inline int WithinRect( int x, int y, SDL_Rect rect );
void ChangeMode( int mousex, int mousey, int* current_mode, double complex* c,
                 double* remin, double* remax, double* immin, double* immax );

void Scale( double* remin, double* remax, double* immin, double* immax );

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

    FNT_Font* font = FNT_InitFont( winrend, "rsc/540x20Font.bmp", "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789",
                                   2, 2, (SDL_Color){0, 40, 70} );

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

#define ACTION_FRACTAL 0
#define ACTION_ZOOM 1

    int current_mode = MODE_MANDELBROT;
    int current_action = ACTION_ZOOM;

    double remin = RE_MIN_MANDELBROT;
    double remax = RE_MAX_MANDELBROT;
    double immin = IM_MIN_MANDELBROT;
    double immax = IM_MAX_MANDELBROT;

    int replot = 1;
    int overlay_active = 0;
    int show_overlay = 0;
    int clear_overlay = 0;

    double complex c = 0;

    PlotFunction plotter = Plotter;

    // Wait for the user to quit.
    SDL_Event event;
    do
    {
        SDL_WaitEvent( &event );
        if ( event.type == SDL_MOUSEBUTTONDOWN )
        {
            if ( overlay_active )
            {
                SDL_Rect F;
                F.x = 0;
                F.y = 0;
                F.h = OVERLAY_SIZE;
                F.w = FNT_GetFontCharacterWidth( font, OVERLAY_SIZE ) * 3;

                SDL_Rect Z;
                Z.x = F.x + F.w;
                Z.y = 0;
                Z.h = OVERLAY_SIZE;
                Z.w = FNT_GetFontCharacterWidth( font, OVERLAY_SIZE ) * 3;

                SDL_Rect O;
                O.x = Z.x + Z.w;
                O.y = 0;
                O.h = OVERLAY_SIZE;
                O.w = FNT_GetFontCharacterWidth( font, OVERLAY_SIZE ) * 3;

                SDL_Rect L;
                L.x = O.x + O.w;
                L.y = 0;
                L.h = OVERLAY_SIZE;
                L.w = FNT_GetFontCharacterWidth( font, OVERLAY_SIZE ) * 3;

                int x = event.button.x;
                int y = event.button.y;

                if ( WithinRect( x, y, F ) )
                {
                    current_action = ACTION_FRACTAL;
                }
                else if ( WithinRect( x, y, Z ) )
                {
                    current_action = ACTION_ZOOM;
                }
                else if ( WithinRect( x, y, O ) )
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
                else if ( WithinRect( x, y, L ) )
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

                clear_overlay = 1;
            }
            else if ( event.button.y < OVERLAY_SIZE )
            {
                show_overlay = 1;
            }
            else if ( current_action == ACTION_FRACTAL )
            {
                ChangeMode( event.button.x, event.button.y, &current_mode,
                    &c, &remin, &remax, &immin, &immax );
                replot = 1;
            }
            else if ( current_action == ACTION_ZOOM )
            {
                Scale( &remin, &remax, &immin, &immax );
                replot = 1;
            }
        }
        else if ( event.type == SDL_KEYDOWN )
        {
            if ( event.key.keysym.sym == SDLK_z )
            {
                Scale( &remin, &remax, &immin, &immax );
                replot = 1;
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
        }

        if ( replot || show_overlay || clear_overlay )
        {
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
                replot = 0;
            }

            if ( show_overlay )
            {
                overlay_active = 1;
                show_overlay = 0;

                SDL_RenderCopy( winrend, fractex, NULL, NULL );
                
                SDL_Rect overlay_rect;
                overlay_rect.x = 0;
                overlay_rect.y = 0;
                overlay_rect.w = SCREEN_WIDTH;
                overlay_rect.h = OVERLAY_SIZE;

                SDL_SetRenderDrawColor( winrend, 0, 0, 0, SDL_ALPHA_OPAQUE );
                SDL_RenderFillRect( winrend, &overlay_rect );

                FNT_DrawText( winrend, font, "F  Z  O  L", 0, 0, OVERLAY_SIZE, FNT_ALIGNLEFT | FNT_ALIGNTOP );
            }

            if ( clear_overlay )
            {
                overlay_active = 0;
                clear_overlay = 0;

                SDL_RenderCopy( winrend, fractex, NULL, NULL );
            }

            SDL_RenderPresent( winrend );
        }
    } while ( event.type != SDL_QUIT );

    FNT_DestroyFont( font );

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

int WithinRect( int x, int y, SDL_Rect rect )
{
    if ( x >= rect.x && x <= rect.x + rect.w
         && y >= rect.y && y <= rect.y + rect.h )
    {
        return 1;
    }
    else return 0;
}

void ChangeMode( int mousex, int mousey, int* current_mode, double complex* c,
                 double* remin, double* remax, double* immin, double* immax )
{
    if ( *current_mode == MODE_MANDELBROT )
    {
        *current_mode = MODE_JULIA;

        double scalex = (*remax - *remin) / SCREEN_WIDTH;
        double scaley = (*immax - *immin) / SCREEN_HEIGHT;
        double adjustx = mousex * scalex + *remin;
        double adjusty = mousey * scaley + *immin;
        *c = adjustx + adjusty * I;

        *remin = RE_MIN_JULIA;
        *remax = RE_MAX_JULIA;
        *immin = IM_MIN_JULIA;
        *immax = IM_MAX_JULIA;
    }
    else
    {
        *current_mode = MODE_MANDELBROT;

        *remin = RE_MIN_MANDELBROT;
        *remax = RE_MAX_MANDELBROT;
        *immin = IM_MIN_MANDELBROT;
        *immax = IM_MAX_MANDELBROT;
    }
}

void Scale( double* remin, double* remax, double* immin, double* immax )
{
    int mousex, mousey;
    SDL_GetMouseState( &mousex, &mousey );

    double scalex = (*remax - *remin) / SCREEN_WIDTH;
    double scaley = (*immax - *immin) / SCREEN_HEIGHT;
    double adjustx = mousex * scalex + *remin;
    double adjusty = mousey * scaley + *immin;

    *remin = *remin + (adjustx - *remin) * ZOOM_RATIO;
    *remax = *remax - (*remax - adjustx) * ZOOM_RATIO;
    *immin = *immin + (adjusty - *immin) * ZOOM_RATIO;
    *immax = *immax - (*immax - adjusty) * ZOOM_RATIO;
}
