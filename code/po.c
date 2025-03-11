#include <stdio.h>

int num(int *nuam){
    printf("%p\n",nuam);
    (*nuam)++;
}


int main() {
    int a  =0;

    num(&a);
    int b = 1;
    printf("%d\n",b);
    printf("%d\n",a);

return 0;
}
