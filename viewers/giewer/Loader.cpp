#include "Loader.h"
#include "glm/vec3.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

Loader::Loader()
{
}

////////////////////////////////////////////////////////////////////////////////

/*void parallel_sort(Vertex* begin, Vertex* end, int threads)
{
    if (threads < 2 || end - begin < 2)
    {
        std::sort(begin, end);
    }
    else
    {
        const auto mid = begin + (end - begin) / 2;
        if (threads == 2)
        {
            auto future = std::async(parallel_sort, begin, mid, threads / 2);
            std::sort(mid, end);
            future.wait();
        }
        else
        {
            auto a = std::async(std::launch::async, parallel_sort, begin, mid, threads / 2);
            auto b = std::async(std::launch::async, parallel_sort, mid, end, threads / 2);
            a.wait();
            b.wait();
        }
        std::inplace_merge(begin, mid, end);
    }
}*/

////////////////////////////////////////////////////////////////////////////////

std::vector<glm::vec3> Loader::load_stl(const std::string &filename)
{
  std::ifstream file(filename, std::ifstream::in);
  if (file.fail()) {
    throw std::runtime_error("missing/bad file: ");
  }

  return load_stl(file);
}

std::vector<glm::vec3> Loader::load_stl(std::istream &input)
{
  const int HEADER_LEN(5);
  char header[HEADER_LEN + 1];

  input.seekg(0);
  input.read(header, HEADER_LEN + 1);

  if (std::string(header, HEADER_LEN) == "solid")
  {
    std::string line;
    
    // Skip over rest of header line
    input >> std::skipws >> line;
    input >> std::skipws >> line;
    
    if (line.find("facet") == 0 ||
        line.find ("endsolid") == 0)
    {
        input.seekg(0);
        return read_stl_ascii(input);
    }
      // confusing_stl = true;
  }
  else
  {
      // confusing_stl = false;
  }

  input.seekg(0);
  return read_stl_binary(input);
}

std::vector<glm::vec3> Loader::read_stl_binary(std::istream &input)
{
  return {};
    /*QDataStream data(&file);
    data.setByteOrder(QDataStream::LittleEndian);
    data.setFloatingPointPrecision(QDataStream::SinglePrecision);

    // Load the triangle count from the .stl file
    file.seek(80);
    uint32_t tri_count;
    data >> tri_count;

    // Verify that the file is the right size
    if (file.size() != 84 + tri_count*50)
    {
        //emit error_bad_stl();
        return NULL;
    }

    // Extract vertices into an array of xyz, unsigned pairs
    QVector<Vertex> verts(tri_count*3);

    // Dummy array, because readRawData is faster than skipRawData
    std::unique_ptr<uint8_t> buffer(new uint8_t[tri_count * 50]);
    data.readRawData((char*)buffer.get(), tri_count * 50);

    // Store vertices in the array, processing one triangle at a time.
    auto b = buffer.get() + 3 * sizeof(float);
    for (auto v=verts.begin(); v != verts.end(); v += 3)
    {
        // Load vertex data from .stl file into vertices
        for (unsigned i=0; i < 3; ++i)
        {
            memcpy(&v[i], b, 3*sizeof(float));
            b += 3 * sizeof(float);
        }

        // Skip face attribute and next face's normal vector
        b += 3 * sizeof(float) + sizeof(uint16_t);
    }

    if (confusing_stl)
    {
        emit warning_confusing_stl();
    }

    return mesh_from_verts(tri_count, verts);*/
}

std::vector<glm::vec3> Loader::read_stl_ascii(std::istream &input)
{
  auto nextLine = [&input]() -> std::string {
    std::string line;

    while (input.peek() == ' ')
      input.get();

    do {
      std::getline(input, line);
    }
    while (line.size() == 0 && !input.eof());
    return line;
  };

  auto startsWith = [](std::string &s, std::string f) -> bool {
    return s.find(f) == 0;
  };

  uint32_t tri_count = 0;
  std::vector<glm::vec3> verts(tri_count*3);

  nextLine();
  bool okay = true;
  while (!input.eof() && okay)
  {
    std::string line = nextLine();
    if (startsWith(line, "endsolid")) {
      break;
    }
    
    if (!startsWith(line, "facet normal") ||
        !startsWith(nextLine(), "outer loop"))
    {
      okay = false;
      break;
    }

    for (int i=0; i < 3; ++i)
    {
      std::string tok;
      std::istringstream l(nextLine());
      
      l >> tok;
      if (tok != "vertex")
      {
        okay = false;
        break;
      }

      try {
        l >> tok; 
        const float x = std::stof(tok);
        l >> tok;
        const float y = std::stof(tok);
        l >> tok;
        const float z = std::stof(tok);
        verts.push_back(glm::vec3(x, y, z));
      }
      catch (std::invalid_argument) {
        okay = false;
      }
    }

    if (!startsWith(nextLine(), "endloop") ||
        !startsWith(nextLine(), "endfacet"))
    {
      okay = false;
      break;
    }

    tri_count++;
  }

  if (okay)
  {
      return verts;
  }
  else
  {
      //emit error_bad_stl();
      return {};
  }
}

