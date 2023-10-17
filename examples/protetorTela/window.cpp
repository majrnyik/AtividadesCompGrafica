#include "window.hpp"

void Window::onCreate() {
  auto const *vertexShader{R"gl(#version 300 es

    layout(location = 0) in vec2 inPosition;
    layout(location = 1) in vec4 inColor;

    uniform vec2 translation;
    uniform float scale;

    out vec4 fragColor;

    void main() {
      vec2 newPosition = inPosition * scale + translation;
      gl_Position = vec4(newPosition, 0, 1);
      fragColor = inColor;
    }
  )gl"};

  auto const *fragmentShader{R"gl(#version 300 es

    precision mediump float;  

    in vec4 fragColor;

    out vec4 outColor;

    void main() { outColor = fragColor; }
  )gl"};

  m_program = abcg::createOpenGLProgram(
      {{.source = vertexShader, .stage = abcg::ShaderStage::Vertex},
       {.source = fragmentShader, .stage = abcg::ShaderStage::Fragment}});

  abcg::glClearColor(0, 0, 0, 1);
  abcg::glClear(GL_COLOR_BUFFER_BIT);

  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  //seleciono um angulo aleatório para comecar
  std::random_device device;
  std::mt19937 engine(device());
  std::uniform_real_distribution<> thetaDist(0, 2 * M_PI);
  angulo_velocidade = thetaDist(engine); 
}

void Window::onPaint() {

  /* Se quisermos manter um rastro, podemos adicionar um checkbox no UI
    aqui verificamos se o checkbox está ticado, e, se quisermos deixar um rastro, pulamos a instrução do clear
  */
  //limpo a tela para dar a ilusão de movimento
  //abcg::glClear(GL_COLOR_BUFFER_BIT);
  
  setupModel();

  abcg::glUseProgram(m_program);

  //pego o tempo decorrido desde a ultima pintura, para calcular a variação de movimento
  tempo_decorrido = getDeltaTime();

  //verificamos se a bolinha rebate
  rebate();
  
  glm::vec2 const translation{velocidade_atual * cos(angulo_velocidade) * tempo_decorrido + posicao_atual.x ,velocidade_atual * sin(angulo_velocidade) * tempo_decorrido + posicao_atual.y};

  //após calcular a nova posição, faço essa atribuição para atualizar nossa variavel de posição
  posicao_atual = translation;
  
  //coloco a nova posição na variavel do vertex shader
  auto const translationLocation{
      abcg::glGetUniformLocation(m_program, "translation")};
  abcg::glUniform2fv(translationLocation, 1, &translation.x);

 //coloco a escala escolhida na variavel do vetex shader
  auto const scale{m_scale};
  auto const scaleLocation{abcg::glGetUniformLocation(m_program, "scale")};
  abcg::glUniform1f(scaleLocation, scale);

  // Render
  abcg::glBindVertexArray(m_VAO);
  abcg::glDrawArrays(GL_TRIANGLE_FAN, 0, m_sides + 2);
  abcg::glBindVertexArray(0);

  abcg::glUseProgram(0);
}

void Window::onPaintUI() {
  abcg::OpenGLWindow::onPaintUI();

  {
    auto const widgetSize{ImVec2(220, 150)};
    ImGui::SetNextWindowPos(ImVec2(m_viewportSize.x - widgetSize.x - 5,
                                   m_viewportSize.y - widgetSize.y - 5));
    ImGui::SetNextWindowSize(widgetSize);
    auto const windowFlags{ImGuiWindowFlags_NoResize |
                           ImGuiWindowFlags_NoCollapse |
                           ImGuiWindowFlags_NoTitleBar};
    ImGui::Begin(" ", nullptr, windowFlags);

    ImGui::PushItemWidth(140);
    //crio os sliders de lados e de escala
    ImGui::SliderInt("Sides", &m_sides, 3, 10, "%d");
    ImGui::SliderFloat("Scale", &m_scale, 0.05, 0.25, "%f");
    /*aqui viria o slider de velocidade também.
    o slider alteraria o valor do velocidade_atual*/

    //checkbox dos angulos aleatórios
    ImGui::Checkbox("Angulos de reflexão aleatórios", &m_angulosAleatorios);
    ImGui::PopItemWidth();
    
    /*contemplei a ideia de resetar a posição da bolinha para (0,0) toda vez que clicarmos no clear window*/
    if (ImGui::Button("Clear window", ImVec2(-1, 30))) {
      abcg::glClear(GL_COLOR_BUFFER_BIT);
      posicao_atual.x = 0;
      posicao_atual.y = 0;
    }
  
  static bool limparRastro{};
  ImGui::Checkbox("Limpar rastro?", &limparRastro);

  if (limparRastro) {
    abcg::glClear(GL_COLOR_BUFFER_BIT);
  }

    ImGui::End();
  }
}

void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;
  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);
  posicao_atual = {0.0f , 0.0f};
  abcg::glClear(GL_COLOR_BUFFER_BIT);
  
}

void Window::onDestroy() {
  abcg::glDeleteProgram(m_program);
  abcg::glDeleteBuffers(1, &m_VBOPositions);
  abcg::glDeleteBuffers(1, &m_VBOColors);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}

void Window::setupModel() {
  // Release previous resources, if any
  abcg::glDeleteBuffers(1, &m_VBOPositions);
  abcg::glDeleteBuffers(1, &m_VBOColors);
  abcg::glDeleteVertexArrays(1, &m_VAO);

  //definimos as cores (ainda não achei uma combinação boa)
  glm::vec3 const color1{0.0f, 0.8f, 1.0f};
  glm::vec3 const color2{0.255f, 0.153f, 0.255f};
  
  std::vector<glm::vec2> positions;
  std::vector<glm::vec3> colors;

  // Polygon center
  positions.emplace_back(0, 0);
  colors.push_back(color1);

  // Border vertices
  auto const step{M_PI * 2 / m_sides};
  for (auto const angle : iter::range(0.0, M_PI * 2, step)) {
    positions.emplace_back(std::cos(angle), std::sin(angle));
    colors.push_back(color2);
  }

  // Duplicate second vertex
  positions.push_back(positions.at(1));
  colors.push_back(color2);

  // Generate VBO of positions
  abcg::glGenBuffers(1, &m_VBOPositions);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOPositions);
  abcg::glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
                     positions.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate VBO of colors
  abcg::glGenBuffers(1, &m_VBOColors);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOColors);
  abcg::glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3),
                     colors.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  auto const positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};
  auto const colorAttribute{abcg::glGetAttribLocation(m_program, "inColor")};

  // Create VAO
  abcg::glGenVertexArrays(1, &m_VAO);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_VAO);

  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOPositions);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glEnableVertexAttribArray(colorAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOColors);
  abcg::glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void Window::rebate(){
  //essa função verifica se a bolinha "bateu" em alguma "parede"
  //se ela bateu, vou rebater a bolinha 
  //primeira implementação: apenas mudando o sinal da velocidade da bolinha.
  bool hitsTopOrBottom = posicao_atual.y > 1 || posicao_atual.y < -1;
  bool hitsRight = posicao_atual.x > 1;
  bool hitsLeft = posicao_atual.x < -1;
  bool hitsSide = hitsTopOrBottom || hitsRight || hitsLeft;

  //crio o RNG para o angulo
  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());
  std::random_device device;
  std::mt19937 engine(device());

  if (hitsSide) {
    if (hitsTopOrBottom) {
        if (m_angulosAleatorios){
          //sorteio angulo aleatório que não é horizontal (para ter mais movimento)
          std::uniform_real_distribution<> thetaDist(0.3, 0.7 * M_PI);
          angulo_velocidade = posicao_atual.y > 0 ? - thetaDist(engine) : thetaDist(engine); 
        } else{
          angulo_velocidade = -angulo_velocidade;
        }
        //temos que atribuir a posiçãonovamente, para qando a bolinha passar da borda, ela não fique sendo rebatida infinitamente 
        posicao_atual.y = (posicao_atual.y > 0 ? 1.0f : -1.0f);
    } else if (hitsRight) {
      if (m_angulosAleatorios){
        //sorteio angulo aleatório que não vertical
        std::uniform_real_distribution<> thetaDist(M_PI/2 + 0.3, (3 * M_PI) / 2 - 0.3);
        angulo_velocidade = thetaDist(engine);
      } else{
        angulo_velocidade = -angulo_velocidade - M_PI;
      }
        posicao_atual.x = 1;
    } else if (hitsLeft) {
      if (m_angulosAleatorios){
        //sorteio angulo aleatório que não vertical
        std::uniform_real_distribution<> thetaDist(M_PI/2 - 0.3, (3 * M_PI) / 2 + 0.3);
        angulo_velocidade = thetaDist(engine);
      } else{
        angulo_velocidade = M_PI - angulo_velocidade;
      }
        posicao_atual.x = -1;
    }
  }
}