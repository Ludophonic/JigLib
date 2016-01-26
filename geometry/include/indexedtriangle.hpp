#ifndef INDEXEDTRIANGLE_HPP
#define INDEXEDTRIANGLE_HPP

#include "../maths/include/vector3.hpp"
#include "../geometry/include/plane.hpp"
#include "../geometry/include/aabox.hpp"

namespace JigLib
{
  /// structure used to set up the mesh
  struct tTriangleVertexIndices 
  {
    tTriangleVertexIndices(unsigned i0 = 0, unsigned i1 = 0, unsigned i2 = 0) {Set(i0, i1, i2);}
    void Set(unsigned _i0, unsigned _i1, unsigned _i2) {
      i0 = _i0; i1 = _i1; i2 = _i2;}
    unsigned i0, i1, i2;
  };

  /// Support for an indexed triangle - assumes ownership by something that 
  /// has an array of vertices and an array of tIndexedTriangle
  class tIndexedTriangle
  {
  public:
    tIndexedTriangle();

    /// Set the indices into the relevant vertex array for this
    /// triangle. Also sets the plane and bounding box
    void SetVertexIndices(unsigned i0, unsigned i1, unsigned i2, const tVector3 * vertexArray);
    /// Get the indices into the relevant vertex array for this
    /// triangle.
    void GetVertexIndices(unsigned & i0, unsigned & i1, unsigned & i2) const {
      i0 = mVertexIndices[0]; i1 = mVertexIndices[1]; i2 = mVertexIndices[2];}

    /// Get the vertex index association with iCorner (which should be
    /// 0, 1 or 2)
    unsigned GetVertexIndex(unsigned iCorner) const {return mVertexIndices[iCorner];}

    /// Get the triangle plane
    const tPlane& GetPlane() const {return mPlane;}

    /// Has the edge been marked as convex. Same convention for edge
    /// numbering as in tTriangle.
    bool IsEdgeConvex(unsigned iEdge) const {return 0 != (mConvexFlags & (1 << iEdge));}
    void SetEdgeConvex(unsigned iEdge, bool convex) {
      if (convex) mConvexFlags |= (1 << iEdge); else mConvexFlags &= ~(1 << iEdge);}

    /// Has the point been marked as convex. Same convention for point
    /// numbering as in tTriangle
    bool IsPointConvex(unsigned iPoint) const {return 0 != (mConvexFlags & (1 << (iPoint+3)));}
    void SetPointConvex(unsigned iPoint, bool convex) {
      if (convex) mConvexFlags |= (1 << (iPoint+3)); else mConvexFlags &= ~(1 << (iPoint+3));}

    const tAABox & GetBoundingBox() const {return mBoundingBox;}

    // used when traversing to stop us visiting the same triangle twice
    mutable unsigned m_counter;
  private:
    /// indices into our owner's array of vertices 
    int mVertexIndices[3];
    unsigned char mConvexFlags;
    tPlane mPlane;
    tAABox mBoundingBox;
  };
}
#endif
