#include <stdio.h>
#include <stdlib.h>

int main(){
    FILE *fptr;
    char s[10];

    fptr = fopen("dummy.txt", "r");
    if(fptr == NULL){
        printf("File cannot be opened");
        exit(0);
    }

    //fscanf(fptr, control_string, variable_list);

    while((fscanf(fptr, "%s", s)) != EOF){
        printf("%s", s);
    }
    printf("\n");

    fclose(fptr);
    return 0;
}