#include "indexedtriangle.hpp"

using namespace JigLib;
using namespace std;

//==============================================================
// tIndexedTriangle
//==============================================================
tIndexedTriangle::tIndexedTriangle()
{
  mVertexIndices[0] = mVertexIndices[1] = mVertexIndices[2] = -1;
  mConvexFlags = ~0;
  mPlane.Set(tVector3(0.0f, 0.0f, 0.0f), 0.0f);
  mBoundingBox.Clear();
}

//==============================================================
// SetVertexIndices
//==============================================================
void tIndexedTriangle::SetVertexIndices(unsigned i0, unsigned i1, unsigned i2, 
                                        const tVector3 * vertexArray) 
{
  mVertexIndices[0] = i0;
  mVertexIndices[1] = i1;
  mVertexIndices[2] = i2;

  mPlane.Set(vertexArray[i0], vertexArray[i1], vertexArray[i2]);
  mBoundingBox.Clear();
  mBoundingBox.AddPoint(vertexArray[i0]);
  mBoundingBox.AddPoint(vertexArray[i1]);
  mBoundingBox.AddPoint(vertexArray[i2]);
}

