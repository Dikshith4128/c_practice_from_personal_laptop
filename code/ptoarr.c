#include <stdio.h>

#define SIZE 20

int main (){

    char arr[SIZE] = "adbcef adwedfa";
    char *ptr_arr = &arr[0];

    printf("%s\n", ptr_arr);

return 0;
}
