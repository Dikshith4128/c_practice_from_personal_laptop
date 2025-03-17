#include <stdio.h>
#include <stdlib.h>

int main (){

    int *ptr;
    char *ptr_ch;
    float *ptr_f;

    ptr = malloc(sizeof(int));
    ptr_ch = malloc(sizeof(char));
    ptr_f = malloc(sizeof(float));

    *ptr = 1;
    *ptr_ch = 'A';
    *ptr_f = 1.111;

    printf("int %d\n",*ptr);
    printf("char %c\n",*ptr_ch);
    printf("float %f\n",*ptr_f);

    free(ptr);
    free(ptr_f);
    free(ptr_ch);

return 0;
}
