#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <ctime>
#include "../include/Player.hpp"
#include "../include/Plataform.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

const float MIN_DIST_X = 50.0f;
const float MAX_DIST_X = 180.0f;

const float MIN_DIST_Y = 70.0f;
const float MAX_DIST_Y = 120.0f;

float lilyX = 200;
float lilyY = 6950;

float lilyVelY = 0;

const float GRAVITY_LILY = -0.05f;

// altura máxima do jogo (para limitar a geração de plataformas)
const float MAX_HEIGHT = 7000.0f;

// posição da câmera
float cameraY = 0;

int sequenciaVertical = 3;// contador de sequências verticais de plataformas

bool isDead = false;
float deathTimer = 0.0f;
bool win = false;

const float DEATH_TIME = 1.5f; // tempo até reset
const float MAX_FALL_SPEED = -1.2f; // velocidade limite pra morrer

void drawSprite(float x, float y, float w, float h, GLuint texture)
{
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, texture);

    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);

    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(x, y);

    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(x + w, y);

    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(x + w, y + h);

    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(x, y + h);

    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
}

void drawBackground(GLuint texture)
{
    drawSprite(
        0,
        cameraY,
        800,
        600,
        texture
    );
}

// movimentação do jogador
void processInput(GLFWwindow* window, Player& player) {
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        player.moveLeft();
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        player.moveRight();
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        player.jump();
          
    }
}

void drawPlayer(Player& player, GLuint parado, GLuint dir, GLuint esq, GLuint caindo) 
{
    GLuint tex;

    if (player.getVelY() > 0)
        tex = dir;
    else if (player.getVelY() < 0)
        tex = caindo;
    else
        tex = parado;

    drawSprite(
        player.getX(),
        player.getY(),
        40,
        40,
        tex
    );
}

// função para desenhar a Lily
void drawLily(GLuint texture)
{
    drawSprite(
        lilyX,
        lilyY,
        40,
        40,
        texture
    );
}

void checkWin(Player& player) {

    float playerLeft = player.getX();
    float playerRight = player.getX() + 40;
    float playerBottom = player.getY();
    float playerTop = player.getY() + 40;

    float lilyLeft = lilyX;
    float lilyRight = lilyX + 40;
    float lilyBottom = lilyY;
    float lilyTop = lilyY + 40;

    if (playerRight > lilyLeft &&
        playerLeft < lilyRight &&
        playerTop > lilyBottom &&
        playerBottom < lilyTop) {

        win = true;
    }
}

// função para atualizar a posição da Lily
void updateLily(std::vector<Plataform>& plataform) {
    lilyVelY += GRAVITY_LILY;
    lilyY += lilyVelY;

    float lilyBottom = lilyY;
    float lilyLeft = lilyX;
    float lilyRight = lilyX + 20;

    for (auto& p : plataform) {

        float platTop = p.y + p.height - 35;
        float platLeft = p.x;
        float platRight = p.x + p.width;
        
        if (lilyBottom <= platTop &&
            lilyBottom >= platTop - 10 &&
            lilyRight > platLeft &&
            lilyLeft < platRight &&
            lilyVelY <= 0) {

            lilyY = platTop;
            lilyVelY = 0;
        }
    }
}

void drawPlataforms(
    std::vector<Plataform>& plataforms,
    GLuint textura)
{
    for(auto& p : plataforms)
    {
        drawSprite(
            p.x,
            p.y,
            p.width,
            p.height,
            textura
        );
    }
}

// função para gerar um número aleatório
float randFloat(float min, float max) {
    return min + (rand() / (float)RAND_MAX) * (max - min);
}

void generatePlataforms(std::vector<Plataform>& plats, int quantidade) {

    float lastX = 100;
    float lastY = 100;

    plats.push_back(Plataform(lastX, lastY, 170, 60));

    for (int i = 1; i < quantidade; i++) {

        float dx = randFloat(MIN_DIST_X, MAX_DIST_X);
        float dy;
        if (sequenciaVertical >= 2) {
            // força mudança (plataforma mais deslocada)
            dy = randFloat(70.0f, 90.0f); // menor subida
            sequenciaVertical = 0;
        } else {
            dy = randFloat(MIN_DIST_Y, MAX_DIST_Y);
            sequenciaVertical++;
        }

        if (rand() % 2 == 0)
            dx *= -1;        
        float newX = lastX + dx;
        float newY = lastY + dy;

        // limitar tela
        if (newX < 0) newX = 0;
        if (newX > 600) newX = 600;

        plats.push_back(Plataform(newX, newY, 130, 60));

        lastX = newX;
        lastY = newY;
        // limitar altura máxima
        if (newY > MAX_HEIGHT){
            break;
        }

    }
}

GLuint loadTexture(const char* path)
{
    stbi_set_flip_vertically_on_load(true);

    int width, height, channels;

    unsigned char* data =
        stbi_load(path, &width, &height, &channels, 4);

        if (!data) {
            std::cout << "ERRO AO CARREGAR: " << path << std::endl;
            return 0;
        }

        std::cout << "Imagem carregada: " << path
        << " (" << width << "x" << height << ")" << std::endl;

    GLuint texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        data
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(data);

    return texture;
}

int main() {
    
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(800, 600, "Leap & Lily", NULL, NULL);
    if (!window) return -1;

    glfwMakeContextCurrent(window);

    // sistema de coordenadas 2D
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 800, 0, 600, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // guardando as plataformas em um vetor
    std::vector<Plataform> plataforms;  
    
    srand(time(0));
    generatePlataforms(plataforms, 200);


    Player player;

    plataforms.push_back(Plataform(0, 0, 800, 20));  // chão
    //plataforma para o teto
    plataforms.push_back(Plataform(0, 7085, 800, 20));

    Plataform ultima = plataforms[plataforms.size() - 3];

    // mais para a direita
    lilyX = ultima.x + 80;

    // em cima da plataforma
    lilyY = ultima.y + ultima.height;

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLuint tParado = loadTexture("assets/parado.png");
    GLuint tPuloDir = loadTexture("assets/pulo_dir.png");
    GLuint tPuloEsq = loadTexture("assets/pulo_esq.png");
    GLuint tCaindo = loadTexture("assets/caindo.png");
    GLuint tLily = loadTexture("assets/lily.png");

    GLuint tPlataforma =
    loadTexture("assets/plataforma.png");

    GLuint tBackground =
    loadTexture("assets/background.png");

    while (!glfwWindowShouldClose(window)) {

    if (!win)
    {
        processInput(window, player);
        player.update(plataforms);
        checkWin(player);
    }

        if (win) {
            glClear(GL_COLOR_BUFFER_BIT);

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            glPushMatrix();
            glTranslatef(0, -cameraY, 0);

            drawBackground(tBackground);
            drawPlayer(
            player,
            tParado,
            tPuloDir,
            tPuloEsq,
            tCaindo);        
            
            drawLily(tLily);

            glPopMatrix();

            glfwSwapBuffers(window);
            glfwPollEvents();
            continue;
        }

        if (player.getVelY() < MAX_FALL_SPEED && player.isOnGround() || player.getY() < cameraY - 200) {
            isDead = true;
        }

        if (isDead) {
            deathTimer += 0.016f; // ~60 FPS
            if (deathTimer >= DEATH_TIME) {
                // RESET
                player.setPosition(100, 150);
                player.setVelY(0);

                cameraY = 0;

                isDead = false;
                deathTimer = 0.0f;
            }   
            glColor3f(1.0f, 0.3f, 0.3f);    
            return 0; // bloquear movimentação
        }

        // fazer a camera seguir o jogador
        if (player.getY() > cameraY + 300) {
            cameraY = player.getY() - 300;
        }else if (player.getY() < cameraY + 100) {
            cameraY = player.getY() - 100;
            if (cameraY < 0) cameraY = 0;
        }

        glClear(GL_COLOR_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glPushMatrix();
        glTranslatef(0, -cameraY, 0);

        drawBackground(tBackground);

        drawPlataforms(
        plataforms,
        tPlataforma);

        drawPlayer(
        player,
        tParado,
        tPuloDir,
        tPuloEsq,
        tCaindo);

        drawLily(tLily);

// updateLily(plataforms);
glPopMatrix();

        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    glfwTerminate();
    
}
