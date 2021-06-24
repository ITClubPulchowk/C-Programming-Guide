#include <stdio.h>

int main(){
    FILE *fptr;
    char sentence[100];

    fptr = fopen("dummy.txt", "a");
    if(fptr == NULL){
        printf("File cannot be opened. \n");
        exit(0);
    }
    // "%[^\n]" for multiple words.
    printf("Enter a sentence : ");
    scanf("%[^\n]", sentence);

    // fputs
    // Syntax : fputs(string, file_ptr); 
    fputs(sentence, fptr);
    
    fclose(fptr);
    return 0;
}