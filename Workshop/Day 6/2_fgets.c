#include <stdio.h>

int main(){
    FILE *fptr;
    char s[100];

    fptr = fopen("dummy123.txt", "r");
    if(fptr == NULL){
        printf("File cannot be opened. \n");
        exit(0);
    }
    // fgets
    // Syntax : fgets(string_variable, int_value, fptr)
    fgets(s, 20, fptr);

    fclose(fptr);
    return 0;
}