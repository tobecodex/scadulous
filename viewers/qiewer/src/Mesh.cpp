#include <QFile>
#include <QDataStream>
#include <QVector3D>

#include <cmath>

#include "Mesh.h"

////////////////////////////////////////////////////////////////////////////////

Mesh::Mesh(std::vector<float> &v, std::vector<unsigned int> &i)
    : vertices(std::move(v)), indices(std::move(i))
{
}