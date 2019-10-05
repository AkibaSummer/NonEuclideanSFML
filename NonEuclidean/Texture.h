#pragma once
#include <SFML/OpenGL.hpp>

class Texture {
public:
  Texture(const char* fname, int rows, int cols);

  void Use();

private:
  GLuint texId;
  bool is3D;
};
