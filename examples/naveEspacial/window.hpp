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
    float m_asteroidScale;
  };

  std::array<Asteroide, 500> m_asteroides;


  float m_angle{};

  glm::mat4 m_viewMatrix{1.0f};
  glm::mat4 m_projMatrix{1.0f};
  glm::mat4 m_modelMatrix{1.0f};


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
 

  glm::vec4 m_lightPos{5.0f, 5.0f, 0.0f, 1.0f};

  //as características de reflexão de luz de cada materia
  //asteroide (preda)
  glm::vec4 m_IaAsteroide{1.0f};
  glm::vec4 m_IdAsteroide{1.0f};
  glm::vec4 m_IsAsteroide{0.1f};//vai causar um brilho muuito baixinho
  glm::vec4 m_KaAsteroide{0.1f, 0.1f, 0.1f, 1.0f};
  glm::vec4 m_KdAsteroide{0.7f, 0.7f, 0.7f, 1.0f};
  glm::vec4 m_KsAsteroide{1.0f};
  //shininess baixa pois é de pedra
  float m_shininessAsteroide{5.0f};
  //nave(metal)
  glm::vec4 m_IaNave{1.0f};
  glm::vec4 m_IdNave{1.0f};
  glm::vec4 m_IsNave{1.5f};
  glm::vec4 m_KaNave{0.1f, 0.1f, 0.1f, 1.0f};
  glm::vec4 m_KdNave{0.7f, 0.7f, 0.7f, 1.0f};
  glm::vec4 m_KsNave{1.0f};
  //shininess alta pois é de metal
  float m_shininessNave{30.0f};

  void randomizeAsteroides(Asteroide &asteroide);
  void checkCollisionAsteroide(Asteroide &asteroide);
  void loadModel(std::string_view path);
};

#endif