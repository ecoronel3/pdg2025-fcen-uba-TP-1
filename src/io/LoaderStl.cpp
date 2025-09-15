//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-08-04 22:12:21 gtaubin>
//------------------------------------------------------------------------
//
// LoaderStl.cpp
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

#include <stdio.h>
#include "TokenizerFile.hpp"
#include "LoaderStl.hpp"

#include <unordered_map>

#include "StrException.hpp"

#include "wrl/Shape.hpp"
#include "wrl/Appearance.hpp"
#include "wrl/Material.hpp"
#include "wrl/IndexedFaceSet.hpp"

// reference
// https://en.wikipedia.org/wiki/STL_(file_format)

const char* LoaderStl::_ext = "stl";

inline void hash_combine(std::size_t& seed) { }

template <typename T, typename... Rest>
inline void hash_combine(std::size_t& seed, const T& v, Rest... rest) {
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
  hash_combine(seed, rest...);
}

bool LoaderStl::load(const char* filename, SceneGraph& sceneGraph) {
  bool success = false;

  // clear the scene graph
  sceneGraph.clear();
  sceneGraph.setUrl("");

  FILE* fp = (FILE*)0;
  try {

    // open the file
    if(filename==(char*)0) throw StrException("filename==null");
    fp = fopen(filename,"r");
    if (fp== nullptr) throw StrException("fp==(FILE*)0");

    // use the io/Tokenizer class to parse the input ascii file

    TokenizerFile tkn(fp);
    // first token should be "solid"
    if(tkn.expecting("solid") && tkn.get()) {
      string stlName = tkn; // second token should be the solid name

      // TODO ...

      // create the scene graph structure:
      // 1) the SceneGraph should have a single Shape node a child
      auto* shapeNode = new Shape();
      sceneGraph.addChild(shapeNode);

      // 2) the Shape node should have an Appearance node in its appearance field
      auto* appearanceNode = new Appearance();
      shapeNode->setAppearance(appearanceNode);

      // 3) the Appearance node should have a Material node in its material field
      appearanceNode->setMaterial(new Material());

      // 4) the Shape node should have an IndexedFaceSet node in its geometry node
      auto* geometryNode = new IndexedFaceSet();
      geometryNode->setName(stlName);
      shapeNode->setGeometry(geometryNode);

      // from the IndexedFaceSet
      // 5) get references to the coordIndex, coord, and normal arrays
      std::vector<int>& coordIndex = geometryNode->getCoordIndex();
      std::vector<float>& coord = geometryNode->getCoord();
      std::vector<float>& normal = geometryNode->getNormal();

      // 6) set the normalPerVertex variable to false (i.e., normals per face)  
      geometryNode->setNormalPerVertex(false);

      // the file should contain a list of triangles in the following format

      // facet normal ni nj nk
      //   outer loop
      //     vertex v1x v1y v1z
      //     vertex v2x v2y v2z
      //     vertex v3x v3y v3z
      //   endloop
      // endfacet

      // - run an infinite loop to parse all the faces
      // - write a private method to parse each face within the loop
      // - the method should return true if successful, and false if not
      // - if your method returns true
      //     update the normal, coord, and coordIndex variables
      // - if your method returns false
      //     throw an StrException explaining why the method failed

      std::unordered_map<std::size_t, int> vertexMap;

      while(true) {
        if(tkn.expecting("facet") && tkn.expecting("normal")) {
          float ni, nj, nk;
          tkn.get();
          ni = std::strtof(tkn.c_str(), nullptr);

          tkn.get();
          nj = std::strtof(tkn.c_str(), nullptr);

          tkn.get();
          nk = std::strtof(tkn.c_str(), nullptr);

          normal.push_back(ni);
          normal.push_back(nj);
          normal.push_back(nk);

          if(tkn.expecting("outer") && tkn.expecting("loop")) {
            while (tkn.expecting("vertex")) {
              float vx, vy, vz;
              tkn.get();
              vx = std::strtof(tkn.c_str(), nullptr);

              tkn.get();
              vy = std::strtof(tkn.c_str(), nullptr);

              tkn.get();
              vz = std::strtof(tkn.c_str(), nullptr);

              std::size_t hash = 0;
              hash_combine(hash, vx, vy, vz);

              if (auto iter = vertexMap.find(hash); iter != vertexMap.end()) {
                coordIndex.push_back(iter->second);
              }
              else {
                coord.push_back(vx);
                coord.push_back(vy);
                coord.push_back(vz);
                coordIndex.push_back(static_cast<int>(vertexMap.size()));
                vertexMap.insert_or_assign(hash, static_cast<int>(vertexMap.size()));
              }
            }

            if(tkn != "endloop") {
                throw StrException("unexpected token: " + tkn);
            }
            coordIndex.push_back(-1);
          }

          tkn.get();
          if(tkn != "endfacet") {
            throw StrException("unexpected token: " + tkn);
          }
        }
        else
        {
            if (!tkn.empty())
            {
                throw StrException("unexpected token: " + tkn);
            }
            // EOF
            success = true;
            break;
        }
      }
    }

    // close the file (this statement may not be reached)
    fclose(fp);
    
  } catch(const StrException& e) {
    
    if( fp!= nullptr) fclose(fp);
    fprintf(stderr,"ERROR | %s\n",e.what());

  }

  return success;
}

