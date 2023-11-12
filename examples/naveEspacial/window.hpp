#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include <random>

#include "abcgOpenGL.hpp"
#include "model.hpp"
#include "gamedata.hpp"


class Window : public abcg::OpenGLWindow {
protected:
  void onEvent(SDL_Event const &event) override;
  void onCreate() override;
  void onUpdate() override;
  void onPaint() override;
  void onPaintUI() override;
  void onResize(glm::ivec2 const &size) override;
  void onDestroy() override;
  

private:
  std::default_random_engine m_randomEngine;

  glm::ivec2 m_viewportSize{};

  Model m_modelAsteroide;
  Model m_modelNave;

  GameData m_gameData;

  struct Asteroide {
    glm::vec3 m_position{};
    glm::vec3 m_rotationAxis{};
  };

  std::array<Asteroide, 500> m_asteroides;

  float m_angle{};

  glm::mat4 m_viewMatrix{1.0f};
  glm::mat4 m_projMatrix{1.0f};
  float m_FOV{30.0f};
  glm::vec3 m_shipPosition = glm::vec3(0.0f, -0.12f, -1.0f);
  
  //defino os deixos de rotação da nave
  //ela não vai girar em Y, pois vai simular um avião fazendo curvas
  glm::vec3 m_axisZNave = glm::vec3(0.0f, 0.0f, 1.0f);
  float m_anguloZNave{0.0f};
  glm::vec3 m_axisXNave = glm::vec3(1.0f, 0.0f, 0.0f);
  float m_anguloXNave{0.0f};

  //para usermos mais de um modelo, precisamos instanciar programas individuais.
  //isso poderia ser feito com um vetor de programas, mas, por temos apenas 2 objetos, é mais organizado assim
  GLuint m_programAsteroide{};
  GLuint m_programNave{};

  void randomizeAsteroides(Asteroide &asteroide);
  void checkCollision(Asteroide &asteroide);
};

#endif