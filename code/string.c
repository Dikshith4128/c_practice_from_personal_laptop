#include <stdio.h>

#define SIZE 20


void pstr(char[SIZE]);

int main (){

    char str[SIZE];

    printf("enter company name\n");
    fgets(str,SIZE,stdin);
    pstr(str);


return 0;
}

void pstr(char s[SIZE]){

    printf("%s\n",s);

}
