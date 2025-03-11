#include <stdio.h>

typedef struct {
int i ;
char a ;
}n1;

int main(){
 n1 new;

new.i = 5;
new.a = 'A';
printf("%d",new.i);
printf("%c",new.a);


return 0;
}
