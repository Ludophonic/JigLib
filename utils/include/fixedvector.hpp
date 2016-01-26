//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file fixedvector.hpp
//                     
//==============================================================
#ifndef FIXEDVECTOR_HPP
#define FIXEDVECTOR_HPP

namespace JigLib
{
  /// like std::vector, but the size is known at compile time
  template<typename T, int SIZE>
  class tFixedVector
  {
  public:
    tFixedVector() : ind(0) {}
    ~tFixedVector() {}
    
    // This is safe against trying to push back too many, but it doesn't indicate
    // when that happens
    void PushBack(const T & val) {
      if (ind < SIZE) mVals[ind++] = val;}
    void PopBack() {if (ind > 0) --ind;}
    
    /// random access, but you can only access up to the working size, not
    /// the max possible size
    const T & operator[](unsigned i) const {Assert(i < ind); return mVals[i];}
    T & operator[](unsigned i) {Assert(i < ind); return mVals[i];}
    
    void Resize(unsigned newSize) {Assert(newSize <= SIZE) ; ind = newSize;}
    void Clear() {ind = 0;}
    unsigned Size() const {return ind;}
    bool Empty() const {return ind == 0;}
  private:
    /// ind points to the element one beyond the last valid element
    /// so size = ind
    unsigned ind;
    T mVals[SIZE];
  };
}

#endif
