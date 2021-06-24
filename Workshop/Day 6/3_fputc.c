#include <stdio.h>

int main(){
    FILE *fptr;
    char c;

    fptr = fopen("dummy.txt", "w");
    if(fptr == NULL){
        printf("File cannot be opened");
        exit(0);
    }

    // fputc
    // Syntax : fputc(c, file_ptr_variable);
    printf("Start writing what you want to enter in file...");
    while((c = getchar()) != '0'){
        fputc(c, fptr);
    }
    
    fclose(fptr);
    return 0;
}