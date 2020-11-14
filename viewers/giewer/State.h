#ifndef __STATE_H
#define __STATE_H

#include <mutex>
#include <vector>

class Mesh;
#include "Camera.h"

class State
{
private:
  std::mutex _lock;

  Camera _camera;
  std::vector <Mesh *> _meshes;

public:
  State();
  ~State();

  Camera &camera();
  std::vector<Mesh *> &meshes();

  bool lock();
  void unlock();

  void tick(float dt);
};

#endif