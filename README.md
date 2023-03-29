# Mandelbrot-Fractal
 Here's is my implementation of Mandelbrot Fractal drawing with SSE optimizations.
 </br>
 </br>
<img src="https://user-images.githubusercontent.com/57039216/227485430-7cd9c1d4-45a2-491a-a618-9aba52ce0a21.png" width="500px"/>
 </br>
 </br>

## Annotation
Each dot of this fractal is being drawn independently according to the formula:
~~~
Z_(n + 1) = Z_n ^ 2 + C0
~~~
Where C0 is the point on the complex plane. It's color depends on how much iterations are needed for sequence to go out of radius of 2.

## Optimization ideas

In order to speed up the calculation proccess we can calculate 8 dot's at once, using vector algebra. To implement it in C I used AVX2 instructions which provide the fastest calculations with the objects.

Here is the small exaple of calculating the next step of sequence
~~~
x = _mm256_add_ps (_mm256_sub_ps (x2, y2), real_part_avx); // Z_{n+1} = (Z_{n}) ^ 2 + C_0
y = _mm256_add_ps (_mm256_add_ps (xy, xy), im_part_avx);   // According to this formula counting Real and Imm parts 
~~~

## Performance

| Version      | Compilation flags | FPS |
| ------      | :---------------: | :------------: | 
| No AVX      | none              | 6.5            |  
| No AVX      | -О3               | 11             |  
| No AVX      | -О3 -fprofile-use | 11.3           | 
| No AVX      | -Оfast            | 11.5           | 
| AVX         | none              | 16             |  
| AVX         | -О3               | 29             | 
| AVX         | -О3 -fprofile-use | 29             | 
| AVX         | -Ofast            | 30.8           | 


As we can see AVX optimization speeded up the program more than 3 times. With the help of compiler flags we went from 6 to whole 30 fps, brilliant.

## SFML strikes back

While using SFML as the library to draw the fractal, I stumbled upon the speed problem due to the very slow pixel-drawing algorithms. Moreover, the event handling is something I can blame SFML dev's for, because it is also slower than my grandma.

## Useful links 
https://www.laruence.com/sse/
