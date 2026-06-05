#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "../include/Player.hpp"
#include "../include/Plataform.hpp"

//movimentação do jogador
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

int main() {
    
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(800, 600, "Jump King", NULL, NULL);
    if (!window) return -1;

    glfwMakeContextCurrent(window);

    // sistema de coordenadas 2D
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 800, 0, 600, -1, 1);

    // guardando as plataformas em um vetor
    std::vector<Plataform> plataforms;

    Player player;

    plataforms.push_back(Plataform(0, 0, 800, 20));     // chão
    plataforms.push_back(Plataform(200, 150, 200, 20)); // plataforma
    plataforms.push_back(Plataform(450, 300, 200, 20));

    while (!glfwWindowShouldClose(window)) {
        processInput(window, player);

        player.update(plataforms);

        glClear(GL_COLOR_BUFFER_BIT);

        drawPlataforms(plataforms);
        drawPlayer(player);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}