#include <stdio.h>

int main () {


int begin, end;

printf("Enter begin of range\n");
scanf("%d", &begin);
printf("Enter end of range\n");
scanf("%d", &end);


printf("Range is starting in %d, and finish at %d\n", begin, end); 


int i, j;

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

return 0;
}
