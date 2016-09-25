/*

    FractalViewer main source file.
    Created by Jesse Pritchard.

*/


#include "SDL.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define WINDOW_FLAGS SDL_WINDOW_SHOWN

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

    // Clear the screen to white.
    SDL_SetRenderDrawColor( winrend, 255, 255, 255, SDL_ALPHA_OPAQUE );
    SDL_RenderClear( winrend );
    SDL_RenderPresent( winrend );

    // Wait for the user to quit.
    SDL_Event event;
    do
    {
        SDL_WaitEvent( &event );
    } while ( event.type != SDL_QUIT );

    // Destroy the window and rendering context.
    SDL_DestroyWindow( window );
    SDL_DestroyRenderer( winrend );

    // Quit SDL.
    SDL_Quit( );

    // Success!
    return 0;
}
