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

void criarCubo(float tamanho, std::vector<float> &vertices,
               std::vector<unsigned int> &indices, const glm::vec3 &color);

void renderizarCubo(unsigned int &cubeVAO, unsigned int &cubeVBO,
                    unsigned int &cubeEBO,
                    const std::vector<float> &cubeVertices,
                    const std::vector<unsigned int> &cubeIndices);

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

  int num_cubos = 5;
  std::vector<std::vector<float>> cuboVerticesList(num_cubos);
  std::vector<std::vector<unsigned int>> cuboIndicesList(num_cubos);

  std::array<glm::vec3, 5> cuboCores = {
      glm::vec3(1.0f, 0.0f, 0.0f), // Vermelho
      glm::vec3(0.0f, 1.0f, 0.0f), // Verde
      glm::vec3(0.0f, 0.0f, 1.0f), // Azul
      glm::vec3(1.0f, 1.0f, 0.0f), // Amarelo
      glm::vec3(1.0f, 0.5f, 0.0f)  // Laranja
  };

  std::vector<glm::vec3> cuboPosicoes = {
      glm::vec3(9.0f, 2.0f, -1.0f), // Vermelho
      glm::vec3(-1.0f, 0.6f, 2.0f), // Verde
      glm::vec3(-3.0f, 4.0f, 2.0f), // Azul
      glm::vec3(2.0f, 3.0f, 4.0f),  // Amarelo
      glm::vec3(-5.0f, 2.0f, 5.0f), // Laranja
  };

  std::vector<float> cuboTamanhos = {
      1.0f, // Tamanho do cubo Vermelho
      1.5f, // Tamanho do cubo Verde
      0.5f, // Tamanho do cubo Azul
      2.0f, // Tamanho do cubo Amarelo
      1.2f  // Tamanho do cubo Laranja
  };

  for (int i = 0; i < num_cubos; ++i) {
    std::vector<float> cuboVertices;
    std::vector<unsigned int> cuboIndices;

    criarCubo(cuboTamanhos[i], cuboVertices, cuboIndices, cuboCores[i]);

    cuboVerticesList[i] = std::move(cuboVertices);
    cuboIndicesList[i] = std::move(cuboIndices);
  }

  std::vector<unsigned int> cuboVAOs(num_cubos);
  std::vector<unsigned int> cuboVBOs(num_cubos);
  std::vector<unsigned int> cuboEBOs(num_cubos);

  for (int i = 0; i < num_cubos; ++i) {
    renderizarCubo(cuboVAOs[i], cuboVBOs[i], cuboEBOs[i], cuboVerticesList[i],
                     cuboIndicesList[i]);
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

        glBindVertexArray(VAO);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE,
                           glm::value_ptr(glm::mat4(1.0f)));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE,
                           glm::value_ptr(projection));
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        for (int i = 0; i < num_cubos; ++i) {
            glm::mat4 cuboModel = glm::mat4(1.0f);
            cuboModel = glm::translate(cuboModel, cuboPosicoes[i]);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cuboModel));

            glBindVertexArray(cuboVAOs[i]);
            glDrawElements(GL_TRIANGLES, cuboIndicesList[i].size(),
                           GL_UNSIGNED_INT, 0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

  // Limpeza e encerramento
  glfwTerminate();

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);

  for (int i = 0; i < num_cubos; i++) {
    glDeleteVertexArrays(1, &cuboVAOs[i]);
    glDeleteBuffers(1, &cuboVBOs[i]);
    glDeleteBuffers(1, &cuboEBOs[i]);
  }

  return 0;
}
