/*

    Definition file for fractal plotting.

    Created by Jesse Pritchard

*/

#ifndef FRACTALS_H
#define FRACTALS_H

#include "stdlib.h"
#include "stdint.h"
#include "math.h"
#include "complex.h"

typedef void (*PlotFunction)(uint16_t iterations, void* copyloc );

void PlotJulia( complex double c, uint16_t* buf, uint16_t w, uint16_t h,
                complex double upperleft, complex double lowerright, uint16_t* maxiter );
void PlotMandelbrot( uint16_t* buf, uint16_t w, uint16_t h,
                     complex double upperleft, complex double lowerright, uint16_t* maxiter );

void PlotJuliaF( complex double c, void* buf, size_t elsize, int pitch, uint16_t w, uint16_t h,
                 complex double upperleft, complex double lowerright, uint16_t* maxiter,
                 PlotFunction func );

void PlotMandelbrotF( void* buf, size_t elsize, int pitch, uint16_t w, uint16_t h,
                     complex double upperleft, complex double lowerright, uint16_t* maxiter,
                     PlotFunction func );

#endif
