#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <vector>          
#include "Plataform.hpp"        


class Player {
    private:
        float x, y;
        float velY;
        bool noChao;
        bool olhandoEsquerda;

    public:
    Player();

    void moveLeft();
    void moveRight();
    void jump();

    void applyGravity();
    void checkCollision(std::vector<Plataform>& plataforms);
    void update(std::vector<Plataform>& plataforms);

    float getX();
    float getY();
    float getVelY();
};

#endif
