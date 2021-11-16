// ConsoleApplication23.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <mpi.h>

int rank, size;
const int SIZE_OF_ARRAY = 1000;

// Умножение матрицы на вектор при разделении данных по столбцам
void t7() {
	// кол-во элементов в матрице и одном шарде
	const int matrix = SIZE_OF_ARRAY * SIZE_OF_ARRAY;
	const int matrix_shard = matrix / size;
	const int vector_shard = SIZE_OF_ARRAY / size;
	int* buffer_matrix = new int[matrix_shard];
	int* buffer_vector = new int[SIZE_OF_ARRAY / size];
	int* vector = new int[SIZE_OF_ARRAY]; //вектор
	int* array = new int[matrix]; // матрица

	if (rank == 0) {
		for (int i = 0; i < SIZE_OF_ARRAY; i++) {
			vector[i] = rand() % 100;
			for (int k = 0; k < SIZE_OF_ARRAY; k++) {             // заполняем рандомными числами
				array[k * SIZE_OF_ARRAY + i] = rand() % 100;
			}
		}
	}
	// отправляет чанки разные данных процессам 
	// бродкаст отправляет одну и ту же кипу данных всем процессам
	MPI_Scatter(array, matrix_shard, MPI_INT, buffer_matrix, matrix_shard, MPI_INT, 0, MPI_COMM_WORLD); // разделяем матрицу по процам


	//in	array - адрес начала размещения блоков распределяемых данных(используется только в процессе - отправителе root);
	//in	shard - число элементов, посылаемых каждому процессу;
	//in	MPI_INT - тип посылаемых элементов;
	//out	buffer_matrix - адрес начала буфера приема;
	//in	shard - число получаемых элементов;
	//in	MPI_INT - тип получаемых элементов;
	//in	0 - номер процесса - отправителя;
	//in	MPI_COMM_WORLD - коммуникатор.
	
	
	MPI_Scatter(vector, vector_shard, MPI_INT, buffer_vector, vector_shard, MPI_INT, 0, MPI_COMM_WORLD); // разделяем вектор по процам
	for (int i = 0; i < SIZE_OF_ARRAY; i++) {
		vector[i] = 0;
		for (int k = 0; k < vector_shard; k++) {
			vector[i] += (buffer_matrix[k * SIZE_OF_ARRAY + i] * buffer_vector[i]) % 100;
			vector[i] %= 100;
		}
	}
	int* result_vector = new int[SIZE_OF_ARRAY];
	int* buffer_vector_result = new int[SIZE_OF_ARRAY * size];
	// Собираем данные с проц-в
	MPI_Gather(vector, SIZE_OF_ARRAY, MPI_INT, buffer_vector_result, SIZE_OF_ARRAY, MPI_INT, 0, MPI_COMM_WORLD);

	//IN	vector - адрес начала размещения посылаемых данных;
	//IN	array_size - число посылаемых элементов;
	//IN	MPI_INT - тип посылаемых элементов;
	//OUT	buffer_vector_result - адрес начала буфера приема(используется только в процессе - получателе root);
	//IN	array_size - число элементов, получаемых от каждого процесса(используется только в процессе - получателе root);
	//IN	MPI_INT - тип получаемых элементов;
	//IN	0 - номер процесса - получателя;
	//IN	MPI_COMM_WORLD - коммуникатор.

	if (rank == 0) {
		for (int i = 0; i < SIZE_OF_ARRAY; i++) {
			for (int k = 0; k < size; k++) {
				result_vector[i] += buffer_vector_result[k * SIZE_OF_ARRAY + i];
				result_vector[i] %= 100;
			}
			printf("%d ,", result_vector[i]);
		}
	}
}

int main(int argc, char** argv) {
	MPI_Init(&argc, &argv);  // Инициализация библиотеки
	// MPI_COMM_WORLD - ID глобального коммуникатора
	MPI_Comm_size(MPI_COMM_WORLD, &size); // Функция определения числа процессов в области связи
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Функция определения номера процесса
	t7();
	MPI_Finalize();
	return EXIT_SUCCESS;
}
