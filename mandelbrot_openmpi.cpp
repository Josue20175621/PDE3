#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <complex>
#include <algorithm>
#include <ctime>

#define CYAN   "\x1B[36m"

#define BOLDYELLOW "\033[1m\033[33m"
#define RESET "\x1B[0m"

#define MASTER_RANK 0
#define MAX_ITERATIONS 80

// Dimensiones de la imagen
#define WIDTH 500
#define HEIGHT 500

#define NOT_ENOUGH_PROCESSES_NUM_ERROR 1

MPI_Status status;

// Almacena el color de cada pixel
int color[WIDTH][HEIGHT];

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

int main(int argc, char *argv[])
{
	int communicator_size;
	int rank;
	int process_id;
	int offset;
	int rows_num;
	int workers_num;
	int remainder;
	int whole_part;
	
	int i;
    int j;

	int x, y;

	// Plot window
	double x_start = -2.25, x_end = 1.25, y_start = -1.75, y_end = 1.75;

	std::string filename = "mandelbrot_openmpi.ppm";

    std::ofstream output(filename, std::ios_base::out | std::ios_base::binary);

	// Inicia entorno de ejecucion MPI
	MPI_Init(&argc, &argv);

	// Retorna el size del grupo
	MPI_Comm_size(MPI_COMM_WORLD, &communicator_size);

	// Determina el rango del proceso en el comunicador
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	
	if(communicator_size < 2){
		MPI_Abort(MPI_COMM_WORLD, NOT_ENOUGH_PROCESSES_NUM_ERROR);
	}

	if(rank == MASTER_RANK){

		double starttime, endtime;
		starttime = MPI_Wtime();
		std::cout << "\n";
		
		workers_num = communicator_size - 1;
		whole_part = WIDTH / workers_num;
		remainder = WIDTH % workers_num;
		
		offset = 0;
		
		for(process_id = 1; process_id <= workers_num; process_id++){
			
			// Calcula el numero de filas que va a trabajar este proceso
			rows_num = process_id <= remainder ? whole_part + 1 : whole_part;
			
			printf("%sEnvia %d pixeles al proceso %d %s\n", BOLDYELLOW, rows_num * HEIGHT, process_id, RESET);
			
			// Envia el offset
			MPI_Send(&offset, 1, MPI_INT, process_id, 0, MPI_COMM_WORLD);
			
			// Envia el numero de filas con las que va a trabajar
			MPI_Send(&rows_num, 1, MPI_INT, process_id, 0, MPI_COMM_WORLD);

			// Actualiza el offset para el siguiente proceso
			offset += rows_num;
		}

		for(process_id = 1; process_id <= workers_num; process_id++){
			// Recibe el offset
			MPI_Recv(&offset, 1, MPI_INT, process_id, 0, MPI_COMM_WORLD, &status);

			// Recibe el numero de rows
			MPI_Recv(&rows_num, 1, MPI_INT, process_id, 0, MPI_COMM_WORLD, &status);

			// Recibe los valores de cada pixel
			MPI_Recv(&color[offset][0], rows_num * HEIGHT, MPI_INT, process_id, 0, MPI_COMM_WORLD, &status);
		}
		
		endtime = MPI_Wtime();

		// Escribe el header de un archivo PPM P3 WIDTH HEIGHT 255
		output << "P3" << std::endl << WIDTH << ' ' << HEIGHT << std::endl << 255 << std::endl;
		
		// Por cada pixel
		for (i = 0; i < WIDTH; i++)
		{
			for (j = 0; j < HEIGHT; j++)
			{
				output << "  " << color[i][j] << "  " << color[i][j] << "  " << color[i][j] << "\n";
			}
			output << "\n";
		}

		// Cierra el archivo
		output.close();

		double diff = endtime - starttime;
		std::cout << "\n";
		std::cout << "Se escribio el PPM en el archivo " << BOLDYELLOW << "\"" << filename << "\"" << RESET << "."
				  << "\n";
		printf("\nImagen %dx%d - %sTiempo %s%.3f %ssegundos\n", WIDTH, HEIGHT, BOLDYELLOW, CYAN, diff, RESET);
	}
	
	if(rank > MASTER_RANK){

		MPI_Recv(&offset, 1, MPI_INT, MASTER_RANK, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&rows_num, 1, MPI_INT, MASTER_RANK, 0, MPI_COMM_WORLD, &status);

		// Por cada pixel
		for(x = 0; x < rows_num; x++){
			for(y = 0; y < HEIGHT; y++){
				// Convierte la coordenada del pixel a un numero complejo
				std::complex<double> c = {
                    x_start + ((double)(offset + x) / (double)WIDTH) * (x_end - x_start),
                    y_start + ((double)y / (double)HEIGHT) * (y_end - y_start)
                };

				// Calcula numero de iteraciones
				int iterations = mandelbrot(c);

				// Almacena el color del pixel
				color[x][y] = 255 - (int)(iterations * 255 / MAX_ITERATIONS);
			}
		}

		MPI_Send(&offset, 1, MPI_INT, MASTER_RANK, 0, MPI_COMM_WORLD);
		MPI_Send(&rows_num, 1, MPI_INT, MASTER_RANK, 0, MPI_COMM_WORLD);
		MPI_Send(&color, rows_num * HEIGHT, MPI_INT, MASTER_RANK, 0, MPI_COMM_WORLD);
	}

	MPI_Finalize();
	return 0;
}
