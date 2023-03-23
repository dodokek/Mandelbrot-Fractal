#ifndef MANDL_H
#define MANDL_H


#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <stdio.h>

//------------------------------------------

const int MAX_ITERATIONS = 100;

const int MAX_DISTANCE = 2;

const float LEFT_B = -3;

const float RIGHT_B = 3;

const int W_WIDTH = 800;

const int W_HEIGHT = 600;

//------------------------------------------

struct Complex
{
    float real;
    float im;
};


//------------------------------------------


void StartDrawing();

void DrawMndlSet (sf::RenderWindow &window);

sf::RectangleShape GenerateRectangle (float width, float height, float x, float y);


#endif