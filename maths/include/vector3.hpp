// JigLib - Copyright (C) 2004 Danny Chapman
#ifndef JIGVECTOR3_HPP
#define JIGVECTOR3_HPP

#include "../maths/include/precision.hpp"
#include "../utils/include/trace.hpp"
#include "../utils/include/assert.hpp"
#include "../maths/include/mathsmisc.hpp"

namespace JigLib
{
  class tMatrix33;
  
  /// A 3x1 matrix
  class tVector3
  {
  public:
    // public access
    tScalar x, y, z;

    /// default constructor does not initialise
    tVector3() {}
    explicit tVector3(tScalar val);
    tVector3(tScalar x, tScalar y, tScalar z);
    // sets *this = scale * vec, avoiding temporary
    tVector3(tScalar scale, const tVector3& vec);
    ~tVector3() {}

    enum tZero {ZERO};
    tVector3(tZero) : x(0.0f), y(0.0f), z(0.0f) {}
    
    // Some static "helpers"
    enum {LOOK_INDEX = 0, LEFT_INDEX = 1, UP_INDEX = 2};
    static const tVector3 & Look() {return mLook;}
    static const tVector3 & Left() {return mLeft;}
    static const tVector3 & Up() {return mUp;}
    /// all zeros
    static const tVector3 & Zero() {return mZero;}
    
    /// Set all to zero
    void SetToZero() {x = y = z = 0.0f;}
    /// set all values to val
    void SetTo(tScalar val);
    /// set the individual values
    void Set(tScalar x, tScalar y, tScalar z);
    /// Indicate if the values are finite
    bool IsSensible() const;
    
    // checked access (checked in debug)
    tScalar & operator[](unsigned i);
    const tScalar & operator[](unsigned i) const;
    tScalar & operator()(unsigned i);
    const tScalar & operator()(unsigned i) const;
    
    /// returns pointer to the first element
    tScalar * GetData() {return &x;} 
    /// returns pointer to the first element
    const tScalar * GetData() const {return &x;} 
    
    /// pass in array of size 3
    void SetData(const tScalar * d);
    
    // operators
    tVector3 & operator+=(const tVector3 & rhs);
    tVector3 & operator-=(const tVector3 & rhs);
    
    tVector3 & operator*=(const tScalar rhs);
    tVector3 & operator/=(const tScalar rhs);
    
    tVector3 operator-() const {
      return tVector3(-x, -y, -z);}
    
    tVector3 operator+(const tVector3 & rhs) const;
    tVector3 operator-(const tVector3 & rhs) const;
    
    tVector3 & Negate();
    
    void Show(const char * str = "") const;
    
    /// return the square of the length
    tScalar GetLengthSq() const {
      return (x*x + y*y + z*z);}
    /// return the length
    tScalar GetLength() const {
      return Sqrt(x*x + y*y + z*z);}
    
    /// Normalise, and return the result
    tVector3 & Normalise();
    /// Get a normalised copy
    tVector3 GetNormalised() const;
    
    /// If the length is OK, normalises, else returns an arbitrary
    /// unit vector
    tVector3 & NormaliseSafe(const tVector3& safe = tVector3::Up());
    /// Get a safe normalised copy
    tVector3 GetNormalisedSafe(const tVector3& safe = tVector3::Up()) const;
    
    // friends
    friend tVector3 operator*(const tVector3 & lhs, const tScalar rhs);
    friend tVector3 operator*(const tScalar lhs, const tVector3 & rhs);
    friend tVector3 operator/(const tVector3 & lhs, const tScalar rhs);
    friend tScalar Dot(const tVector3 & lhs, const tVector3 & rhs);
    friend tVector3 Cross(const tVector3 & lhs, const tVector3 & rhs);
    friend tVector3 operator*(const tMatrix33 & lhs, const tVector3 & rhs);
  private:
    static tVector3 mZero;
    static tVector3 mLook;
    static tVector3 mLeft;
    static tVector3 mUp;
  };
  
  // global operators
  inline tVector3 operator*(const tVector3 & lhs, const tScalar rhs) {
    return tVector3(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs); }
  inline tVector3 operator/(const tVector3 & lhs, const tScalar rhs) {
    const tScalar inv_rhs = 1.0f/rhs; 
    return tVector3(lhs.x * inv_rhs, lhs.y * inv_rhs, lhs.z * inv_rhs);}
  tScalar Dot(const tVector3 & lhs, const tVector3 & rhs);
  tVector3 Cross(const tVector3 & lhs, const tVector3 & rhs);
  tVector3 ElementMult(const tVector3 & lhs, const tVector3 & rhs);

  inline tVector3 operator*(const tScalar lhs, const tVector3 & rhs) {
    return rhs * lhs;}
  inline tScalar Min(const tVector3& vec) {return Min(vec.x, vec.y, vec.z);}
  inline tScalar Max(const tVector3& vec) {return Max(vec.x, vec.y, vec.z);}

  // C-style fns to avoid copy
  /// out = vec1 + vec2
  /// out can also be vec1/vec2
  void AddVector3(tVector3 & out, const tVector3 & vec1, const tVector3 & vec2);
  /// out = vec1 + vec2 + vec3
  /// out can also be vec1/vec2
  void AddVector3(tVector3 & out, const tVector3 & vec1, const tVector3 & vec2, const tVector3 & vec3);
  /// out = vec1 - vec2
  /// out can also be vec1/vec2
  void SubVector3(tVector3 & out, const tVector3 & vec1, const tVector3 & vec2);
  /// out = vec1 + scale * vec2
  /// out can also be vec1/vec2
  void AddScaleVector3(tVector3 & out, const tVector3 & vec1, tScalar scale, const tVector3 & vec2);
  /// out = vec1 * scale
  void ScaleVector3(tVector3 & out, const tVector3 & vec1, tScalar scale);

  bool ApproxEqual(const tVector3 & a, 
                   const tVector3 & b, 
                   tScalar tol = SCALAR_TINY);
  
// tVector3 inline implementation
#include "../maths/include/vector3.inl"
}

#endif
