#ifndef LOADER_H
#define LOADER_H

#include <QThread>

#include "mesh.h"

class Loader
{
public:
    explicit Loader(const QString& filename, bool is_reload = false);

    Mesh* load_stl();

protected:

    /*  Reads an ASCII stl, starting from the start of the file*/
    Mesh* read_stl_ascii(QFile& file);
    /*  Reads a binary stl, assuming we're at the end of the header */
    Mesh* read_stl_binary(QFile& file);

private:
    const QString filename;
};

#endif // LOADER_H
