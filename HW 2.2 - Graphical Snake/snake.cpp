// TJ Wolschon
// DSA 1
// HW2.2 - Graphical Snake
#include "pch.h"
#include "Snake.h"
#include <Windows.h>
#include <conio.h>
#include <iostream>
#include <iomanip>
#include <Box2D/Box2D.h>
#include <math.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>


//using namespace std;
Snake::Snake(b2Body* snakeBody, b2Vec2* _target, b2World* _world, int* _currentScore)
{
    body = snakeBody;
    target = _target;
    world = _world;
    currentScore = _currentScore;
}

Snake::~Snake()
{
    body, target, world, currentScore = nullptr;
}

//fixed time step to slow down the simulation a bit, goes too fast otherwise
void Snake::Update()
{
    world->Step(timeStep, 8, 3);
}

//basic vector distance formula
float Snake::GetDistance(sf::Vector2f pos1, sf::Vector2f pos2)
{
    float x = (pos1.x+10) - (pos2.x+10);
    float y = (pos1.y-10) - (pos2.y-10);
    float toReturn = sqrt((y*y) + (x*x));
    return toReturn;
}

int Snake::GetScore(int numOfTargets)
{
    //max possible score is 200 per target hit
    int score = 200 * numOfTargets;

    //final score is dependant on how many times movement was applied
    score -= timesPressed/2;

    //minimum score of 10 per target hit
    if(score < 10*numOfTargets)
    {
        score = 10*numOfTargets;
    }
    return score;
}

//handles keyboard input
void Snake::ProcessInput() 
{
    ApplyForcePtr forceToUse = nullptr;
    
    //wasd or arrow keys for movement
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        forceToUse = &Snake::ApplyForceLeft;
        timesPressed++;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        forceToUse = &Snake::ApplyForceRight;
        timesPressed++;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        forceToUse = &Snake::ApplyForceDown;
        timesPressed++;
    }
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        forceToUse = &Snake::ApplyForceUp;
        timesPressed++;
    }

    //shift keys to reverse gravity
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))
    {
        ReverseGravity(*world);
    }
    //space to set velocity to 0
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
    {
        forceToUse = &Snake::StopMoving;
    }

    //if one of the keys was pressed
    if(forceToUse != nullptr)
    {
        //call the function with the player's body
        (this->*forceToUse)(*body);
    }
}

void Snake::ApplyForceUp(b2Body &player)
{
    player.ApplyForceToCenter(b2Vec2(0, 10), true);
}

void Snake::ApplyForceDown(b2Body &player)
{
    player.ApplyForceToCenter(b2Vec2(0, -10), true);
}

void Snake::ApplyForceLeft(b2Body &player)
{
    player.ApplyForceToCenter(b2Vec2(-10, 0), true);
}

void Snake::ApplyForceRight(b2Body &player)
{
    player.ApplyForceToCenter(b2Vec2(10, 0), true);
}

void Snake::StopMoving(b2Body &player)
{
    player.SetLinearVelocity(b2Vec2_zero);
}

void Snake::ReverseGravity(b2World &world)
{
    world.SetGravity(-world.GetGravity());
}

sf::Vector2f Snake::GetPosition()
{
    //convert the box2d coordinates to SFML coordinates
    return sf::Vector2f((body->GetPosition().x*93)+490, (-body->GetPosition().y*93)+490);
}

