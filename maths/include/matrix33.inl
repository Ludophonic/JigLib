// JigLib - Copyright (C) 2004 Danny Chapman
// should only be included from matrix33.inl

// avoid horrible syntax
#define M33_GET(i, j) (*this)(i, j)
// avoid horrible syntax and ease conversion for 1-based indexing
#define M33_GET1(i, j) (*this)(i-1, j-1)

//==============================================================
// tMatrix33
//==============================================================
inline tMatrix33::tMatrix33() {}

//==============================================================
// ~tMatrix33
//==============================================================
inline tMatrix33::~tMatrix33() {}

//==============================================================
// SetTo
//==============================================================
inline void tMatrix33::SetTo(tScalar val)
{
  mCols[0].SetTo(val);
  mCols[1].SetTo(val);
  mCols[2].SetTo(val);
}

//==============================================================
// tMatrix33
//==============================================================
inline tMatrix33::tMatrix33(tScalar val) 
{
  SetTo(val);
}

//==============================================================
// tMatrix33
//==============================================================
inline tMatrix33::tMatrix33(tScalar v11, tScalar v12, tScalar v13,
                            tScalar v21, tScalar v22, tScalar v23,
                            tScalar v31, tScalar v32, tScalar v33  )
{
  mCols[0].Set(v11, v21, v31);
  mCols[1].Set(v12, v22, v32);
  mCols[2].Set(v13, v23, v33);
}

//==============================================================
// tMatrix33
//==============================================================
inline tMatrix33::tMatrix33(tIdentity)
{
  mCols[0].Set(1.0f, 0.0f, 0.0f);
  mCols[1].Set(0.0f, 1.0f, 0.0f);
  mCols[2].Set(0.0f, 0.0f, 1.0f);
}


//==============================================================
// tMatrix33
//==============================================================
inline tMatrix33::tMatrix33(const tVector3 & v1, // first column
                            const tVector3 & v2, 
                            const tVector3 & v3)
{
  mCols[0] = v1;
  mCols[1] = v2;
  mCols[2] = v3;
}

//==============================================================
// Set
//==============================================================
inline void tMatrix33::Set(tScalar v11, tScalar v12, tScalar v13,
                           tScalar v21, tScalar v22, tScalar v23,
                           tScalar v31, tScalar v32, tScalar v33  )
{
  mCols[0].Set(v11, v21, v31);
  mCols[1].Set(v12, v22, v32);
  mCols[2].Set(v13, v23, v33);
}

//==============================================================
// SetData
//==============================================================
inline void tMatrix33::SetData(const tScalar * d)
{
  mCols[0].SetData(&d[0]);
  mCols[1].SetData(&d[3]);
  mCols[2].SetData(&d[6]);
}

//==============================================================
// GetCol
//==============================================================
inline const tVector3 & tMatrix33::GetCol(unsigned i) const
{
  return mCols[i];
}

//==============================================================
// GetCol
//==============================================================
inline tVector3 & tMatrix33::GetCol(unsigned i)
{
  return mCols[i];
}

//==============================================================
// SetCol
//==============================================================
inline void tMatrix33::SetCol(unsigned i, const tVector3 & col)
{
  mCols[i] = col;
}

//==============================================================
// Transpose
//==============================================================
inline tMatrix33 & tMatrix33::Transpose()
{
  Swap( M33_GET(0, 1), M33_GET(1, 0) );
  Swap( M33_GET(0, 2), M33_GET(2, 0) );
  Swap( M33_GET(1, 2), M33_GET(2, 1) );
  return *this;
}

//==============================================================
// GetTranspose
//==============================================================
inline tMatrix33 tMatrix33::GetTranspose() const
{
  return tMatrix33(*this).Transpose();
}

//==============================================================
// GetDeterminant
//==============================================================
inline tScalar tMatrix33::GetDeterminant() const
{
  return 
      M33_GET1(1, 1) * (M33_GET1(2, 2) * M33_GET1(3, 3) - M33_GET1(2, 3) * M33_GET1(3, 2))
    - M33_GET1(1, 2) * (M33_GET1(2, 1) * M33_GET1(3, 3) - M33_GET1(2, 3) * M33_GET1(3, 1))
    + M33_GET1(1, 3) * (M33_GET1(2, 1) * M33_GET1(3, 2) - M33_GET1(2, 2) * M33_GET1(3, 1));
}


//==============================================================
// GetInverted
//==============================================================
inline tMatrix33 tMatrix33::GetInverted() const
{
  tMatrix33 result;
  tScalar det = GetDeterminant();
  if (det <= 0.0f)
  {
    TRACE("Failed to invert matrix\n");
    return mIdentity;
  }
  result(0, 0) = (M33_GET1(2, 2) * M33_GET1(3, 3) - M33_GET1(2, 3) * M33_GET1(3, 2))/det;
  result(0, 1) = (M33_GET1(1, 3) * M33_GET1(3, 2) - M33_GET1(1, 2) * M33_GET1(3, 3))/det;
  result(0, 2) = (M33_GET1(1, 2) * M33_GET1(2, 3) - M33_GET1(1, 3) * M33_GET1(2, 2))/det;

  result(1, 0) = (M33_GET1(2, 3) * M33_GET1(3, 1) - M33_GET1(2, 1) * M33_GET1(3, 3))/det;
  result(1, 1) = (M33_GET1(1, 1) * M33_GET1(3, 3) - M33_GET1(1, 3) * M33_GET1(3, 1))/det;
  result(1, 2) = (M33_GET1(1, 3) * M33_GET1(2, 1) - M33_GET1(1, 1) * M33_GET1(2, 3))/det;

  result(2, 0) = (M33_GET1(2, 1) * M33_GET1(3, 2) - M33_GET1(2, 2) * M33_GET1(3, 1))/det;
  result(2, 1) = (M33_GET1(1, 2) * M33_GET1(3, 1) - M33_GET1(1, 1) * M33_GET1(3, 2))/det;
  result(2, 2) = (M33_GET1(1, 1) * M33_GET1(2, 2) - M33_GET1(1, 2) * M33_GET1(2, 1))/det;

#ifdef _DEBUG
  tMatrix33 check = *this * result;
  check -= mIdentity;
  for (unsigned i = 0 ; i < 3 ; ++i)
    for (unsigned j = 0 ; j < 3 ; ++j)
      Assert(Abs(check(i, j)) < 0.01f);
#endif
  return result;
}

//==============================================================
// Invert
//==============================================================
inline tMatrix33 &tMatrix33::Invert()
{
  *this = GetInverted();
  return *this;
}

//==============================================================
// IsSensible
//==============================================================
inline bool tMatrix33::IsSensible() const
{
  for (unsigned i = 0 ; i < 3 ; ++i)
  {
    if (!mCols[i].IsSensible())
      return false;
  }
  return true;
}

//==============================================================
// Show
//==============================================================
inline void tMatrix33::Show(const char * str) const
{
  unsigned i, j;
  TRACE("%s tMatrix33::this = 0x%p \n", str, this);
  for (i = 0 ; i < 3 ; i++)
  {
    for (j = 0 ; j < 3 ; j++)
    {
      TRACE("%4f ", operator()(i, j));
    }
    TRACE("\n");
  }
}

//==============================================================
// operator+=
//==============================================================
inline tMatrix33 & tMatrix33::operator+=(const tMatrix33 & rhs)
{
  for (unsigned i = 3 ; i-- != 0 ;)
    mCols[i] += rhs.mCols[i];
  return *this;
}

//==============================================================
// operator-=
//==============================================================
inline tMatrix33 & tMatrix33::operator-=(const tMatrix33 & rhs)
{
  for (unsigned i = 3 ; i-- != 0 ;)
    mCols[i] -= rhs.mCols[i];
  return *this;
}

//==============================================================
// operator*=
//==============================================================
inline tMatrix33 & tMatrix33::operator*=(const tScalar rhs)
{
  mCols[0].x *= rhs;
  mCols[0].y *= rhs;
  mCols[0].z *= rhs;
  mCols[1].x *= rhs;
  mCols[1].y *= rhs;
  mCols[1].z *= rhs;
  mCols[2].x *= rhs;
  mCols[2].y *= rhs;
  mCols[2].z *= rhs;
  return *this;
}

//==============================================================
// operator/=
//==============================================================
inline tMatrix33 & tMatrix33::operator/=(const tScalar rhs)
{
  const tScalar invRhs = 1.0f / rhs;
  mCols[0].x *= invRhs;
  mCols[0].y *= invRhs;
  mCols[0].z *= invRhs;
  mCols[1].x *= invRhs;
  mCols[1].y *= invRhs;
  mCols[1].z *= invRhs;
  mCols[2].x *= invRhs;
  mCols[2].y *= invRhs;
  mCols[2].z *= invRhs;
  return *this;
}

//==============================================================
// operator+
//==============================================================
inline tMatrix33 tMatrix33::operator+(const tMatrix33 & rhs) const
{
  return tMatrix33(*this) += rhs;
}

//==============================================================
// operator-
//==============================================================
inline tMatrix33 tMatrix33::operator-(const tMatrix33 & rhs) const
{
  return tMatrix33(*this) -= rhs;
}

// global operators

//==============================================================
// operator*
//==============================================================
inline tMatrix33 operator*(const tMatrix33 & lhs, const tScalar rhs)
{
  return tMatrix33(lhs) *= rhs;
}

//==============================================================
// operator*
//==============================================================
inline tMatrix33 operator*(const tScalar lhs, const tMatrix33 & rhs)
{
  return tMatrix33(rhs) *= lhs;
}

//==============================================================
// operator*
//==============================================================
inline tMatrix33 operator*(const tMatrix33 & lhs, const tMatrix33 & rhs)
{
  tMatrix33 out; 

  out.mCols[0].x = lhs.mCols[0].x * rhs.mCols[0].x + lhs.mCols[1].x * rhs.mCols[0].y + lhs.mCols[2].x * rhs.mCols[0].z;
  out.mCols[0].y = lhs.mCols[0].y * rhs.mCols[0].x + lhs.mCols[1].y * rhs.mCols[0].y + lhs.mCols[2].y * rhs.mCols[0].z;
  out.mCols[0].z = lhs.mCols[0].z * rhs.mCols[0].x + lhs.mCols[1].z * rhs.mCols[0].y + lhs.mCols[2].z * rhs.mCols[0].z;
  out.mCols[1].x = lhs.mCols[0].x * rhs.mCols[1].x + lhs.mCols[1].x * rhs.mCols[1].y + lhs.mCols[2].x * rhs.mCols[1].z;
  out.mCols[1].y = lhs.mCols[0].y * rhs.mCols[1].x + lhs.mCols[1].y * rhs.mCols[1].y + lhs.mCols[2].y * rhs.mCols[1].z;
  out.mCols[1].z = lhs.mCols[0].z * rhs.mCols[1].x + lhs.mCols[1].z * rhs.mCols[1].y + lhs.mCols[2].z * rhs.mCols[1].z;
  out.mCols[2].x = lhs.mCols[0].x * rhs.mCols[2].x + lhs.mCols[1].x * rhs.mCols[2].y + lhs.mCols[2].x * rhs.mCols[2].z;
  out.mCols[2].y = lhs.mCols[0].y * rhs.mCols[2].x + lhs.mCols[1].y * rhs.mCols[2].y + lhs.mCols[2].y * rhs.mCols[2].z;
  out.mCols[2].z = lhs.mCols[0].z * rhs.mCols[2].x + lhs.mCols[1].z * rhs.mCols[2].y + lhs.mCols[2].z * rhs.mCols[2].z;

  return out;
}

//==============================================================
// Proj
// Helper for orthonormalise - projection of v2 onto v1
//==============================================================
static inline tVector3 Proj(const tVector3 & v1, const tVector3 & v2)
{
  return Dot(v1, v2) * v1 / v1.GetLengthSq();
}

//==============================================================
// Orthonormalise
//==============================================================
inline void tMatrix33::Orthonormalise()
{
  tVector3 u1(operator()(0, 0), operator()(1, 0), operator()(2, 0));
  tVector3 u2(operator()(0, 1), operator()(1, 1), operator()(2, 1));
  tVector3 u3(operator()(0, 2), operator()(1, 2), operator()(2, 2));
  
  tVector3 w1 = u1.Normalise();
  
  tVector3 w2 = (u2 - Proj(w1, u2)).Normalise();
  tVector3 w3 = (u3 - Proj(w1, u3) - Proj(w2, u3)).Normalise();
  
  operator()(0, 0) = w1[0];
  operator()(1, 0) = w1[1];
  operator()(2, 0) = w1[2];
  
  operator()(0, 1) = w2[0];
  operator()(1, 1) = w2[1];
  operator()(2, 1) = w2[2];
  
  operator()(0, 2) = w3[0];
  operator()(1, 2) = w3[1];
  operator()(2, 2) = w3[2];
  
  if (IsSensible() == false)
  {
    TRACE("Orthonormalise() resulted in bad matrix\n");
    *this = tMatrix33(tVector3(1, 0, 0), tVector3(0, 1, 0), tVector3(0, 0, 1));
  }
}

//==============================================================
// operator*
// matrix * vector
//==============================================================
inline tVector3 operator*(const tMatrix33 & lhs, const tVector3 & rhs)
{
  return tVector3(
    lhs(0,0) * rhs.x +
    lhs(0,1) * rhs.y +
    lhs(0,2) * rhs.z,
    lhs(1,0) * rhs.x +
    lhs(1,1) * rhs.y +
    lhs(1,2) * rhs.z,
    lhs(2,0) * rhs.x +
    lhs(2,1) * rhs.y +
    lhs(2,2) * rhs.z);
}

//==============================================================
// Trace
//==============================================================
inline tScalar Trace(const tMatrix33 & rhs)
{
  return rhs(0,0) + rhs(1,1) + rhs(2,2);
}

//==============================================================
// Matrix33Alpha
//==============================================================
inline tMatrix33 Matrix33Alpha(tScalar alpha)
{
  tMatrix33 result(0.0f);
  tScalar s = SinDeg(alpha);
  tScalar c = CosDeg(alpha);
  
  result(0,0) = 1.0f;
  result(1,1) = c;
  result(2,2) = c;
  result(2,1) = s;
  result(1,2) = -s;
  
  return result;
}

//==============================================================
// Matrix33Beta
//==============================================================
inline tMatrix33 Matrix33Beta(tScalar beta)
{
  tMatrix33 result(0.0f);
  tScalar s = SinDeg(beta);
  tScalar c = CosDeg(beta);
  
  result(1,1) = 1.0f;
  result(2,2) = c;
  result(0,0) = c;
  result(0,2) = s;
  result(2,0) = -s;
  
  return result;
}

//==============================================================
// Matrix33Gamma
//==============================================================
inline tMatrix33 Matrix33Gamma(tScalar gamma)
{
  tMatrix33 result(0.0f);
  tScalar s = SinDeg(gamma);
  tScalar c = CosDeg(gamma);
  
  result(2,2) = 1.0f;
  result(0,0) = c;
  result(1,1) = c;
  result(1,0) = s;
  result(0,1) = -s;
  
  return result;
}

//==============================================================
// RotationMatrix
//==============================================================
inline tMatrix33 RotationMatrix(tScalar ang, const tVector3 & dir)
{
// from page 32(45) of glspec.dvi
  tMatrix33 uut(dir[0]*dir[0], dir[0]*dir[1], dir[0]*dir[2],
                dir[1]*dir[0], dir[1]*dir[1], dir[1]*dir[2],
                dir[2]*dir[0], dir[2]*dir[1], dir[2]*dir[2]);
  
  tMatrix33 s(0, -dir[2], dir[1],
              dir[2], 0, -dir[0],
              -dir[1], dir[0], 0);
  
  return (uut + (tScalar) CosDeg(ang) * 
          (tMatrix33::Identity() - uut) + SinDeg(ang) * s);
}


//==============================================================
// ApproxEqual
//==============================================================
inline bool ApproxEqual(const tMatrix33 & a, 
                        const tMatrix33 & b, 
                        tScalar tol)
{
  const tScalar * da = a.GetData();
  const tScalar * db = b.GetData();
  int i;
  for (i = 0 ; i < 9 ; ++i)
    if (!ApproxEqual(da[i], db[i], tol)) return false;
  return true;
}

//==============================================================
// MultMatrix33
//==============================================================
inline void MultMatrix33(tVector3 & out,
                         const tMatrix33 & mat,
                         const tVector3 & vec)
{
  out.x = mat(0,0) * vec[0] + mat(0,1) * vec[1] + mat(0,2) * vec[2];
  out.y = mat(1,0) * vec[0] + mat(1,1) * vec[1] + mat(1,2) * vec[2];
  out.z = mat(2,0) * vec[0] + mat(2,1) * vec[1] + mat(2,2) * vec[2];
}


//==============================================================
// MultMatrix33
//==============================================================
inline void MultMatrix33(tMatrix33 & out,
                         const tMatrix33 & lhs,
                         const tMatrix33 & rhs)
{
  for (unsigned oj = 3 ; oj-- != 0 ;)
  {
    for (unsigned oi = 3 ; oi-- != 0 ;)
    {
      out(oi, oj) =
        lhs(oi, 0)*rhs(0, oj) +
        lhs(oi, 1)*rhs(1, oj) +
        lhs(oi, 2)*rhs(2, oj);
    }
  }
}

//==============================================================
// ApplyTransformation
//==============================================================
inline void ApplyTransformation(tVector3 & out,
                                const tVector3 & vec,
                                const tMatrix33 & mat,
                                const tVector3 & pos)
{
  Assert(&out != &vec && &out != &pos);
  
  out.x = vec.x + 
    mat(0,0) * pos[0] + mat(0,1) * pos[1] + mat(0,2) * pos[2];
  out.y = vec.y + 
    mat(1,0) * pos[0] + mat(1,1) * pos[1] + mat(1,2) * pos[2];
  out.z = vec.z + 
    mat(2,0) * pos[0] + mat(2,1) * pos[1] + mat(2,2) * pos[2];
}

    
    
