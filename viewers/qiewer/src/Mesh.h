#ifndef MESH_H
#define MESH_H

#include <vector>

class Mesh
{
public:
    Mesh(std::vector<float> &vertices, std::vector<unsigned int> &indices);

    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};

#endif // MESH_H
