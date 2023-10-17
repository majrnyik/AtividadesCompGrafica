#ifndef WINDOWHPP
#define WINDOWHPP

#include <random>
#include <cmath>
#include "abcgOpenGL.hpp"

class Window : public abcg::OpenGLWindow {
protected:
  void onCreate() override;
  void onPaint() override;
  void onPaintUI() override;
  void onResize(glm::ivec2 const &size) override;
  void onDestroy() override;

private:
  glm::ivec2 m_viewportSize{};

  GLuint m_VAO{};
  GLuint m_VBOPositions{};
  GLuint m_VBOColors{};
  GLuint m_program{};

  std::default_random_engine m_randomEngine;

  abcg::Timer m_timer;
  int m_delay{200};
  int m_sides{3};
  float m_scale{0.2f};
  float velocidade_atual{0.8f};
  float angulo_velocidade{};
  glm::vec2  posicao_atual{0.5f, 0.5f};
  float tempo_decorrido{0.0f};

  //carrega a informação se os angulos serão aleatórios ou não
  bool m_angulosAleatorios{false};

  void setupModel();
  void rebate();
};

#endif