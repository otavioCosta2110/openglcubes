#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

void criarCubo(float tamanho, std::vector<float> &vertices, std::vector<unsigned int> &indices, const glm::vec3 &color) {
    float metade = tamanho / 2.0f;

    vertices = {
        // Posições           // Cores
        // Frente
        -metade, -metade,  metade, color.r, color.g, color.b,
         metade, -metade,  metade, color.r, color.g, color.b,
         metade,  metade,  metade, color.r, color.g, color.b,
        -metade,  metade,  metade, color.r, color.g, color.b,

        // Trás
        -metade, -metade, -metade, color.r, color.g, color.b,
         metade, -metade, -metade, color.r, color.g, color.b,
         metade,  metade, -metade, color.r, color.g, color.b,
        -metade,  metade, -metade, color.r, color.g, color.b,
    };

    indices = {
        // Frente
        0, 1, 2, 2, 3, 0,
        // Trás
        4, 5, 6, 6, 7, 4,
        // Esquerda
        0, 3, 7, 7, 4, 0,
        // Direita
        1, 5, 6, 6, 2, 1,
        // Topo
        3, 2, 6, 6, 7, 3,
        // Base
        0, 1, 5, 5, 4, 0,
    };
}

void renderizarCubo(unsigned int &cubeVAO, unsigned int &cubeVBO,
                    unsigned int &cubeEBO,
                    const std::vector<float> &cubeVertices,
                    const std::vector<unsigned int> &cubeIndices) {
  // Gerar VAO, VBO e EBO para o cubo
  glGenVertexArrays(1, &cubeVAO);
  glGenBuffers(1, &cubeVBO);
  glGenBuffers(1, &cubeEBO);

  // Ligar o VAO do cubo
  glBindVertexArray(cubeVAO);

  // Ligar e enviar dados de vértices para o VBO
  glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
  glBufferData(GL_ARRAY_BUFFER, cubeVertices.size() * sizeof(float),
               cubeVertices.data(), GL_STATIC_DRAW);

  // Ligar e enviar dados de índices para o EBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               cubeIndices.size() * sizeof(unsigned int),
               cubeIndices.data(), GL_STATIC_DRAW);

  // Atributo de posição (3 floats)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // Atributo de cor (3 floats)
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // Desligar o VAO
  glBindVertexArray(0);
}

