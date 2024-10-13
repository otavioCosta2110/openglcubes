#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <array>
#include <cstdio>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

void criarEsfera(float radius, int sectors, int stacks,
                 std::vector<float> &vertices,
                 std::vector<unsigned int> &indices, const glm::vec3 &color) {
  float x, y, z, xy;
  float nx, ny, nz, lengthInv = 1.0f / radius;
  float s, t;

  float sectorStep = 2 * M_PI / sectors;
  float stackStep = M_PI / stacks;
  float sectorAngle, stackAngle;

  for (int i = 0; i <= stacks; ++i) {
    stackAngle = M_PI / 2 - i * stackStep;
    xy = radius * cosf(stackAngle);
    z = radius * sinf(stackAngle);

    for (int j = 0; j <= sectors; ++j) {
      sectorAngle = j * sectorStep;

      x = xy * cosf(sectorAngle);
      y = xy * sinf(sectorAngle);
      vertices.push_back(x);
      vertices.push_back(y);
      vertices.push_back(z);

      // Push color
      vertices.push_back(color.r);
      vertices.push_back(color.g);
      vertices.push_back(color.b);

      // Push normal
      vertices.push_back(nx);
      vertices.push_back(ny);
      vertices.push_back(nz);

      // Texture coordinates
      s = (float)j / sectors;
      t = (float)i / stacks;
      vertices.push_back(s);
      vertices.push_back(t);
    }
  }

  unsigned int k1, k2;
  for (int i = 0; i < stacks; ++i) {
    k1 = i * (sectors + 1);
    k2 = k1 + sectors + 1;

    for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
      if (i != 0) {
        indices.push_back(k1);
        indices.push_back(k2);
        indices.push_back(k1 + 1);
      }
      if (i != (stacks - 1)) {
        indices.push_back(k2);
        indices.push_back(k2 + 1);
        indices.push_back(k1 + 1);
      }
    }
  }
}

void renderizarEsfera(unsigned int &sphereVAO, unsigned int &sphereVBO,
                      unsigned int &sphereEBO,
                      const std::vector<float> &sphereVertices,
                      const std::vector<unsigned int> &sphereIndices) {
  glGenVertexArrays(1, &sphereVAO);
  glGenBuffers(1, &sphereVBO);
  glGenBuffers(1, &sphereEBO);

  glBindVertexArray(sphereVAO);

  // Bind and set the vertex buffer
  glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
  glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float),
               sphereVertices.data(), GL_STATIC_DRAW);

  // Bind and set the element buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sphereIndices.size() * sizeof(unsigned int),
               sphereIndices.data(), GL_STATIC_DRAW);

  // Set the vertex attribute pointers
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
                        (void *)0);
  glEnableVertexAttribArray(0); // Position attribute

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1); // Color attribute

  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2); // Normal attribute

  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float),
                        (void *)(9 * sizeof(float)));
  glEnableVertexAttribArray(3); // Texture coordinates

  glBindVertexArray(0);
}
