// TJ Wolschon
// DSA 1
// HW2.2 - Graphical Snake
#pragma once
#include <SFML/System.hpp>
#include <Box2D/Box2D.h>

class Snake
{
public:
    Snake(b2Body* snakeBody, b2Vec2* _target, b2World* _world, int* _currentScore);
    ~Snake();
    void Update();
    float GetDistance(sf::Vector2f pos1, sf::Vector2f pos2);
    int GetScore(int numOfTargets);
    void ProcessInput();
    void ApplyForceUp(b2Body& player);
    void ApplyForceDown(b2Body& player);
    void ApplyForceLeft(b2Body& player);
    void ApplyForceRight(b2Body& player);
    void StopMoving(b2Body& player);
    void ReverseGravity(b2World& world);
    sf::Vector2f GetPosition();

private:

    b2Body* body;//snake
    b2Vec2* target;
    b2World* world;

    int* currentScore;//current score from main
    int timesPressed;//number of times up has been "pressed" (counts holding too)

    const float timeStep = 1.0f / 60.0f;
};

typedef void(Snake::*ApplyForcePtr)(b2Body &player);
