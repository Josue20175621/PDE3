#include <iostream>
#include <complex>
#include <fstream>
#include <omp.h>

#define CYAN "\033[36m"

#define BOLDYELLOW "\033[1m\033[33m"

#define RESET "\033[0m"

#define MAX_ITERATIONS 80

// Dimensiones de la imagen
#define WIDTH 500
#define HEIGHT 500

#define NUM_THREADS 4

int mandelbrot(std::complex<double> c);

int main(void)
{
    double start, end;

    // Almacena el color de cada pixel
    int color[WIDTH][HEIGHT];

    int i;
    int j;
    int k;
    int l;

    // Nombre del archivo
    std::string filename = "mandelbrot_openmp.ppm";

    // Class para escribir archivos
    std::ofstream output(filename, std::ios_base::out | std::ios_base::binary);

    int x, y;

    // Plot window
    double x_start = -2.25, x_end = 1.25, y_start = -1.75, y_end = 1.75;

    omp_set_num_threads(NUM_THREADS);
    start = omp_get_wtime();

    // Por cada pixel
    #pragma omp parallel shared(x_start, x_end, y_start, y_end, color) private(x, y)
    {   
        #pragma omp for
        for (x = 0; x < WIDTH; x++)
        {
            for (y = 0; y < HEIGHT; y++)
            {
                // Convierte la coordenada del pixel a un numero complejo
                std::complex<double> c = {
                    x_start + ((double)x / (double)WIDTH) * (x_end - x_start),
                    y_start + ((double)y / (double)HEIGHT) * (y_end - y_start)
                };

                // Calcula numero de iteraciones
                int iterations = mandelbrot(c);
                
                // Almacena el color del pixel
                color[x][y] = 255 - (int)(iterations * 255 / MAX_ITERATIONS);
            }
        }
    }
    end = omp_get_wtime();

    // Escribe el header de un archivo PPM P3 WIDTH HEIGHT 255
    output << "P3" << std::endl << WIDTH << ' ' << HEIGHT << std::endl << 255 << std::endl;
    
    // Por cada pixel
    for (i = 0; i < WIDTH; i++)
    {
        for (k = 0; k < HEIGHT; k += 4)
        {
            l = std::min(k + 4, HEIGHT);
            for (j = k; j < l; j++)
            {
                // Escribe el color al PPM
                output << "  " << color[i][j] << "  " << color[i][j] << "  " << color[i][j] + j << "\n";
            }
            output << "\n";
        }
    }
    
    output.close();
    std::cout << "\n";
    std::cout << "Se escribio el PPM en el archivo " << BOLDYELLOW << "\"" << filename << "\"" << RESET << "." 
              << "\n";
    std::cout << "Imagen " << WIDTH << "x" << HEIGHT << " -" << BOLDYELLOW << " Tiempo " << CYAN << end - start << RESET << " segundos\n";

    return 0;
}

// Calcula numero de iteraciones
int mandelbrot(std::complex<double> c)
{
    std::complex<double> z = {0, 0};
    int n = 0;
    while (std::abs(z) <= 2.0 and n < MAX_ITERATIONS)
    {
        z = z*z + c;
        n++;
    }
    return n;
}
