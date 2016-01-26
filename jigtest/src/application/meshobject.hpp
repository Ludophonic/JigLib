//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file meshobject.hpp 
//
//==============================================================
#ifndef MESHOBJECT_HPP
#define MESHOBJECT_HPP

#include "object.hpp"
#include "graphics.hpp"
#include "jiglib.hpp"

#include <vector>

#ifndef NO_XML
#include <libxml/xmlreader.h>
#endif

/// This is a game-object - i.e. a mesh that has no body, has
/// collision, and can be rendered.
class tMeshObject : public tObject
{
public:
  /// Load a mesh from XML file, and also scale it
  tMeshObject(const char* meshXMLFile, JigLib::tScalar meshScale,
              int maxTrianglesPerCell, JigLib::tScalar minCellSize);

  /// Construct from vertices
  tMeshObject(const std::vector<JigLib::tVector3> &vertices, 
    const std::vector<JigLib::tTriangleVertexIndices> &triangleVertexIndices,
    int maxTrianglesPerCell, JigLib::tScalar minCellSize);

  ~tMeshObject();
  void SetProperties(JigLib::tScalar elasticity, 
                     JigLib::tScalar staticFriction,
                     JigLib::tScalar dynamicFriction);

  JigLib::tBody * GetBody() {return 0;}
  JigLib::tCollisionSkin * GetCollisionSkin() {return &mCollisionSkin;}

  // inherited from tRenderObject
  void Render(tRenderType renderType);
  const JigLib::tSphere & GetRenderBoundingSphere() const;
  const JigLib::tVector3 & GetRenderPosition() const;
  const JigLib::tMatrix33 & GetRenderOrientation() const;

private:
  void LoadMesh(const char* meshXMLFile, 
                std::vector<JigLib::tVector3>& vertices, 
                std::vector<JigLib::tTriangleVertexIndices>& triangleVertexIndices);

#ifndef NO_XML
  void ProcessNode(xmlTextReaderPtr reader, 
                   std::vector<JigLib::tVector3>& vertices, 
                   std::vector<JigLib::tTriangleVertexIndices>& triangleVertexIndices);
  void ReadVertices(xmlTextReaderPtr reader, 
                    std::vector<JigLib::tVector3>& vertices, 
                    int numVertices);
  void ReadFaces(xmlTextReaderPtr reader, 
                 std::vector<JigLib::tTriangleVertexIndices>& faces, 
                 int numFaces);
#endif

  void AddToTriangles(std::vector<JigLib::tVector3>& triangleVertices, 
                      std::vector<JigLib::tTriangleVertexIndices>& triangleVertexIndices,
                      const std::vector<JigLib::tVector3>& vertices,
                      const std::vector<JigLib::tTriangleVertexIndices>& faces);

  JigLib::tCollisionSkin mCollisionSkin;

  JigLib::tVector3 mRenderPosition;
  JigLib::tMatrix33 mRenderOrientation;

  GLuint mDisplayListNum;
};

#endif
