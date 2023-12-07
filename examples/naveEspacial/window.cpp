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
      abcg::createOpenGLProgram({{.source = assetsPath + "texture.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "texture.frag",
                                  .stage = abcg::ShaderStage::Fragment}});



//com o programa criado, eu carrego o modelo .obj
//crio o VAO e preencho o vetor de asteroides com vários asteroides
  m_modelAsteroide.loadDiffuseTexture(assetsPath + "textura_preda.jpg");
  m_modelAsteroide.loadObj(assetsPath + "10464_Asteroid_v1_Iterations-2.obj");
  
  m_modelAsteroide.setupVAO(m_programAsteroide);
  // Setup asteroides
  for (auto &asteroide : m_asteroides) {
    randomizeAsteroides(asteroide);
  }


    //tenho que fazer a mesma coisa para a nave
    //criar programa, definir vertex e fragmente shader (os mesmos), carregar vao
m_programNave =
    abcg::createOpenGLProgram({{.source = assetsPath + "texture.vert",
                                .stage = abcg::ShaderStage::Vertex},
                                {.source = assetsPath + "texture.frag",
                                .stage = abcg::ShaderStage::Fragment}});
  m_modelNave.loadDiffuseTexture(assetsPath + "textura_nave_principal.jpg");
  m_modelNave.loadObj(assetsPath + "nave.obj");
  m_modelNave.setupVAO(m_programNave);

  
}

    
void Window::randomizeAsteroides(Asteroide &asteroide) {
  //crio os asteroides em um paralelepípedo de 40 por 40 por 100
  //na hora de criar os asteroides, devo cria-los completamente espalhados pelo paralelepípedo
  std::uniform_real_distribution<float> distPosXY(-20.0f, 20.0f);
  std::uniform_real_distribution<float> distPosZ(-100.0f, 0.0f);
  std::uniform_real_distribution<float> scale(0.2f, 1.5f);

  asteroide.m_position =
      glm::vec3(distPosXY(m_randomEngine), distPosXY(m_randomEngine),
                distPosZ(m_randomEngine));

  // dou uma angulo inicial de rotação a todos os asteroides
  asteroide.m_rotationAxis = glm::sphericalRand(1.0f);
  asteroide.m_asteroidScale = scale(m_randomEngine);
}


void Window::onUpdate() {
  //aqui eu atualizo a posição e a rotação dos asteroides
  //aumento o angulo em 90 graus por segundo de cada asteroide
  //o angulo inicial é diferente, mas a velocidade de rotação é igual para todos
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};
  m_angle = glm::wrapAngle(m_angle + glm::radians(90.0f) * deltaTime);

  //zero os angulos de rotação para chegarmos ao angulo correto em cada um dos eixos
  m_anguloZNave = 0.0f;
  m_anguloXNave = 0.0f;
  //rotaciono a nave de acordo com o movimento
  if (m_gameData.m_input[gsl::narrow<size_t>(Input::Left)])
        m_anguloZNave += 20.0f;
  
  if (m_gameData.m_input[gsl::narrow<size_t>(Input::Right)])
      m_anguloZNave -= 20.0f;
  
  if (m_gameData.m_input[gsl::narrow<size_t>(Input::Up)])        
      m_anguloXNave += 20.0f;
  
  if (m_gameData.m_input[gsl::narrow<size_t>(Input::Down)])        
      m_anguloXNave -= 20.0f;
    

  // Update asteroides
  for (auto &asteroide : m_asteroides) {
    // faço eles se moveremna direção da nave 
    //eu divido pela escala, de forma que pedras grandes se movam devagar, e as pequenas bem rápido
    asteroide.m_position.z += (deltaTime * 10.0f) / asteroide.m_asteroidScale;

   

    //eu movo os asteroides para dar a impressão de movimento da nave
    //eles vão se mover no sentido oposto ao input do jogador
    //se eu clicar para a nave ir para a direita, devo mosver meus asteroides para a esquerda
    //movo eles em velocidade uniforme, sem implementar nenhuma aceleração
    if (m_gameData.m_input[gsl::narrow<size_t>(Input::Left)])
        asteroide.m_position.x += deltaTime * 3.0f;
       
    if (m_gameData.m_input[gsl::narrow<size_t>(Input::Right)])
        asteroide.m_position.x -= deltaTime * 3.0f;
        
    if (m_gameData.m_input[gsl::narrow<size_t>(Input::Up)])
        asteroide.m_position.y -= deltaTime * 3.0f;
        
    if (m_gameData.m_input[gsl::narrow<size_t>(Input::Down)])
        asteroide.m_position.y += deltaTime * 3.0f;
       
    //após mover o asteroide, eu verifico se ele bateu na nave
    checkCollisionAsteroide(asteroide);

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

void Window::checkCollisionAsteroide(Asteroide &asteroide){
     
    auto const distance{
        glm::distance(m_shipPosition, asteroide.m_position)
    };

    //essa distância foi decidida na tentativa e erro. Acho que ficou boa.
    //if (distance < 0.5f) {
    if(distance < (asteroide.m_asteroidScale * 0.5f + 0.15f * 0.5f)){
        m_gameData.m_state = State::GameOver;
    }
    
}


void Window::onPaint() {
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

    //se o estado do jogo não for playing (gameOVer ou restart) eu não
  if(m_gameData.m_state != State::Playing)
    return;

  //Defino a matriz de perspectiva. Ela ficará travada em 30 graus
    auto const aspect{gsl::narrow<float>(m_viewportSize.x) / gsl::narrow<float>(m_viewportSize.y)};
    m_projMatrix = glm::perspective(glm::radians(m_FOV), aspect, 0.01f, 100.0f);

  //essa parte do código localiza as variáveis que serão passadas para o vertex shader
  //nós criamos variáveis locais que vão ser atreladas as veriáveis no shader pelo nome das variáveis no .vert
  //obtendo todas as matrizes para o shader de blinn-Phong
  auto const viewMatrixLoc{abcg::glGetUniformLocation(m_programAsteroide, "viewMatrix")};
  auto const projMatrixLoc{abcg::glGetUniformLocation(m_programAsteroide, "projMatrix")};
  auto const modelMatrixLoc{abcg::glGetUniformLocation(m_programAsteroide, "modelMatrix")};
  auto const normalMatrixLoc{abcg::glGetUniformLocation(m_programAsteroide, "normalMatrix")};

  auto const lightPosLoc{abcg::glGetUniformLocation(m_programAsteroide, "lightPosWorldSpace")};
  auto const shininessLoc{abcg::glGetUniformLocation(m_programAsteroide, "shininess")};
  auto const IaLoc{abcg::glGetUniformLocation(m_programAsteroide, "Ia")};
  auto const IdLoc{abcg::glGetUniformLocation(m_programAsteroide, "Id")};
  auto const IsLoc{abcg::glGetUniformLocation(m_programAsteroide, "Is")};
  auto const KaLoc{abcg::glGetUniformLocation(m_programAsteroide, "Ka")};
  auto const KdLoc{abcg::glGetUniformLocation(m_programAsteroide, "Kd")};
  auto const KsLoc{abcg::glGetUniformLocation(m_programAsteroide, "Ks")};
  auto const diffuseTexLoc{abcg::glGetUniformLocation(m_programAsteroide, "diffuseTex")};
  auto const mappingModeLoc{abcg::glGetUniformLocation(m_programAsteroide, "mappingMode")};
 
  
  abcg::glUseProgram(m_programAsteroide);

  //agora que temos variáveis locais atreladas às veriáveis do shader
  //podemos atribuir valores.
  //é assim que passamos valores para o shader!

  //passo os valores das matrizes de visão e projeção
  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
  abcg::glUniform1i(diffuseTexLoc, 0);
  abcg::glUniform1i(mappingModeLoc, 2);//2 para mapeamento esférico

  //a direção da luz é a mesma para tudo
  abcg::glUniform4fv(lightPosLoc, 1, &m_lightPos.x);

  //aqui eu defino as características do material
  //Como todos os asteróides são feitos de asteroide, posso definir valores fixos e poupar processamento
  abcg::glUniform4fv(IaLoc, 1, &m_IaAsteroide.x);
  abcg::glUniform4fv(IdLoc, 1, &m_IdAsteroide.x);
  abcg::glUniform4fv(IsLoc, 1, &m_IsAsteroide.x);
  abcg::glUniform4fv(KaLoc, 1, &m_KaAsteroide.x);
  abcg::glUniform4fv(KdLoc, 1, &m_KdAsteroide.x);
  abcg::glUniform4fv(KsLoc, 1, &m_KsAsteroide.x);
  abcg::glUniform1f(shininessLoc, m_shininessAsteroide);

  
  
  //com as variáveis imutáveis preenchidas no shader, eu vou criar cada asteróide
  for (auto &asteroide : m_asteroides) {
    //crio a matriz de modelo de cada asteroide.
    //a matriz de modelo é única para cada objeto, pois ele terá posição e ângulo unicos

    //crio uma matriz identidade
    glm::mat4 modelMatrix{1.0f};
    //transformo a matriz para dar o efeito para cada asteróide
    modelMatrix = glm::translate(modelMatrix, asteroide.m_position);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(asteroide.m_asteroidScale));
    modelMatrix = glm::rotate(modelMatrix, m_angle, asteroide.m_rotationAxis);

    //com a matriz de modelo feita, posso calcular minha matriz de modelView
    //ess matriz transforma o universo do objeto no universo da camera
    auto const modelViewMatrix{glm::mat4(m_viewMatrix * modelMatrix)};

    //calculo a matriz de normais, que depende do objeto no universo da camera (por isso usamos a modelViewMatrix)
    auto const normalMatrix{glm::inverseTranspose(glm::mat3(modelViewMatrix))};

    //como já havia atrelado a variável local com a variável do shader, basta colocar valores
    //insiro a matriz de normais no meu shader
    abcg::glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);
    //insiro a matriz de modelo no meu shader
    abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);

    //finalmente eu faço a renderização
    m_modelAsteroide.render();
  }

  
  abcg::glUseProgram(m_programNave);

  //como mudei de programa, tenho que fazer todas as atribuições dos shaders novamente!
  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
  abcg::glUniform1i(diffuseTexLoc, 0);
  abcg::glUniform1i(mappingModeLoc, 3);//3 para mapeamento from mesh

  //a direção da luz é a mesma para tudo
  abcg::glUniform4fv(lightPosLoc, 1, &m_lightPos.x);

  //aqui eu defino as características do material
  abcg::glUniform4fv(IaLoc, 1, &m_IaNave.x);
  abcg::glUniform4fv(IdLoc, 1, &m_IdNave.x);
  abcg::glUniform4fv(IsLoc, 1, &m_IsNave.x);

  abcg::glUniform4fv(KaLoc, 1, &m_KaNave.x);
  abcg::glUniform4fv(KdLoc, 1, &m_KdNave.x);
  abcg::glUniform4fv(KsLoc, 1, &m_KsNave.x);
  abcg::glUniform1f(shininessLoc, m_shininessNave);

  
  glm::mat4 modelMatrixNave{1.0f};
  modelMatrixNave = glm::translate(modelMatrixNave, m_shipPosition);
  modelMatrixNave = glm::scale(modelMatrixNave, glm::vec3(0.15f));
  //agora faço duas rotações: uma no eixo z e uma em x
  //o anguo foi definido de acordo com o input do usuário
  //o angulo sempre será 20 graus, para dar uma eve inclinada na nave
  modelMatrixNave = glm::rotate(modelMatrixNave, glm::radians(m_anguloXNave), m_axisXNave);
  modelMatrixNave = glm::rotate(modelMatrixNave, glm::radians(m_anguloZNave), m_axisZNave);

    auto const modelViewMatrix{glm::mat4(m_viewMatrix * modelMatrixNave)};
    //calculo a matriz de normais, que depende do objeto no universo da camera (por isso usamos a modelViewMatrix)
    auto const normalMatrix{glm::inverseTranspose(glm::mat3(modelViewMatrix))};

    //como já havia atrelado a variável local com a variável do shader, basta colocar valores
    //insiro a matriz de normais no meu shader
    abcg::glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);
    //insiro a matriz de modelo no meu shader
    abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrixNave[0][0]);


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