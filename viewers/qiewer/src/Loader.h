#ifndef LOADER_H
#define LOADER_H

#include <vector>
#include <string>

#include "mesh.h"

class Loader
{
public:
    Loader();

    Mesh* load_stl(std::istream &);
    Mesh* load_stl(const std::string &);

protected:

    Mesh* read_stl_ascii(std::istream &);
    Mesh* read_stl_binary(std::istream &);

private:
    const std::string _filename;
};

#endif // LOADER_H
