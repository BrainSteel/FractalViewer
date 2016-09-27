/*

    Implementation file for self-squared fractals.
    Julia Set
    Mandelbrot Set

    Created by Jesse Pritchard
*/

#include "fractals.h"

static inline uint16_t getIterations(double zx0, double zy0, double cx0, double cy0, uint16_t max)
{
    uint16_t iterations = 1;
    double xtemp;

    if ( cx0 < 0.5 && cx0 > -0.5 && cy0 < 0.5 && cy0 > -0.5 )
    {
        int breakme = 5;
    }
    
    while (zx0 * zx0 + zy0 * zy0 < 2 * 2 && iterations < max) {
        xtemp = zx0;
        zx0 = zx0 * zx0 - zy0 * zy0 + cx0;
        zy0 = 2 * xtemp * zy0 + cy0;
        iterations++;
    }

    return iterations;
}

#define MAX_ITERATIONS_DEFAULT 400
void PlotJulia( complex double c, uint16_t* buf, uint16_t w, uint16_t h,
                complex double upperleft, complex double lowerright, uint16_t* maxiter )
{
    uint16_t realmax = MAX_ITERATIONS_DEFAULT;
    if ( maxiter )
    {
        realmax = *maxiter;
    }

    double cx0 = creal( c );
    double cy0 = cimag( c );

    double xi = creal( upperleft );
    double xf = creal( lowerright );

    double yi = cimag( upperleft );
    double yf = cimag( lowerright );

    double dx = xf - xi;
    double dy = yf - yi;

    double x = xi;
    double y = yi;

    int i, j;
    for ( j = 0; j < h; j++, y += dy )
    {
        for ( i = 0; i < w; i++, x += dx )
        {
            buf[i + j * w] = getIterations( x, y, cx0, cy0, realmax );
        }
    }
}

void PlotMandelbrot( uint16_t* buf, uint16_t w, uint16_t h,
                     complex double upperleft, complex double lowerright, uint16_t* maxiter )
{
    uint16_t realmax = MAX_ITERATIONS_DEFAULT;
    if ( maxiter )
    {
        realmax = *maxiter;
    }

    double xi = creal( upperleft );
    double xf = creal( lowerright );

    double yi = cimag( upperleft );
    double yf = cimag( lowerright );

    double dx = xf - xi;
    double dy = yf - yi;

    double x = xi;
    double y = yi;

    int i, j;
    for ( j = 0; j < h; j++, y += dy )
    {
        for ( i = 0; i < w; i++, x += dx )
        {
            buf[i + j * w] = getIterations( 0, 0, x, y, realmax );
        }
    }
}


void PlotJuliaF( complex double c, uint16_t* buf, size_t elsize, int pitch, uint16_t w, uint16_t h,
                 complex double upperleft, complex double lowerright, uint16_t* maxiter,
                 PlotFunction func )
{
    uint16_t realmax = MAX_ITERATIONS_DEFAULT;
    if ( maxiter )
    {
        realmax = *maxiter;
    }

    double cx0 = creal( c );
    double cy0 = cimag( c );

    double xi = creal( upperleft );
    double xf = creal( lowerright );

    double yi = cimag( upperleft );
    double yf = cimag( lowerright );

    double dx = xf - xi;
    double dy = yf - yi;

    double x = xi;
    double y = yi;

    uint32_t dypixels = pitch - elsize * w;

    int i, j;
    for ( j = 0; j < h; j++, y += dy )
    {
        for ( i = 0; i < w; i++, x += dx )
        {
            (*func)( getIterations( x, y, cx0, cy0, realmax ), buf );
            buf += elsize;
        }
        buf += dypixels;
    }
}

void PlotMandelbrotF( void* buf, size_t elsize, int pitch, uint16_t w, uint16_t h,
                     complex double upperleft, complex double lowerright, uint16_t* maxiter,
                     PlotFunction func )
{
    uint16_t realmax = MAX_ITERATIONS_DEFAULT;
    if ( maxiter )
    {
        realmax = *maxiter;
    }

    double xi = creal( upperleft );
    double xf = creal( lowerright );

    double yi = cimag( upperleft );
    double yf = cimag( lowerright );

    double dx = (xf - xi) / (double)w;
    double dy = (yf - yi) / (double)h;

    double x = xi;
    double y = yi;

    uint32_t dypixels = pitch - elsize * w;

    int i, j;
    for ( j = 0; j < h; j++, y += dy )
    {
        for ( i = 0; i < w; i++, x += dx )
        {
            (*func)( getIterations( 0, 0, x, y, realmax ), buf );
            buf += elsize;
        }
        x = xi;
        buf += dypixels;
    }
}
