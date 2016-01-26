//==============================================================
// Copyright (C) 2004 DanmNy Chapman 
//               danmNy@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file array2d.hpp 
//                     
//==============================================================
#ifndef JIGARRAY2D_H
#define JIGARRAY2D_H

#include "../utils/include/trace.hpp"
#include "../maths/include/mathsmisc.hpp"

#include <string.h>
#include <math.h>

namespace JigLib
{
  /// Defines a 2D array
  template<class T>
  class tArray2D
  {
  public:
    tArray2D(int mNx = 1, int mNy = 1) : mNx(mNx), mNy(mNy), mData(new T[mNx*mNy]), mWrap(false)
      {};
    tArray2D(int mNx, int mNy, const T & val);
    tArray2D(const tArray2D & orig);
    
    ~tArray2D() {delete [] mData; mData = 0;}
    
    tArray2D<T> & operator=(const tArray2D & rhs);
    
    /// allows resizing. Data will be lost if resizing occurred
    void Resize(unsigned nx, unsigned ny) {
      if (nx == mNx && ny == mNy) return;
      delete [] mData; mData = new T[nx*ny]; mNx = nx; mNy = ny;}

    /// enables/disables wrapping
    void SetWrap(bool wrap) {mWrap = wrap;}
    
    //! Unchecked access - no wrapping
    T & operator()(unsigned int i, unsigned int j) {
      return mData[i + j*mNx];}
    //! Unchecked const access
    const T & operator()(unsigned int i, unsigned int j) const {
      return mData[i + j*mNx];}
    //! checked access - unwraps if wrapping set
    T & At(int i, int j);
    //! checked const access
    const T & At(int i, int j) const;
    
    /// Set to a constant value
    void SetTo(T val);

    // interpolate
    T Interpolate(float fi, float fj);

    //! Calculate gradient using centred differences in the x dir
    void CalcGradientX();
    //! Calculate gradient using centred differences in the y dir
    void CalcGradientY();
    
    // sets each value to its absolute value by comparison with T(0)
    void Abs();
    
    // requires T::operator<(...)
    T GetMin() const;
    T GetMax() const;
    
    // scale to fit within range...
    void SetRange(T val_min, T val_max);
    
    //! add
    tArray2D<T> & Add(const tArray2D<T> & rhs);
    //! add scalar
    template <class T1>
    tArray2D<T> & operator+=(const T1 & rhs)
      {
        for (unsigned int i = 0 ; i < mNx*mNy ; ++i) mData[i] += rhs;
        return *this;
      }
    
    //! subtract
    tArray2D<T> & Subtract(const tArray2D<T> & rhs);
    //! subtract scalar
    template <class T1>
    tArray2D<T> & operator-=(const T1 & rhs)
      {
        for (unsigned int i = 0 ; i < mNx*mNy ; ++i) mData[i] -= rhs;
        return *this;
      }
    
    //! multiply
    tArray2D<T> & Multiply(const tArray2D<T> & rhs);
    //! multiply scalar
    template <class T1>
    tArray2D<T> & operator*=(const T1 & rhs)
      {
        for (unsigned int i = 0 ; i < mNx*mNy ; ++i) mData[i] *= rhs;
        return *this;
      }
    template <class T1>
    tArray2D<T> operator*(const T1 & rhs)
      {
        tArray2D<T> result(*this);
        result *= rhs;
        return result;
      }
    
    //! divide
    tArray2D<T> & Divide(const tArray2D<T> & rhs);
    //! divide scalar
    template <class T1>
    tArray2D<T> & operator/=(const T1 & rhs)
      {
        for (unsigned int i = 0 ; i < mNx*mNy ; ++i) mData[i] /= rhs;
        return *this;
      }
    
    //! raise to a power
    template <class T1>
    tArray2D<T> & Pow(const T1 & rhs)
      {
        for (unsigned int i = 0 ; i < mNx*mNy ; ++i) mData[i] = ::pow(mData[i], rhs);
        return *this;
      }
    
    //! Apply a Gaussian filter with length scale r, extending over a
    //! square of half-width n (so n=1 uses a square of 9 points, n = 2
    //! uses 25 etc). Suggest using n at least 2*r.
    void GaussianFilter(float r, int n);
    
    //! return the 'x' size of the array
    unsigned int GetNx() const {return mNx;}
    //! return the 'y' size of the array
    unsigned int GetNy() const {return mNy;}
    
    /// shifts all the elements...
    void Shift(int offsetX, int offsetY);
    
  private:
    inline void UnwrapIndices(int & i, int & y) const;
    
    unsigned int mNx, mNy;
    T * mData;
    bool mWrap;
  };
  
  template<class T>
  void tArray2D<T>::UnwrapIndices(int & i, int & j) const
  {
    if (mWrap == false)
      return;
    
    while (i < 0)
      i += mNx;
    while (j < 0)
      j += mNy;
    
    i = i % mNx;
    j = j % mNy;
  }
  
  
  template<class T>
  tArray2D<T>::tArray2D(int mNx, int mNy, const T & val)
    :
    mNx(mNx), mNy(mNy), mData(new T[mNx*mNy]), mWrap(false)
  {
    unsigned int num = mNx*mNy;
    for (unsigned int i = 0 ; i < num ; ++i)
      mData[i] = val;
  }
  
  template<class T>
  tArray2D<T>::tArray2D(const tArray2D & orig)
    : mNx(orig.mNx), mNy(orig.mNy), mData(new T[mNx*mNy]), mWrap(orig.mWrap)
  {
    memcpy(mData, orig.mData, mNx*mNy*sizeof(T));
  }
  
  template<class T>
  tArray2D<T> & tArray2D<T>::operator=(const tArray2D & rhs)
  {
    if (&rhs == this)
      return *this;
    
    if (mData)
      delete [] mData;
    
    mNx = rhs.mNx;
    mNy = rhs.mNy;
    mData = new T[mNx*mNy];
    
    mWrap = rhs.mWrap;
    
    memcpy(mData, rhs.mData, mNx*mNy*sizeof(T));
    return *this;
  }
  
  template<class T>
  T & tArray2D<T>::At(int i, int j)
  {
    UnwrapIndices(i, j);
    return operator()(i, j);
  }
  
  template<class T>
  const T & tArray2D<T>::At(int i, int j) const
  {
    UnwrapIndices(i, j);
    return operator()(i, j);
  }
  
  template<class T>
  void tArray2D<T>::CalcGradientX()
  {
    unsigned int i, j;
    
    // could do this much more efficiently by using temporaries on the
    // stack in the loop?
    const tArray2D<T> orig(*this);
    
    if (mWrap == false)
    {
      for (j = 0 ; j < mNy ; ++j)
      {
        operator()(0, j) = orig(1, j) - orig(0, j);
        operator()(mNx-1, j) = orig(mNx-1, j) - orig(mNx-2, j);
        
        for (i = 1 ; i < (mNx-1) ; ++i)
        {
          operator()(i, j) = (orig(i+1, j) - orig(i-1, j))/2;
        }
      }
    }
    else
    {
      for (j = 0 ; j < mNy ; ++j)
      {
        for (i = 0 ; i < mNx ; ++i)
        {
          operator()(i, j) = (orig.at(i+1, j) - orig.at(i-1, j))/2;
        }
      }
    }
  }
  
  template<class T>
  void tArray2D<T>::CalcGradientY()
  {
    unsigned int i, j;
    const tArray2D<T> orig(*this);
    
    if (mWrap == false)
    {
      for (i = 0 ; i < mNx ; ++i)
      {
        operator()(i, 0) = (orig(i, 1) - orig(i, 0))/2;
        operator()(i, mNy-1) = (orig(i, mNy-1) - orig(i, mNy-2))/2;
        for (j = 1 ; j < (mNy-1) ; ++j)
        {
          operator()(i, j) = (orig(i, j+1) - orig(i, j-1))/2;
        }
      }
    }
    else
    {
      for (j = 0 ; j < mNy ; ++j)
      {
        for (i = 0 ; i < mNx ; ++i)
        {
          operator()(i, j) = (orig.At(i, j+1) - orig.At(i, j-1))/2;
        }
      }
    }    
  }
  
  template<class T>
  void tArray2D<T>::Shift(int offsetX, int offsetY)
  {
    tArray2D orig(*this);
    for (unsigned i = 0 ; i < mNx ; ++i)
    {
      for (unsigned j = 0 ; j < mNy ; ++j)
      {
        unsigned i0 = (i + offsetX) % mNx;
        unsigned j0 = (j + offsetY) % mNy;
        this->At(i0, j0) = orig.At(i, j);
      }
    }
  }
  
  
  template<class T>
  void tArray2D<T>::GaussianFilter(float r, int n)
  {
    int i, j, ii, jj, iii, jjj;
    
    int size = (n*2 + 1);
    float * filter = new float[size * size];
    
    for (i = 0 ; i < size ; ++i)
    {
      for (j = 0 ; j < size ; ++j)
      {
        filter[i + j * size] = exp ( -( (i-n) * (i-n) + (j-n) * (j-n) ) /
                                     ((float) r * r) );
      }
    }
    
    for (i = 0 ; i < (int) mNx ; ++i)
    {
      for (j = 0 ; j < (int) mNy ; ++j)
      {
        T total(0);
        float weight_total = 0;
        for (ii = -n ; ii < (int) n ; ++ii)
        {
          if ( ( ( (iii = i + ii) >= 0 ) && 
                 (iii < (int) mNx) ) ||
               ( mWrap ) )
          {
            for (jj = -n ; jj < (int) n ; ++jj)
            {
              if ( ( ( (jjj = j + jj) >= 0 ) && 
                     (jjj < (int) mNy) ) ||
                   ( mWrap ) )
              {
                // in a valid location
                int index = (n+ii) + (n+jj)*size;
                weight_total += filter[index];
                total += filter[index] * At(iii, jjj);
              }
            }
          }
        }
        operator()(i, j) = total / weight_total;
      }
    }
    delete [] filter;
  }
  
  template<class T>
  void tArray2D<T>::Abs()
  {
    unsigned int i;
    for (i = 0 ; i < mNx*mNy ; ++i)
    {
      if (mData[i] < T(0))
        mData[i] = -mData[i];
    }
  }
  
  
  template<class T>
  tArray2D<T> & tArray2D<T>::Add(const tArray2D<T> & rhs)
  {
    Assert(rhs.mNx == mNx);
    Assert(rhs.mNy == mNy);
    
    unsigned int i;
    for (i = 0 ; i < mNx*mNy ; ++i)
      mData[i] += rhs.mData[i];
    
    return *this;
  }
  
  template<class T>
  tArray2D<T> & tArray2D<T>::Subtract(const tArray2D<T> & rhs)
  {
    Assert(rhs.mNx == mNx);
    Assert(rhs.mNy == mNy);
    
    unsigned int i;
    for (i = 0 ; i < mNx*mNy ; ++i)
      mData[i] -= rhs.mData[i];
    
    return *this;
  }
  
  template<class T>
  tArray2D<T> & tArray2D<T>::Multiply(const tArray2D<T> & rhs)
  {
    Assert(rhs.mNx == mNx);
    Assert(rhs.mNy == mNy);
    
    unsigned int i;
    for (i = 0 ; i < mNx*mNy ; ++i)
      mData[i] *= rhs.mData[i];
    
    return *this;
  }
  
  template<class T>
  tArray2D<T> & tArray2D<T>::Divide(const tArray2D<T> & rhs)
  {
    Assert(rhs.mNx == mNx);
    Assert(rhs.mNy == mNy);
    
    unsigned int i;
    for (i = 0 ; i < mNx*mNy ; ++i)
      mData[i] /= rhs.mData[i];
    
    return *this;
  }
  
  template<class T>
  T tArray2D<T>::GetMin() const
  {
    T min = mData[0];
    for (unsigned i = 0 ; i < mNx*mNy ; ++i)
    {
      if (mData[i] < min)
        min = mData[i];
    }
    return min;
  }
  
  template<class T>
  T tArray2D<T>::GetMax() const
  {
    
    T max = mData[0];
    for (unsigned i = 0 ; i < mNx*mNy ; ++i)
    {
      if (max < mData[i])
        max = mData[i];
    }
    return max;
  }
  
  template<class T>
  void tArray2D<T>::SetRange(T valMin, T valMax)
  {
    unsigned i;
    T origMin = GetMin();
    T origMax = GetMax();
    // set min to 0 and scale...
    float scale = (valMax - valMin) / (origMax - origMin);
    float offset = valMin - scale * origMin;
    
    for (i = 0 ; i < mNx*mNy ; ++i)
    {
      mData[i] = scale * mData[i] + offset;
    }
  }
  
  template<class T>
  void tArray2D<T>::SetTo(T val)
  {
    for (unsigned i = 0 ; i < mNx*mNy ; ++i)
    {
      mData[i] = val;
    }
  }


  template<class T>
  T tArray2D<T>::Interpolate(float fi, float fj)
  {
    Limit(fi, 0.0f, (mNx - 1.0f));
    Limit(fj, 0.0f, (mNy - 1.0f));
    unsigned i0 = (int) (fi);
    unsigned j0 = (int) (fj);
    unsigned i1 = i0 + 1;
    unsigned j1 = j0 + 1;
    if (i1 >= mNx) i1 = mNx - 1;
    if (j1 >= mNy) j1 = mNy - 1;
    float iFrac = fi - i0;
    float jFrac = fj - j0;
    T result =          jFrac * ( iFrac * (*this)(i1, j1) + (1.0f - iFrac) * (*this)(i0, j1) ) +
      (1.0f - jFrac) * ( iFrac * (*this)(i1, j0) + (1.0f - iFrac) * (*this)(i0, j0) );
    return result;
  }
}


#endif // file included
