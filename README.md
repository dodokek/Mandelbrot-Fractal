# Mandelbrot set optimization

## Overview

The goal of this project is drawing the *Mandelbrot set* the most effecient way. To maximize performance we are going to use *AVX2 instructions*. 

Example:
 </br>
 </br>
<img src="https://user-images.githubusercontent.com/57039216/227485430-7cd9c1d4-45a2-491a-a618-9aba52ce0a21.png" width="500px"/>
 </br>
 </br>

## Drawing algorithm

We draw each dot of this set independently according to the algorithm:

Let's take a sequence on a complex plain wich looks like this:

$Z_{n + 1} = Z_n ^ 2 + C_0$

Where $C_0$ is the point on the complex plane.

 The color of the point depends on how much iterations were made before the distance from $Z_n$ to $C_0$ got bigger than 2.

Here is the main part of calculations:
~~~C++
for (x = c0_real_part, y = c0_im_part; iters < 256; iters++)
{
    x_pow    = x * x;
    y_pow    = y * y;
    x_mul_y  = x * y;

    sqr_vector_length = x_pow + y_pow;

    if (vector_length >= 2)
        break;
        
    x = x_pow - y_pow     + c0_real_part;
    y = x_mul_y + x_mul_y + c0_im_part;    
}
~~~

## Analysing the perfromance

As we can see from the code above, calculation of the single pixel on the screen can take up to 256 iterations.

 This means that if our screen size is 800 x 600, there is a posssibility of doing 800 * 600 * 256 = **122 880 000 iterations** per drawing cycle

The performance of this approach is nothing more than really slow. **FPS: 7**

>System info: Core i5 9th gen. No optimization flags

We definetly need to do something with it... Wait, I have an idea!

## Using AVX2 instructions
 
We draw each point independently. This means, we can calculate several points simultaniously. 

AVX2 instructions are just perfect for this task. With them we able to count up to 8 points at a time. Let's improve our program:

~~~C++

__m256 x2   = _mm256_mul_ps (x, x);
__m256 y2   = _mm256_mul_ps (y, y);
__m256 xy   = _mm256_mul_ps (x, y);

__m256 length2_avx = _mm256_add_ps (x2, y2);

__m256 cmp_res = _mm256_cmp_ps (MAX_VECTOR_LEN, length2_avx, _CMP_GT_OQ); 

comparison_mask = _mm256_movemask_ps (cmp_res); 

if (!comparison_mask)     
    break;

total_iterations = _mm256_sub_epi32 (total_iterations, _mm256_castps_si256 (cmp_res));  

x = _mm256_add_ps (_mm256_sub_ps (x2, y2), real_part_avx); 
y = _mm256_add_ps (_mm256_add_ps (xy, xy), im_part_avx);  

~~~

According to our calculations, the FPS should increace tremendeously, let's check it out.




## Performance

| Version      | Compilation flags | FPS           | Speed growth  |
| ------      | :---------------: | :------------: | :----------: |
| No AVX      | none              | 7.5            |   0.49        |
| No AVX      | -О3               | 14             |   0.91        |
| No AVX      | -Оfast            | 15.3           |   1          |
| AVX         | none              | 24             |   1.56      |
| AVX         | -О3               | 80             |   5.2       |
| AVX         | -Ofast            | 84             |   5.8      |


Performance was tested on the same scale and position of fractal. According to the table, with AVX instructions FPS has increased approximately 6 times.

## Graphic library issues

The performance strongly depends on the graphic library that we use. 

SFML library, which I used in my project has it's disadvantages, such as slow event handling and displaying the pixels. Anyway, we loose around 3 FPS if the drawing is toggled, so we can close our eyes on it.

## Conclusion

AVX optimization made a huge impact on FPS. By calculating 8 points at once, cycle of drawing speeded up almost 6 times. 

Also, without optimization flags this method is not valid. As we could see from performance table, the rise in performance without compilaion flags was really small. 

Interesting fact: As it turned out during the tests, FPS can rise up to 100 if battery settings on PC are set to *High performance*.

## Useful links 
https://www.laruence.com/sse/   -    AVX documentation </br>
https://mathworld.wolfram.com/MandelbrotSet.html  - More info about Mandelbrot Set
