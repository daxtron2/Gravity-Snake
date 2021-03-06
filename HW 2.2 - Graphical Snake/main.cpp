// TJ Wolschon
// DSA 1
// HW2.2 - Graphical Snake

#include "pch.h"
#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>
#include <conio.h>
#include <iostream>
#include <time.h>
#include <Windows.h>
#include "Snake.h"
#include <cstdlib>
#include <crtdbg.h>

int TARGETS_TO_GET;
int totalTargets;

b2Vec2* targetLocations;
b2Vec2 currentLocation;

#define _CRTDB_MAP_ALLOC

bool SelectNextTarget()
{
    currentLocation = *++targetLocations;
    return currentLocation.x != -1000;
}

void SetupTargets(int* cnt)
{
    //if given target count is out of range
    if(*cnt < 1 || *cnt > 15)
    {
        //inform user
        std::cout << std::endl << "Invalid number of targets. Defaulting to 10." << std::endl;
        //set to default of 10
        *cnt = 10;
    }
    //setup our array of target vectors
    targetLocations = new b2Vec2[*cnt+1];

    
    for(int i = 0; i < *cnt; i++)
    {
        //generate random x and y in a -5 to 5 range
        float x = (float)(rand() % 10) - 5;
        float y = (float)(rand() % 10) - 5;
        //create a vector with those x/y and store in array
        targetLocations[i] = b2Vec2(x, y);
    }
    //add the "end vector" to the array
    targetLocations[*cnt] = b2Vec2(-1000, -1000);
    //set current target as the first in the array
    currentLocation = targetLocations[0];
    //store our array size for later use, like deletion
    totalTargets = *cnt;
}

//wrapper for main to find mem leaks at end of runtime
int wrapper()
{
    //setup our SFML window
    sf::RenderWindow window(sf::VideoMode(1000, 1000), "Graphical Snake");
    //turn on v-sync to slow down the sim to 60hz for a more playable experience
    //window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    //set random seed with time
    srand((u_int)time(NULL));

    //introduce user, give controls, prompt for num of targets
    std::cout << "Welcome to Graphical Gravity Snake!" << std::endl
        << "Controls: Arrow Keys or WASD to move. Shift to flip gravity. Space to freeze velocity." << std::endl
        << "Enter number of targets(1-15): ";

    //store response
    std::cin >> TARGETS_TO_GET;
    //pass to setup
    SetupTargets(&TARGETS_TO_GET);

    //distance between snake and target to consider a hit
    const float HIT_DISTANCE = 40.f;

    //amount hit so far
    int targetsHit = 0;


    //create world w/ low gravity
    b2Vec2 gravity(0.0, -1.f);
    b2World world(gravity);

#pragma region CreateWalls
    //create left wall
    b2BodyDef leftWallDef;
    leftWallDef.position.Set(-11.0f, 0.0f);
    b2Body* leftWallBody = world.CreateBody(&leftWallDef);
    b2PolygonShape leftWallBox;
    leftWallBox.SetAsBox(5.0f, 5.0f);
    leftWallBody->CreateFixture(&leftWallBox, 0.0f);

    //create right wall
    b2BodyDef rightWallDef;
    rightWallDef.position.Set(11.0f, 0.0f);
    b2Body* rightWallBody = world.CreateBody(&rightWallDef);
    b2PolygonShape rightWallBox;
    rightWallBox.SetAsBox(5.0f, 5.0f);
    rightWallBody->CreateFixture(&rightWallBox, 0.0f);

    //create Top wall
    b2BodyDef topWallDef;
    topWallDef.position.Set(0.0f, 11.0f);
    b2Body* topWallBody = world.CreateBody(&topWallDef);
    b2PolygonShape topWallBox;
    topWallBox.SetAsBox(5.0f, 5.0f);
    topWallBody->CreateFixture(&topWallBox, 0.0f);

    //create Bottom wall
    b2BodyDef bottomWallDef;
    bottomWallDef.position.Set(0.0f, -11.0f);
    b2Body* bottomWallBody = world.CreateBody(&bottomWallDef);
    b2PolygonShape bottomWallBox;
    bottomWallBox.SetAsBox(5.0f, 5.0f);
    bottomWallBody->CreateFixture(&bottomWallBox, 0.0f);
#pragma endregion

    //create snake
    b2BodyDef snakeDef;
    snakeDef.type = b2_dynamicBody;
    snakeDef.position.Set(0.0f, 0.0f);
    b2Body* snakeBody = world.CreateBody(&snakeDef);
    b2PolygonShape snakeBox;
    snakeBox.SetAsBox(1.0f, 1.0f);
    b2FixtureDef snakeFixtureDef;
    snakeFixtureDef.shape = &snakeBox;
    snakeFixtureDef.density = 1.0f;
    snakeFixtureDef.friction = 0.3f;
    snakeBody->CreateFixture(&snakeFixtureDef);

    //create object of our functions
    Snake player(snakeBody, &currentLocation, &world, &targetsHit);

    //poll event for use in loop
    sf::Event pollEvent;

    //game loop
    while(targetsHit < TARGETS_TO_GET && window.isOpen())//while we haven't hit all targets and window is open
    {
        //setup player's circle
        sf::CircleShape circleSnake(20);
        circleSnake.setFillColor(sf::Color::Magenta);
        circleSnake.setPosition(player.GetPosition());

        //setup player's rectangle
        sf::RectangleShape targRect(sf::Vector2f(40, 40));
        targRect.setFillColor(sf::Color::Green);
        targRect.setPosition(sf::Vector2f((currentLocation.x * 93) + 490, -(currentLocation.y * 93) + 490));

        //update the physics simulation
        player.Update();

        //get distance between snake and target
        float dist = player.GetDistance(player.GetPosition(), sf::Vector2f((currentLocation.x * 93) + 490, -(currentLocation.y * 93) + 490));

        //if the distance is closer than hit distance
        if(dist < HIT_DISTANCE)
        {
            //select the next target, if returns false no more targets
            if(!SelectNextTarget())
            {
                //will end loop after this iteration.
                TARGETS_TO_GET = -1;
            }
            //increase number of targets hit
            targetsHit++;
        }

        //clear screen
        window.clear(sf::Color::Black);
        //draw snake
        window.draw(circleSnake);
        //draw current target
        window.draw(targRect);
        //push to display
        window.display();
        //process player input
        player.ProcessInput();

        //poll the window for events, if not done windows thinks it crashed
        while(window.pollEvent(pollEvent))
        {
            //if a close event is thrown, i.e. clicked X
            if(pollEvent.type == sf::Event::Closed)
            {
                //break the game loop
                TARGETS_TO_GET = -1;
                window.close();
            }
        }
    }
    std::cout << std::endl
        << "You got all " << totalTargets << " pieces of food!" << std::endl
        << "Your score was " << player.GetScore(totalTargets) << "/" << totalTargets * 200 << "." << std::endl//announce score
        << "Thanks for playing!" << std::endl
        << "Press ESCAPE to exit..."<< std::endl;

    while(1)//wait for user to press escape before exiting so they can read their score
    {
        if(_getch() == 27)
        {
            break;
        }
    }

    return 0;
}

int main()
{    
    //call the true main
    wrapper();
    //after main, reset the array to the beginning
    targetLocations -= totalTargets;
    //delete the array
    delete[] targetLocations;
    //set to null
    targetLocations = nullptr;
    //check for mem leaks
    _CrtDumpMemoryLeaks();
    return 0;
}