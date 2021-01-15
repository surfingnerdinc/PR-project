#define _CRT_SECURE_NO_DEPRECATE
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
		


int main(int argc, char* argv[]) {
	
	//Deklarowanie zmiennych czasowych do obliczenia czasu dzialania programu. 
	clock_t start, stop;
	
	//Proces wejsciowy. Pytanie o zakres, ilosc watkow, ktore chcemy uzyc. 
	int beginOfRange, endOfRange;
	printf("Set begin of range \n");
	scanf("%d", &beginOfRange);

	printf("Set end of range \n");
	scanf("%d", &endOfRange);

	int THREADS;
	printf("Set amount of threads (1-4)\n");
	scanf("%d", &THREADS);

	switch (THREADS)
	{
		case 1: THREADS = 1;
		break;
		case 2: THREADS = 2;
			break;
		case 3: THREADS = 3;
			break;
		case 4: THREADS = 4;
			break;
		default: printf("ERROR. Available max 4 threads... \n");
			return EXIT_FAILURE;
			break;
	}

	//deklarowanie zmiennych. 
	int mysqrt = (int)sqrt(endOfRange);
	int matrixColumn = 256;
	int matrixRow = (endOfRange / matrixColumn) + 1;
	int tabIndex = 0;
	int i =0, j = 0;
	
	
	//Deklaracja tablicy z dzielnikami, ktora bedzie uzywana do pozniejszego wykreslania liczb
	int* divisors;
	divisors = (int*)calloc((mysqrt + 1), sizeof(int));


	//inicjalizacja macierzy u�ywanej podczas wykre�lania liczb pierwszych
	bool** divTab;
	divTab = malloc((matrixColumn + 1) * sizeof(bool*));
	for (int i = 0; i <= matrixColumn; ++i) {
		divTab[i] = malloc((matrixRow + 1) * sizeof(bool));
	}


	//Rozpoczecie pomiaru czasu.
	start = clock();

	//szukanie liczb pierwszych w zakresie od 2..sqrt(n)
	for (i = 2; i <= mysqrt; i++) {
		for (int j = 2; j <= i; j++) {
			if (i % j == 0)
			{
				if (i != j) {
					break;
				}
				else {
					divisors[tabIndex] = i;
					tabIndex++;
				}
			}
		}
	}


	omp_set_num_threads(THREADS);
#pragma omp parallel for shared(divTab, matrixRow, matrixColumn) private(i,j) schedule(dynamic,512)
	for (i = 0; i < matrixRow; i++) {
		for (j = 0; j < matrixColumn; j++)
		{
			if (matrixColumn * i + j <= endOfRange && matrixColumn * i + j >= beginOfRange) {
				divTab[j][i] = 1;
			}
			else {
				divTab[j][i] = 0;
			}
		}
	}


	//Wykreslanie liczb z tablicy za pomoca liczb parzystych. 
	int col_index = 0, row_index = 0, end_of_range = 0;
	end_of_range = endOfRange / 2;
#pragma omp parallel for shared(divTab,end_of_range) private(col_index, row_index, i) schedule(static,32)
	for (i = 2; i <= end_of_range; i++) {
		col_index = 2 * i % matrixColumn;
		row_index = 2 * i / matrixColumn;
		divTab[col_index][row_index] = false;
	}

	//Wykreslanie liczb z tablicy (tych, ktore tutaj pozostaly) za pomoca liczb pierwszych.
#pragma omp parallel for private(col_index, row_index,i,j,end_of_range) schedule(dynamic,1) shared(divTab, tabIndex)
	for (i = 1; i < tabIndex; i++) {
		end_of_range = endOfRange / divisors[i];
		for (int j = 3; j <= end_of_range; j = j + 2) {
			col_index = divisors[i] * j % matrixColumn;
			row_index = divisors[i] * j / matrixColumn;
			divTab[col_index][row_index] = false;
		}
	}


	//Liczenie ile udało się uzyskać liczb pierwszych 
	int temp_primeCounter = 0;
	int primeCounter = 0;
#pragma omp parallel for shared(divTab, matrixRow, matrixColumn) private(i,j) reduction(+: temp_primeCounter)
	for (i = 0; i < matrixRow; i++) {
		for (j = 0; j < matrixColumn; j++)
		{
			if (matrixColumn * i + j <= endOfRange && matrixColumn * i + j >= beginOfRange && divTab[j][i] == true) {
				temp_primeCounter++;
			}
		}
	}

	//Zakonczenie przetwarzania.
	stop = clock();

	primeCounter = temp_primeCounter;
	printf("Prime counter: %d, App was running: %fs. Was used: %d thread(s)\n", primeCounter, ((double)(stop-start) / 1000.0), THREADS);

	int save_to_xls;
	printf("\n\nDo you want save and see prime numbers?\n");
	printf("Type 1 for yest \n");
	scanf("%d", &save_to_xls);



	//Zmienne potrzebne do zapisywanie wynikow do pliku xls. 
	FILE* excel_file;
	char filename[17] = "primeCounter.xls\0";
	int temporary = 0;

	switch (save_to_xls)
	{
	case 1: 
		excel_file = fopen(filename, "w"); //flaga W otwiera plik w trybie nadpisywania - jednoczesnie usuwajac stare dane. 
		fprintf(excel_file, "Prime counter: %d, App was running at: %fs. Was used: %d thread(s)\n", primeCounter, ((double)(stop - start) / 1000.0), THREADS);
		for (i = 0; i < matrixRow; i++) {
			for (j = 0; j < matrixColumn; j++) {
				if (divTab[j][i] == 1) {
					if (temporary % 10 == 0)fprintf(excel_file, "\n");
					fprintf(excel_file, "%d, \t", matrixColumn * i + j);
					temporary++;
				}
			}
		}
		printf("Successfully saved to %s\n", filename);
		break;
	default:
		return EXIT_SUCCESS;
		break;
	}

	return 0;
}