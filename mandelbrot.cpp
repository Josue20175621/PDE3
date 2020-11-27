#include <iostream>
#include <iomanip>
#include <complex>
#include <fstream>
#include <chrono>

#define MAX_ITERATIONS 80

int mandelbrot_set(std::complex<double> c);

int main()
{
    // Dimensiones de la imagen
    int width = 500;
    int height = 500;

    // Almacena el color
    int color;

    // El nombre del archivo ppm
    std::string filename = "mandelbrot.ppm";
    
    // Class para escribir archivos
    std::ofstream output(filename, std::ios_base::out | std::ios_base::binary);

    // Escribe los Magic Bytes 50 33 0A para que los programas sepan que esto es un Portable Pixmap
    output << "P3" << std::endl << width << ' ' << height << std::endl << 255 << std::endl;

    int x, y;

    // Plot window
    double x_start = -1.5, x_end = 0.7, y_start = -1, y_end = 1;
    
    // Toma el tiempo
    auto start = std::chrono::high_resolution_clock::now();
    
    // Por cada pixel
    for (x = 0; x < width; x++)
    {
        for (y = 0; y < height; y++)
        {
            // Convierte la coordenada del pixel a un numero complejo
            std::complex<double> c = {
                x_start + ((double)x / (double)width) * (x_end - x_start),
                y_start + ((double)y / (double)height) * (y_end - y_start)
            };

            // Calcula numero de iteraciones
            int iterations = mandelbrot_set(c);
            
            // El color depende del numero de iteraciones
            color = 255 - (int)(iterations * 255 / MAX_ITERATIONS);
            
            // Dibuja el punto
            if (iterations == MAX_ITERATIONS)
            {
                // Si mandelbrot retorna MAX_ITERATIONS, usa el color negro
                output << "  " << color << "  " << color << "  " << color << "\n";
            }
            else
            {
                // Sino usa un color dependiendo el numero de iteraciones
                output << "  " << color + 40 << "  " << color + 30 << "  " << color + 80 << "\n";
            }            

        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    // Diferencia
    std::chrono::duration<double> diff = end-start;

    // Close the file
    output.close();

    std::cout << "\n";
    std::cout << "Se escribio el PPM en el archivo \"" << filename << "\".\n";
    std::cout << "Tiempo " << diff.count() << " segundos\n";

    return 0;
}

// Calcula numero de iteraciones
int mandelbrot_set(std::complex<double> c)
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