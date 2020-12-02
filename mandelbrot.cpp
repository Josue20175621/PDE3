#include <iostream>
#include <iomanip>
#include <complex>
#include <fstream>
#include <chrono>


#define CYAN "\033[36m"

#define BOLDYELLOW "\033[1m\033[33m"

#define RESET "\033[0m"

#define MAX_ITERATIONS 80

// Dimensiones de la imagen
#define WIDTH 500
#define HEIGHT 500

int mandelbrot(std::complex<double> c);

int main()
{

    // Almacena el color
    int color;

    // El nombre del archivo ppm
    std::string filename = "mandelbrot.ppm";
    
    // Class para escribir archivos
    std::ofstream output(filename, std::ios_base::out | std::ios_base::binary);

    // Escribe el header de un archivo PPM P3 WIDTH HEIGHT 255
    output << "P3" << std::endl << WIDTH << ' ' << HEIGHT << std::endl << 255 << std::endl;

    int x, y;

    // Plot window
    // double x_start = -1.5, x_end = 0.7, y_start = -1, y_end = 1;
    double x_start = -2.25, x_end = 1.25, y_start = -1.75, y_end = 1.75;

    // Toma el tiempo
    auto start = std::chrono::high_resolution_clock::now();
    
    // Por cada pixel
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
            
            // El color depende del numero de iteraciones
            color = 255 - (int)(iterations * 255 / MAX_ITERATIONS);

            output << "  " << color << "  " << color << "  " << color << "\n";
            /*
            // Dibuja el punto
            if (iterations == MAX_ITERATIONS)
            {
                // Si mandelbrot retorna MAX_ITERATIONS, usa el color negro
                output << "  " << color << "  " << color << "  " << color << "\n";
            }
            else
            {
                // Color depende del numero de iteraciones
                output << "  " << color + 40 << "  " << color + 30 << "  " << color << "\n";
            }
            */
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    // Diferencia
    std::chrono::duration<double> diff = end-start;

    // Cierra el archivo
    output.close();

    std::cout << "\n";
    std::cout << "Se escribio el PPM en el archivo " << BOLDYELLOW << "\"" << filename << "\"" << RESET << "." 
              << "\n";
    std::cout << "Imagen " << WIDTH << "x" << HEIGHT << " - " << BOLDYELLOW << "Tiempo " << CYAN << diff.count() << RESET << " segundos\n";

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