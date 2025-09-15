//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-08-04 22:14:44 gtaubin>
//------------------------------------------------------------------------
//
// SaverStl.cpp
//
// Written by: <Your Name>
//
// Software developed for the course
// Digital Geometry Processing
// Copyright (c) 2025, Gabriel Taubin
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the Brown University nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL GABRIEL TAUBIN BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "SaverStl.hpp"

#include <format>
#include <fstream>

#include "wrl/Shape.hpp"
#include "wrl/Appearance.hpp"
#include "wrl/Material.hpp"
#include "wrl/IndexedFaceSet.hpp"

#include "core/Faces.hpp"

const char* SaverStl::_ext = "stl";

//////////////////////////////////////////////////////////////////////
bool SaverStl::save(const char* filename, SceneGraph& sceneGraph) const {
  return save(std::filesystem::path(filename), sceneGraph );
}

bool SaverStl::save(const std::filesystem::path& filename, SceneGraph& sceneGraph) const
{

  // Check these conditions

  // 1) the SceneGraph should have a single child
  const std::vector<pNode>& sgChildren = sceneGraph.getChildren();
  if (sgChildren.size() != 1) {
    return false;
  }

  Node* sgChild = sgChildren[0];

  // 2) the child should be a Shape node
  if (!sgChild->isShape()) {
    return false;
  }

  // 3) the geometry of the Shape node should be an IndexedFaceSet node
  auto* shapeNode = static_cast<Shape*>(sgChild);
  if (!shapeNode->getGeometry() || !shapeNode->getGeometry()->isIndexedFaceSet()) {
    return false;
  }
  auto* ifs = static_cast<IndexedFaceSet*>(shapeNode->getGeometry());

  // 4) the IndexedFaceSet should be a triangle mesh
  if (!ifs->isTriangleMesh()) {
    return false;
  }

  // 5) the IndexedFaceSet should have normals per face
  if (ifs->getNormalPerVertex()
    || ifs->getNumberOfNormal() != ifs->getNumberOfFaces()
    || !ifs->getNormalIndex().empty()) {
    return false;
  }

  // construct an instance of the Faces class from the IndexedFaceSet
  // remember to delete it when you are done with it (if necessary) before returning
  Faces faces(ifs->getNumberOfCoord(),  ifs->getCoordIndex());

  if (!exists(filename)) {
    // warning, overwriting file
  }

  std::ofstream out(filename.string(), std::ios::out | std::ios::trunc);
  if (out.is_open()) {
    // if set, use ifs->getName()
    // otherwise use filename,
    // but first remove directory and extension

    std::string name = ifs->getName();
    name = name.empty() ?
      filename.stem().string()
        : name;

    out << "solid " << name << std::endl;

    const int numbOfFaces = faces.getNumberOfFaces();
    for (int iF = 0; iF < numbOfFaces; iF++) {

      const std::vector<float>& normals = ifs->getNormal();
      const std::vector<float>& coords = ifs->getCoord();

      out << std::format("facet normal {:.6e} {:.6e} {:.6e}\n", normals[3*iF], normals[3*iF + 1], normals[3*iF + 2]);

      out << "  outer loop\n";
      int iC = faces.getFaceFirstCorner(iF);
      int numbCorners = faces.getFaceSize(iF);
      for (int j = 0; j < numbCorners; j++) {

        if (iC == -1)
          break;

        const int coordIndex = faces.getFaceVertex(iF, iC);
        out << std::format("    vertex {:.6e} {:.6e} {:.6e}\n", coords[3*coordIndex], coords[3*coordIndex + 1], coords[3*coordIndex + 2]);
        iC = faces.getNextCorner(iC);
      }
      out << "  endloop\n";
      out << "endfacet\n";
    }

  }
  out.close();

  return true;
}
