#include <stdio.h>
#include <string.h>


int main() {
    FILE *fptr;
    char *ch = "Skillicon is Goated";
    char ch1;
    int i,  len = strlen(ch);

    fptr = fopen("/mnt/c/Users/diksh/Downloads/Training/c_prog/code/test.txt", "a+");
    if (fptr == NULL) {
        printf("ERROR in file opening\n");
        return 1;
    }
    else{
        for(i = 0; i < len; i++){
            fputc(ch[i],fptr);
        }
        fptr-=(len-1);
        while((ch1 = getc(fptr)) !=EOF){
            printf("%c",ch1);
        }

    }
    fclose(fptr);
    return 0;
}

