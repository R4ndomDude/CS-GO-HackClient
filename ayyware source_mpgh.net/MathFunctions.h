/*
Syn's AyyWare Framework 2015
*/

#pragma once

#include "Vector.h"

#include <stdint.h>

#define PI 3.14159265358979323846f
#define DEG2RAD( x ) ( ( float )( x ) * ( float )( ( float )( PI ) / 180.0f ) )
#define RAD2DEG( x ) ( ( float )( x ) * ( float )( 180.0f / ( float )( PI ) ) )
#define RADPI 57.295779513082f

void AngleVectors(const Vector &angles, Vector *forward);
void VectorTransform(const Vector in1, float in2[3][4], Vector &out);
void SinCos(float a, float* s, float*c);
void VectorAngles(Vector forward, Vector &angles);
void AngleVectors(const Vector &angles, Vector *forward, Vector *right, Vector *up);
void Normalize(Vector &vIn, Vector &vOut);
void CalcAngle(Vector src, Vector dst, Vector &angles);