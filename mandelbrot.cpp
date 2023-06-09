#include "./include/mandelbrot.hpp"

#define AVX_OPTIMIZE
#define DRAW

void StartDrawing()
{
    float center_x = 0;     // initial parameters
    float center_y = 0;
    float scale = 0.005f;   

    sf::Clock clock; // starts the clock

    sf::Font font;
    font.loadFromFile("img/SEASRN__.ttf");
    sf::Text cycle_text = *GenerateTextSprite (font, "Cycle time: ", 50, 50);
    cycle_text.setCharacterSize (20);
    cycle_text.setFillColor (sf::Color::Red);

    sf::RenderWindow window(sf::VideoMode(W_WIDTH, W_HEIGHT), "Mandelbebra");
    window.setFramerateLimit(30);

    sf::Image canvas;
    canvas.create( W_WIDTH, W_HEIGHT, sf::Color::White );

    sf::Texture texture;
    texture.loadFromImage( canvas );

    sf::Sprite sprite;
    sprite.setTexture( texture );


    while (window.isOpen())
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            center_x -= 10.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            center_x += 10.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            center_y -= 10.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            center_y += 10.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::F2))
            scale *= 1.25f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::F1))
            scale /= 1.25f;

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {        
                window.close();
            }
        }

        clock.restart();

        #ifndef AVX_OPTIMIZE
            DrawMndlSet (canvas, center_x, center_y, scale);
        #else
            DrawMndlSetAVX (canvas, center_x, center_y, scale);
        #endif

        sf::Time elapsed_time = clock.getElapsedTime();
        
        char text_buffer[100] = "";
        sprintf (text_buffer, "FPS: %.2f\n", 1/elapsed_time.asSeconds());
        printf (text_buffer);

        cycle_text.setString (text_buffer);

        texture.update( canvas );
        window.clear();
        window.draw ( sprite );
        window.draw (cycle_text);
        window.display();
    }

    return;
}


void DrawMndlSet (sf::Image &canvas, float center_offset_x, float center_offset_y, float scale)
{
    sf::RectangleShape CurPixel(sf::Vector2f(1, 1));

    float center_x = W_WIDTH  / 2.f + W_HEIGHT * 0.3f + center_offset_x;          // Calculating figure's center coords according to window size
    float center_y = W_HEIGHT / 2.f + center_offset_y;                            


    for (int cur_y = 0; cur_y < W_HEIGHT; cur_y++)              // Iterating through all imaginary parts of "c"
    {
        float c0_im_part = ((float)cur_y - center_y) * scale;

        for (int cur_x = 0; cur_x < W_WIDTH; cur_x++)           // Iterating through all real parts of "c"
        {
            // for (int i = 0; i < 100; i++)
            {

            float c0_real_part = ((float)cur_x - center_x) * scale;

            int total_iterations = 0;

                for (float x = c0_real_part, y = c0_im_part; total_iterations < MAX_ITERATIONS; total_iterations++)
                {
                    float x_pow    = x * x;
                    float y_pow    = y * y;                 // avoiding arethmetic bugs
                    float x_mul_y  = x * y;

                    float vector_length = x_pow + y_pow;

                    if (vector_length >= MAX_DISTANCE)
                        break;
                        
                    x = x_pow - y_pow     + c0_real_part;   // Z_{n+1} = (Z_{n}) ^ 2 + C_0
                    y = x_mul_y + x_mul_y + c0_im_part;     // According to this formula counting Real and Imm parts
                }

            #ifdef DRAW
                sf::Color color = sf::Color{(unsigned char)(total_iterations * 5),(unsigned char) (total_iterations * 10), 0};
                canvas.setPixel (cur_x, cur_y, color);
            #endif
        }
        }
    }
}

// ======================= O - for Optimization with AVX =========================

void DrawMndlSetAVX (sf::Image &canvas, float center_offset_x, float center_offset_y, float scale)
{
    sf::RectangleShape CurPixel(sf::Vector2f(1, 1));

    float center_x = W_WIDTH  / 2.f + W_HEIGHT * 0.3f + center_offset_x;          // Calculating figure's center coords according to window size
    float center_y = W_HEIGHT / 2.f + center_offset_y;                            

    __m256 center_x_avx = _mm256_set1_ps (center_x);
    __m256 center_y_avx = _mm256_set1_ps (center_y);

    __m256 offset_vector = _mm256_set_ps  (7.f, 6.f, 5.f, 4.f, 3.f, 2.f, 1.f, 0.f);

    // __m256 offset_vector = _mm256_set_ps  (0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f);  //- if you want some funny drawings

    for (int cur_y = 0; cur_y < W_HEIGHT; cur_y++)              // Iterating through all imaginary parts of "c"
    {
        __m256 im_part_avx = _mm256_set1_ps (float(cur_y));                                                        // Calculating the vector of y 
        im_part_avx = _mm256_mul_ps(_mm256_sub_ps (im_part_avx, center_y_avx), _mm256_set1_ps (scale));     // in decart system (y_j, ... , y_j)

        for (int cur_x = 0; cur_x < W_WIDTH; cur_x += 8)           // Iterating through all real parts of "c"
        {     
            // for (int i = 0; i < 100; i++)
            {
            
            __m256 real_part_avx = _mm256_add_ps(_mm256_set1_ps (float(cur_x)), offset_vector);                                   // Calculating the vector of x 
            real_part_avx        = _mm256_mul_ps(_mm256_sub_ps (real_part_avx, center_x_avx), _mm256_set1_ps (scale));     // (x_i, x_i + 1, ... , x_i + 7)

             _mm256_set1_epi32 (0);        // Initializing vector of operations counter

            __m256 x = real_part_avx;
            __m256 y = im_part_avx;
            
            __m256i total_iterations = CalcOneCycle (x, y);

            uint32_t* iterations_array = (uint32_t*) &total_iterations;

            #ifdef DRAW

            for (int point_ctr = 0; point_ctr < 8; point_ctr++)
            {
                // printf ("Iterations: %d\n", iterations_array[point_ctr]);

                if (iterations_array[point_ctr] < MAX_ITERATIONS)
                {
                    sf::Color color = sf::Color{(unsigned char)(iterations_array[point_ctr] * 5),(unsigned char) (iterations_array[point_ctr] * 10), 0};
                    canvas.setPixel (cur_x + point_ctr, cur_y, color);
                }
                else
                {
                    canvas.setPixel (cur_x + point_ctr, cur_y, sf::Color::Black);
                }
            }

            #endif
            }
        }
    }
}


__m256i CalcOneCycle (__m256 x, __m256 y)
{
    __m256 real_part_avx = x;
    __m256 im_part_avx = y;
    __m256i total_iterations = _mm256_set1_epi32 (0);

    for (int iterator = 0; iterator <= MAX_ITERATIONS; iterator++)
    {
        __m256 x2   = _mm256_mul_ps (x, x);
        __m256 y2   = _mm256_mul_ps (y, y);
        __m256 xy   = _mm256_mul_ps (x, y);

        __m256 length2_avx = _mm256_add_ps (x2, y2);

        __m256 cmp_res = _mm256_cmp_ps (MAX_VECTOR_LEN, length2_avx, _CMP_GT_OQ); // comparing each distance with max len

        int comparison_mask = _mm256_movemask_ps (cmp_res); // moves the most significant bit of each float to integer bits

        if (!comparison_mask) // if all points out of range then break
        {    
            break;
        }    

        total_iterations = _mm256_sub_epi32 (total_iterations, _mm256_castps_si256 (cmp_res));  // here need to sub instead of add cuz __mm256_cmp_ps sets -1 in result of comparison

        x = _mm256_add_ps (_mm256_sub_ps (x2, y2), real_part_avx); // Z_{n+1} = (Z_{n}) ^ 2 + C_0
        y = _mm256_add_ps (_mm256_add_ps (xy, xy), im_part_avx);   // According to this formula counting Real and Imm parts 
    }
    
    return total_iterations;
}




sf::Text *GenerateTextSprite (sf::Font &font, char* content, float x_coord, float y_coord)
{
    sf::Text *text = new sf::Text;          // НЕ ЗАБЫТЬ СЛОВИТЬ Exceptiont сука!!!

    text->setFont(font);
    text->setFillColor(sf::Color::Red);
    text->setString(content);
    text->setCharacterSize(48);
    text->setPosition(x_coord, y_coord);

    return text;
}