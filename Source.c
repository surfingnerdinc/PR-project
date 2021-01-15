#define _CRT_SECURE_NO_DEPRECATE
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>


#define THREADS 4			


int main(int argc, char* argv[]) {
	
	//moje
	clock_t start, stop;

	
	int beginOfRange, endOfRange;
	printf("Set begin of range \n");
	scanf("%d", &beginOfRange);


	printf("Set end of range \n");
	scanf("%d", &endOfRange);


	int mysqrt = (int)sqrt(endOfRange);
	int matrixColumn = 256;
	int matrixRow = (endOfRange / matrixColumn) + 1;
	int end_of_range = 0;
	int tmpCounter = 0;
	int tabIndex = 0;
	int matrix_currentColumn = 0;
	int matrix_currentRow = 0;
	int i, j = 0;
	int col_index = 0, row_index = 0;
	int temp_primeCounter = 0;
	int primeCounter = 0;
	int* divisors;
	bool** divTab;



				//inicjalizacja macierzy u�ywanej podczas wykre�lania liczb pierwszych
	divTab = malloc((matrixColumn + 1) * sizeof(bool*));
	for (int i = 0; i <= matrixColumn; ++i) {
		divTab[i] = malloc((matrixRow + 1) * sizeof(bool));
	}


	//inicjalizacja tablicy z podzielnikami
	divisors = (int*)calloc((mysqrt + 1), sizeof(int));


	//zerowanie czasu rozpocz�cia i zko�czenia
	//ustalanie ilo�ci w�tk�w
	//czas rozpocz�cia wykonywania programu
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


	//wykre�lanie liczb, liczbami parzystymi
	end_of_range = endOfRange / 2;
	printf("%d", end_of_range);
#pragma omp parallel for shared(divTab,end_of_range) private(col_index, row_index, i) schedule(static,32)
	for (i = 2; i <= end_of_range; i++) {
		col_index = 2 * i % matrixColumn;
		row_index = 2 * i / matrixColumn;
		divTab[col_index][row_index] = false;
	}

	//wykre�lanie pozosta�ych liczb, liczbami pierwszych
#pragma omp parallel for private(col_index, row_index,i,j,end_of_range) schedule(dynamic,1) shared(divTab, tabIndex)
	for (i = 1; i < tabIndex; i++) {
		end_of_range = endOfRange / divisors[i];
		for (int j = 3; j <= end_of_range; j = j + 2) {
			col_index = divisors[i] * j % matrixColumn;
			row_index = divisors[i] * j / matrixColumn;
			divTab[col_index][row_index] = false;
		}
	}

	//wypisywanie - zlicznie pierwszych
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
	printf("Prime counter: %d, App was running at: %fs. Was used: %d thread(s)\n", primeCounter, ((double)(stop-start) / 1000.0), THREADS);

	int save_to_xls;
	printf("Do you want save and see prime numbers?\n");
	printf("Type 1 for yest \n");
	scanf("%d", &save_to_xls);

	if (save_to_xls == 1) {
		FILE* f = fopen("primeCounter.xls", "w");
		fprintf(f, "Prime counter: %d, App was running at: %fs. Was used: %d thread(s)\n", primeCounter, ((double)(stop - start) / 1000.0), THREADS);
		for (i = 0; i < matrixRow; i++) {
			for (j = 0; j < matrixColumn; j++) {
				if (divTab[j][i] == 1) {
					if (tmpCounter % 10 == 0)fprintf(f, "\n");
					fprintf(f, "%d, \t", matrixColumn * i + j);
					tmpCounter++;
				}
			}
		}
		fclose(f);
	}


	return 0;
}