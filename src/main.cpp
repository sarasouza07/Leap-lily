#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <ctime>
#include "../include/Player.hpp"
#include "../include/Plataform.hpp"

const float MIN_DIST_X = 50.0f;
const float MAX_DIST_X = 180.0f;

const float MIN_DIST_Y = 70.0f;
const float MAX_DIST_Y = 120.0f;

float lilyX = 200;
float lilyY = 7115;

float lilyVelY = 0;

const float GRAVITY_LILY = -0.05f;

// altura máxima do jogo (para limitar a geração de plataformas)
const float MAX_HEIGHT = 7000.0f;

// posição da câmera
float cameraY = 0;

int sequenciaVertical = 3;// contador de sequências verticais de plataformas

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

void drawPlayer(Player& player) {
    float x = player.getX();
    float y = player.getY();

    glColor3f(1.0f, 0.0f, 0.0f);

    glBegin(GL_QUADS);  
        glVertex2f(x, y);
        glVertex2f(x + 20, y);
        glVertex2f(x + 20, y + 20);
        glVertex2f(x, y + 20);
    glEnd();
}

// função para desenhar a Lily
void drawLily() {

    glColor3f(1, 0, 1);

    glBegin(GL_QUADS);
        glVertex2f(lilyX, lilyY);
        glVertex2f(lilyX + 20, lilyY);
        glVertex2f(lilyX + 20, lilyY + 20);
        glVertex2f(lilyX, lilyY + 20);
    glEnd();

}

// função para atualizar a posição da Lily
void updateLily(std::vector<Plataform>& plataform) {
    lilyVelY += GRAVITY_LILY;
    lilyY += lilyVelY;

    float lilyBottom = lilyY;
    float lilyLeft = lilyX;
    float lilyRight = lilyX + 20;

    for (auto& p : plataform) {

        float platTop = p.y + p.height;
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

void drawPlataforms(std::vector<Plataform>& plataforms) {
    glColor3f(1.0f, 1.0f, 1.0f);

    for (auto& p : plataforms) {
        glBegin(GL_QUADS);
            glVertex2f(p.x, p.y);
            glVertex2f(p.x + p.width, p.y);
            glVertex2f(p.x + p.width, p.y + p.height);
            glVertex2f(p.x, p.y + p.height);
        glEnd();
    }
}

// função para gerar um número aleatório
float randFloat(float min, float max) {
    return min + (rand() / (float)RAND_MAX) * (max - min);
}

void generatePlataforms(std::vector<Plataform>& plats, int quantidade) {

    float lastX = 100;
    float lastY = 100;

    plats.push_back(Plataform(lastX, lastY, 100, 20));

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

        plats.push_back(Plataform(newX, newY, 100, 20));

        lastX = newX;
        lastY = newY;
        // limitar altura máxima
        if (newY > MAX_HEIGHT){
            break;
        }

    }
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

    // guardando as plataformas em um vetor
    std::vector<Plataform> plataforms;  
    
    srand(time(0));
    generatePlataforms(plataforms, 200);

    Player player;

    plataforms.push_back(Plataform(0, 0, 800, 20));  // chão
    //plataforma para o teto
    plataforms.push_back(Plataform(0, 7085, 800, 20));

    while (!glfwWindowShouldClose(window)) {
        processInput(window, player);

        player.update(plataforms);

        // fazer a camera seguir o jogador
        if (player.getY() > cameraY + 300) {
            cameraY = player.getY() - 300;
        }else if (player.getY() < cameraY + 100) {
            cameraY = player.getY() - 100;
            if (cameraY < 0) cameraY = 0;
        }

        glClear(GL_COLOR_BUFFER_BIT);

        glPushMatrix();
        glTranslatef(0, -cameraY, 0);

        drawPlataforms(plataforms);
        drawPlayer(player);
        drawLily();

        updateLily(plataforms);

        glfwSwapBuffers(window);
        glfwPollEvents();
        glPopMatrix();
    }

    glfwTerminate();
    
}