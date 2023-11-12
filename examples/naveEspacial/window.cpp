#include "window.hpp"

#include <glm/gtc/random.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

void Window::onEvent(SDL_Event const &event){//
  //quando aperta tecla, a nave se move com velocidade constante.
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_UP)
      m_gameData.m_input.set(gsl::narrow<size_t>(Input::Up));
    if (event.key.keysym.sym == SDLK_DOWN)
      m_gameData.m_input.set(gsl::narrow<size_t>(Input::Down));
    if (event.key.keysym.sym == SDLK_LEFT)
      m_gameData.m_input.set(gsl::narrow<size_t>(Input::Left));
    if (event.key.keysym.sym == SDLK_RIGHT)
      m_gameData.m_input.set(gsl::narrow<size_t>(Input::Right));
    //eu vou mudar o estado do jogo para restart apenas se o jogador perder.
    //para reiniciar, basta clicar r
    if (event.key.keysym.sym == SDLK_r && m_gameData.m_state == State::GameOver)
        m_gameData.m_state = State::Playing;
  }
  //quando eu parar de apertar a tecla, quero que minha nave pare (jogo fica mais facil)
  if (event.type == SDL_KEYUP) {
    if (event.key.keysym.sym == SDLK_UP)
      m_gameData.m_input.reset(gsl::narrow<size_t>(Input::Up));
    if (event.key.keysym.sym == SDLK_DOWN)
      m_gameData.m_input.reset(gsl::narrow<size_t>(Input::Down));
    if (event.key.keysym.sym == SDLK_LEFT)
      m_gameData.m_input.reset(gsl::narrow<size_t>(Input::Left));
    if (event.key.keysym.sym == SDLK_RIGHT)
      m_gameData.m_input.reset(gsl::narrow<size_t>(Input::Right));
  }

}

void Window::onCreate() {
  auto const assetsPath{abcg::Application::getAssetsPath()};

  abcg::glClearColor(0, 0, 0, 1);
  abcg::glEnable(GL_DEPTH_TEST);

 //instancio a camera
 //importante notar que ela olha para z negativo
  glm::vec3 const eye{0.0f, 0.0f, 0.0f};
  glm::vec3 const at{0.0f, 0.0f, -1.0f};
  glm::vec3 const up{0.0f, 1.0f, 0.0f};
  m_viewMatrix = glm::lookAt(eye, at, up);

  


//crio o programa do asteroide
  m_programAsteroide =
      abcg::createOpenGLProgram({{.source = assetsPath + "depth.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "depth.frag",
                                  .stage = abcg::ShaderStage::Fragment}});



//com o programa criado, eu carrego o modelo .obj
//crio o VAO e preencho o vetor de asteroides com vários asteroides
  m_modelAsteroide.loadObj(assetsPath + "10464_Asteroid_v1_Iterations-2.obj");
  m_modelAsteroide.setupVAO(m_programAsteroide);
  // Setup asteroides
  for (auto &asteroide : m_asteroides) {
    randomizeAsteroides(asteroide);
  }
    
    //tenho que fazer a mesma coisa para a nave
    //criar programa, definir vertex e fragmente shader (os mesmos), carregar vao
m_programNave =
    abcg::createOpenGLProgram({{.source = assetsPath + "depth.vert",
                                .stage = abcg::ShaderStage::Vertex},
                                {.source = assetsPath + "depth.frag",
                                .stage = abcg::ShaderStage::Fragment}});

  m_modelNave.loadObj(assetsPath + "nave.obj");
  m_modelNave.setupVAO(m_programNave);

  

  
}



    
void Window::randomizeAsteroides(Asteroide &asteroide) {
  //crio os asteroides em um paralelepípedo de 40 por 40 por 100
  //na hora de criar os asteroides, devo cria-los completamente espalhados pelo paralelepípedo
  std::uniform_real_distribution<float> distPosXY(-20.0f, 20.0f);
  std::uniform_real_distribution<float> distPosZ(-100.0f, 0.0f);

  asteroide.m_position =
      glm::vec3(distPosXY(m_randomEngine), distPosXY(m_randomEngine),
                distPosZ(m_randomEngine));

    //usado para testar a distancia de colisão
    //asteroide.m_position = glm::vec3(0.0f, 0.0f, -50.0f);

  // dou uma angulo inicial de rotação a todos os asteroides
  asteroide.m_rotationAxis = glm::sphericalRand(1.0f);
}

void Window::onUpdate() {
  //aqui eu atualizo a posição e a rotação dos asteroides
  //aumento o angulo em 90 graus por segundo de cada asteroide
  //o angulo inicial é diferente, mas a velocidade de rotação é igual para todos
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};
  m_angle = glm::wrapAngle(m_angle + glm::radians(90.0f) * deltaTime);

  // Update asteroides
  for (auto &asteroide : m_asteroides) {
    // faço eles se moveremna direção da nave em 10 unidades por segundo
    asteroide.m_position.z += deltaTime * 10.0f;

    //zero os angulos de rotação para chegarmos ao angulo correto em cada um dos eixos
    m_anguloZNave = 0.0f;
    m_anguloXNave = 0.0f;

    //eu movo os asteroides para dar a impressão de movimento da nave
    //eles vão se mover no sentido oposto ao input do jogador
    //se eu clicar para a nave ir para a direita, devo mosver meus asteroides para a esquerda
    //movo eles em velocidade uniforme, sem implementar nenhuma aceleração
    if (m_gameData.m_input[gsl::narrow<size_t>(Input::Left)]){
        asteroide.m_position.x += deltaTime * 3.0f;
        //faco o angulo assim para ele ser zerado caso o usuário esteja apertando duas teclas opostas ao mesmo tempo
        m_anguloZNave += 20.0f;
    }
    if (m_gameData.m_input[gsl::narrow<size_t>(Input::Right)]){
        asteroide.m_position.x -= deltaTime * 3.0f;
        m_anguloZNave -= 20.0f;
    }
    if (m_gameData.m_input[gsl::narrow<size_t>(Input::Up)]){
        asteroide.m_position.y -= deltaTime * 3.0f;
        m_anguloXNave += 20.0f;
    }
    if (m_gameData.m_input[gsl::narrow<size_t>(Input::Down)]){
        asteroide.m_position.y += deltaTime * 3.0f;
        m_anguloXNave -= 20.0f;
    }

    //após mover o asteroide, eu verifico se ele bateu na nave
    checkCollision(asteroide);

    //se o asteroide saiu do campo de visão da camera, eu retorno ele para a posição -100 em z
    if (asteroide.m_position.z > 0.1f) {
      randomizeAsteroides(asteroide);
      asteroide.m_position.z = -100.0f; 
    }
    //se o asteroide sair do nosso paralelepípedo, devo coloca-lo no outro extremo do paralelepipedo
    //e.g.: se ele atingiu x=20, eu envio ele para x =-20
    //dessa forma, vai parecer que temos um campo infinito de asteroides
    //os asteroides podem sair do paralelepípedo pois movo eles para dar a impressão de mover a nave
    if (asteroide.m_position.x > 20.0f)     
      asteroide.m_position.x = -20.0f; 
    
    if (asteroide.m_position.x < -20.0f) 
      asteroide.m_position.x = 20.0f; 
    
    if (asteroide.m_position.y > 20.0f) 
      asteroide.m_position.y = -20.0f; 
    
    if (asteroide.m_position.y < -20.0f) 
      asteroide.m_position.y = 20.0f; 
    

  }
}   

void Window::checkCollision(Asteroide &asteroide){
     
    auto const distance{
        glm::distance(m_shipPosition, asteroide.m_position)
    };

    //essa distância foi decidida na tentativa e erro. Acho que ficou boa.
    if (distance < 0.5f) {
        m_gameData.m_state = State::GameOver;
        //m_restartWaitTimer.restart();
    }
    
}

void Window::onPaint() {
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

    //se o estado do jogo não for playing (gameOVer ou restart) eu não
  if(m_gameData.m_state != State::Playing)
    return;

  abcg::glUseProgram(m_programAsteroide);


  //Defino a matriz de perspectiva. Ela ficará travada em 30 graus
    auto const aspect{gsl::narrow<float>(m_viewportSize.x) / gsl::narrow<float>(m_viewportSize.y)};
    m_projMatrix = glm::perspective(glm::radians(m_FOV), aspect, 0.01f, 100.0f);

  // Get location of uniform variables
  auto const viewMatrixLoc{abcg::glGetUniformLocation(m_programAsteroide, "viewMatrix")};
  auto const projMatrixLoc{abcg::glGetUniformLocation(m_programAsteroide, "projMatrix")};
  auto const modelMatrixLoc{
      abcg::glGetUniformLocation(m_programAsteroide, "modelMatrix")};
  auto const colorLoc{abcg::glGetUniformLocation(m_programAsteroide, "color")};

  // Set uniform variables that have the same value for every model
  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
  abcg::glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f); // White

  // Render each asteroide
  for (auto &asteroide : m_asteroides) {
    // Compute model matrix of the current asteroide
    glm::mat4 modelMatrix{1.0f};
    modelMatrix = glm::translate(modelMatrix, asteroide.m_position);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.6f));
    modelMatrix = glm::rotate(modelMatrix, m_angle, asteroide.m_rotationAxis);

    // Set uniform variable
    abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);
    m_modelAsteroide.render();
  }

  abcg::glUseProgram(m_programNave);
  auto const viewMatrixLocNave{abcg::glGetUniformLocation(m_programNave, "viewMatrix")};
  auto const projMatrixLocNave{abcg::glGetUniformLocation(m_programNave, "projMatrix")};
  auto const modelMatrixLocNave{
      abcg::glGetUniformLocation(m_programNave, "modelMatrix")};
  auto const colorLocNave{abcg::glGetUniformLocation(m_programNave, "color")};

  glm::mat4 modelMatrixNave{1.0f};
  modelMatrixNave = glm::translate(modelMatrixNave, m_shipPosition);
  modelMatrixNave = glm::scale(modelMatrixNave, glm::vec3(0.15f));
  //agora faço duas rotações: uma no eixo z e uma em x
  //o anguo foi definido de acordo com o input do usuário
  //o angulo sempre será 20 graus, para dar uma eve inclinada na nave
  modelMatrixNave = glm::rotate(modelMatrixNave, glm::radians(m_anguloXNave), m_axisXNave);
  modelMatrixNave = glm::rotate(modelMatrixNave, glm::radians(m_anguloZNave), m_axisZNave);

  // Set uniform variables that have the same value for every model
  abcg::glUniformMatrix4fv(viewMatrixLocNave, 1, GL_FALSE, &m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLocNave, 1, GL_FALSE, &m_projMatrix[0][0]);
  abcg::glUniform4f(colorLocNave, 1.0f, 1.0f, 1.0f, 1.0f); // White
  abcg::glUniformMatrix4fv(modelMatrixLocNave, 1, GL_FALSE, &modelMatrixNave[0][0]);

  m_modelNave.render();

  abcg::glUseProgram(0);
}

void Window::onPaintUI() {
  abcg::OpenGLWindow::onPaintUI();

  
    {
        //defino uma janela de 150 por 150 para aparecer o texto de game over
        const auto size{ImVec2(150, 150)};
        //posiciono ela bem no centro da tela
        const auto position{ImVec2((m_viewportSize.x - size.x) / 2.0f,
                                (m_viewportSize.y - size.y) / 2.0f)};
        ImGui::SetNextWindowPos(position);
        ImGui::SetNextWindowSize(size);
        //torno ela "invisivel". Só aparecerá o texto quando perder
        ImGuiWindowFlags flags{ImGuiWindowFlags_NoBackground |
                            ImGuiWindowFlags_NoTitleBar |
                            ImGuiWindowFlags_NoInputs};
        ImGui::Begin(" ", nullptr, flags);
        //imprimo o game over
        if (m_gameData.m_state == State::GameOver) {
        ImGui::Text("Game Over!");
    }
    
    ImGui::End();
  }
}
   
void Window::onResize(glm::ivec2 const &size) { m_viewportSize = size; }

void Window::onDestroy() {
  m_modelAsteroide.destroy();
  m_modelNave.destroy();
  abcg::glDeleteProgram(m_programAsteroide);
  abcg::glDeleteProgram(m_programNave);
}