# The Expanse

Beatriz C. M. Farkas. RA: 11201722860
Felipe P. Franke. RA: 11201721293

***
## Descrição Geral 
  Para a atividade 3 foi implementado um jogo baseado em ‘Space Invaders’ no qual o objetivo é guiar uma nave no espaço de forma que ela não colida como nenhum asteroide.
- O objetivo do jogo é guiar uma espaço-nave usando as teclas 'cima, baixo, lado direito, lado esquerdo' de modo que ela não colida com nenhum asteróide. 
- Caso a nave colida o jogo termina com uma tela escrito 'Game Over' até que o jogador aperte outra tecla para reinicar o jogo.

***
## Controles 
Tecla | Comando | 
--- | --- | 
←↑→↓ | Movimentação da nave | 
r | Reiniciar o jogo | 

## Implementação
### Arquivos
#### main.cpp
Esse arquivo é responsável pela inicialização geral do programa assim como a criação da janela.

#### window.cpp
 Aqui é implementado a maioria dos comandos e funcionalidades do nosso jogo. 
 Primeiramente é definido os comandos de movimentação e reinício do jogo na função onEvent(): 
```
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
```

Já na função onCreate() fazemos a criação dos asteroides e da nossa nave, assim como a pré-definição da orientação da câmera dentro do jogo.
```
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
      abcg::createOpenGLProgram({{.source = assetsPath + "blinPhong.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "blinPhong.frag",
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
    abcg::createOpenGLProgram({{.source = assetsPath + "blinPhong.vert",
                                .stage = abcg::ShaderStage::Vertex},
                                {.source = assetsPath + "blinPhong.frag",
                                .stage = abcg::ShaderStage::Fragment}});

  m_modelNave.loadObj(assetsPath + "nave.obj");
  m_modelNave.setupVAO(m_programNave);
}
```
Existe uma função chamada randomizeAsteroids() que adiciona um elemento de aleatoriedade aos asteroides dentro do jogo que ajuda a criar uma sensação de realismo.

Já a função onUpdate() construímos a jogabilidade da aplicação. Dado que o ângulo inicial de cada asteroide é diferente, conseguimos manter uma mesma velocidade de rotação que todos eles irão ter movimentações diferentes entre si. 

```
auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};
m_angle = glm::wrapAngle(m_angle + glm::radians(90.0f) * deltaTime);
```
A movimentação deles se dá na direção da nave, e eles se movem contrariamente ao input do jogador, ou seja, se eu clicar para a nave ir para a direita, os asteroides vão para a esquerda. Isso faz com que haja a sensação de que é a nave quem se move, sem de fato ser necessário a implementação de uma mecânica própria de movimentação da nave. 

```
 if (m_gameData.m_input[gsl::narrow<size_t>(Input::Left)]){
        asteroide.m_position.x += deltaTime * 3.0f;
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
```
Assim, é possível então checar se houve colisão ou não com a nave, reiniciando o jogo caso tenha ocorrido.

Já no onPaint() temos todas as variáveis locais que irão ser atreladas ao shader.  Ao definirmos as características do material, como temos que todos os asteroides possuem a mesma constituição, podemos fixar valores constantes para eles, criando assim para cada um a matriz de modelo, matriz de identidade e matriz de normais para realizar a renderização dos objetos. O mapeamento dos asteroides é esférico, ou seja, utilizamos um  mapeamento esférico para um objeto que é “relativamente” redondo como um asteroide.
Para a nave o processo é praticamente o mesmo, só que incluímos uma rotação na nave - uma no eixo z e uma em x.

#### model.cpp
Uma diferença do model.cpp em relação ao projeto anterior são as funções para suporte de texturas, em especial a loadDiffuseTexture, que carrega uma textura a partir de um caminho.

```
void Model::loadDiffuseTexture(std::string_view path) {
    if (!std::filesystem::exists(path))
       return;
    
    abcg::glDeleteTextures(1, &m_diffuseTexture);
    m_diffuseTexture = abcg::loadOpenGLTexture({.path = path});
}
```

Também há a função de cálculo de normais, que calcula os vetores normais para cada trio de vértices, o que é importante para implementação dos shaders.

#### model.hpp
Aqui temos a definição da classe “model” assim como a instanciação de algumas variáveis constantes.

#### window.hpp
Aqui temos a definição da classe “window” assim como a instanciação de algumas variáveis constantes, como os eixos de rotação. Para que fosse possível utilizar mais de um modelo, foi necessário instanciar programas individuais. Isso também poderia ser feito com um vetor de programas, mas, por termos apenas 2 objetos, ficou mais organizado dessa forma.

```
  GLuint m_programAsteroide{};
  GLuint m_programNave{};
```

#### texture.frag
No fragment shader, é decidida a cor de cada fragmento, baseada na textura de cada fragmento, e a incidencia da luz.
Destaca-se o uso do fator de atenuação, calculado no vertex shader, e passado ao fragment shader.

```
if (gl_FrontFacing) {
   outColor = color * attenuation;
}
```
#### texture.vert
Aqui foi criado um fator de atenuação no shader para que os asteroides surjam de forma sutil na tela. 

```
float distanceFromLight = length(L);
  float i = 1.0 - (distanceFromLight / 100.0);
  fragL = L;
  fragV = -P;
  fragN = N;
  attenuation = vec4(i, i, i, 1);
```
## Possíveis próximos passos

Acreditamos que numa próxima iteração do jogo seja possível fazer a inclusão de objetos diferentes que poderão colidir com a nave, como outras naves, planetas, satélites, etc. Assim como implementar a mecânica de “tiro” da nave, que poderia destruir algum objeto antes que ocorra a colisão. Assim como mecânicas de aceleração e freio da nave
