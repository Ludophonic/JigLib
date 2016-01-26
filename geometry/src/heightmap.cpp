//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file heightmap.cpp 
//                     
//==============================================================
#include "heightmap.hpp"
#include "distance.hpp"

using namespace JigLib;

//==============================================================
// Clone
//==============================================================
tPrimitive* tHeightmap::Clone() const
{
  return new tHeightmap(*this);
}


//==============================================================
// tHeightmap
//==============================================================
tHeightmap::tHeightmap(const tArray2D<tScalar> & heights, 
                       tScalar x0, tScalar y0,
                       tScalar dx, tScalar dy)
  :
  tPrimitive(tPrimitive::HEIGHTMAP),
  mHeights(heights),
  mX0(x0), mY0(y0),
  mDx(dx), mDy(dy)
{
  Assert(mHeights.GetNx() >= 2);
  Assert(mHeights.GetNy() >= 2);
  
  mXMin = mX0 - (mHeights.GetNx() - 1) * 0.5f * mDx;
  mYMin = mY0 - (mHeights.GetNy() - 1) * 0.5f * mDy;
  mXMax = mX0 + (mHeights.GetNx() - 1) * 0.5f * mDx;
  mYMax = mY0 + (mHeights.GetNy() - 1) * 0.5f * mDy;
}

//==============================================================
// GetHeight
//==============================================================
tScalar tHeightmap::GetHeight(int i, int j) const
{
  Limit(i, 0, (int) mHeights.GetNx() - 1);
  Limit(j, 0, (int) mHeights.GetNy() - 1);
  return mHeights(i, j);
}
//==============================================================
// GetNormal
//==============================================================
tVector3 tHeightmap::GetNormal(int i, int j) const
{
  int i0 = i-1;
  int i1 = i+1;
  int j0 = j-1;
  int j1 = j+1;
  Limit(i0, 0, (int) mHeights.GetNx() - 1);
  Limit(j0, 0, (int) mHeights.GetNy() - 1);
  Limit(i1, 0, (int) mHeights.GetNx() - 1);
  Limit(j1, 0, (int) mHeights.GetNy() - 1);
  
  tScalar dx = (i1 - i0) * mDx;
  tScalar dy = (j1 - j0) * mDy;
  if (i0 == i1) dx = 1.0f;
  if (j0 == j1) dy = 1.0f;
  if (i0 == i1 && j0 == j1) return tVector3::Up();
  
  tScalar hFwd = mHeights(i1, j);
  tScalar hBack = mHeights(i0, j);
  tScalar hLeft = mHeights(i, j1);
  tScalar hRight = mHeights(i, j0);
  
  tVector3 normal = Cross(tVector3(dx, 0.0f, hFwd - hBack),
                          tVector3(0.0f, dy, hLeft - hRight)).Normalise();
  return normal;
}

//==============================================================
// GetHeightAndNormal
//==============================================================
void tHeightmap::GetHeightAndNormal(tScalar & h,
                                    tVector3 & normal, 
                                    int i, int j) const
{
  h = GetHeight(i, j);
  normal = GetNormal(i, j);
}

//==============================================================
// GetSurfacePos
//==============================================================
void tHeightmap::GetSurfacePos(tVector3 & pos, int i, int j) const
{
  tScalar h = GetHeight(i, j);
  pos = tVector3(mXMin + i * mDx, mYMin + j * mDy, h);
}

//==============================================================
// GetSurfacePosAndNormal
//==============================================================
void tHeightmap::GetSurfacePosAndNormal(tVector3 & pos, 
                                        tVector3 & normal,
                                        int i, int j) const
{
  tScalar h = GetHeight(i, j);
  pos = tVector3(mXMin + i * mDx, mYMin + j * mDy, h);
  normal = GetNormal(i, j);
}

//==============================================================
// GetHeight
//==============================================================
tScalar tHeightmap::GetHeight(const tVector3 & point) const
{
  // todo - optimise
  tScalar h;
  tVector3 normal;
  GetHeightAndNormal(h, normal, point);
  return h;
}
//==============================================================
// GetNormal
//==============================================================
tVector3 tHeightmap::GetNormal(const tVector3 & point) const
{
  // todo - optimise
  tScalar h;
  tVector3 normal;
  GetHeightAndNormal(h, normal, point);
  return normal;
}

//==============================================================
// GetHeightAndNormal
//==============================================================
void tHeightmap::GetHeightAndNormal(tScalar & h, 
                                    tVector3 & normal, 
                                    const tVector3 & point) const
{
  tScalar x = point.x;
  tScalar y = point.y;
  Limit(x, mXMin, mXMax);
  Limit(y, mYMin, mYMax);
  
  int i0 = (int) ((x - mXMin)/mDx);
  int j0 = (int) ((point.y - mYMin)/mDy);
  Limit(i0, 0, (int) mHeights.GetNx() - 1);
  Limit(j0, 0, (int) mHeights.GetNy() - 1);
  
  int i1 = i0 + 1;
  int j1 = j0 + 1;
  if (i1 >= (int) mHeights.GetNx()) i1 = mHeights.GetNx() - 1;
  if (j1 >= (int) mHeights.GetNy()) j1 = mHeights.GetNy() - 1;
  
  tScalar iFrac = (x - (i0 * mDx + mXMin))/mDx;
  tScalar jFrac = (y - (j0 * mDy + mYMin))/mDy;
  Limit(iFrac, SCALAR(0.0f), SCALAR(1.0f));
  Limit(jFrac, SCALAR(0.0f), SCALAR(1.0f));
  
  tScalar h00 = mHeights(i0, j0);
  tScalar h01 = mHeights(i0, j1);
  tScalar h10 = mHeights(i1, j0);
  tScalar h11 = mHeights(i1, j1);
  
  // All the triangles are orientated the same way.
  // work out the normal, then z is in the plane of this normal
  if ( (i0 == i1) && (j0 == j1) )
  {
    normal = tVector3(0.0f, 0.0f, 1.0f);
  }
  else if (i0 == i1)
  {
    normal = Cross(tVector3::Look(),
                   tVector3(0.0f, mDy, h01 - h00)).Normalise();
  }
  if (j0 == j1)
  {
    normal = Cross(tVector3(mDx, 0.0f, h10 - h00), 
                   tVector3::Left()).Normalise();
  }
  else if (iFrac > jFrac)
  {
    // fwd tri
    normal = Cross(tVector3(mDx, 0.0f, h10 - h00), 
                   tVector3(mDx, mDy, h11 - h00)).Normalise();
  }
  else
  {
    // left tri
    normal = Cross(tVector3(mDx, mDy, h11 - h00), 
                   tVector3(0.0f, mDy, h01 - h00)).Normalise();
  }
  
  // get the plane equation
  // h00 is in all the triangles
  tPlane plane(normal, tVector3((i0 * mDx + mXMin), (j0 * mDy + mYMin), h00));
  h = PointPlaneDistance(point, plane);
}
//==============================================================
// GetSurfacePos
//==============================================================
void tHeightmap::GetSurfacePos(tVector3 & pos, const tVector3 & point) const
{
  // todo - optimise
  tScalar h = GetHeight(point);
  pos = tVector3(point.x, point.y, h);
}
//==============================================================
// GetSurfacePosAndNormal
//==============================================================
void tHeightmap::GetSurfacePosAndNormal(tVector3 & pos, 
                                        tVector3 & normal, 
                                        const tVector3 & point) const
{
  tScalar h;
  GetHeightAndNormal(h, normal, point);
  pos = tVector3(point.x, point.y, h);
}
//==============================================================
// SegmentIntersect
// assume that the segment doesn't pass through the terrain and out.
//==============================================================
bool tHeightmap::SegmentIntersect(tScalar &frac, tVector3 &pos, tVector3 &normal, const class tSegment &seg) const
{
  if (seg.mDelta.z > -SCALAR_TINY)
    return false;
  
  tVector3 normalStart;
  tScalar heightStart;
  GetHeightAndNormal(heightStart, normalStart, seg.mOrigin);
  
  if (heightStart < 0.0f)
    return false;
  
  tVector3 normalEnd;
  tScalar heightEnd;
  tVector3 end = seg.GetEnd();
  GetHeightAndNormal(heightEnd, normalEnd, end);
  
  if (heightEnd > 0.0f)
    return false;
  
  // start is above, end is below...
  tScalar depthEnd = -heightEnd;
  
  // normal is the weighted mean of these...
  tScalar weightStart = 1.0f / (SCALAR_TINY + heightStart);
  tScalar weightEnd = 1.0f / (SCALAR_TINY + depthEnd);
  
  normal = (normalStart * weightStart + normalEnd * weightEnd) /
    (weightStart + weightEnd);
  
  frac = heightStart / (heightStart + depthEnd + SCALAR_TINY);
  
  pos = seg.GetPoint(frac);
  
  return true;
}

//==============================================================
// GetMassProperties
//==============================================================
void tHeightmap::GetMassProperties(const tPrimitiveProperties &primitiveProperties, 
                                   tScalar &mass, 
                                   tVector3 &centerOfMass, 
                                   tMatrix33 &inertiaTensor) const
{
  mass = 0.0f;
  centerOfMass.SetToZero();
  inertiaTensor.SetTo(0.0f);
}

