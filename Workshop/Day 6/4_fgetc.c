#include <stdio.h>
#include <stdlib.h>

int main(){
    FILE *f;
    char c;

    f = fopen("dummy.txt", "r");
    if(f == NULL){
        printf("File cannot be opened.");
        exit(0);
    }
    // fgetc
    // syntax : fgetc(fptr)
    while((c = fgetc(f)) != EOF){
        printf("%c", c);
    }
    printf("\n");

    fclose(f);
    return 0;
}