#include <stdio.h>

#define SIZE 20

int main (){

    char arr[SIZE];
    printf("GETSTRING\n");
    int x = 0;
    
    fgets(arr,SIZE,stdin);
    printf("%s\n",arr);
    while (arr[x] !='\0'){
          x+=1;
    }
    printf("%d\n",(x-1));

return 0;
}
