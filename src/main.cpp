/* Este programa cria um cenário simples com um terreno plano verde usando
 * OpenGL moderno com GLEW, GLFW e GLM. O terreno é renderizado como um quadrado
 * plano no plano XZ, e a câmera está posicionada acima e atrás do terreno,
 * olhando para o centro.*/

#include "init.cpp"
#include "utils.cpp"
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
                 std::vector<unsigned int> &indices, const glm::vec3 &color);

void renderizarEsfera(unsigned int &sphereVAO, unsigned int &sphereVBO,
                      unsigned int &sphereEBO,
                      const std::vector<float> &sphereVertices,
                      const std::vector<unsigned int> &sphereIndices);

int main() {
  // Inicialização do GLFW
  if (!glfwInit()) {
    std::cerr << "Falha ao inicializar GLFW" << std::endl;
    return -1;
  }

  // Configuração do GLFW
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Criação da janela
  GLFWwindow *window =
      glfwCreateWindow(WIDTH, HEIGHT, "Terreno Plano OpenGL", NULL, NULL);
  if (window == NULL) {
    std::cerr << "Falha ao criar janela GLFW" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // Inicialização do GLEW
  if (glewInit() != GLEW_OK) {
    std::cerr << "Falha ao inicializar GLEW" << std::endl;
    return -1;
  }

  // Vertex Shader
  const char *vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
    out vec3 ourColor;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    void main() {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        ourColor = aColor;
    }
)";

  // Fragment Shader
  const char *fragmentShaderSource = R"(
#version 330 core
in vec3 ourColor;
out vec4 FragColor;
void main() {
    FragColor = vec4(ourColor, 1.0);
}
)";

  // Compilação e link dos shaders
  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  unsigned int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // Criação do terreno plano
  std::vector<float> vertices = {// Posições            // Cores
                                 -10.0f, 0.0f, -10.0f, 0.0f, 0.8f, 0.0f,
                                 10.0f,  0.0f, -10.0f, 0.0f, 0.8f, 0.0f,
                                 10.0f,  0.0f, 10.0f,  0.0f, 0.8f, 0.0f,
                                 -10.0f, 0.0f, 10.0f,  0.0f, 0.8f, 0.0f};

  std::vector<unsigned int> indices = {0, 1, 2, 2, 3, 0};

  unsigned int VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
               vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // Configuração da matriz de projeção
  glm::mat4 projection = glm::perspective(
      glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

  // Configuração da matriz de visualização
  glm::mat4 view =
      glm::lookAt(glm::vec3(0.0f, 5.0f, 15.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, 1.0f, 0.0f));

  // Configuração da matriz de modelo
  glm::mat4 model = glm::mat4(1.0f);

  int num_esferas = 5;

  std::vector<std::vector<float>> sphereVerticesList(num_esferas);
  std::vector<std::vector<unsigned int>> sphereIndicesList(num_esferas);

  std::array<glm::vec3, 5> esferaCores = {
      glm::vec3(1.0f, 0.0f, 0.0f), // Vermelho
      glm::vec3(0.0f, 1.0f, 0.0f), // Verde
      glm::vec3(0.0f, 0.0f, 1.0f), // Azul
      glm::vec3(1.0f, 1.0f, 0.0f), // Amarelo
      glm::vec3(1.0f, 0.5f, 0.0f)  // Laranja
  };
  std::vector<glm::vec3> esferaPosicoes = {
      glm::vec3(9.0f, 2.0f, -1.0f), // Vermelho
      glm::vec3(-1.0f, 0.6f, 2.0f), // Verde
      glm::vec3(-3.0f, 4.0f, 2.0f), // Azul
      glm::vec3(2.0f, 3.0f, 4.0f),  // Amarelo
      glm::vec3(-5.0f, 2.0f, 5.0f), // Laranja
  };

  std::vector<float> esferaTamanhos = {
      1.0f, // Tamanho da esfera Vermelha
      1.5f, // Tamanho da esfera Verde
      0.5f, // Tamanho da esfera Azul
      2.0f, // Tamanho da esfera Amarela
      1.2f  // Tamanho da esfera Laranja
  };

  // Criação das esferas
  for (int i = 0; i < num_esferas; ++i) {
    std::vector<float> sphereVertices;
    std::vector<unsigned int> sphereIndices;

    criarEsfera(esferaTamanhos[i], 30, 30, sphereVertices, sphereIndices,
                esferaCores[i]);

    sphereVerticesList[i] = std::move(sphereVertices);
    sphereIndicesList[i] = std::move(sphereIndices);
  }

  // Vetores de VAOs, VBOs e EBOs para as esferas
  // Precisa disso pra renderizar cada esfera individualmente, ai da pra colocar
  // cor, tamanho, etc
  std::vector<unsigned int> sphereVAOs(num_esferas);
  std::vector<unsigned int> sphereVBOs(num_esferas);
  std::vector<unsigned int> sphereEBOs(num_esferas);

  for (int i = 0; i < num_esferas; ++i) {
    renderizarEsfera(sphereVAOs[i], sphereVBOs[i], sphereEBOs[i],
                     sphereVerticesList[i], sphereIndicesList[i]);
  }

  // Loop principal
  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);

    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    unsigned int projectionLoc =
        glGetUniformLocation(shaderProgram, "projection");

    // Renderiza o terreno
    glBindVertexArray(VAO);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    for (int i = 0; i < num_esferas; ++i) {
      glm::mat4 sphereModel = glm::mat4(1.0f);
      sphereModel = glm::translate(sphereModel, esferaPosicoes[i]);
      glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(sphereModel));

      glBindVertexArray(sphereVAOs[i]);
      glDrawElements(GL_TRIANGLES, sphereIndicesList[i].size(), GL_UNSIGNED_INT,
                     0);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Limpeza e encerramento
  glfwTerminate();

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);

  for (int i = 0; i < num_esferas; i++) {
    glDeleteVertexArrays(1, &sphereVAOs[i]);
    glDeleteBuffers(1, &sphereVBOs[i]);
    glDeleteBuffers(1, &sphereEBOs[i]);
  }

  return 0;
}
