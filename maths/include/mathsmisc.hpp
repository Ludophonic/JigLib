//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file mathsmisc.hpp 
//                     
//==============================================================
#ifndef MATHSMISC_HPP
#define MATHSMISC_HPP
#include "../maths/include/precision.hpp"
#include <cmath>

namespace JigLib
{
  
// some useful angle things
#define PI ((tScalar) 3.1415926535897932384626433832795)
#define TWO_PI ((tScalar) 6.28318530717958647692528676655901)
#define PI_DIV_180 ((tScalar) 0.0174532925199432957692369076848861)
#define _180_DIV_PI ((tScalar) 57.2957795130823208767981548141052)
  
#define SCALAR_TINY ((tScalar) 1.0e-6)
#define SCALAR_HUGE ((tScalar) 1.0e6)
  
  inline tScalar Deg2Rad(tScalar deg) {return (tScalar) (deg * PI_DIV_180);}
  inline tScalar Rad2Deg(tScalar rad) {return (tScalar) (rad * _180_DIV_PI);}
  
#ifdef USING_DOUBLE
  inline tScalar Sin(tScalar radians) {return sin(radians);}
  inline tScalar Cos(tScalar radians) {return cos(radians);}
  inline tScalar Tan(tScalar radians) {return tan(radians);}
  inline tScalar ASin(tScalar x) {return asin(x);}
  inline tScalar ACos(tScalar x) {return acos(x);}
  inline tScalar ATan(tScalar x) {return atan(x);}
  inline tScalar ATan2(tScalar x, tScalar y) {return atan2(x, y);}
  inline tScalar Sqrt(tScalar x) {return sqrt(x);}
  inline tScalar Hypot(tScalar x, tScalar y) {return (tScalar) (hypot(x, y));}
#else
  // I don't think sinf etc are part of ANSI C, so if the compiler barfs
  // then force using the double versions
  inline tScalar Sin(tScalar radians) {return sinf(radians);}
  inline tScalar Cos(tScalar radians) {return cosf(radians);}
  inline tScalar Tan(tScalar radians) {return tanf(radians);}
  inline tScalar ASin(tScalar x) {return asinf(x);}
  inline tScalar ACos(tScalar x) {return acosf(x);}
  inline tScalar ATan(tScalar x) {return atanf(x);}
  inline tScalar ATan2(tScalar x, tScalar y) {return atan2f(x, y);}
  inline tScalar Sqrt(tScalar x) {return sqrtf(x);}
  inline tScalar Hypot(tScalar x, tScalar y) {return sqrtf(x*x + y*y);}
#endif
  inline tScalar SinDeg(tScalar deg) {return Sin(Deg2Rad(deg));}
  inline tScalar CosDeg(tScalar deg) {return Cos(Deg2Rad(deg));}
  inline tScalar AsinDeg(tScalar x) {return Rad2Deg(ASin((tScalar) x));}
  inline tScalar AcosDeg(tScalar x) {return Rad2Deg(ACos((tScalar) x));}
  inline tScalar TanDeg(tScalar deg) {return Tan(Deg2Rad(deg));}
  inline tScalar ATan2Deg(tScalar x, tScalar y) {return Rad2Deg(ATan2(x, y));}
  inline tScalar ATanDeg(tScalar x) {return Rad2Deg(ATan(x));}
  
// Other useful things we might want to re-implement...
  template<typename T> inline T Sq(T val) {return val * val;}
  template<typename T> inline T Cube(T val) {return val * val * val;}
  inline tScalar Hypot(tScalar x, tScalar y, tScalar z) {
    return Sqrt(Sq(x) + Sq(y) + Sq(z));}
  inline tScalar HypotSq(tScalar x, tScalar y) {return (Sq(x) + Sq(y));}
  inline tScalar HypotSq(tScalar x, tScalar y, tScalar z) {
    return (Sq(x) + Sq(y) + Sq(z));}
  inline tScalar Exp(tScalar x) {return (tScalar) (exp(x));}
  
  template<typename T>
  inline T Abs(T val) {
    return (val > T(0) ? val : -val); }
  
  inline tScalar SafeInvScalar(tScalar val) {
    return Abs(val) > SCALAR_TINY ? (1.0f / val) : SCALAR_HUGE; }
  
  template<typename T>
  inline void Limit(T & val, T min, T max) {
    if (val < min) val = min; else if (val > max) val = max;}
  
  template<typename T>
  inline void Swap(T & a, T & b) {T temp = a; a = b; b = temp;}
  
  template<typename T>
  inline void Wrap(T & val, const T & min, const T & max) {
    const T delta(max - min); while (val < min) val += delta; while (val > max) val -= delta;}

  // there is an STL version somewhere...
  template<class T>
  inline T Min(const T & a, const T & b) {return (a < b ? a : b);}
  template<class T>
  inline T Max(const T & a, const T & b) {return (a > b ? a : b);}

  // Useful for getting the min/max of vector3
  template<class T>
  inline T Min(const T & a, const T & b, const T & c) {return Min(Min(a, b), c);}
  template<class T>
  inline T Max(const T & a, const T & b, const T & c) {return Max(Max(a, b), c);}
  
  /// Returns a random number between v1 and v2
  inline tScalar RangedRandom(tScalar v1, tScalar v2) {
    return v1 + (v2-v1)*((tScalar)rand())/((tScalar)RAND_MAX);}
  
  /// Indicates if two scalars are equal to within a tolerance
  inline bool ApproxEqual(tScalar a, tScalar b, tScalar tol = SCALAR_TINY) {
    return (Abs(a - b) < tol ? true : false); }
  
  /// Indicates if the input is finite
  inline bool IsFinite(tScalar val)
  {
    return ((val < SCALAR_HUGE) && (val > -SCALAR_HUGE));
  }
}  

#endif
