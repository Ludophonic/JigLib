//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file matrix33.hpp
//                     
//==============================================================
#ifndef JIGMATRIX33_HPP
#define JIGMATRIX33_HPP

#include "../maths/include/precision.hpp"
#include "../utils/include/trace.hpp"
#include "../utils/include/assert.hpp"
#include "../maths/include/vector3.hpp"
#include "../maths/include/mathsmisc.hpp"

namespace JigLib
{
  /// Defines a 3x3 matrix - data is column major, so when using as a
  /// transformation matrix use pre-multiplication
  class tMatrix33
  {
  public:
    // public access to the data...
    tVector3 mCols[3];

    // Args are as you'd write out
    tMatrix33(tScalar v11, tScalar v12, tScalar v13, 
              tScalar v21, tScalar v22, tScalar v23, 
              tScalar v31, tScalar v32, tScalar v33  );
    /// Set the 3 columns
    tMatrix33(const tVector3 & c1,
              const tVector3 & c2, 
              const tVector3 & c3);
    /// default ctor does not initialise
    tMatrix33();
    explicit tMatrix33(tScalar val);
    ~tMatrix33();

    enum tIdentity {IDENTITY};
    tMatrix33(tIdentity);
    
    static const tMatrix33 & Identity() {return mIdentity;}
    
    void SetTo(tScalar val);

    void SetToIdentity() {*this = mIdentity;}

    // Args are as you'd write out
    void Set(tScalar v11, tScalar v12, tScalar v13, 
             tScalar v21, tScalar v22, tScalar v23, 
             tScalar v31, tScalar v32, tScalar v33  );

    void Orthonormalise();
    /// indicates if all is OK
    bool IsSensible() const; 
    
    tScalar & operator()(const unsigned i, const unsigned j) {
      return mCols[j][i];}
    const tScalar & operator()(const unsigned i, const unsigned j) const {
      return mCols[j][i];}
    
    /// returns pointer to the first element - the first of 9
    const tScalar * GetData() {return mCols[0].GetData();} 
    const tScalar * GetData() const {return mCols[0].GetData();} 
    /// pointer to value returned from get_data
    void SetData(const tScalar * d); 
    
    /// Returns a column
    const tVector3 & GetCol(unsigned i) const;
    tVector3 & GetCol(unsigned i);
    const tVector3 & operator[](const unsigned i) const {
      return mCols[i];}    
    tVector3 & operator[](const unsigned i) {
      return mCols[i];}    
    
    /// Get the directions when used as an orientaiton matrix
    const tVector3 & GetLook() const {
      return mCols[tVector3::LOOK_INDEX];}
    const tVector3 & GetLeft() const {
      return mCols[tVector3::LEFT_INDEX];}
    const tVector3 & GetUp() const {
      return mCols[tVector3::UP_INDEX];}
    
    // sets a column
    void SetCol(unsigned i, const tVector3 & col);
    
    tScalar GetDeterminant() const;

    /// Transposes this matrix
    tMatrix33 & Transpose();
    
    /// Gets a transposed copy
    tMatrix33 GetTranspose() const;
    
    /// Full inversion
    tMatrix33 &Invert();

    /// Gets an inverted copy
    tMatrix33 GetInverted() const;

    // operators
    tMatrix33 & operator+=(const tMatrix33 & rhs);
    tMatrix33 & operator-=(const tMatrix33 & rhs);
    
    tMatrix33 & operator*=(const tScalar rhs);
    tMatrix33 & operator/=(const tScalar rhs);
    
    tMatrix33 operator+(const tMatrix33 & rhs) const;
    tMatrix33 operator-(const tMatrix33 & rhs) const;

    friend tMatrix33 operator*(const tMatrix33 & lhs, const tScalar rhs);
    friend tMatrix33 operator*(const tScalar lhs, const tMatrix33 & rhs);
    friend tMatrix33 operator*(const tMatrix33 & lhs, const tMatrix33 & rhs);
    friend tScalar Trace(const tMatrix33 & rhs);  
    friend tVector3 operator*(const tMatrix33 & lhs, const tVector3 & rhs);
    
    // Some function forms for non-copy means to multiply etc
    /// out = mat * vec
    friend void MultMatrix33(tVector3 & out,
                             const tMatrix33 & mat,
                             const tVector3 & vec);

    /// out = mat1 * mat2
    friend void MultMatrix33(tMatrix33 & out,
                             const tMatrix33 & mat1,
                             const tMatrix33 & mat2);
    /// out = mat * vec + pos i.e. mat and pos are treated as a 4x4
    /// transformation
    friend void ApplyTransformation(tVector3 & out,
                                    const tVector3 & vec,
                                    const tMatrix33 & mat,
                                    const tVector3 & pos);
    
    

    void Show(const char * str = "") const;
    
  private:
    static tMatrix33 mIdentity;
  };
  
  // angles in degrees
  tMatrix33 Matrix33Alpha(tScalar alpha);
  tMatrix33 Matrix33Beta(tScalar beta);
  tMatrix33 Matrix33Gamma(tScalar gamma);
  /// Assumes dir is normalised. angle is in deg
  tMatrix33 RotationMatrix(tScalar ang, const tVector3 & dir);
  
  /// converts a rotation matrix into a rotation of degrees about axis
  void CalculateRotFromMatrix(const tMatrix33 & matrix, 
                              tVector3 & axis, 
                              tScalar & degrees);
  
  bool ApproxEqual(const tMatrix33 & a, 
                   const tMatrix33 & b, 
                   tScalar tol = SCALAR_TINY);
  
// tMatrix33 inline implementation
#include "../maths/include/matrix33.inl"
}

#endif

