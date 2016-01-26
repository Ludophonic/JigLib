//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file trianglemesh.hpp 
//                     
//==============================================================
#include "trianglemesh.hpp"
#include "triangle.hpp"
#include "intersection.hpp"
#include "trace.hpp"

using namespace JigLib;
using namespace std;

//==============================================================
// Clone
//==============================================================
tPrimitive* tTriangleMesh::Clone() const
{
  return new tTriangleMesh(*this);
}

//====================================================================
// tTriangleMesh
//====================================================================
tTriangleMesh::tTriangleMesh() : tPrimitive(tPrimitive::TRIANGLEMESH)
{
}

//==============================================================
// CreateMesh
//==============================================================
void tTriangleMesh::CreateMesh(const tVector3 * vertices, unsigned numVertices,
                               const tTriangleVertexIndices * triangleVertexIndices, unsigned numTriangles,
                               int maxTrianglesPerCell, tScalar minCellSize)
{
  TRACE_METHOD_ONLY(ONCE_2);
  mOctree.Clear(true);
  mOctree.AddTriangles(vertices, numVertices, triangleVertexIndices, numTriangles);
  mOctree.BuildOctree(maxTrianglesPerCell, minCellSize);
}

//==============================================================
// SegmentIntersect
//==============================================================
bool tTriangleMesh::SegmentIntersect(tScalar &frac, tVector3 &pos, tVector3 &normal, const class tSegment &seg) const
{
  TRACE_METHOD_ONLY(MULTI_FRAME_2);

  tAABox segBox(true);
  segBox.AddSegment(seg);

  static std::vector<unsigned> potentialTriangles;
  const unsigned numTriangles = GetTrianglesIntersectingtAABox(potentialTriangles, segBox);

  tScalar bestFrac = SCALAR_HUGE;
  for (unsigned iTriangle = 0 ; iTriangle < numTriangles ; ++iTriangle)
  {
    const tIndexedTriangle& meshTriangle = GetTriangle(potentialTriangles[iTriangle]);
    tScalar thisFrac;
    tTriangle tri(
      GetVertex(meshTriangle.GetVertexIndex(0)),
      GetVertex(meshTriangle.GetVertexIndex(1)),
      GetVertex(meshTriangle.GetVertexIndex(2)));
      
    if (SegmentTriangleIntersection(&thisFrac, 0, 0, seg, tri))
    {
      if (thisFrac < bestFrac)
      {
        bestFrac = thisFrac;
        pos = seg.GetPoint(thisFrac);
        normal = meshTriangle.GetPlane().GetN();
      }
    }
  }
  frac = bestFrac;
  if (bestFrac < SCALAR_HUGE)
    return true;
  else
    return false;
}

//==============================================================
// GetMassProperties
//==============================================================
void tTriangleMesh::GetMassProperties(const tPrimitiveProperties &primitiveProperties, 
                                      tScalar &mass, 
                                      tVector3 &centerOfMass, 
                                      tMatrix33 &inertiaTensor) const
{
  mass = 0.0f;
  centerOfMass.SetToZero();
  inertiaTensor.SetTo(0.0f);
}

