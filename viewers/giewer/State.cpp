#include "State.h"

#include "Mesh.h"
#include "Camera.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

State::State() : _camera(800.0f, 600.0f)
{
}

State::~State()
{
}

bool State::lock()
{
  _lock.lock();
  return true;
}

void State::unlock()
{
  _lock.unlock();
}

Camera &State::camera()
{
  return _camera;
}

std::vector<Mesh *> &State::meshes()
{
  return _meshes;
}

void State::tick(float dt)
{
  for (auto m : _meshes) {
    m->transform(glm::rotate(m->transform(), 0.01f, glm::vec3(0,1,0)));
  }
}
