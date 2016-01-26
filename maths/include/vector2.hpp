#ifndef JIGVECTOR2_HPP
#define JIGVECTOR2_HPP

#include "../maths/include/precision.hpp"
#include "../utils/include/trace.hpp"
#include "../utils/include/assert.hpp"
#include "../maths/include/mathsmisc.hpp"

namespace JigLib
{
  /// A 2x1 matrix
  class tVector2
  {
  public:
    /// public access
    tScalar x, y;

    tVector2(tScalar x, tScalar y = 0.0f) : x(x), y(y) {}
    tVector2() {}

    /// returns pointer to the first element
    tScalar * GetData() {return &x;} 
    /// returns pointer to the first element
    const tScalar * GetData() const {return &y;} 

    void Set(tScalar _x, tScalar _y) {x = _x; y = _y;}

    tScalar GetLength() const {return Hypot(x, y);}
    tScalar GetLengthSq() const {return HypotSq(x, y);}

    /// Normalise, and return the result
    tVector2 & Normalise() {
      (*this) *= (1.0f / this->GetLength()); return *this;}
    /// Get a normalised copy
    tVector2 GetNormalised() const {
      return tVector2(*this).Normalise();}

    tVector2 & operator+=(const tVector2 & v) {x += v.x, y += v.y; return *this;}
    tVector2 & operator-=(const tVector2 & v) {x -= v.x, y -= v.y; return *this;}

    tVector2 & operator*=(tScalar f) {x *= f; y *= f; return *this;}
    tVector2 & operator/=(tScalar f) {x /= f; y /= f; return *this;}

    tVector2 operator-() const {
      return tVector2(-x, -y);}

    void Show(const char * str) const;

    friend tVector2 operator+(const tVector2 & v1, const tVector2 & v2);
    friend tVector2 operator-(const tVector2 & v1, const tVector2 & v2);
    friend tVector2 operator*(const tVector2 & v1, tScalar f);
    friend tVector2 operator*(tScalar f, const tVector2 & v1);
    friend tVector2 operator/(const tVector2 & v1, tScalar f);
    friend tScalar Dot(const tVector2 & v1, const tVector2 & v2);

    // c-style fns avoiding copies
    // out can also be vec1, vec2 or vec3
    friend void AddVector2(tVector2 & out, const tVector2 & vec1, const tVector2 & vec2);
    friend void AddVector2(tVector2 & out, const tVector2 & vec1, const tVector2 & vec2, const tVector2 & vec3);
    friend void SubVector2(tVector2 & out, const tVector2 & vec1, const tVector2 & vec2);
    /// out = scale * vec1
    friend void ScaleVector2(tVector2 & out, const tVector2 & vec1, tScalar scale);
    /// out = vec1 + scale * vec2
    /// out can also be vec1/vec2
    friend void AddScaleVector2(tVector2 & out, const tVector2 & vec1, tScalar scale, const tVector2 & vec2);

  };

  inline tVector2 operator+(const tVector2 & v1, const tVector2 & v2)
  {
    return tVector2(v1.x + v2.x, v1.y + v2.y);
  }

  inline tVector2 operator-(const tVector2 & v1, const tVector2 & v2)
  {
    return tVector2(v1.x - v2.x, v1.y - v2.y);
  }

  inline tVector2 operator*(const tVector2 & v1, tScalar f)
  {
    return tVector2(v1.x * f, v1.y * f);
  }

  inline tVector2 operator*(tScalar f, const tVector2 & v1)
  {
    return tVector2(v1.x * f, v1.y * f);
  }

  inline tVector2 operator/(const tVector2 & v1, tScalar f)
  {
    return tVector2(v1.x / f, v1.y / f);
  }

  inline tScalar Dot(const tVector2 & v1, const tVector2 & v2)
  {
    return v1.x * v2.x + v1.y * v2.y;
  }

  inline void AddVector2(tVector2 & out, const tVector2 & vec1, const tVector2 & vec2)
  {
    out.x = vec1.x + vec2.x;
    out.y = vec1.y + vec2.y;
  }

  inline void AddVector2(tVector2 & out, const tVector2 & vec1, const tVector2 & vec2, const tVector2 & vec3)
  {
    out.x = vec1.x + vec2.x + vec3.x;
    out.y = vec1.y + vec2.y + vec3.y;
  }

  inline void SubVector2(tVector2 & out, const tVector2 & vec1, const tVector2 & vec2)
  {
    out.x = vec1.x - vec2.x;
    out.y = vec1.y - vec2.y;
  }

  inline void ScaleVector2(tVector2 & out, const tVector2 & vec1, tScalar scale)
  {
    out.x = vec1.x * scale;
    out.y = vec1.y * scale;
  }

  inline void AddScaleVector2(tVector2 & out, const tVector2 & vec1, tScalar scale, const tVector2 & vec2)
  {
    out.x = vec1.x + scale * vec2.x;
    out.y = vec1.y + scale * vec2.y;
  }

  inline void tVector2::Show(const char * str) const
  {
    TRACE("%s tVector2::this = 0x%p \n", str, this);
    TRACE("%4f %4f", x, y);
    TRACE("\n");
  }

  inline tScalar Min(const tVector2& vec) {return Min(vec.x, vec.y);}
  inline tScalar Max(const tVector2& vec) {return Max(vec.x, vec.y);}
}


#endif
