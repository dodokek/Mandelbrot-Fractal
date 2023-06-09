#ifndef MANDL_H
#define MANDL_H


#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <stdio.h>
#include <math.h>
#include <immintrin.h>


//------------------------------------------

const int   MAX_ITERATIONS  = 256;

const int MAX_DISTANCE = 100.f;

const float LEFT_B = -3;

const float RIGHT_B = 3;

const int W_WIDTH = 800;

const int W_HEIGHT = 600;

__m256 MAX_VECTOR_LEN = _mm256_set1_ps (MAX_DISTANCE);


//------------------------------------------

struct Complex
{
    float real;
    float im;
};


//------------------------------------------


void StartDrawing();

void DrawMndlSet (sf::Image &canvas, float center_offset_x, float center_offset_y, float scale);

void DrawMndlSetAVX (sf::Image &canvas, float center_offset_x, float center_offset_y, float scale);

sf::Text *GenerateTextSprite (sf::Font &font, char* content, float x_coord, float y_coord);

__m256i CalcOneCycle (__m256 x, __m256 y);

#endif