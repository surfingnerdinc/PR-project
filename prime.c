#include <stdio.h>
#include <time.h>
#include <omp.h>




int main () {

clock_t start, stop;

int begin, end;

printf("Enter begin of range\n");
scanf("%d", &begin);
printf("Enter end of range\n");
scanf("%d", &end);


printf("Range is starting in %d, and finish at %d\n", begin, end); 

start = clock();
int i, j;

omp_set_num_threads(4);
#pragma omp parallel for

for(i = begin; i <= end; i++) { 
 for(j=2; j<=i; j++){
  if(i%j == 0){
   break;
}
}
 if(i == j) {
  printf("Yup for %d\n", i); 
}
}

stop = clock();


printf("Start: %d\n", start);
printf("Stop: %d\n", stop);

printf("czas przetwarzania wynosi %f sekund\n", ((double)(stop-start) / 1000.0));
printf("Czas przetwarzania w minutach wynosi %f min \n", ((double)(stop-start) / 1000.0 / 60));


return 0;
}
