#ifndef GLMESH_H
#define GLMESH_H

#include <QVector>

// forward declaration
class Mesh;

class GLMesh
{
public:
    GLMesh(const Mesh* const mesh);
    void draw(GLuint vp);
private:
	QVector<GLfloat> vertices;
};

#endif // GLMESH_H