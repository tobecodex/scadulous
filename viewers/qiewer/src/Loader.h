#ifndef LOADER_H
#define LOADER_H

#include <vector>
#include <string>

#include "mesh.h"

class Loader
{
public:
    Loader();

    Mesh* load_stl(const std::string &);
    Mesh* load_stl(std::istream &);

protected:

    Mesh* read_stl_ascii(std::istream &);
    /*  Reads a binary stl, assuming we're at the end of the header */
    Mesh* read_stl_binary(std::istream &);

private:
    const std::string _filename;
};

#endif // LOADER_H
