//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-08-04 22:09:56 gtaubin>
//------------------------------------------------------------------------
//
// Faces.cpp
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

#include <cmath>
#include "Faces.hpp"

#include <stdexcept>
#include <unordered_set>

Faces::Faces(const int nV, const vector<int>& coordIndex)
  : _coordIndex(coordIndex) {

  std::unordered_set<int> uniqueIndexes;
  int faceIndex = 0;
  int coordIndexStart = 0;
  int coordIndexEnd = 0;

  for(int i : _coordIndex) {
    if(i>=0) {
      uniqueIndexes.insert(i);
    }
    else {
      _faceMap.insert_or_assign(faceIndex, FaceCornerIndexes{ coordIndexStart, coordIndexEnd});
      ++faceIndex;
      coordIndexStart = coordIndexEnd + 1;
    }
    ++coordIndexEnd;
  }

  if (nV != uniqueIndexes.size()) {
    throw std::runtime_error("Faces::Faces: number of vertices and number of indices must be equal");
  }
  _numbOfVertices = nV;
}

int Faces::getNumberOfVertices() const {
  return _numbOfVertices;
}

int Faces::getNumberOfFaces() const {
  return static_cast<int>(_faceMap.size());
}

int Faces::getNumberOfCorners() const {
  // for the time being, we only support triangles
  return static_cast<int>(_faceMap.size()) * 3;
}

int Faces::getFaceSize(const int iF) const {

  if (!isValidFace(iF))
    return -1;

  if (const auto iter = _faceMap.find(iF); iter != _faceMap.end()) {
    return iter->second.end - iter->second.start;
  }
  return -1;
}

int Faces::getFaceFirstCorner(const int iF) const {
  if (!isValidFace(iF))
    return -1;

  if (const auto iter = _faceMap.find(iF); iter != _faceMap.end()) {
    return iter->second.start;
  }
  return -1;
}

int Faces::getFaceVertex(const int iF, const int iC) const {
  if (!isValidFace(iF))
    return -1;

  if (!isValidCorner(iC))
    return -1;

  if (const auto iter = _faceMap.find(iF); iter != _faceMap.end()) {
    if (iter->second.start <= iC && iC <= iter->second.end)
      return _coordIndex[iC];
    else
      return -1;
  }
  return -1;
}

int Faces::getCornerFace(const int iC) const {

  if (!isValidCorner(iC)) {
    return -1;
  }

  if (_coordIndex[iC] == -1)
    return -1;

  for (const auto& [iF, face] : _faceMap) {
    if (face.start <= iC && iC < face.end) {
      return iF;
    }
  }

  return -1;
}

int Faces::getNextCorner(const int iC) const {
  if (!isValidCorner(iC)) {
    return -1;
  }

  if (_coordIndex[iC] == -1)
    return -1;

  for (const auto& [iF, face] : _faceMap) {
    if (face.start <= iC && iC < face.end) {
      if (iC + 1 < face.end)
        return iC + 1;
      else {
        // it's in the next face
        if (iF + 1 < _faceMap.size())
          return _faceMap.at(iF + 1).start;
      }
    }
  }

  return -1;
}

bool Faces::isValidFace(const int iF) const
{
  return iF >= 0 && iF < static_cast<int>(_faceMap.size());
}

bool Faces::isValidCorner(const int iC) const
{
  return (iC >= 0 && iC < _coordIndex.size());
}

