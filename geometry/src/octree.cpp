#include "octree.hpp"
#include "triangle.hpp"
#include "line.hpp"
#include "distance.hpp"
#include "overlap.hpp"

using namespace std;
using namespace JigLib;

//====================================================================
// tOctree
//====================================================================
tOctree::tOctree()
  :
  m_testCounter(0)
{
}

//====================================================================
// tOctree
//====================================================================
tOctree::~tOctree()
{
}

//====================================================================
// Clear
//====================================================================
void tOctree::Clear(bool freeMemory)
{
  if( freeMemory )
  {
    mCells.clear();
    mVertices.clear();
    mTriangles.clear();
  }
  else
  {
    mCells.resize(0);
    mVertices.resize(0);
    mTriangles.resize(0);
  }
}

//====================================================================
// CreateMesh
//====================================================================
void tOctree::AddTriangles(const tVector3 * vertices, unsigned numVertices,
                           const tTriangleVertexIndices * triangleVertexIndices, unsigned numTriangles)
{
  mVertices.resize(0);
  mTriangles.resize(0);
  mCells.resize(0);
  
  Assert(vertices);
  Assert(triangleVertexIndices);

  mVertices.resize(numVertices);
  for (unsigned iVertex = 0 ; iVertex < numVertices ; ++iVertex)
    mVertices[iVertex] = vertices[iVertex];

  mTriangles.reserve(numTriangles);
  for (unsigned iTriangle = 0 ; iTriangle < numTriangles ; ++iTriangle)
  {
    unsigned i0 = triangleVertexIndices[iTriangle].i0;
    unsigned i1 = triangleVertexIndices[iTriangle].i1;
    unsigned i2 = triangleVertexIndices[iTriangle].i2;
    Assert(i0 < numVertices);
    Assert(i1 < numVertices);
    Assert(i2 < numVertices);

    tVector3 dr1 = vertices[i1] - vertices[i0];
    tVector3 dr2 = vertices[i2] - vertices[i0];
    tVector3 N = Cross(dr1, dr2);
    tScalar NLen = N.GetLength();
    // only add if it's not degenerate. Note that this could be a problem it we use connectivity info
    // since we're actually making a hole in the mesh...
    if (NLen > SCALAR_TINY)
    {
      mTriangles.push_back(tIndexedTriangle());
      mTriangles.back().SetVertexIndices(i0, i1, i2, vertices);
    }
  }
}

//====================================================================
// Clear
//====================================================================
void tOctree::tOctreeCell::Clear()
{
  for (unsigned i = 0 ; i < NUM_CHILDREN ; ++i)
    mChildCellIndices[i] = -1;
  mTriangleIndices.clear();
}

//====================================================================
// tOctreeCell
//====================================================================
tOctree::tOctreeCell::tOctreeCell()
{
  Clear();
}

//====================================================================
// tOctreeCell
//====================================================================
tOctree::tOctreeCell::tOctreeCell(const tAABox & aabb)
  :
  mAABox(aabb)
{
  Clear();
}

//====================================================================
// IncrementTestCounter
//====================================================================
inline void tOctree::IncrementTestCounter() const
{
  ++m_testCounter;
  if (m_testCounter == 0)
  {
    // wrap around - clear all the triangle counters
    unsigned numTriangles = mTriangles.size();
    for (unsigned i = 0 ; i < numTriangles ; ++i)
      mTriangles[i].m_counter = 0;
    m_testCounter = 1;
  }
}

//====================================================================
// DoesTriangleIntersectCell
//====================================================================
bool tOctree::DoesTriangleIntersectCell(const tIndexedTriangle & triangle, 
                                        const tOctreeCell & cell) const
{
  if (!OverlapTest(triangle.GetBoundingBox(), cell.mAABox))
    return false;

  // quick test
  if (cell.mAABox.IsPointInside(GetVertex(triangle.GetVertexIndex(0))) ||
      cell.mAABox.IsPointInside(GetVertex(triangle.GetVertexIndex(1))) ||
      cell.mAABox.IsPointInside(GetVertex(triangle.GetVertexIndex(2))) )
    return true;
  // all points are outside... so if there is intersection it must be due to the
  // box edges and the triangle...
  tTriangle tri(GetVertex(triangle.GetVertexIndex(0)), GetVertex(triangle.GetVertexIndex(1)), GetVertex(triangle.GetVertexIndex(2)));
  tBox box(cell.mAABox.GetMinPos(), tMatrix33::Identity(), cell.mAABox.GetSideLengths());
  tVector3 pts[8];
  box.GetCornerPoints(pts);
  const tBox::tEdge * edges = box.GetAllEdges();
  for (unsigned i = 0 ; i < 12 ; ++i)
  {
    const tBox::tEdge& edge = edges[i];
    tSegment seg(pts[edge.ind0], pts[edge.ind1] - pts[edge.ind0]);
    if (SegmentTriangleOverlap(seg, tri))
      return true;
  }
  // Unless it's the triangle edges and the box
  tVector3 pos, n;
  // now each edge of the triangle with the box
  for (unsigned iEdge = 0 ; iEdge < 3 ; ++iEdge)
  {
    const tVector3 pt0 = tri.GetPoint(iEdge);
    const tVector3 pt1 = tri.GetPoint((iEdge + 1) % 3);
    if (SegmentAABoxOverlap(tSegment(pt0, pt1 - pt0), cell.mAABox))
      return true;
  }

  return false;
}

//====================================================================
// CreateAABox
//====================================================================
tAABox tOctree::CreateAABox(const tAABox & aabb, tOctreeCell::EChild child) const
{
  tVector3 dims = 0.5f * (aabb.GetMaxPos() - aabb.GetMinPos());
  tVector3 offset;
  switch (child)
  {
  case tOctreeCell::PPP: offset.Set(1, 1, 1); break;
  case tOctreeCell::PPM: offset.Set(1, 1, 0); break;
  case tOctreeCell::PMP: offset.Set(1, 0, 1); break;
  case tOctreeCell::PMM: offset.Set(1, 0, 0); break;
  case tOctreeCell::MPP: offset.Set(0, 1, 1); break;
  case tOctreeCell::MPM: offset.Set(0, 1, 0); break;
  case tOctreeCell::MMP: offset.Set(0, 0, 1); break;
  case tOctreeCell::MMM: offset.Set(0, 0, 0); break;
  default:
    TRACE("tOctree::CreateAABox Got impossible child: %d", child);
    offset.Set(0, 0, 0);
    break;
  }

  tAABox result;
  result.SetMinPos(aabb.GetMinPos() + tVector3(offset.x * dims.x, offset.y * dims.y, offset.z * dims.z));
  result.SetMaxPos(result.GetMinPos() + dims);
  // expand it just a tiny bit just to be safe!
  tScalar extra = 0.00001f;
  result.SetMinPos(result.GetMinPos() - extra * dims);
  result.SetMaxPos(result.GetMaxPos() + extra * dims);
  return result;
}


//====================================================================
// BuildOctree
//====================================================================
void tOctree::BuildOctree(unsigned maxTrianglesPerCell, tScalar minCellSize)
{
  mBoundingBox.Clear();
  unsigned i;
  for (i = 0 ; i < mVertices.size() ; ++i)
    mBoundingBox.AddPoint(mVertices[i]);

  // clear any existing cells
  mCells.clear();

  // set up the root
  mCells.push_back(tOctreeCell(mBoundingBox));
  unsigned numTriangles = mTriangles.size();
  mCells.back().mTriangleIndices.resize(numTriangles);
  for (i = 0 ; i < numTriangles ; ++i)
    mCells.back().mTriangleIndices[i] = i;

  // rather than doing things recursively, use a stack of cells that need
  // to be processed - for each cell if it contains too many triangles we 
  // create child cells and move the triangles down into them (then we
  // clear the parent triangles).
  std::vector<int> cellsToProcess;
  cellsToProcess.push_back(0);

  // bear in mind during this that any time a new cell gets created any pointer
  // or reference to an existing cell may get invalidated - so use indexing.
  while (!cellsToProcess.empty())
  {
    int cellIndex = cellsToProcess.back();
    cellsToProcess.pop_back();

    if ( (mCells[cellIndex].mTriangleIndices.size() <= maxTrianglesPerCell) ||
         (mCells[cellIndex].mAABox.GetRadiusAboutCentre() < minCellSize) )
      continue;

    // we need to put these triangles into the children
    for (unsigned iChild = 0 ; iChild < tOctreeCell::NUM_CHILDREN ; ++iChild)
    {
      mCells[cellIndex].mChildCellIndices[iChild] = (int) mCells.size();
      cellsToProcess.push_back((int) mCells.size());
      mCells.push_back(tOctreeCell(CreateAABox(mCells[cellIndex].mAABox, 
                                               (tOctreeCell::EChild) iChild)));

      tOctreeCell & childCell = mCells.back();
      unsigned numTriangles = mCells[cellIndex].mTriangleIndices.size();
      for (unsigned i = 0 ; i < numTriangles ; ++i)
      {
        int iTri = mCells[cellIndex].mTriangleIndices[i];
        const tIndexedTriangle & tri = mTriangles[iTri];
        if (DoesTriangleIntersectCell(tri, childCell))
        {
          childCell.mTriangleIndices.push_back(iTri);
        }
      }
    }
    // the children handle all the triangles now - we no longer need them
    mCells[cellIndex].mTriangleIndices.clear();
  }
}

//====================================================================
// GetTrianglesIntersectingtAABox
//====================================================================
unsigned tOctree::GetTrianglesIntersectingtAABox(
  std::vector<unsigned>& triangles, const tAABox& aabb) const
{
  if (mCells.empty())
    return 0;
  
  triangles.resize(0);
  mCellsToTest.resize(0);
  mCellsToTest.push_back(0);

  IncrementTestCounter();

  while (!mCellsToTest.empty())
  {
    int cellIndex = mCellsToTest.back();
    mCellsToTest.pop_back();
    const tOctreeCell & cell = mCells[cellIndex];

    if (!OverlapTest(aabb, cell.mAABox))
      continue;

    if (cell.IsLeaf())
    {
      unsigned nTris = cell.mTriangleIndices.size();
      for (unsigned i = 0 ; i < nTris ; ++i)
      {
        const tIndexedTriangle& triangle = GetTriangle(cell.mTriangleIndices[i]);
        if (triangle.m_counter != m_testCounter)
        {
          triangle.m_counter = m_testCounter;
          if (OverlapTest(aabb, triangle.GetBoundingBox()))
            triangles.push_back(cell.mTriangleIndices[i]);
        }
      }
    }
    else
    {
      // if non-leaf, just add the children to check
      for (unsigned iChild = 0 ; iChild < tOctreeCell::NUM_CHILDREN ; ++iChild)
      {
        int childIndex = cell.mChildCellIndices[iChild];
        mCellsToTest.push_back(childIndex);
      }
    }
  }
  return triangles.size();
}

//====================================================================
// DumpStats
//====================================================================
void tOctree::DumpStats() const
{
  TRACE("tOctree::DumpStats:");
  TRACE("Num tris = %d, num cells = %d", mTriangles.size(), mCells.size());

  unsigned maxTris = 0;

  std::vector<int> cellsToProcess;
  cellsToProcess.push_back(0);
  while (!cellsToProcess.empty())
  {
    int cellIndex = cellsToProcess.back();
    cellsToProcess.pop_back();
    const tOctreeCell & cell = mCells[cellIndex];

    if (cell.IsLeaf())
    {
      // check this hasn't got any children
      for (unsigned i = 0 ; i < tOctreeCell::NUM_CHILDREN ; ++i)
      {
        if (cell.mChildCellIndices[i] != -1)
          TRACE("Found unexpected child cell index");
      }
      // record if this cell has the most found
      unsigned numTris = cell.mTriangleIndices.size();
      if (numTris > maxTris)
        maxTris = numTris;
    }
    else
    {
      // non-leaf so check and descend
      if (!cell.mTriangleIndices.empty())
        TRACE("Found %d triangles in supposedly non-leaf cell", cell.mTriangleIndices.size());
      for (unsigned i = 0 ; i < tOctreeCell::NUM_CHILDREN ; ++i)
      {
        if ( (cell.mChildCellIndices[i] >= 0) &&
             (cell.mChildCellIndices[i] < (int) mCells.size()) )
          cellsToProcess.push_back(cell.mChildCellIndices[i]);
        else
          TRACE("Found invalid child cell index %d in non-leaf cell", cell.mChildCellIndices[i]);
      }
    }
  }
  TRACE("Max triangles in a cell = %d", maxTris);
}

