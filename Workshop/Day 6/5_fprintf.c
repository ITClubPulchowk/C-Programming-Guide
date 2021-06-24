#include <stdio.h>
#include <stdlib.h>

int main(){
    FILE *fptr;
    char name[20];

    fptr = fopen("dummy.txt", "w");
    if(fptr == NULL){
        printf("File cannot be opened");
        exit(0);
    }

    printf("Enter your name : ");
    scanf("%s", name);

    // fprintf();
    fprintf(fptr, "Name : %s", name);

    fclose(fptr);
    return 0;
}