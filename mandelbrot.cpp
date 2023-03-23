#include "./include/mandelbrot.hpp"

void StartDrawing()
{

    sf::RenderWindow window(sf::VideoMode(W_WIDTH, W_HEIGHT), "Mandelbebra");
    window.setFramerateLimit(30);

    DrawMndlSet (window);

    window.display();
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {        
                window.close();
            }
        }

    }

    return;
}


void DrawMndlSet (sf::RenderWindow &window)
{
    sf::RectangleShape CurPoint = GenerateRectangle (1, 1, 0, 0);

    Complex point = {0, 0};
    int counter = 0;

    for (float real_part = LEFT_B; real_part < RIGHT_B; real_part += 0.01f)      // Перебираю все такие "C"
    {
        counter++;
        // printf ("Iteration %d\n", counter);

        int sub_counter = 0;

        for (float im_part = LEFT_B; im_part < RIGHT_B; im_part += 0.01f)
        {
            // printf ("Iterating Real: %f, Im: %f \n", real_part, im_part);

            point = {0, 0};
            int out_of_bound_iteration = 0;

            for (int iteration = 0; iteration < MAX_ITERATIONS; iteration++)
            {
                float cur_real = point.real * point.real - point.im * point.im + real_part;
                float cur_im = 2 * point.im * point.real + im_part;

                // printf("\tCur real: %f, Cur Im: %f\n", cur_real, cur_im);

                if (cur_real * cur_real + cur_im * cur_im > MAX_DISTANCE * MAX_DISTANCE)
                {
                    out_of_bound_iteration = iteration;
                    break;
                }

                point = {cur_real, cur_im};
            }

            if (out_of_bound_iteration < MAX_ITERATIONS)
            {
                // printf ("Suitable dot(%f;%f): %d : %d\n",
                //         point.real, point.im, counter, sub_counter++);

                CurPoint.setPosition (real_part * 10 + 600, im_part * 10 + 400);
                window.draw(CurPoint);
            }

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