//==============================================================
// Copyright (C) 2004 Danny Chapman 
//               danny@rowlhouse.freeserve.co.uk
//--------------------------------------------------------------
//               
/// @file meshobject.cpp 
//                     
//==============================================================
#include "meshobject.hpp"
#include "debugconfig.hpp"

using namespace JigLib;

//==============================================================
// SetProperties
//==============================================================
void tMeshObject::SetProperties(tScalar elasticity, 
                                tScalar staticFriction,
                                tScalar dynamicFriction)
{
  mCollisionSkin.SetMaterialProperties(0, tMaterialProperties(elasticity, staticFriction, dynamicFriction));
}

//==============================================================
// tMeshObject
//==============================================================
tMeshObject::tMeshObject(const std::vector<tVector3> &vertices, 
                         const std::vector<tTriangleVertexIndices> &triangleVertexIndices,
                         int maxTrianglesPerCell, tScalar minCellSize)
{
  TRACE_METHOD_ONLY(ONCE_2);

  tTriangleMesh mesh;

  // Create the mesh
  if (!vertices.empty() && !triangleVertexIndices.empty())
    mesh.CreateMesh(&vertices[0], vertices.size(), 
    &triangleVertexIndices[0], triangleVertexIndices.size(),
    maxTrianglesPerCell, minCellSize);
  else
    TRACE("Unable to create mesh from vertex list\n");

  mCollisionSkin.AddPrimitive(mesh, tMaterialTable::UNSET);

  SetProperties(0.6f, 0.4f, 0.2f);

  mRenderPosition.SetTo(0.0f);
  mRenderOrientation.SetToIdentity();

  mDisplayListNum = 0;
}


//==============================================================
// tMeshObject
//==============================================================
tMeshObject::tMeshObject(const char* meshXMLFile, tScalar meshScale,
                         int maxTrianglesPerCell, tScalar minCellSize)
{
  TRACE_METHOD_ONLY(ONCE_2);

  // Load the mesh
  std::vector<tVector3> vertices;
  std::vector<tTriangleVertexIndices> triangleVertexIndices;
  LoadMesh(meshXMLFile, vertices, triangleVertexIndices);

  // scale it
  for (unsigned i = 0 ; i < vertices.size() ; ++i)
    vertices[i] *= meshScale;

  tTriangleMesh mesh;

  // Create the mesh
  if (!vertices.empty() && !triangleVertexIndices.empty())
    mesh.CreateMesh(&vertices[0], vertices.size(), 
                         &triangleVertexIndices[0], triangleVertexIndices.size(),
                         maxTrianglesPerCell, minCellSize);
  else
    TRACE("Unable to create mesh from %s\n", meshXMLFile);
  
  mCollisionSkin.AddPrimitive(mesh, tMaterialTable::UNSET);

  SetProperties(0.6f, 0.4f, 0.2f);

  mRenderPosition.SetTo(0.0f);
  mRenderOrientation.SetToIdentity();

  mDisplayListNum = 0;
}

//==============================================================
// ~tMeshObject
//==============================================================
tMeshObject::~tMeshObject()
{
  TRACE_METHOD_ONLY(ONCE_2);
}


//==============================================================
// GetRenderBoundingMesh
//==============================================================
const tSphere & tMeshObject::GetRenderBoundingSphere() const
{
  // ugly...
  static tSphere sphere;
  sphere.SetPos(mCollisionSkin.GetWorldBoundingBox().GetCentre());
  sphere.SetRadius(mCollisionSkin.GetWorldBoundingBox().GetRadiusAboutCentre());
  return sphere;
}

//==============================================================
// GetRenderPosition
//==============================================================
const tVector3 & tMeshObject::GetRenderPosition() const
{
  return mRenderPosition;
}

//==============================================================
// GetRenderOrientation
//==============================================================
const tMatrix33 & tMeshObject::GetRenderOrientation() const
{
  return mRenderOrientation;
}

//==============================================================
// Render
//==============================================================
void tMeshObject::Render(tRenderObject::tRenderType renderType)
{
  TRACE_METHOD_ONLY(FRAME_2);

  if (mDisplayListNum == 0)
  {
    mDisplayListNum = glGenLists(1);
    glNewList(mDisplayListNum, GL_COMPILE);
    GLCOLOR4(RangedRandom(0.8f, 0.9f), 
             RangedRandom(0.8f, 0.9f),
             RangedRandom(0.8f, 0.9f), 
             1.0f);

    const tTriangleMesh& mesh = mCollisionSkin.GetPrimitiveLocal(0)->GetTriangleMesh();

    glBegin(GL_TRIANGLES);

    unsigned numTriangles = mesh.GetNumTriangles();
    for (unsigned iTriangle = 0 ; iTriangle < numTriangles ; ++iTriangle)
    {
#if 0
      GLCOLOR4(RangedRandom(0.2f, 1.0f), 
               RangedRandom(0.2f, 1.0f),
               RangedRandom(0.2f, 1.0f), 
               0.5f);
#endif
      const tIndexedTriangle& triangle = mesh.GetTriangle(iTriangle);
      unsigned i0, i1, i2;
      triangle.GetVertexIndices(i0, i1, i2);
      const tVector3& v0 = mesh.GetVertex(i0);
      const tVector3& v1 = mesh.GetVertex(i1);
      const tVector3& v2 = mesh.GetVertex(i2);
      const tVector3& N = triangle.GetPlane().GetN();

      GLNORMAL3V(N.GetData());
      GLVERTEX3V(v0.GetData());
      GLVERTEX3V(v1.GetData());
      GLVERTEX3V(v2.GetData());
    }  
    glEnd();

    glEndList();
  }

  if (tDebugConfig::mRenderCollisionBoundingBoxes)
    RenderWorldBoundingBox();

  ApplyTransformation(mRenderPosition, mRenderOrientation);
  glCallList(mDisplayListNum);
}

//====================================================================
// AddToTriangles
//====================================================================
void tMeshObject::AddToTriangles(std::vector<tVector3>& triangleVertices, 
                                 std::vector<tTriangleVertexIndices>& triangleVertexIndices,
                                 const std::vector<tVector3>& vertices,
                                 const std::vector<tTriangleVertexIndices>& faces)
{
  unsigned offset = triangleVertices.size();
  unsigned i;
  for (i = 0 ; i < vertices.size() ; ++i)
    triangleVertices.push_back(vertices[i]);
  for (i = 0 ; i < faces.size() ; ++i)
  {
    if (faces[i].i0 >= vertices.size() ||faces[i].i1 >= vertices.size() ||faces[i].i2 >= vertices.size())
    {
      TRACE("face indices out of range!\n");
    }
    else
    {
      triangleVertexIndices.push_back(faces[i]);
      triangleVertexIndices.back().i0 += offset;
      triangleVertexIndices.back().i1 += offset;
      triangleVertexIndices.back().i2 += offset;
    }
  }
}

#ifdef NO_XML
//====================================================================
// LoadMesh
//====================================================================
void tMeshObject::LoadMesh(const char* meshXMLFile, 
                           std::vector<tVector3>& vertices, 
                           std::vector<tTriangleVertexIndices>& triangleVertexIndices)
{
  vertices.clear();
  triangleVertexIndices.clear();
  TRACE("Warning: no XML support so skipping mesh %s\n", meshXMLFile);
}

#else
//====================================================================
// ReadVertices
//====================================================================
void tMeshObject::ReadVertices(xmlTextReaderPtr reader, std::vector<JigLib::tVector3>& vertices, int numVertices)
{
  int numVerticesRead = 0;
  while (true)
  {
    int ret = xmlTextReaderRead(reader);

    if (xmlTextReaderNodeType(reader) == XML_ELEMENT_NODE)
    {
      const xmlChar *name = xmlTextReaderConstName(reader);
      if (name == NULL)
      {
        TRACE("Got null name\n");
        return;
      }
      if (xmlStrcmp(name, (const xmlChar*) "position") == 0)
      {
        // Dele3D coordinates are left-handed, and y is up... so the
        // mapping onto my coords is:
        // x -> -y
        // y -> z
        // z -> x
        tScalar x, y, z;
        const xmlChar* value;
        if (1 != xmlTextReaderMoveToNextAttribute(reader))
        {
          TRACE("Unable to read vertex x\n"); return;
        }
        name = xmlTextReaderConstName(reader);
        value = xmlTextReaderConstValue(reader);
        y = -(tScalar) atof((const char*) value);

        if (1 != xmlTextReaderMoveToNextAttribute(reader))
        {
          TRACE("Unable to read vertex y\n"); return;
        }
        name = xmlTextReaderConstName(reader);
        value = xmlTextReaderConstValue(reader);
        z = (tScalar) atof((const char*) value);
        
        if (1 != xmlTextReaderMoveToNextAttribute(reader))
        {
          TRACE("Unable to read vertex z\n"); return;
        }
        name = xmlTextReaderConstName(reader);
        value = xmlTextReaderConstValue(reader);
        x = (tScalar) atof((const char*) value);

        vertices.push_back(tVector3(x, y, z));
        if (vertices.size() == numVertices)
        {
          TRACE("Read %d vertices\n", numVertices);
          return;
        }
      }
      else if (xmlStrcmp(name, (const xmlChar*) "vertexbuffer") == 0)
      {
      }
      else if (xmlStrcmp(name, (const xmlChar*) "vertex") == 0)
      {
      }
      else if (xmlStrcmp(name, (const xmlChar*) "texcoord") == 0)
      {
      }
      else if (xmlStrcmp(name, (const xmlChar*) "normal") == 0)
      {
      }
      else
      {
        TRACE("Read unexpected value %s\n", name);
        return;
      }
    }
  }
}

//====================================================================
// ReadFaces
//====================================================================
void tMeshObject::ReadFaces(xmlTextReaderPtr reader, std::vector<JigLib::tTriangleVertexIndices>& faces, int numFaces)
{
  int numFacesRead = 0;
  while (true)
  {
    int ret = xmlTextReaderRead(reader);

    if (xmlTextReaderNodeType(reader) == XML_ELEMENT_NODE)
    {
      const xmlChar *name = xmlTextReaderConstName(reader);
      if (name == NULL)
      {
        TRACE("Got null name\n");
        return;
      }
      if (xmlStrcmp(name, (const xmlChar*) "face") == 0)
      {
        // need to flip vertices to convert winding
        int v1, v2, v3;
        const xmlChar* value;
        if (1 != xmlTextReaderMoveToNextAttribute(reader))
        {
          TRACE("Unable to read face index 1\n"); return;
        }
        name = xmlTextReaderConstName(reader);
        value = xmlTextReaderConstValue(reader);
        v2 = atoi((const char*) value);

        if (1 != xmlTextReaderMoveToNextAttribute(reader))
        {
          TRACE("Unable to read face index 2\n"); return;
        }
        name = xmlTextReaderConstName(reader);
        value = xmlTextReaderConstValue(reader);
        v1 = atoi((const char*) value);
        
        if (1 != xmlTextReaderMoveToNextAttribute(reader))
        {
          TRACE("Unable to read face index 3\n"); return;
        }
        name = xmlTextReaderConstName(reader);
        value = xmlTextReaderConstValue(reader);
        v3 = atoi((const char*) value);

        faces.push_back(tTriangleVertexIndices(v1, v2, v3));
        if (faces.size() == numFaces)
        {
          TRACE("Read %d faces\n", numFaces);
          return;
        }
      }
      else
      {
        TRACE("Read unexpected value %s\n", name);
        return;
      }
    }
  }
}

//====================================================================
// ProcessNode
//====================================================================
void tMeshObject::ProcessNode(xmlTextReaderPtr reader, 
                              std::vector<tVector3>& triangleVertices, 
                              std::vector<tTriangleVertexIndices>& triangleVertexIndices) 
{
  if (xmlTextReaderNodeType(reader) != XML_ELEMENT_NODE)
    return;

  const xmlChar *name = xmlTextReaderConstName(reader);
  if (name == NULL)
  {
    TRACE("Got null name\n");
    return;
  }

  static std::vector<tVector3> vertices;
  static std::vector<tTriangleVertexIndices> faces;

  if (xmlStrcmp(name, (const xmlChar*) "faces") == 0)
  {
    if (1 == xmlTextReaderMoveToNextAttribute(reader))
    {
      name = xmlTextReaderConstName(reader);
      const xmlChar* value = xmlTextReaderConstValue(reader);
      if (xmlStrcmp(name, (const xmlChar*) "count") == 0)
      {
        int numFaces = atoi((const char*) value);
        TRACE("Reading %d faces\n", numFaces);

        // now read them
        faces.clear();
        vertices.clear();
        ReadFaces(reader, faces, numFaces);
      }
      else
      {
        TRACE("Number of faces attribute should be called count\n");
      }
    }
    else
    {
      TRACE("Unable to get number of faces attribute\n");
    }
  }
  else if (xmlStrcmp(name, (const xmlChar*) "geometry") == 0)
  {
    if (1 == xmlTextReaderMoveToNextAttribute(reader))
    {
      name = xmlTextReaderConstName(reader);
      const xmlChar* value = xmlTextReaderConstValue(reader);
      if (xmlStrcmp(name, (const xmlChar*) "vertexcount") == 0)
      {
        int numVertices = atoi((const char*) value);
        TRACE("Reading %d vertices\n", numVertices);

        // now read them
        ReadVertices(reader, vertices, numVertices);

        // add em
        AddToTriangles(triangleVertices, triangleVertexIndices, vertices, faces);

        faces.clear();
        vertices.clear();
      }
      else
      {
        TRACE("Number of vertices attribute should be called vertexcount\n");
      }
    }
    else
    {
      TRACE("Unable to get number of faces attribute\n");
    }
  }
}

//====================================================================
// LoadMesh
//====================================================================
void tMeshObject::LoadMesh(const char* meshXMLFile, 
                           std::vector<tVector3>& vertices, 
                           std::vector<tTriangleVertexIndices>& triangleVertexIndices)
{
  vertices.clear();
  triangleVertexIndices.clear();

  // this initialize the library and check potential ABI mismatches
  // between the version it was compiled for and the actual shared
  // library used.
  LIBXML_TEST_VERSION;

  xmlTextReaderPtr reader = xmlReaderForFile(meshXMLFile, NULL,
                                             XML_PARSE_PEDANTIC);
  if (reader != NULL) 
  {
    int ret = xmlTextReaderRead(reader);
    while (ret == 1) 
    {
      ProcessNode(reader, vertices, triangleVertexIndices);
      ret = xmlTextReaderRead(reader);
    }
    /*
    // Once the document has been fully parsed check the validation results
    if (xmlTextReaderIsValid(reader) != 1) 
    {
    TRACE("Document %s does not validate\n", meshXMLFile);
    vertices.clear();
    triangleVertexIndices.clear();
    }
    */
    xmlFreeTextReader(reader);
  } 
  else 
  {
    TRACE("Unable to open %s\n", meshXMLFile);
  }
  xmlCleanupParser();
}

#endif
