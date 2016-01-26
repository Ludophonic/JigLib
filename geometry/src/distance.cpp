//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file distance.cpp 
//                     
//==============================================================
#include "distance.hpp"
#include "intersection.hpp"
#include <limits>
using namespace JigLib;

//==============================================================
// SegmentSegmentDistanceSq
//==============================================================
tScalar JigLib::SegmentSegmentDistanceSq(tScalar *t0, tScalar *t1, 
                                         const tSegment & seg0, const tSegment & seg1)
{
  tVector3 kDiff = seg0.mOrigin - seg1.mOrigin;
  tScalar fA00 = seg0.mDelta.GetLengthSq();
  tScalar fA01 = -Dot(seg0.mDelta, seg1.mDelta);
  tScalar fA11 = seg1.mDelta.GetLengthSq();
  tScalar fB0 = Dot(kDiff, seg0.mDelta);
  tScalar fC = kDiff.GetLengthSq();
  tScalar fDet = Abs(fA00*fA11-fA01*fA01);
  tScalar fB1, fS, fT, fSqrDist, fTmp;

  if ( fDet >= SCALAR_TINY )
  {
    // line segments are not parallel
    fB1 = -Dot(kDiff, seg1.mDelta);
    fS = fA01*fB1-fA11*fB0;
    fT = fA01*fB0-fA00*fB1;

    if ( fS >= (tScalar)0.0 )
    {
      if ( fS <= fDet )
      {
        if ( fT >= (tScalar)0.0 )
        {
          if ( fT <= fDet )  // region 0 (interior)
          {
            // minimum at two interior points of 3D lines
            tScalar fInvDet = ((tScalar)1.0)/fDet;
            fS *= fInvDet;
            fT *= fInvDet;
            fSqrDist = fS*(fA00*fS+fA01*fT+((tScalar)2.0)*fB0) +
              fT*(fA01*fS+fA11*fT+((tScalar)2.0)*fB1)+fC;
          }
          else  // region 3 (side)
          {
            fT = (tScalar)1.0;
            fTmp = fA01+fB0;
            if ( fTmp >= (tScalar)0.0 )
            {
              fS = (tScalar)0.0;
              fSqrDist = fA11+((tScalar)2.0)*fB1+fC;
            }
            else if ( -fTmp >= fA00 )
            {
              fS = (tScalar)1.0;
              fSqrDist = fA00+fA11+fC+((tScalar)2.0)*(fB1+fTmp);
            }
            else
            {
              fS = -fTmp/fA00;
              fSqrDist = fTmp*fS+fA11+((tScalar)2.0)*fB1+fC;
            }
          }
        }
        else  // region 7 (side)
        {
          fT = (tScalar)0.0;
          if ( fB0 >= (tScalar)0.0 )
          {
            fS = (tScalar)0.0;
            fSqrDist = fC;
          }
          else if ( -fB0 >= fA00 )
          {
            fS = (tScalar)1.0;
            fSqrDist = fA00+((tScalar)2.0)*fB0+fC;
          }
          else
          {
            fS = -fB0/fA00;
            fSqrDist = fB0*fS+fC;
          }
        }
      }
      else
      {
        if ( fT >= (tScalar)0.0 )
        {
          if ( fT <= fDet )  // region 1 (side)
          {
            fS = (tScalar)1.0;
            fTmp = fA01+fB1;
            if ( fTmp >= (tScalar)0.0 )
            {
              fT = (tScalar)0.0;
              fSqrDist = fA00+((tScalar)2.0)*fB0+fC;
            }
            else if ( -fTmp >= fA11 )
            {
              fT = (tScalar)1.0;
              fSqrDist = fA00+fA11+fC+((tScalar)2.0)*(fB0+fTmp);
            }
            else
            {
              fT = -fTmp/fA11;
              fSqrDist = fTmp*fT+fA00+((tScalar)2.0)*fB0+fC;
            }
          }
          else  // region 2 (corner)
          {
            fTmp = fA01+fB0;
            if ( -fTmp <= fA00 )
            {
              fT = (tScalar)1.0;
              if ( fTmp >= (tScalar)0.0 )
              {
                fS = (tScalar)0.0;
                fSqrDist = fA11+((tScalar)2.0)*fB1+fC;
              }
              else
              {
                fS = -fTmp/fA00;
                fSqrDist = fTmp*fS+fA11+((tScalar)2.0)*fB1+fC;
              }
            }
            else
            {
              fS = (tScalar)1.0;
              fTmp = fA01+fB1;
              if ( fTmp >= (tScalar)0.0 )
              {
                fT = (tScalar)0.0;
                fSqrDist = fA00+((tScalar)2.0)*fB0+fC;
              }
              else if ( -fTmp >= fA11 )
              {
                fT = (tScalar)1.0;
                fSqrDist = fA00+fA11+fC+
                  ((tScalar)2.0)*(fB0+fTmp);
              }
              else
              {
                fT = -fTmp/fA11;
                fSqrDist = fTmp*fT+fA00+((tScalar)2.0)*fB0+fC;
              }
            }
          }
        }
        else  // region 8 (corner)
        {
          if ( -fB0 < fA00 )
          {
            fT = (tScalar)0.0;
            if ( fB0 >= (tScalar)0.0 )
            {
              fS = (tScalar)0.0;
              fSqrDist = fC;
            }
            else
            {
              fS = -fB0/fA00;
              fSqrDist = fB0*fS+fC;
            }
          }
          else
          {
            fS = (tScalar)1.0;
            fTmp = fA01+fB1;
            if ( fTmp >= (tScalar)0.0 )
            {
              fT = (tScalar)0.0;
              fSqrDist = fA00+((tScalar)2.0)*fB0+fC;
            }
            else if ( -fTmp >= fA11 )
            {
              fT = (tScalar)1.0;
              fSqrDist = fA00+fA11+fC+((tScalar)2.0)*(fB0+fTmp);
            }
            else
            {
              fT = -fTmp/fA11;
              fSqrDist = fTmp*fT+fA00+((tScalar)2.0)*fB0+fC;
            }
          }
        }
      }
    }
    else 
    {
      if ( fT >= (tScalar)0.0 )
      {
        if ( fT <= fDet )  // region 5 (side)
        {
          fS = (tScalar)0.0;
          if ( fB1 >= (tScalar)0.0 )
          {
            fT = (tScalar)0.0;
            fSqrDist = fC;
          }
          else if ( -fB1 >= fA11 )
          {
            fT = (tScalar)1.0;
            fSqrDist = fA11+((tScalar)2.0)*fB1+fC;
          }
          else
          {
            fT = -fB1/fA11;
            fSqrDist = fB1*fT+fC;
          }
        }
        else  // region 4 (corner)
        {
          fTmp = fA01+fB0;
          if ( fTmp < (tScalar)0.0 )
          {
            fT = (tScalar)1.0;
            if ( -fTmp >= fA00 )
            {
              fS = (tScalar)1.0;
              fSqrDist = fA00+fA11+fC+((tScalar)2.0)*(fB1+fTmp);
            }
            else
            {
              fS = -fTmp/fA00;
              fSqrDist = fTmp*fS+fA11+((tScalar)2.0)*fB1+fC;
            }
          }
          else
          {
            fS = (tScalar)0.0;
            if ( fB1 >= (tScalar)0.0 )
            {
              fT = (tScalar)0.0;
              fSqrDist = fC;
            }
            else if ( -fB1 >= fA11 )
            {
              fT = (tScalar)1.0;
              fSqrDist = fA11+((tScalar)2.0)*fB1+fC;
            }
            else
            {
              fT = -fB1/fA11;
              fSqrDist = fB1*fT+fC;
            }
          }
        }
      }
      else   // region 6 (corner)
      {
        if ( fB0 < (tScalar)0.0 )
        {
          fT = (tScalar)0.0;
          if ( -fB0 >= fA00 )
          {
            fS = (tScalar)1.0;
            fSqrDist = fA00+((tScalar)2.0)*fB0+fC;
          }
          else
          {
            fS = -fB0/fA00;
            fSqrDist = fB0*fS+fC;
          }
        }
        else
        {
          fS = (tScalar)0.0;
          if ( fB1 >= (tScalar)0.0 )
          {
            fT = (tScalar)0.0;
            fSqrDist = fC;
          }
          else if ( -fB1 >= fA11 )
          {
            fT = (tScalar)1.0;
            fSqrDist = fA11+((tScalar)2.0)*fB1+fC;
          }
          else
          {
            fT = -fB1/fA11;
            fSqrDist = fB1*fT+fC;
          }
        }
      }
    }
  }
  else
  {
    // line segments are parallel
    if ( fA01 > (tScalar)0.0 )
    {
      // direction vectors form an obtuse angle
      if ( fB0 >= (tScalar)0.0 )
      {
        fS = (tScalar)0.0;
        fT = (tScalar)0.0;
        fSqrDist = fC;
      }
      else if ( -fB0 <= fA00 )
      {
        fS = -fB0/fA00;
        fT = (tScalar)0.0;
        fSqrDist = fB0*fS+fC;
      }
      else
      {
        fB1 = -Dot(kDiff, seg1.mDelta);
        fS = (tScalar)1.0;
        fTmp = fA00+fB0;
        if ( -fTmp >= fA01 )
        {
          fT = (tScalar)1.0;
          fSqrDist = fA00+fA11+fC+((tScalar)2.0)*(fA01+fB0+fB1);
        }
        else
        {
          fT = -fTmp/fA01;
          fSqrDist = fA00+((tScalar)2.0)*fB0+fC+fT*(fA11*fT+
                                                    ((tScalar)2.0)*(fA01+fB1));
        }
      }
    }
    else
    {
      // direction vectors form an acute angle
      if ( -fB0 >= fA00 )
      {
        fS = (tScalar)1.0;
        fT = (tScalar)0.0;
        fSqrDist = fA00+((tScalar)2.0)*fB0+fC;
      }
      else if ( fB0 <= (tScalar)0.0 )
      {
        fS = -fB0/fA00;
        fT = (tScalar)0.0;
        fSqrDist = fB0*fS+fC;
      }
      else
      {
        fB1 = -Dot(kDiff, seg1.mDelta);
        fS = (tScalar)0.0;
        if ( fB0 >= -fA01 )
        {
          fT = (tScalar)1.0;
          fSqrDist = fA11+((tScalar)2.0)*fB1+fC;
        }
        else
        {
          fT = -fB0/fA01;
          fSqrDist = fC+fT*(((tScalar)2.0)*fB1+fA11*fT);
        }
      }
    }
  }

  if ( t0 )
    *t0 = fS;

  if ( t1 )
    *t1 = fT;

  return Abs(fSqrDist);
}

//==============================================================
// PointRectangleDistance
//==============================================================
tScalar JigLib::PointRectangleDistanceSq(tScalar * pfSParam, tScalar * pfTParam, const tVector3 & rkPoint, const tRectangle & rkRct)
{
  tVector3 kDiff = rkRct.mOrigin - rkPoint;
  tScalar fA00 = rkRct.mEdge0.GetLengthSq();
  tScalar fA11 = rkRct.mEdge1.GetLengthSq();
  tScalar fB0 = Dot(kDiff, rkRct.mEdge0);
  tScalar fB1 = Dot(kDiff, rkRct.mEdge1);
  tScalar fS = -fB0, fT = -fB1;

  tScalar fSqrDist = kDiff.GetLengthSq();

  if ( fS < (tScalar)0.0 )
  {
    fS = (tScalar)0.0;
  }
  else if ( fS <= fA00 )
  {
    fS /= fA00;
    fSqrDist += fB0*fS;
  }
  else
  {
    fS = (tScalar)1.0;
    fSqrDist += fA00 + ((tScalar)2.0)*fB0;
  }

  if ( fT < (tScalar)0.0 )
  {
    fT = (tScalar)0.0;
  }
  else if ( fT <= fA11 )
  {
    fT /= fA11;
    fSqrDist += fB1*fT;
  }
  else
  {
    fT = (tScalar)1.0;
    fSqrDist += fA11 + ((tScalar)2.0)*fB1;
  }

  if ( pfSParam )
    *pfSParam = fS;

  if ( pfTParam )
    *pfTParam = fT;

  return Abs(fSqrDist);

}


//==============================================================
// SegmentRectDistanceSq
//==============================================================
tScalar JigLib::SegmentRectDistanceSq(tScalar* pfSegP, tScalar* pfRctP0, tScalar* pfRctP1,
                                      const tSegment & seg, const tRectangle & rect)
{
  tVector3 kDiff = rect.mOrigin - seg.mOrigin;
  tScalar fA00 = seg.mDelta.GetLengthSq();
  tScalar fA01 = -Dot(seg.mDelta, rect.mEdge0);
  tScalar fA02 = -Dot(seg.mDelta, rect.mEdge1);
  tScalar fA11 = rect.mEdge0.GetLengthSq();
  tScalar fA22 = rect.mEdge1.GetLengthSq();
  tScalar fB0  = -Dot(kDiff, seg.mDelta);
  tScalar fB1  = Dot(kDiff, rect.mEdge0);
  tScalar fB2  = Dot(kDiff, rect.mEdge1);
  tScalar fCof00 = fA11*fA22;
  tScalar fCof01 = -fA01*fA22;
  tScalar fCof02 = -fA02*fA11;
  tScalar fDet = fA00*fCof00+fA01*fCof01+fA02*fCof02;

  tSegment kSegPgm;
  tVector3 kPt;
  tScalar fSqrDist, fSqrDist0, fR, fS, fT, fR0, fS0, fT0;

  if ( Abs(fDet) >= SCALAR_TINY )
  {
    tScalar fCof11 = fA00*fA22-fA02*fA02;
    tScalar fCof12 = fA02*fA01;
    tScalar fCof22 = fA00*fA11-fA01*fA01;
    tScalar fInvDet = ((tScalar)1.0)/fDet;
    tScalar fRhs0 = -fB0*fInvDet;
    tScalar fRhs1 = -fB1*fInvDet;
    tScalar fRhs2 = -fB2*fInvDet;

    fR = fCof00*fRhs0+fCof01*fRhs1+fCof02*fRhs2;
    fS = fCof01*fRhs0+fCof11*fRhs1+fCof12*fRhs2;
    fT = fCof02*fRhs0+fCof12*fRhs1+fCof22*fRhs2;

    if ( fR < (tScalar)0.0 )
    {
      if ( fS < (tScalar)0.0 )
      {
        if ( fT < (tScalar)0.0 )  // region 6m
        {
          // min on face s=0 or t=0 or r=0
          kSegPgm.mOrigin = rect.mOrigin;
          kSegPgm.mDelta = rect.mEdge1;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //          fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fT);
          fS = (tScalar)0.0;
          kSegPgm.mOrigin = rect.mOrigin;
          kSegPgm.mDelta = rect.mEdge0;
          fSqrDist0 = SegmentSegmentDistanceSq(&fR0, &fS0, seg, kSegPgm);
          //          fSqrDist0 = SqrDistance(seg,kSegPgm,&fR0,&fS0);
          fT0 = (tScalar)0.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
          fSqrDist0 = PointRectangleDistanceSq(&fS0, &fT0, seg.mOrigin, rect);
          //          fSqrDist0 = SqrDistance(seg.mOrigin,rkRct,&fS0,&fT0);
          fR0 = (tScalar)0.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
        else if ( fT <= (tScalar)1.0 )  // region 5m
        {
          // min on face s=0 or r=0
          kSegPgm.mOrigin = rect.mOrigin;
          kSegPgm.mDelta = rect.mEdge1;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //          fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fT);
          fS = (tScalar)0.0;
          fSqrDist0 = PointRectangleDistanceSq(&fS0, &fT0, seg.mOrigin, rect);
          //          fSqrDist0 = SqrDistance(seg.mOrigin,rkRct,&fS0,&fT0);
          fR0 = (tScalar)0.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
        else  // region 4m
        {
          // min on face s=0 or t=1 or r=0
          kSegPgm.mOrigin = rect.mOrigin;
          kSegPgm.mDelta = rect.mEdge1;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //          fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fT);
          fS = (tScalar)0.0;
          kSegPgm.mOrigin = rect.mOrigin+rect.mEdge1;
          kSegPgm.mDelta = rect.mEdge0;
          fSqrDist0 = SegmentSegmentDistanceSq(&fR0, &fS0, seg, kSegPgm);
          //          fSqrDist0 = SqrDistance(seg,kSegPgm,&fR0,&fS0);
          fT0 = (tScalar)1.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
          fSqrDist0 = PointRectangleDistanceSq(&fS0, &fT0, seg.mOrigin, rect);
          //          fSqrDist0 = SqrDistance(seg.mOrigin,rkRct,&fS0,&fT0);
          fR0 = (tScalar)0.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
      }
      else if ( fS <= (tScalar)1.0 )
      {
        if ( fT < (tScalar)0.0 )  // region 7m
        {
          // min on face t=0 or r=0
          kSegPgm.mOrigin = rect.mOrigin;
          kSegPgm.mDelta = rect.mEdge0;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //          fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fS);
          fT = (tScalar)0.0;
          fSqrDist0 = PointRectangleDistanceSq(&fS0, &fT0, seg.mOrigin, rect);
          //          fSqrDist0 = SqrDistance(seg.mOrigin,rkRct,&fS0,&fT0);
          fR0 = (tScalar)0.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
        else if ( fT <= (tScalar)1.0 )  // region 0m
        {
          // min on face r=0
          fSqrDist = PointRectangleDistanceSq(&fS, &fT, seg.mOrigin, rect);
          //          fSqrDist = SqrDistance(seg.mOrigin,rkRct,&fS,&fT);
          fR = (tScalar)0.0;
        }
        else  // region 3m
        {
          // min on face t=1 or r=0
          kSegPgm.mOrigin = rect.mOrigin+rect.mEdge1;
          kSegPgm.mDelta = rect.mEdge0;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //          fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fS);
          fT = (tScalar)1.0;
          fSqrDist0 = PointRectangleDistanceSq(&fS0, &fT0, seg.mOrigin, rect);
          //          fSqrDist0 = SqrDistance(seg.mOrigin,rkRct,&fS0,&fT0);
          fR0 = (tScalar)0.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
      }
      else
      {
        if ( fT < (tScalar)0.0 )  // region 8m
        {
          // min on face s=1 or t=0 or r=0
          kSegPgm.mOrigin = rect.mOrigin+rect.mEdge0;
          kSegPgm.mDelta = rect.mEdge1;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //          fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fT);
          fS = (tScalar)1.0;
          kSegPgm.mOrigin = rect.mOrigin;
          kSegPgm.mDelta = rect.mEdge0;
          fSqrDist0 = SegmentSegmentDistanceSq(&fR0, &fS0, seg, kSegPgm);
          //          fSqrDist0 = SqrDistance(seg,kSegPgm,&fR0,&fS0);
          fT0 = (tScalar)0.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
          fSqrDist0 = PointRectangleDistanceSq(&fS0, &fT0, seg.mOrigin, rect);
          //          fSqrDist0 = SqrDistance(seg.mOrigin,rkRct,&fS0,&fT0);
          fR0 = (tScalar)0.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
        else if ( fT <= (tScalar)1.0 )  // region 1m
        {
          // min on face s=1 or r=0
          kSegPgm.mOrigin = rect.mOrigin+rect.mEdge0;
          kSegPgm.mDelta = rect.mEdge1;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //          fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fT);
          fS = (tScalar)1.0;
          fSqrDist0 = PointRectangleDistanceSq(&fS0, &fT0, seg.mOrigin, rect);
          //          fSqrDist0 = SqrDistance(seg.mOrigin,rkRct,&fS0,&fT0);
          fR0 = (tScalar)0.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
        else  // region 2m
        {
          // min on face s=1 or t=1 or r=0
          kSegPgm.mOrigin = rect.mOrigin+rect.mEdge0;
          kSegPgm.mDelta = rect.mEdge1;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //          fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fT);
          fS = (tScalar)1.0;
          kSegPgm.mOrigin = rect.mOrigin+rect.mEdge1;
          kSegPgm.mDelta = rect.mEdge0;
          fSqrDist0 = SegmentSegmentDistanceSq(&fR0, &fS0, seg, kSegPgm);
          //          fSqrDist0 = SqrDistance(seg,kSegPgm,&fR0,&fS0);
          fT0 = (tScalar)1.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
          fSqrDist0 = PointRectangleDistanceSq(&fS0, &fT0, seg.mOrigin, rect);
          //          fSqrDist0 = SqrDistance(seg.mOrigin,rkRct,&fS0,&fT0);
          fR0 = (tScalar)0.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
      }
    }
    else if ( fR <= (tScalar)1.0 )
    {
      if ( fS < (tScalar)0.0 )
      {
        if ( fT < (tScalar)0.0 )  // region 6
        {
          // min on face s=0 or t=0
          kSegPgm.mOrigin = rect.mOrigin;
          kSegPgm.mDelta = rect.mEdge1;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //              fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fT);
          fS = (tScalar)0.0;
          kSegPgm.mOrigin = rect.mOrigin;
          kSegPgm.mDelta = rect.mEdge0;
          fSqrDist0 = SegmentSegmentDistanceSq(&fR0, &fS0, seg, kSegPgm);
          //              fSqrDist0 = SqrDistance(seg,kSegPgm,&fR0,&fS0);
          fT0 = (tScalar)0.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
        else if ( fT <= (tScalar)1.0 )  // region 5
        {
          // min on face s=0
          kSegPgm.mOrigin = rect.mOrigin;
          kSegPgm.mDelta = rect.mEdge1;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //              fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fT);
          fS = (tScalar)0.0;
        }
        else // region 4
        {
          // min on face s=0 or t=1
          kSegPgm.mOrigin = rect.mOrigin;
          kSegPgm.mDelta = rect.mEdge1;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //              fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fT);
          fS = (tScalar)0.0;
          kSegPgm.mOrigin = rect.mOrigin+rect.mEdge1;
          kSegPgm.mDelta = rect.mEdge0;
          fSqrDist0 = SegmentSegmentDistanceSq(&fR0, &fS0, seg, kSegPgm);
          //              fSqrDist0 = SqrDistance(seg,kSegPgm,&fR0,&fS0);
          fT0 = (tScalar)1.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
      }
      else if ( fS <= (tScalar)1.0 )
      {
        if ( fT < (tScalar)0.0 )  // region 7
        {
          // min on face t=0
          kSegPgm.mOrigin = rect.mOrigin;
          kSegPgm.mDelta = rect.mEdge0;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fS, seg, kSegPgm);
          //              fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fS);
          fT = (tScalar)0.0;
        }
        else if ( fT <= (tScalar)1.0 )  // region 0
        {
          // global minimum is interior
          fSqrDist = fR*(fA00*fR+fA01*fS+fA02*fT+((tScalar)2.0)*fB0)
            +fS*(fA01*fR+fA11*fS+((tScalar)2.0)*fB1)
            +fT*(fA02*fR+fA22*fT+((tScalar)2.0)*fB2)
            +kDiff.GetLengthSq();
        }
        else  // region 3
        {
          // min on face t=1
          kSegPgm.mOrigin = rect.mOrigin+rect.mEdge1;
          kSegPgm.mDelta = rect.mEdge0;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fS, seg, kSegPgm);
          //              fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fS);
          fT = (tScalar)1.0;
        }
      }
      else
      {
        if ( fT < 0.0 )  // region 8
        {
          // min on face s=1 or t=0
          kSegPgm.mOrigin = rect.mOrigin+rect.mEdge0;
          kSegPgm.mDelta = rect.mEdge1;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //              fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fT);
          fS = (tScalar)1.0;
          kSegPgm.mOrigin = rect.mOrigin;
          kSegPgm.mDelta = rect.mEdge0;
          fSqrDist0 = SegmentSegmentDistanceSq(&fR0, &fS0, seg, kSegPgm);
          //              fSqrDist0 = SqrDistance(seg,kSegPgm,&fR0,&fS0);
          fT0 = (tScalar)0.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
        else if ( fT <= (tScalar)1.0 )  // region 1
        {
          // min on face s=1
          kSegPgm.mOrigin = rect.mOrigin+rect.mEdge0;
          kSegPgm.mDelta = rect.mEdge1;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //              fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fT);
          fS = (tScalar)1.0;
        }
        else  // region 2
        {
          // min on face s=1 or t=1
          kSegPgm.mOrigin = rect.mOrigin+rect.mEdge0;
          kSegPgm.mDelta = rect.mEdge1;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //              fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fT);
          fS = (tScalar)1.0;
          kSegPgm.mOrigin = rect.mOrigin+rect.mEdge1;
          kSegPgm.mDelta = rect.mEdge0;
          fSqrDist0 = SegmentSegmentDistanceSq(&fR0, &fS0, seg, kSegPgm);
          //              fSqrDist0 = SqrDistance(seg,kSegPgm,&fR0,&fS0);
          fT0 = (tScalar)1.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
      }
    }
    else
    {
      if ( fS < (tScalar)0.0 )
      {
        if ( fT < (tScalar)0.0 )  // region 6p
        {
          // min on face s=0 or t=0 or r=1
          kSegPgm.mOrigin = rect.mOrigin;
          kSegPgm.mDelta = rect.mEdge1;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //              fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fT);
          fS = (tScalar)0.0;
          kSegPgm.mOrigin = rect.mOrigin;
          kSegPgm.mDelta = rect.mEdge0;
          fSqrDist0 = SegmentSegmentDistanceSq(&fR0, &fS0, seg, kSegPgm);
          //              fSqrDist0 = SqrDistance(seg,kSegPgm,&fR0,&fS0);
          fT0 = (tScalar)0.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
          kPt = seg.mOrigin + seg.mDelta;
          fSqrDist0 = PointRectangleDistanceSq(&fS0, &fT0, kPt, rect);
          //              fSqrDist0 = SqrDistance(kPt,rkRct,&fS0,&fT0);
          fR0 = (tScalar)1.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
        else if ( fT <= (tScalar)1.0 )  // region 5p
        {
          // min on face s=0 or r=1
          kSegPgm.mOrigin = rect.mOrigin;
          kSegPgm.mDelta = rect.mEdge1;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //              fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fT);
          fS = (tScalar)0.0;
          fSqrDist0 = PointRectangleDistanceSq(&fS0, &fT0, seg.mOrigin, rect);
          //              fSqrDist0 = SqrDistance(seg.mOrigin,rkRct,&fS0,&fT0);
          kPt = seg.mOrigin+seg.mDelta;
          fSqrDist0 = PointRectangleDistanceSq(&fS0, &fT0, kPt, rect);
          //              fSqrDist0 = SqrDistance(kPt,rkRct,&fS0,&fT0);
          fR0 = (tScalar)1.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
        else  // region 4p
        {
          // min on face s=0 or t=1 or r=1
          kSegPgm.mOrigin = rect.mOrigin;
          kSegPgm.mDelta = rect.mEdge1;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //              fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fT);
          fS = (tScalar)0.0;
          kSegPgm.mOrigin = rect.mOrigin+rect.mEdge1;
          kSegPgm.mDelta = rect.mEdge0;
          fSqrDist0 = SegmentSegmentDistanceSq(&fR0, &fS0, seg, kSegPgm);
          //              fSqrDist0 = SqrDistance(seg,kSegPgm,&fR0,&fS0);
          fT0 = (tScalar)1.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
          kPt = seg.mOrigin+seg.mDelta;
          fSqrDist = PointRectangleDistanceSq(&fS0, &fT0, kPt, rect);
          //              fSqrDist0 = SqrDistance(kPt,rkRct,&fS0,&fT0);
          fR0 = (tScalar)1.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
      }
      else if ( fS <= (tScalar)1.0 )
      {
        if ( fT < (tScalar)0.0 )  // region 7p
        {
          // min on face t=0 or r=1
          kSegPgm.mOrigin = rect.mOrigin;
          kSegPgm.mDelta = rect.mEdge0;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fS, seg, kSegPgm);
          //              fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fS);
          fT = (tScalar)0.0;
          kPt = seg.mOrigin+seg.mDelta;
          fSqrDist0 = PointRectangleDistanceSq(&fS0, &fT0, kPt, rect);
          //              fSqrDist0 = SqrDistance(kPt,rkRct,&fS0,&fT0);
          fR0 = (tScalar)1.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
        else if ( fT <= (tScalar)1.0 )  // region 0p
        {
          // min on face r=1
          kPt = seg.mOrigin+seg.mDelta;
          fSqrDist = PointRectangleDistanceSq(&fS, &fT, kPt, rect);
          //              fSqrDist = SqrDistance(kPt,rkRct,&fS,&fT);
          fR = (tScalar)1.0;
        }
        else  // region 3p
        {
          // min on face t=1 or r=1
          kSegPgm.mOrigin = rect.mOrigin+rect.mEdge1;
          kSegPgm.mDelta = rect.mEdge0;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fS, seg, kSegPgm);
          //              fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fS);
          fT = (tScalar)1.0;
          kPt = seg.mOrigin+seg.mDelta;
          fSqrDist0 = PointRectangleDistanceSq(&fS0, &fT0, kPt, rect);
          //              fSqrDist0 = SqrDistance(kPt,rkRct,&fS0,&fT0);
          fR0 = (tScalar)1.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
      }
      else
      {
        if ( fT < (tScalar)0.0 )  // region 8p
        {
          // min on face s=1 or t=0 or r=1
          kSegPgm.mOrigin = rect.mOrigin+rect.mEdge0;
          kSegPgm.mDelta = rect.mEdge1;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //              fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fT);
          fS = (tScalar)1.0;
          kSegPgm.mOrigin = rect.mOrigin;
          kSegPgm.mDelta = rect.mEdge0;
          fSqrDist0 = SegmentSegmentDistanceSq(&fR0, &fS0, seg, kSegPgm);
          //              fSqrDist0 = SqrDistance(seg,kSegPgm,&fR0,&fS0);
          fT0 = (tScalar)0.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
          kPt = seg.mOrigin+seg.mDelta;
          fSqrDist0 = PointRectangleDistanceSq(&fS0, &fT0, kPt, rect);
          //              fSqrDist0 = SqrDistance(kPt,rkRct,&fS0,&fT0);
          fR0 = (tScalar)1.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
        else if ( fT <= (tScalar)1.0 )  // region 1p
        {
          // min on face s=1 or r=1
          kSegPgm.mOrigin = rect.mOrigin+rect.mEdge0;
          kSegPgm.mDelta = rect.mEdge1;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //              fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fT);
          fS = (tScalar)1.0;
          kPt = seg.mOrigin+seg.mDelta;
          fSqrDist0 = PointRectangleDistanceSq(&fS0, &fT0, kPt, rect);
          //              fSqrDist0 = SqrDistance(kPt,rkRct,&fS0,&fT0);
          fR0 = (tScalar)1.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
        else  // region 2p
        {
          // min on face s=1 or t=1 or r=1
          kSegPgm.mOrigin = rect.mOrigin+rect.mEdge0;
          kSegPgm.mDelta = rect.mEdge1;
          fSqrDist = SegmentSegmentDistanceSq(&fR, &fT, seg, kSegPgm);
          //              fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fT);
          fS = (tScalar)1.0;
          kSegPgm.mOrigin = rect.mOrigin+rect.mEdge1;
          kSegPgm.mDelta = rect.mEdge0;
          fSqrDist0 = SegmentSegmentDistanceSq(&fR0, &fS0, seg, kSegPgm);
          //              fSqrDist0 = SqrDistance(seg,kSegPgm,&fR0,&fS0);
          fT0 = (tScalar)1.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
          kPt = seg.mOrigin+seg.mDelta;
          fSqrDist0 = PointRectangleDistanceSq(&fS0, &fT, kPt, rect);
          //              fSqrDist0 = SqrDistance(kPt,rkRct,&fS0,&fT0);
          fR0 = (tScalar)1.0;
          if ( fSqrDist0 < fSqrDist )
          {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
          }
        }
      }
    }
  }
  else
  {
    // segment and rectangle are parallel
    kSegPgm.mOrigin = rect.mOrigin;
    kSegPgm.mDelta = rect.mEdge0;
    fSqrDist = SegmentSegmentDistanceSq(&fR, &fS, seg, kSegPgm);
    //      fSqrDist = SqrDistance(seg,kSegPgm,&fR,&fS);
    fT = (tScalar)0.0;

    kSegPgm.mDelta = rect.mEdge1;
    fSqrDist0 = SegmentSegmentDistanceSq(&fR0, &fT0, seg, kSegPgm);
    //      fSqrDist0 = SqrDistance(seg,kSegPgm,&fR0,&fT0);
    fS0 = (tScalar)0.0;
    if ( fSqrDist0 < fSqrDist )
    {
      fSqrDist = fSqrDist0;
      fR = fR0;
      fS = fS0;
      fT = fT0;
    }

    kSegPgm.mOrigin = rect.mOrigin+rect.mEdge1;
    kSegPgm.mDelta = rect.mEdge0;
    fSqrDist0 = SegmentSegmentDistanceSq(&fR0, &fS0, seg, kSegPgm);
    //      fSqrDist0 = SqrDistance(seg,kSegPgm,&fR0,&fS0);
    fT0 = (tScalar)1.0;
    if ( fSqrDist0 < fSqrDist )
    {
      fSqrDist = fSqrDist0;
      fR = fR0;
      fS = fS0;
      fT = fT0;
    }

    kSegPgm.mOrigin = rect.mOrigin+rect.mEdge0;
    kSegPgm.mDelta = rect.mEdge1;
    fSqrDist0 = SegmentSegmentDistanceSq(&fR0, &fT0, seg, kSegPgm);
    //      fSqrDist0 = SqrDistance(seg,kSegPgm,&fR0,&fT0);
    fS0 = (tScalar)1.0;
    if ( fSqrDist0 < fSqrDist )
    {
      fSqrDist = fSqrDist0;
      fR = fR0;
      fS = fS0;
      fT = fT0;
    }

    fSqrDist0 = PointRectangleDistanceSq(&fS0, &fT0, seg.mOrigin, rect);
    //      fSqrDist0 = SqrDistance(seg.mOrigin,rkRct,&fS0,&fT0);
    fR0 = (tScalar)0.0;
    if ( fSqrDist0 < fSqrDist )
    {
      fSqrDist = fSqrDist0;
      fR = fR0;
      fS = fS0;
      fT = fT0;
    }

    kPt = seg.mOrigin+seg.mDelta;
    fSqrDist0 = PointRectangleDistanceSq(&fS0, &fT0, kPt, rect);
    //      fSqrDist0 = SqrDistance(kPt,rkRct,&fS0,&fT0);
    fR0 = (tScalar)1.0;
    if ( fSqrDist0 < fSqrDist )
    {
      fSqrDist = fSqrDist0;
      fR = fR0;
      fS = fS0;
      fT = fT0;
    }
  }

  if ( pfSegP )
    *pfSegP = fR;

  if ( pfRctP0 )
    *pfRctP0 = fS;

  if ( pfRctP1 )
    *pfRctP1 = fT;

  return Abs(fSqrDist);
}

//========================================================
// SqrDistance 
//========================================================
tScalar SqrDistance (const tVector3& rkPoint, const tBox& rkBox,
                     tScalar* pfBParam0, tScalar* pfBParam1, tScalar* pfBParam2)
{
  // compute coordinates of point in box coordinate system
  tVector3 kDiff = rkPoint - rkBox.GetCentre();
  tVector3 kClosest(Dot(kDiff, rkBox.GetOrient()[0]), 
                    Dot(kDiff, rkBox.GetOrient()[1]),
                    Dot(kDiff, rkBox.GetOrient()[2]));

  // project test point onto box
  tScalar fSqrDistance = (tScalar)0.0;
  tScalar fDelta;

  if ( kClosest.x < -rkBox.GetHalfSideLengths()(0) )
  {
    fDelta = kClosest.x + rkBox.GetHalfSideLengths()(0);
    fSqrDistance += fDelta*fDelta;
    kClosest.x = -rkBox.GetHalfSideLengths()(0);
  }
  else if ( kClosest.x > rkBox.GetHalfSideLengths()(0) )
  {
    fDelta = kClosest.x - rkBox.GetHalfSideLengths()(0);
    fSqrDistance += fDelta*fDelta;
    kClosest.x = rkBox.GetHalfSideLengths()(0);
  }

  if ( kClosest.y < -rkBox.GetHalfSideLengths()(1) )
  {
    fDelta = kClosest.y + rkBox.GetHalfSideLengths()(1);
    fSqrDistance += fDelta*fDelta;
    kClosest.y = -rkBox.GetHalfSideLengths()(1);
  }
  else if ( kClosest.y > rkBox.GetHalfSideLengths()(1) )
  {
    fDelta = kClosest.y - rkBox.GetHalfSideLengths()(1);
    fSqrDistance += fDelta*fDelta;
    kClosest.y = rkBox.GetHalfSideLengths()(1);
  }

  if ( kClosest.z < -rkBox.GetHalfSideLengths()(2) )
  {
    fDelta = kClosest.z + rkBox.GetHalfSideLengths()(2);
    fSqrDistance += fDelta*fDelta;
    kClosest.z = -rkBox.GetHalfSideLengths()(2);
  }
  else if ( kClosest.z > rkBox.GetHalfSideLengths()(2) )
  {
    fDelta = kClosest.z - rkBox.GetHalfSideLengths()(2);
    fSqrDistance += fDelta*fDelta;
    kClosest.z = rkBox.GetHalfSideLengths()(2);
  }

  if ( pfBParam0 )
    *pfBParam0 = kClosest.x;

  if ( pfBParam1 )
    *pfBParam1 = kClosest.y;

  if ( pfBParam2 )
    *pfBParam2 = kClosest.z;

  return Max(fSqrDistance, 0.0f);
}

//========================================================
// Face 
//========================================================
static void Face(int i0, int i1, int i2, tVector3& rkPnt,
                 const tVector3& rkDir, const tBox& rkBox,
                 const tVector3& rkPmE, tScalar* pfLParam, tScalar& rfSqrDistance)
{
  tVector3 kPpE;
  tScalar fLSqr, fInv, fTmp, fParam, fT, fDelta;

  kPpE[i1] = rkPnt[i1] + rkBox.GetHalfSideLengths()(i1);
  kPpE[i2] = rkPnt[i2] + rkBox.GetHalfSideLengths()(i2);
  if ( rkDir[i0]*kPpE[i1] >= rkDir[i1]*rkPmE[i0] )
  {
    if ( rkDir[i0]*kPpE[i2] >= rkDir[i2]*rkPmE[i0] )
    {
      // v[i1] >= -e[i1], v[i2] >= -e[i2] (distance = 0)
      if ( pfLParam )
      {
        rkPnt[i0] = rkBox.GetHalfSideLengths()(i0);
        fInv = ((tScalar)1.0)/rkDir[i0];
        rkPnt[i1] -= rkDir[i1]*rkPmE[i0]*fInv;
        rkPnt[i2] -= rkDir[i2]*rkPmE[i0]*fInv;
        *pfLParam = -rkPmE[i0]*fInv;
      }
    }
    else
    {
      // v[i1] >= -e[i1], v[i2] < -e[i2]
      fLSqr = rkDir[i0]*rkDir[i0] + rkDir[i2]*rkDir[i2];
      fTmp = fLSqr*kPpE[i1] - rkDir[i1]*(rkDir[i0]*rkPmE[i0] +
                                         rkDir[i2]*kPpE[i2]);
      if ( fTmp <= ((tScalar)2.0)*fLSqr*rkBox.GetHalfSideLengths()(i1) )
      {
        fT = fTmp/fLSqr;
        fLSqr += rkDir[i1]*rkDir[i1];
        fTmp = kPpE[i1] - fT;
        fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*fTmp +
          rkDir[i2]*kPpE[i2];
        fParam = -fDelta/fLSqr;
        rfSqrDistance += rkPmE[i0]*rkPmE[i0] + fTmp*fTmp +
          kPpE[i2]*kPpE[i2] + fDelta*fParam;

        if ( pfLParam )
        {
          *pfLParam = fParam;
          rkPnt[i0] = rkBox.GetHalfSideLengths()(i0);
          rkPnt[i1] = fT - rkBox.GetHalfSideLengths()(i1);
          rkPnt[i2] = -rkBox.GetHalfSideLengths()(i2);
        }
      }
      else
      {
        fLSqr += rkDir[i1]*rkDir[i1];
        fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*rkPmE[i1] +
          rkDir[i2]*kPpE[i2];
        fParam = -fDelta/fLSqr;
        rfSqrDistance += rkPmE[i0]*rkPmE[i0] + rkPmE[i1]*rkPmE[i1] +
          kPpE[i2]*kPpE[i2] + fDelta*fParam;

        if ( pfLParam )
        {
          *pfLParam = fParam;
          rkPnt[i0] = rkBox.GetHalfSideLengths()(i0);
          rkPnt[i1] = rkBox.GetHalfSideLengths()(i1);
          rkPnt[i2] = -rkBox.GetHalfSideLengths()(i2);
        }
      }
    }
  }
  else
  {
    if ( rkDir[i0]*kPpE[i2] >= rkDir[i2]*rkPmE[i0] )
    {
      // v[i1] < -e[i1], v[i2] >= -e[i2]
      fLSqr = rkDir[i0]*rkDir[i0] + rkDir[i1]*rkDir[i1];
      fTmp = fLSqr*kPpE[i2] - rkDir[i2]*(rkDir[i0]*rkPmE[i0] +
                                         rkDir[i1]*kPpE[i1]);
      if ( fTmp <= ((tScalar)2.0)*fLSqr*rkBox.GetHalfSideLengths()(i2) )
      {
        fT = fTmp/fLSqr;
        fLSqr += rkDir[i2]*rkDir[i2];
        fTmp = kPpE[i2] - fT;
        fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*kPpE[i1] +
          rkDir[i2]*fTmp;
        fParam = -fDelta/fLSqr;
        rfSqrDistance += rkPmE[i0]*rkPmE[i0] + kPpE[i1]*kPpE[i1] +
          fTmp*fTmp + fDelta*fParam;

        if ( pfLParam )
        {
          *pfLParam = fParam;
          rkPnt[i0] = rkBox.GetHalfSideLengths()(i0);
          rkPnt[i1] = -rkBox.GetHalfSideLengths()(i1);
          rkPnt[i2] = fT - rkBox.GetHalfSideLengths()(i2);
        }
      }
      else
      {
        fLSqr += rkDir[i2]*rkDir[i2];
        fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*kPpE[i1] +
          rkDir[i2]*rkPmE[i2];
        fParam = -fDelta/fLSqr;
        rfSqrDistance += rkPmE[i0]*rkPmE[i0] + kPpE[i1]*kPpE[i1] +
          rkPmE[i2]*rkPmE[i2] + fDelta*fParam;

        if ( pfLParam )
        {
          *pfLParam = fParam;
          rkPnt[i0] = rkBox.GetHalfSideLengths()(i0);
          rkPnt[i1] = -rkBox.GetHalfSideLengths()(i1);
          rkPnt[i2] = rkBox.GetHalfSideLengths()(i2);
        }
      }
    }
    else
    {
      // v[i1] < -e[i1], v[i2] < -e[i2]
      fLSqr = rkDir[i0]*rkDir[i0]+rkDir[i2]*rkDir[i2];
      fTmp = fLSqr*kPpE[i1] - rkDir[i1]*(rkDir[i0]*rkPmE[i0] +
                                         rkDir[i2]*kPpE[i2]);
      if ( fTmp >= (tScalar)0.0 )
      {
        // v[i1]-edge is closest
        if ( fTmp <= ((tScalar)2.0)*fLSqr*rkBox.GetHalfSideLengths()(i1) )
        {
          fT = fTmp/fLSqr;
          fLSqr += rkDir[i1]*rkDir[i1];
          fTmp = kPpE[i1] - fT;
          fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*fTmp +
            rkDir[i2]*kPpE[i2];
          fParam = -fDelta/fLSqr;
          rfSqrDistance += rkPmE[i0]*rkPmE[i0] + fTmp*fTmp +
            kPpE[i2]*kPpE[i2] + fDelta*fParam;

          if ( pfLParam )
          {
            *pfLParam = fParam;
            rkPnt[i0] = rkBox.GetHalfSideLengths()(i0);
            rkPnt[i1] = fT - rkBox.GetHalfSideLengths()(i1);
            rkPnt[i2] = -rkBox.GetHalfSideLengths()(i2);
          }
        }
        else
        {
          fLSqr += rkDir[i1]*rkDir[i1];
          fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*rkPmE[i1] +
            rkDir[i2]*kPpE[i2];
          fParam = -fDelta/fLSqr;
          rfSqrDistance += rkPmE[i0]*rkPmE[i0] + rkPmE[i1]*rkPmE[i1]
            + kPpE[i2]*kPpE[i2] + fDelta*fParam;

          if ( pfLParam )
          {
            *pfLParam = fParam;
            rkPnt[i0] = rkBox.GetHalfSideLengths()(i0);
            rkPnt[i1] = rkBox.GetHalfSideLengths()(i1);
            rkPnt[i2] = -rkBox.GetHalfSideLengths()(i2);
          }
        }
        return;
      }

      fLSqr = rkDir[i0]*rkDir[i0] + rkDir[i1]*rkDir[i1];
      fTmp = fLSqr*kPpE[i2] - rkDir[i2]*(rkDir[i0]*rkPmE[i0] +
                                         rkDir[i1]*kPpE[i1]);
      if ( fTmp >= (tScalar)0.0 )
      {
        // v[i2]-edge is closest
        if ( fTmp <= ((tScalar)2.0)*fLSqr*rkBox.GetHalfSideLengths()(i2) )
        {
          fT = fTmp/fLSqr;
          fLSqr += rkDir[i2]*rkDir[i2];
          fTmp = kPpE[i2] - fT;
          fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*kPpE[i1] +
            rkDir[i2]*fTmp;
          fParam = -fDelta/fLSqr;
          rfSqrDistance += rkPmE[i0]*rkPmE[i0] + kPpE[i1]*kPpE[i1] +
            fTmp*fTmp + fDelta*fParam;

          if ( pfLParam )
          {
            *pfLParam = fParam;
            rkPnt[i0] = rkBox.GetHalfSideLengths()(i0);
            rkPnt[i1] = -rkBox.GetHalfSideLengths()(i1);
            rkPnt[i2] = fT - rkBox.GetHalfSideLengths()(i2);
          }
        }
        else
        {
          fLSqr += rkDir[i2]*rkDir[i2];
          fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*kPpE[i1] +
            rkDir[i2]*rkPmE[i2];
          fParam = -fDelta/fLSqr;
          rfSqrDistance += rkPmE[i0]*rkPmE[i0] + kPpE[i1]*kPpE[i1] +
            rkPmE[i2]*rkPmE[i2] + fDelta*fParam;

          if ( pfLParam )
          {
            *pfLParam = fParam;
            rkPnt[i0] = rkBox.GetHalfSideLengths()(i0);
            rkPnt[i1] = -rkBox.GetHalfSideLengths()(i1);
            rkPnt[i2] = rkBox.GetHalfSideLengths()(i2);
          }
        }
        return;
      }

      // (v[i1],v[i2])-corner is closest
      fLSqr += rkDir[i2]*rkDir[i2];
      fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*kPpE[i1] +
        rkDir[i2]*kPpE[i2];
      fParam = -fDelta/fLSqr;
      rfSqrDistance += rkPmE[i0]*rkPmE[i0] + kPpE[i1]*kPpE[i1] +
        kPpE[i2]*kPpE[i2] + fDelta*fParam;

      if ( pfLParam )
      {
        *pfLParam = fParam;
        rkPnt[i0] = rkBox.GetHalfSideLengths()(i0);
        rkPnt[i1] = -rkBox.GetHalfSideLengths()(i1);
        rkPnt[i2] = -rkBox.GetHalfSideLengths()(i2);
      }
    }
  }
}
//----------------------------------------------------------------------------
static void CaseNoZeros (tVector3& rkPnt, const tVector3& rkDir,
                         const tBox& rkBox, tScalar* pfLParam, tScalar& rfSqrDistance)
{
  tVector3 kPmE(
    rkPnt.x - rkBox.GetHalfSideLengths()(0),
    rkPnt.y - rkBox.GetHalfSideLengths()(1),
    rkPnt.z - rkBox.GetHalfSideLengths()(2));

  tScalar fProdDxPy = rkDir.x*kPmE.y;
  tScalar fProdDyPx = rkDir.y*kPmE.x;
  tScalar fProdDzPx, fProdDxPz, fProdDzPy, fProdDyPz;

  if ( fProdDyPx >= fProdDxPy )
  {
    fProdDzPx = rkDir.z*kPmE.x;
    fProdDxPz = rkDir.x*kPmE.z;
    if ( fProdDzPx >= fProdDxPz )
    {
      // line intersects x = e0
      Face(0,1,2,rkPnt,rkDir,rkBox,kPmE,pfLParam,rfSqrDistance);
    }
    else
    {
      // line intersects z = e2
      Face(2,0,1,rkPnt,rkDir,rkBox,kPmE,pfLParam,rfSqrDistance);
    }
  }
  else
  {
    fProdDzPy = rkDir.z*kPmE.y;
    fProdDyPz = rkDir.y*kPmE.z;
    if ( fProdDzPy >= fProdDyPz )
    {
      // line intersects y = e1
      Face(1,2,0,rkPnt,rkDir,rkBox,kPmE,pfLParam,rfSqrDistance);
    }
    else
    {
      // line intersects z = e2
      Face(2,0,1,rkPnt,rkDir,rkBox,kPmE,pfLParam,rfSqrDistance);
    }
  }
}
//----------------------------------------------------------------------------
static void Case0 (int i0, int i1, int i2, tVector3& rkPnt,
                   const tVector3& rkDir, const tBox& rkBox, tScalar* pfLParam,
                   tScalar& rfSqrDistance)
{
  tScalar fPmE0 = rkPnt[i0] - rkBox.GetHalfSideLengths()(i0);
  tScalar fPmE1 = rkPnt[i1] - rkBox.GetHalfSideLengths()(i1);
  tScalar fProd0 = rkDir[i1]*fPmE0;
  tScalar fProd1 = rkDir[i0]*fPmE1;
  tScalar fDelta, fInvLSqr, fInv;

  if ( fProd0 >= fProd1 )
  {
    // line intersects P[i0] = e[i0]
    rkPnt[i0] = rkBox.GetHalfSideLengths()(i0);

    tScalar fPpE1 = rkPnt[i1] + rkBox.GetHalfSideLengths()(i1);
    fDelta = fProd0 - rkDir[i0]*fPpE1;
    if ( fDelta >= (tScalar)0.0 )
    {
      fInvLSqr = ((tScalar)1.0)/(rkDir[i0]*rkDir[i0]+rkDir[i1]*rkDir[i1]);
      rfSqrDistance += fDelta*fDelta*fInvLSqr;
      if ( pfLParam )
      {
        rkPnt[i1] = -rkBox.GetHalfSideLengths()(i1);
        *pfLParam = -(rkDir[i0]*fPmE0+rkDir[i1]*fPpE1)*fInvLSqr;
      }
    }
    else
    {
      if ( pfLParam )
      {
        fInv = ((tScalar)1.0)/rkDir[i0];
        rkPnt[i1] -= fProd0*fInv;
        *pfLParam = -fPmE0*fInv;
      }
    }
  }
  else
  {
    // line intersects P[i1] = e[i1]
    rkPnt[i1] = rkBox.GetHalfSideLengths()(i1);

    tScalar fPpE0 = rkPnt[i0] + rkBox.GetHalfSideLengths()(i0);
    fDelta = fProd1 - rkDir[i1]*fPpE0;
    if ( fDelta >= (tScalar)0.0 )
    {
      fInvLSqr = ((tScalar)1.0)/(rkDir[i0]*rkDir[i0]+rkDir[i1]*rkDir[i1]);
      rfSqrDistance += fDelta*fDelta*fInvLSqr;
      if ( pfLParam )
      {
        rkPnt[i0] = -rkBox.GetHalfSideLengths()(i0);
        *pfLParam = -(rkDir[i0]*fPpE0+rkDir[i1]*fPmE1)*fInvLSqr;
      }
    }
    else
    {
      if ( pfLParam )
      {
        fInv = ((tScalar)1.0)/rkDir[i1];
        rkPnt[i0] -= fProd1*fInv;
        *pfLParam = -fPmE1*fInv;
      }
    }
  }

  if ( rkPnt[i2] < -rkBox.GetHalfSideLengths()(i2) )
  {
    fDelta = rkPnt[i2] + rkBox.GetHalfSideLengths()(i2);
    rfSqrDistance += fDelta*fDelta;
    rkPnt[i2] = -rkBox.GetHalfSideLengths()(i2);
  }
  else if ( rkPnt[i2] > rkBox.GetHalfSideLengths()(i2) )
  {
    fDelta = rkPnt[i2] - rkBox.GetHalfSideLengths()(i2);
    rfSqrDistance += fDelta*fDelta;
    rkPnt[i2] = rkBox.GetHalfSideLengths()(i2);
  }
}
//----------------------------------------------------------------------------
static void Case00 (int i0, int i1, int i2, tVector3& rkPnt,
                    const tVector3& rkDir, const tBox& rkBox, tScalar* pfLParam,
                    tScalar& rfSqrDistance)
{
  tScalar fDelta;

  if ( pfLParam )
    *pfLParam = (rkBox.GetHalfSideLengths()(i0) - rkPnt[i0])/rkDir[i0];

  rkPnt[i0] = rkBox.GetHalfSideLengths()(i0);

  if ( rkPnt[i1] < -rkBox.GetHalfSideLengths()(i1) )
  {
    fDelta = rkPnt[i1] + rkBox.GetHalfSideLengths()(i1);
    rfSqrDistance += fDelta*fDelta;
    rkPnt[i1] = -rkBox.GetHalfSideLengths()(i1);
  }
  else if ( rkPnt[i1] > rkBox.GetHalfSideLengths()(i1) )
  {
    fDelta = rkPnt[i1] - rkBox.GetHalfSideLengths()(i1);
    rfSqrDistance += fDelta*fDelta;
    rkPnt[i1] = rkBox.GetHalfSideLengths()(i1);
  }

  if ( rkPnt[i2] < -rkBox.GetHalfSideLengths()(i2) )
  {
    fDelta = rkPnt[i2] + rkBox.GetHalfSideLengths()(i2);
    rfSqrDistance += fDelta*fDelta;
    rkPnt[i2] = -rkBox.GetHalfSideLengths()(i2);
  }
  else if ( rkPnt[i2] > rkBox.GetHalfSideLengths()(i2) )
  {
    fDelta = rkPnt[i2] - rkBox.GetHalfSideLengths()(i2);
    rfSqrDistance += fDelta*fDelta;
    rkPnt[i2] = rkBox.GetHalfSideLengths()(i2);
  }
}
//----------------------------------------------------------------------------
static void Case000 (tVector3& rkPnt, const tBox& rkBox,
                     tScalar& rfSqrDistance)
{
  tScalar fDelta;

  if ( rkPnt.x < -rkBox.GetHalfSideLengths()(0) )
  {
    fDelta = rkPnt.x + rkBox.GetHalfSideLengths()(0);
    rfSqrDistance += fDelta*fDelta;
    rkPnt.x = -rkBox.GetHalfSideLengths()(0);
  }
  else if ( rkPnt.x > rkBox.GetHalfSideLengths()(0) )
  {
    fDelta = rkPnt.x - rkBox.GetHalfSideLengths()(0);
    rfSqrDistance += fDelta*fDelta;
    rkPnt.x = rkBox.GetHalfSideLengths()(0);
  }

  if ( rkPnt.y < -rkBox.GetHalfSideLengths()(1) )
  {
    fDelta = rkPnt.y + rkBox.GetHalfSideLengths()(1);
    rfSqrDistance += fDelta*fDelta;
    rkPnt.y = -rkBox.GetHalfSideLengths()(1);
  }
  else if ( rkPnt.y > rkBox.GetHalfSideLengths()(1) )
  {
    fDelta = rkPnt.y - rkBox.GetHalfSideLengths()(1);
    rfSqrDistance += fDelta*fDelta;
    rkPnt.y = rkBox.GetHalfSideLengths()(1);
  }

  if ( rkPnt.z < -rkBox.GetHalfSideLengths()(2) )
  {
    fDelta = rkPnt.z + rkBox.GetHalfSideLengths()(2);
    rfSqrDistance += fDelta*fDelta;
    rkPnt.z = -rkBox.GetHalfSideLengths()(2);
  }
  else if ( rkPnt.z > rkBox.GetHalfSideLengths()(2) )
  {
    fDelta = rkPnt.z - rkBox.GetHalfSideLengths()(2);
    rfSqrDistance += fDelta*fDelta;
    rkPnt.z = rkBox.GetHalfSideLengths()(2);
  }
}
//========================================================
// SqrDistance 
//========================================================
tScalar SqrDistance(const tLine& rkLine, const tBox& rkBox,
                    tScalar* pfLParam, tScalar* pfBParam0, tScalar* pfBParam1, tScalar* pfBParam2)
{
  // compute coordinates of line in box coordinate system
  tVector3 kDiff = rkLine.mOrigin - rkBox.GetCentre();
  tVector3 kPnt(Dot(kDiff, rkBox.GetOrient()[0]),
                Dot(kDiff, rkBox.GetOrient()[1]),
                Dot(kDiff, rkBox.GetOrient()[2]));
  tVector3 kDir(Dot(rkLine.mDir, rkBox.GetOrient()[0]),
                Dot(rkLine.mDir, rkBox.GetOrient()[1]),
                Dot(rkLine.mDir, rkBox.GetOrient()[2]));

  // Apply reflections so that direction vector has nonnegative components.
  bool bReflect[3];
  int i;
  for (i = 0; i < 3; i++)
  {
    if ( kDir[i] < (tScalar)0.0 )
    {
      kPnt[i] = -kPnt[i];
      kDir[i] = -kDir[i];
      bReflect[i] = true;
    }
    else
    {
      bReflect[i] = false;
    }
  }

  tScalar fSqrDistance = (tScalar)0.0;

  if ( kDir.x > (tScalar)0.0 )
  {
    if ( kDir.y > (tScalar)0.0 )
    {
      if ( kDir.z > (tScalar)0.0 )
      {
        // (+,+,+)
        CaseNoZeros(kPnt,kDir,rkBox,pfLParam,fSqrDistance);
      }
      else
      {
        // (+,+,0)
        Case0(0,1,2,kPnt,kDir,rkBox,pfLParam,fSqrDistance);
      }
    }
    else
    {
      if ( kDir.z > (tScalar)0.0 )
      {
        // (+,0,+)
        Case0(0,2,1,kPnt,kDir,rkBox,pfLParam,fSqrDistance);
      }
      else
      {
        // (+,0,0)
        Case00(0,1,2,kPnt,kDir,rkBox,pfLParam,fSqrDistance);
      }
    }
  }
  else
  {
    if ( kDir.y > (tScalar)0.0 )
    {
      if ( kDir.z > (tScalar)0.0 )
      {
        // (0,+,+)
        Case0(1,2,0,kPnt,kDir,rkBox,pfLParam,fSqrDistance);
      }
      else
      {
        // (0,+,0)
        Case00(1,0,2,kPnt,kDir,rkBox,pfLParam,fSqrDistance);
      }
    }
    else
    {
      if ( kDir.z > (tScalar)0.0 )
      {
        // (0,0,+)
        Case00(2,0,1,kPnt,kDir,rkBox,pfLParam,fSqrDistance);
      }
      else
      {
        // (0,0,0)
        Case000(kPnt,rkBox,fSqrDistance);
        if ( pfLParam )
          *pfLParam = (tScalar)0.0;
      }
    }
  }

  // undo reflections
  for (i = 0; i < 3; i++)
  {
    if ( bReflect[i] )
      kPnt[i] = -kPnt[i];
  }

  if ( pfBParam0 )
    *pfBParam0 = kPnt.x;

  if ( pfBParam1 )
    *pfBParam1 = kPnt.y;

  if ( pfBParam2 )
    *pfBParam2 = kPnt.z;

  return Max(fSqrDistance, 0.0f);
}

//========================================================
// SegmentBoxDistanceSq
//========================================================
tScalar JigLib::SegmentBoxDistanceSq(tScalar * pfLParam, 
                                     tScalar * pfBParam0, tScalar * pfBParam1, tScalar * pfBParam2,
                                     const tSegment & rkSeg, const tBox & rkBox)
{
  tLine kLine;
  kLine.mOrigin = rkSeg.mOrigin;
  kLine.mDir = rkSeg.mDelta;

  tScalar fLP, fBP0, fBP1, fBP2;
  tScalar fSqrDistance = SqrDistance(kLine,rkBox,&fLP,&fBP0,&fBP1,&fBP2);
  if ( fLP >= (tScalar)0.0 )
  {
    if ( fLP <= (tScalar)1.0 )
    {
      if ( pfLParam )
        *pfLParam = fLP;

      if ( pfBParam0 )
        *pfBParam0 = fBP0;// + 0.5f;

      if ( pfBParam1 )
        *pfBParam1 = fBP1;// + 0.5f;

      if ( pfBParam2 )
        *pfBParam2 = fBP2;// + 0.5f;

      return Max(fSqrDistance, 0.0f);
    }
    else
    {
      fSqrDistance = SqrDistance(rkSeg.mOrigin+rkSeg.mDelta,
                                 rkBox,pfBParam0,pfBParam1,pfBParam2);

      if ( pfLParam )
        *pfLParam = (tScalar)1.0;
      /*
        if ( pfBParam0 )
        *pfBParam0 += 0.5f;

        if ( pfBParam1 )
        *pfBParam1 += 0.5f;

        if ( pfBParam2 )
        *pfBParam2 += 0.5f;
        */
      return Max(fSqrDistance, 0.0f);
    }
  }
  else
  {
    fSqrDistance = SqrDistance(rkSeg.mOrigin,rkBox,pfBParam0,pfBParam1,pfBParam2);

    if ( pfLParam )
      *pfLParam = (tScalar)0.0;
    /*
      if ( pfBParam0 )
      *pfBParam0 += 0.5f;

      if ( pfBParam1 )
      *pfBParam1 += 0.5f;

      if ( pfBParam2 )
      *pfBParam2 += 0.5f;
      */
    return Max(fSqrDistance, 0.0f);
  }
}


//==============================================================
// PointTriangleDistanceSq
//==============================================================
tScalar JigLib::PointTriangleDistanceSq(tScalar * pfSParam, tScalar * pfTParam, 
                                        const tVector3 & rkPoint, const tTriangle & rkTri)
{
  tVector3 kDiff = rkTri.GetOrigin() - rkPoint;
  tScalar fA00 = rkTri.GetEdge0().GetLengthSq();
  tScalar fA01 = Dot(rkTri.GetEdge0(), rkTri.GetEdge1());
  tScalar fA11 = rkTri.GetEdge1().GetLengthSq();
  tScalar fB0 = Dot(kDiff, rkTri.GetEdge0());
  tScalar fB1 = Dot(kDiff, rkTri.GetEdge1());
  tScalar fC = kDiff.GetLengthSq();
  tScalar fDet = Abs(fA00*fA11-fA01*fA01);
  tScalar fS = fA01*fB1-fA11*fB0;
  tScalar fT = fA01*fB0-fA00*fB1;
  tScalar fSqrDist;

  if ( fS + fT <= fDet )
  {
    if ( fS < (tScalar)0.0 )
    {
      if ( fT < (tScalar)0.0 )  // region 4
      {
        if ( fB0 < (tScalar)0.0 )
        {
          fT = (tScalar)0.0;
          if ( -fB0 >= fA00 )
          {
            fS = (tScalar)1.0;
            fSqrDist = fA00+((tScalar)2.0)*fB0+fC;
          }
          else
          {
            fS = -fB0/fA00;
            fSqrDist = fB0*fS+fC;
          }
        }
        else
        {
          fS = (tScalar)0.0;
          if ( fB1 >= (tScalar)0.0 )
          {
            fT = (tScalar)0.0;
            fSqrDist = fC;
          }
          else if ( -fB1 >= fA11 )
          {
            fT = (tScalar)1.0;
            fSqrDist = fA11+((tScalar)2.0)*fB1+fC;
          }
          else
          {
            fT = -fB1/fA11;
            fSqrDist = fB1*fT+fC;
          }
        }
      }
      else  // region 3
      {
        fS = (tScalar)0.0;
        if ( fB1 >= (tScalar)0.0 )
        {
          fT = (tScalar)0.0;
          fSqrDist = fC;
        }
        else if ( -fB1 >= fA11 )
        {
          fT = (tScalar)1.0;
          fSqrDist = fA11+((tScalar)2.0)*fB1+fC;
        }
        else
        {
          fT = -fB1/fA11;
          fSqrDist = fB1*fT+fC;
        }
      }
    }
    else if ( fT < (tScalar)0.0 )  // region 5
    {
      fT = (tScalar)0.0;
      if ( fB0 >= (tScalar)0.0 )
      {
        fS = (tScalar)0.0;
        fSqrDist = fC;
      }
      else if ( -fB0 >= fA00 )
      {
        fS = (tScalar)1.0;
        fSqrDist = fA00+((tScalar)2.0)*fB0+fC;
      }
      else
      {
        fS = -fB0/fA00;
        fSqrDist = fB0*fS+fC;
      }
    }
    else  // region 0
    {
      // minimum at interior point
      tScalar fInvDet = ((tScalar)1.0)/fDet;
      fS *= fInvDet;
      fT *= fInvDet;
      fSqrDist = fS*(fA00*fS+fA01*fT+((tScalar)2.0)*fB0) +
        fT*(fA01*fS+fA11*fT+((tScalar)2.0)*fB1)+fC;
    }
  }
  else
  {
    tScalar fTmp0, fTmp1, fNumer, fDenom;

    if ( fS < (tScalar)0.0 )  // region 2
    {
      fTmp0 = fA01 + fB0;
      fTmp1 = fA11 + fB1;
      if ( fTmp1 > fTmp0 )
      {
        fNumer = fTmp1 - fTmp0;
        fDenom = fA00-2.0f*fA01+fA11;
        if ( fNumer >= fDenom )
        {
          fS = (tScalar)1.0;
          fT = (tScalar)0.0;
          fSqrDist = fA00+((tScalar)2.0)*fB0+fC;
        }
        else
        {
          fS = fNumer/fDenom;
          fT = (tScalar)1.0 - fS;
          fSqrDist = fS*(fA00*fS+fA01*fT+2.0f*fB0) +
            fT*(fA01*fS+fA11*fT+((tScalar)2.0)*fB1)+fC;
        }
      }
      else
      {
        fS = (tScalar)0.0;
        if ( fTmp1 <= (tScalar)0.0 )
        {
          fT = (tScalar)1.0;
          fSqrDist = fA11+((tScalar)2.0)*fB1+fC;
        }
        else if ( fB1 >= (tScalar)0.0 )
        {
          fT = (tScalar)0.0;
          fSqrDist = fC;
        }
        else
        {
          fT = -fB1/fA11;
          fSqrDist = fB1*fT+fC;
        }
      }
    }
    else if ( fT < (tScalar)0.0 )  // region 6
    {
      fTmp0 = fA01 + fB1;
      fTmp1 = fA00 + fB0;
      if ( fTmp1 > fTmp0 )
      {
        fNumer = fTmp1 - fTmp0;
        fDenom = fA00-((tScalar)2.0)*fA01+fA11;
        if ( fNumer >= fDenom )
        {
          fT = (tScalar)1.0;
          fS = (tScalar)0.0;
          fSqrDist = fA11+((tScalar)2.0)*fB1+fC;
        }
        else
        {
          fT = fNumer/fDenom;
          fS = (tScalar)1.0 - fT;
          fSqrDist = fS*(fA00*fS+fA01*fT+((tScalar)2.0)*fB0) +
            fT*(fA01*fS+fA11*fT+((tScalar)2.0)*fB1)+fC;
        }
      }
      else
      {
        fT = (tScalar)0.0;
        if ( fTmp1 <= (tScalar)0.0 )
        {
          fS = (tScalar)1.0;
          fSqrDist = fA00+((tScalar)2.0)*fB0+fC;
        }
        else if ( fB0 >= (tScalar)0.0 )
        {
          fS = (tScalar)0.0;
          fSqrDist = fC;
        }
        else
        {
          fS = -fB0/fA00;
          fSqrDist = fB0*fS+fC;
        }
      }
    }
    else  // region 1
    {
      fNumer = fA11 + fB1 - fA01 - fB0;
      if ( fNumer <= (tScalar)0.0 )
      {
        fS = (tScalar)0.0;
        fT = (tScalar)1.0;
        fSqrDist = fA11+((tScalar)2.0)*fB1+fC;
      }
      else
      {
        fDenom = fA00-2.0f*fA01+fA11;
        if ( fNumer >= fDenom )
        {
          fS = (tScalar)1.0;
          fT = (tScalar)0.0;
          fSqrDist = fA00+((tScalar)2.0)*fB0+fC;
        }
        else
        {
          fS = fNumer/fDenom;
          fT = (tScalar)1.0 - fS;
          fSqrDist = fS*(fA00*fS+fA01*fT+((tScalar)2.0)*fB0) +
            fT*(fA01*fS+fA11*fT+((tScalar)2.0)*fB1)+fC;
        }
      }
    }
  }

  if ( pfSParam )
    *pfSParam = fS;

  if ( pfTParam )
    *pfTParam = fT;

  return Abs(fSqrDist);
}

//====================================================================
// SegmentTriangleDistanceSq
//====================================================================
tScalar JigLib::SegmentTriangleDistanceSq(tScalar* segT, tScalar* triT0, tScalar* triT1, const tSegment& seg, const tTriangle& triangle)
{
  // compare segment to all three edges of the triangle
  tScalar distSq = std::numeric_limits<tScalar>::max();

  if (SegmentTriangleIntersection(segT, triT0, triT1, seg, triangle))
  {
    return 0.0f;
  }

  tScalar s, t, u;
  tScalar distEdgeSq;
  distEdgeSq = SegmentSegmentDistanceSq(&s, &t, seg, tSegment(triangle.GetOrigin(), triangle.GetEdge0()));
  if (distEdgeSq < distSq)
  {
    distSq = distEdgeSq;
    if (segT) *segT = s;
    if (triT0) *triT0 = t;
    if (triT1) *triT1 = 0.0f;
  }
  distEdgeSq = SegmentSegmentDistanceSq(&s, &t, seg, tSegment(triangle.GetOrigin(), triangle.GetEdge1()));
  if (distEdgeSq < distSq)
  {
    distSq = distEdgeSq;
    if (segT) *segT = s;
    if (triT0) *triT0 = 0.0f;
    if (triT1) *triT1 = t;
  }
  distEdgeSq = SegmentSegmentDistanceSq(&s, &t, seg, tSegment(triangle.GetOrigin() + triangle.GetEdge0(), triangle.GetEdge2()));
  if (distEdgeSq < distSq)
  {
    distSq = distEdgeSq;
    if (segT) *segT = s;
    if (triT0) *triT0 = 1.0f - t;
    if (triT1) *triT1 = t;
  }

  // compare segment end points to triangle interior
  tScalar startTriSq = PointTriangleDistanceSq(&t, &u, seg.GetOrigin(), triangle);
  if (startTriSq < distSq)
  {
    distSq = startTriSq;
    if (segT) *segT = 0.0f;
    if (triT0) *triT0 = t;
    if (triT1) *triT1 = u;
  }
  tScalar endTriSq = PointTriangleDistanceSq(&t, &u, seg.GetEnd(), triangle);
  if (endTriSq < distSq)
  {
    distSq = endTriSq;
    if (segT) *segT = 1.0f;
    if (triT0) *triT0 = t;
    if (triT1) *triT1 = u;
  }
  return distSq;
}
