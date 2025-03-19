#include <stdio.h>
#include <stdlib.h>

int main (){

    int *ptr_i;
    int i = 0;

    ptr_i = calloc(sizeof(int),10);

    for (i = 0; i < 10; i++){
        *ptr_i = i;
        ptr_i++;
    }

    ptr_i--;
    i = 10;
    while(i > 0){
        printf("%d\n", *ptr_i--);
                i--;
    }
    ptr_i++;    
    free(ptr_i);

return 0;
}
