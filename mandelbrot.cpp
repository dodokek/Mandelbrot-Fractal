#include "./include/mandelbrot.hpp"

void StartDrawing()
{
    float center_x = 0;
    float center_y = 0;


    sf::RenderWindow window(sf::VideoMode(W_WIDTH, W_HEIGHT), "Mandelbebra");
    window.setFramerateLimit(30);


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
    

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {        
                window.close();
            }
        }

        DrawMndlSet (window, center_x, center_y);
        window.display();

        window.clear();
    }

    return;
}


void DrawMndlSet (sf::RenderWindow &window, float center_offset_x, float center_offset_y)
{
    sf::RectangleShape CurPixel = GenerateRectangle (1, 1, 0, 0);

    float center_x = W_WIDTH  / 2.f + W_HEIGHT * 0.3f + center_offset_x;          // Calculating figure's center coords according to window size
    float center_y = W_HEIGHT / 2.f + center_offset_y;                            

    float scale = 0.005f;                                       // Scale coeficient

    for (int cur_y = 0; cur_y < W_HEIGHT; cur_y++)              // Iterating through all imaginary parts of "c"
    {
        float c0_im_part = ((float)cur_y - center_y) * scale;

        for (int cur_x = 0; cur_x < W_WIDTH; cur_x++)           // Iterating through all real parts of "c"
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
            CurPixel.setPosition (float(cur_x), float(cur_y));
            CurPixel.setFillColor (sf::Color::Black);

            // printf ("Iterations: %d\n", total_iterations);

            if (total_iterations < MAX_ITERATIONS)
            {

                CurPixel.setFillColor (sf::Color{(unsigned char)(total_iterations * 5),(unsigned char) (total_iterations * 10), 0});
            }

            window.draw (CurPixel);

        }
    }

}


sf::RectangleShape GenerateRectangle (float width, float height, float x, float y)
{
    sf::RectangleShape rectangle(sf::Vector2f(120, 50));
    rectangle.setSize(sf::Vector2f(height, width));
    rectangle.setPosition(x, y);
    rectangle.setFillColor(sf::Color::Green);
 
    return rectangle;
}