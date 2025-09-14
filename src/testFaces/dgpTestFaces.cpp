//
// Created by Eze on 9/13/2025.
//
#include <cassert>

#include "core/Faces.hpp"
//////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    int nV = 4;
    std::vector<int> coordIndex = {
        2, 1, 0, -1,
        3, 2, 0, -1,
        1, 3, 0, -1,
        2, 3, 1, -1
    };
    Faces faces(nV, coordIndex);

    assert(faces.getNumberOfVertices() == 4);
    assert(faces.getNumberOfFaces() == 4);
    assert(faces.getNumberOfCorners() == 12);
    assert(faces.getFaceSize(2) == 3);

    // getFaceFirstCorner
    assert(faces.getFaceFirstCorner(0) == 0);
    assert(faces.getFaceFirstCorner(1) == 4);
    assert(faces.getFaceFirstCorner(2) == 8);
    assert(faces.getFaceFirstCorner(3) == 12);
    assert(faces.getFaceFirstCorner(4) == -1);

    // getFaceVertex
    assert(faces.getFaceVertex(0, 0) == 2);
    assert(faces.getFaceVertex(0, 2) == 0);
    assert(faces.getFaceVertex(2, 9) == 3);
    assert(faces.getFaceVertex(2, 11) == -1);
    assert(faces.getFaceVertex(3, 14) == 1);

    // getCornerFace
    assert(faces.getCornerFace(0) == 0);
    assert(faces.getCornerFace(1) == 0);
    assert(faces.getCornerFace(2) == 0);
    assert(faces.getCornerFace(4) == 1);
    assert(faces.getCornerFace(5) == 1);
    assert(faces.getCornerFace(6) == 1);
    assert(faces.getCornerFace(8) == 2);
    assert(faces.getCornerFace(9) == 2);
    assert(faces.getCornerFace(10) == 2);
    assert(faces.getCornerFace(12) == 3);
    assert(faces.getCornerFace(13) == 3);
    assert(faces.getCornerFace(14) == 3);

    assert(faces.getCornerFace(3) == -1);
    assert(faces.getCornerFace(7) == -1);
    assert(faces.getCornerFace(11) == -1);
    assert(faces.getCornerFace(15) == -1);

    // getNextCorner
    assert(faces.getNextCorner(0) == 1);
    assert(faces.getNextCorner(1) == 2);
    assert(faces.getNextCorner(2) == 4);
    assert(faces.getNextCorner(3) == -1);

    return 0;
}