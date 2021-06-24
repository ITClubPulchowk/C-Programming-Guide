#include <stdio.h>

int main(){
    // Step 1: declare the file pointer
    FILE *fptr;

    // Step 2: open the file
    fptr = fopen("apple.txt", "w");

    // Step 3: close the file
    fclose(fptr);
    return 0;
}