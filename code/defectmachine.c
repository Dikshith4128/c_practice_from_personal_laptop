#include <stdio.h>

void defmachine(int*,float*,int*);

int main (){

        int n_rj,out;
        float rjp;

        printf("Enter the number of bats rejected\n");
        scanf("%d",&n_rj);
        printf("Enter the error percentage of bats rejected\n");
        scanf("%f",&rjp);

        defmachine(&n_rj, &rjp, &out);

        printf("number of batts manufacture in a day id %d bats are rejected and rejection percentage is %0.3f is \n = %d\n", n_rj, rjp, out);

return 0;
}

void defmachine(int *num, float *per, int *ans){

    *ans = ((*num) * 100) / *per; 

}
