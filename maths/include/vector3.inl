//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file vector3.inl 
//                     
//==============================================================
// should only be included from vector3.hpp

inline tScalar & tVector3::operator[](unsigned i) 
{
  Assert(i < 3);
  return (&x)[i];
}

inline const tScalar & tVector3::operator[](unsigned i) const 
{
  Assert(i < 3);
  return (&x)[i];
}

inline tScalar & tVector3::operator()(unsigned i) 
{
  Assert(i < 3);
  return (&x)[i];
}

inline const tScalar & tVector3::operator()(unsigned i) const 
{
  Assert(i < 3);
  return (&x)[i];
}

inline void tVector3::SetTo(tScalar val)
{
  x = y = z = val;
}

inline void tVector3::Set(tScalar _x, tScalar _y, tScalar _z)
{
  x = _x; y = _y; z = _z;
}

inline tVector3::tVector3(tScalar val) : x(val), y(val), z(val) 
{
}

inline tVector3::tVector3(tScalar _x, tScalar _y, tScalar _z) : x(_x), y(_y), z(_z)
{
}

inline tVector3::tVector3(tScalar scale, const tVector3& vec) : x(scale * vec.x), y(scale * vec.y), z(scale * vec.z) 
{
}

inline void tVector3::SetData(const tScalar * d)
{
  x = d[0];
  y = d[1];
  z = d[2];
}

inline tVector3 & tVector3::operator+=(const tVector3 & rhs)
{
  x += rhs.x;
  y += rhs.y;
  z += rhs.z;
  return *this;
}

inline tVector3 & tVector3::operator-=(const tVector3 & rhs)
{
  x -= rhs.x;
  y -= rhs.y;
  z -= rhs.z;
  return *this;
}

inline tVector3 & tVector3::operator*=(const tScalar rhs)
{
  x *= rhs;
  y *= rhs;
  z *= rhs;
  return *this;
}

inline tVector3 & tVector3::operator/=(const tScalar rhs)
{
  tScalar invRhs = 1.0f / rhs;
  x *= invRhs;
  y *= invRhs;
  z *= invRhs;
  return *this;
}

inline tVector3 tVector3::operator+(const tVector3 & rhs) const
{
  return tVector3(x + rhs.x, y + rhs.y, z + rhs.z);
}

inline tVector3 tVector3::operator-(const tVector3 & rhs) const
{
  return tVector3(x - rhs.x, y - rhs.y, z - rhs.z);
}

inline tVector3 & tVector3::Negate()
{
  x = -x; y = -y; z = -z;
  return *this;
}

inline tVector3 & tVector3::Normalise() 
{
  (*this) *= (1.0f / GetLength()); return *this;
}

inline tVector3 tVector3::GetNormalised() const 
{
  return tVector3(*this).Normalise();
}

inline tVector3 & tVector3::NormaliseSafe(const tVector3& safe)
{
  const tScalar lSq = GetLengthSq();
  if (lSq > SCALAR_TINY)
  {
    (*this) *= (1.0f / Sqrt(lSq));
  }
  else
  {
    *this = safe;
  }
  return *this;
}

inline tVector3 tVector3::GetNormalisedSafe(const tVector3& safe) const 
{
  return tVector3(*this).NormaliseSafe(safe);
}

inline bool tVector3::IsSensible() const
{
  if (!IsFinite(x)) return false;
  if (!IsFinite(y)) return false;
  if (!IsFinite(z)) return false;
  return true;
}

inline void tVector3::Show(const char * str) const
{
  unsigned i;
  TRACE("%s tVector3::this = 0x%p \n", str, this);
  for (i = 0 ; i < 3 ; i++)
  {
    TRACE("%4f ", (&x)[i]);
  }
  TRACE("\n");
}

// global operators
inline tScalar Dot(const tVector3 & lhs, const tVector3 & rhs)
{
  return (lhs.x * rhs.x +
          lhs.y * rhs.y +
          lhs.z * rhs.z);
}

inline tVector3 Cross(const tVector3 & lhs, const tVector3 & rhs)
{
  return tVector3(lhs.y*rhs.z - lhs.z*rhs.y,
                  lhs.z*rhs.x - lhs.x*rhs.z,
                  lhs.x*rhs.y - lhs.y*rhs.x);
}

inline bool ApproxEqual(const tVector3 & a, 
                        const tVector3 & b, 
                        tScalar tol)
{
  if (!ApproxEqual(a.x, b.x, tol)) return false;
  if (!ApproxEqual(a.y, b.y, tol)) return false;
  if (!ApproxEqual(a.z, b.z, tol)) return false;
  return true;
}

inline tVector3 ElementMult(const tVector3 & lhs, const tVector3 & rhs)
{
  return tVector3(lhs.x * rhs.x,
                  lhs.y * rhs.y,
                  lhs.z * rhs.z);
}

inline void AddVector3(tVector3 & out, 
                       const tVector3 & vec1, 
                       const tVector3 & vec2)
{
  out.x = vec1.x + vec2.x;
  out.y = vec1.y + vec2.y;
  out.z = vec1.z + vec2.z;
}

inline void AddVector3(tVector3 & out, 
                       const tVector3 & vec1, 
                       const tVector3 & vec2, 
                       const tVector3 & vec3)
{
  out.x = vec1.x + vec2.x + vec3.x;
  out.y = vec1.y + vec2.y + vec3.y;
  out.z = vec1.z + vec2.z + vec3.z;
}

inline void SubVector3(tVector3 & out, 
                       const tVector3 & vec1, 
                       const tVector3 & vec2)
{
  out.x = vec1.x - vec2.x;
  out.y = vec1.y - vec2.y;
  out.z = vec1.z - vec2.z;
}

inline void ScaleVector3(tVector3 & out, const tVector3 & vec1, tScalar scale)
{
  out.x = vec1.x * scale;
  out.y = vec1.y * scale;
  out.z = vec1.z * scale;
}

inline void AddScaleVector3(tVector3 & out, const tVector3 & vec1, tScalar scale, const tVector3 & vec2)
{
  out.x = vec1.x + scale * vec2.x;
  out.y = vec1.y + scale * vec2.y;
  out.z = vec1.z + scale * vec2.z;
}
