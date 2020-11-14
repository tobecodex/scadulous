#ifndef LOADER_H
#define LOADER_H

#include <vector>
#include <string>
#include <glm/vec3.hpp>

class Loader
{
public:
    Loader();

    std::vector<glm::vec3> load_stl(std::istream &);
    std::vector<glm::vec3> load_stl(const std::string &);

protected:

    std::vector<glm::vec3> read_stl_ascii(std::istream &);
    std::vector<glm::vec3> read_stl_binary(std::istream &);

private:
    const std::string _filename;
};

#endif // LOADER_H
