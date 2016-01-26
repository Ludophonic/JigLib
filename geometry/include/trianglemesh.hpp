//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file tranglemesh.hpp 
//                     
//==============================================================
#ifndef JIGTRIANGLEMESH_HPP
#define JIGTRIANGLEMESH_HPP

#include "../geometry/include/octree.hpp"
#include "../geometry/include/primitive.hpp"
#include <vector>

namespace JigLib
{
  // contains a bunch of triangles and their related information
  // (vertices, edges etc) that can be used for higher level queries.
  class tTriangleMesh : public tPrimitive
  {
  public:
    tTriangleMesh();

    virtual tPrimitive* Clone() const;

    virtual void GetTransform(class tTransform3 &t) const {t.position.SetTo(0.0f); t.orientation.SetToIdentity();}
    virtual void SetTransform(const class tTransform3 &t) {}
    virtual bool SegmentIntersect(tScalar &frac, tVector3 &pos, tVector3 &normal, const class tSegment &seg) const;
    virtual void GetMassProperties(const tPrimitiveProperties &primitiveProperties, 
      tScalar &mass, tVector3 &centerOfMass, tMatrix33 &inertiaTensor) const;
    virtual tScalar GetVolume() const {return 0.0f;}
    virtual tScalar GetSurfaceArea() const {return 0.0f;}

    /// Internally set up and preprocess all numTriangles. Each index
    /// should, of course, be from 0 to numVertices-1. Vertices and
    /// triangles are copied and stored internally.
    void CreateMesh(const tVector3 * vertices, unsigned numVertices,
                    const tTriangleVertexIndices * triangleVertexIndices,
                    unsigned numTriangles,
                    int maxTrianglesPerCell, tScalar minCellSize);

    unsigned GetNumTriangles() const {return mOctree.GetNumTriangles();}

    /// Get a triangle
    const tIndexedTriangle & GetTriangle(unsigned iTriangle) const {
      return mOctree.GetTriangle(iTriangle);}

    /// Get a vertex
    const tVector3 & GetVertex(unsigned iVertex) const {return mOctree.GetVertex(iVertex);}

    /// Gets a list of all triangle indices that intersect an tAABox. The vector passed in resized,
    /// so if you keep it between calls after a while it won't grow any more, and this
    /// won't allocate more memory.
    /// Returns the number of triangles (same as triangles.size())
    unsigned GetTrianglesIntersectingtAABox(std::vector<unsigned>& triangles, const tAABox& aabb) const {
      return mOctree.GetTrianglesIntersectingtAABox(triangles, aabb);}

  private:
    tOctree mOctree;
  };
}

#endif
