#include <vector>
#include "../include/Player.hpp"

#define GRAVITY -0.1f
#define SPEED 1.0f

Player::Player() {
    x = 200;
    y = 200;
    velY = 0;
    noChao = false;
    olhandoEsquerda = false;
}

void Player::applyGravity() {
    velY += GRAVITY;
}

void Player::moveLeft() {
    olhandoEsquerda = true;

    x -= SPEED;

    if (x < 0)
        x = 0;
}

void Player::moveRight() {
    olhandoEsquerda = false;

    x += SPEED;

    if (x > 780)
        x = 780;
}


void Player::jump() {
    if (noChao) {
        velY = 5.0f;
        noChao = false;
    }
}

void Player::checkCollision(std::vector<Plataform>& plataform) {

    float playerBottom = y;
    float playerTop = y + 40;
    float playerLeft = x;
    float playerRight = x + 40;

    noChao = false;

    for (auto& p : plataform) {

        float platTop = p.y + p.height;
        float platLeft = p.x;
        float platRight = p.x + p.width;

        // colisão vindo de cima
        if (playerBottom <= platTop && playerBottom >= platTop - 10 &&
            playerRight > platLeft && playerLeft < platRight && velY <= 0){
                
            y = platTop;
            velY = 0;
            noChao = true;

        }
    }
}

void Player::update(std::vector<Plataform>& plataforms) {
    applyGravity();
    y += velY;

    checkCollision(plataforms);
}

float Player::getX() { return x; }
float Player::getY() { return y; }
float Player::getVelY() { return velY; }
