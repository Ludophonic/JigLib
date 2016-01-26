#ifndef OCTREE_HPP
#define OCTREE_HPP

#include "../geometry/include/indexedtriangle.hpp"

namespace JigLib
{
  /// Stores world collision data in an octree structure for quick ray testing
  /// during CVolumeNavRegion processing.
  class tOctree
  {
  public:
    /// On creation the extents are defined - if anything is subsequently added
    /// that lies entirely outside this bbox it will not get added.
    tOctree();
    ~tOctree();

    /// Gets the number of triangles
    unsigned GetNumTriangles() const {return mTriangles.size();}

    /// Get a triangle
    const tIndexedTriangle & GetTriangle(unsigned iTriangle) const {
      return mTriangles[iTriangle];}

    /// Get a vertex
    const tVector3 & GetVertex(unsigned iVertex) const {return mVertices[iVertex];}

    /// Add the triangles - doesn't actually build the octree
    void AddTriangles(const tVector3 * vertices, unsigned numVertices,
                      const tTriangleVertexIndices * triangleVertexIndices,
                      unsigned numTriangles);


    /// Clears triangles and cells. If freeMemory is set to true, the
    /// triangle index array will be freed, otherwise it will be reset
    /// preserving the allocated memory.
    void Clear( bool freeMemory = true );

    /// Builds the octree from scratch (not incrementally) - deleting
    /// any previous tree.  Building the octree will involve placing
    /// all triangles into the root cell.  Then this cell gets pushed
    /// onto a stack of cells to examine. This stack will get parsed
    /// and every cell containing more than maxTrianglesPerCell will
    /// get split into 8 children, and all the original triangles in
    /// that cell will get partitioned between the children. A
    /// triangle can end up in multiple cells (possibly a lot!) if it
    /// straddles a boundary. Therefore when intersection tests are
    /// done tIndexedTriangle::m_counter can be set/tested using a
    /// counter to avoid properly testing the triangle multiple times
    /// (the counter _might_ wrap around, so when it wraps ALL the
    /// triangle flags should be cleared! Could do this
    /// incrementally...).
    void BuildOctree(unsigned maxTrianglesPerCell, tScalar minCellSize);

    /// Gets a list of all triangle indices that intersect an tAABox. The vector passed in resized,
    /// so if you keep it between calls after a while it won't grow any more, and this
    /// won't allocate more memory.
    /// Returns the number of triangles (same as triangles.size())
    unsigned GetTrianglesIntersectingtAABox(std::vector<unsigned>& triangles, const tAABox& aabb) const;

    /// Write out some info
    void DumpStats() const;

  private:
    /// Internally we don't store pointers but store indices into a single contiguous
    /// array of cells and triangles owned by tOctree (so that the vectors can
    /// get resized).
    ///
    /// Each cell will either contain children OR contain triangles. 
    struct tOctreeCell
    {
      /// constructor clears everything
      tOctreeCell();
      /// constructor clears everything
      tOctreeCell(const tAABox & aabb);
      /// Sets all child indices to -1 and clears the triangle indices.
      void Clear();

      /// Indicates if we contain triangles (if not then we should/might have children)
      bool IsLeaf() const {return mChildCellIndices[0] == -1;}

      /// endices into the children - P means "plus" and M means "minus" and the
      /// letters are xyz. So PPM means +ve x, +ve y, -ve z
      enum EChild
      {
        PPP,
        PPM,
        PMP,
        PMM,
        MPP,
        MPM,
        MMP,
        MMM,
        NUM_CHILDREN
      };

      /// indices of the children (if not leaf). Will be -1 if there is no child
      int mChildCellIndices[NUM_CHILDREN];

      /// indices of the triangles (if leaf)
      std::vector<int> mTriangleIndices;

      /// Bounding box for the space we own
      tAABox mAABox;
    };

    /// Functor that can be passed to std::sort so that it sorts equal sized cells along a specified
    /// direction such that cells near the beginning of a line with dirPtr come at the end of the
    /// sorted container. This means they get processed first when that container is used as a stack.
    struct tCellSorter
    {
      tCellSorter(const tVector3 * dirPtr, const std::vector<tOctreeCell> * cellsPtr) : mDirPtr(dirPtr), mCellsPtr(cellsPtr) {}
      bool operator()(int cell1Index, int cell2Index) const
        {
          tVector3 delta = (*mCellsPtr)[cell2Index].mAABox.GetMinPos() - (*mCellsPtr)[cell1Index].mAABox.GetMinPos();
          return Dot(delta, *mDirPtr) < 0.0f;
        }
      const tVector3 * mDirPtr;
      const std::vector<tOctreeCell> * mCellsPtr;
    };

    /// Create a bounding box appropriate for a child, based on a parents tAABox
    tAABox CreateAABox(const tAABox & aabb, tOctreeCell::EChild child) const;

    /// Returns true if the triangle intersects or is contained by a cell
    bool DoesTriangleIntersectCell(const tIndexedTriangle & triangle, const tOctreeCell & cell) const;

    /// Increment our test counter, wrapping around if necessary and zapping the 
    /// triangle counters.
    /// Const because we only modify mutable members.
    void IncrementTestCounter() const;

    /// All our cells. The only thing guaranteed about this is that m_cell[0] (if
    /// it exists) is the root cell.
    std::vector<tOctreeCell> mCells;
    /// the vertices
    std::vector<tVector3> mVertices;
    /// All our triangles.
    std::vector<tIndexedTriangle> mTriangles;

    tAABox mBoundingBox;

    /// During intersection testing we keep a stack of cells to test (rather than recursing) - 
    /// to avoid excessive memory allocation we don't free the memory between calls unless
    /// the user calls FreeTemporaryMemory();
    mutable std::vector<int> mCellsToTest;

    /// Counter used to prevent multiple tests when triangles are contained in more than
    /// one cell
    mutable unsigned m_testCounter;
  };

} // namespace
#endif
