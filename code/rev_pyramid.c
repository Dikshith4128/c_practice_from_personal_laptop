#include <stdio.h>

int main(){

    int i, n;
    int x =1;

    for(i = 7; i >=0; i--){
        n = i;
        int y =i;
        if (n ==7){
        
            for(n; n !=0; n--){
                printf("*");
                     }
            for(y; y !=0; y--){
                printf("*");
                     }
        printf("\n");
        n--;
             }

    else{
     for (int j = 1; j <=x;j++) {
        printf(" ");
     }
            int w = n;
            int r = n;
            for(w; w !=0; w--){
                printf("*");
                     }
            for(r; r !=0; r--){
                printf("*");
                     }
                if (n ==0){
                    printf("*");
                } 

     for (int j =1; j <=x;j++) {
        printf(" ");
     }
     printf("\n");
     x++;
    }
    }
return 0;
}
