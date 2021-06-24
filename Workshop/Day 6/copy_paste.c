#include <stdio.h>
#include <stdlib.h>

int main(){
    FILE *s, *d;
    char c;

    s = fopen("source.txt", "r");
    d = fopen("destination.txt", "w");

    if(s == NULL){
        printf("Source file cannot be opened.");
        exit(0);
    }
    if(d == NULL){
        printf("Destination file cannot be opened");
        exit(0);
    }

    // fgetc(s) for reading from source file
    // fputc(character_variable, d) for writing 
    while((c = fgetc(s)) != EOF){
        fputc(c, d);
    }

    fclose(s);
    fclose(d);
}
