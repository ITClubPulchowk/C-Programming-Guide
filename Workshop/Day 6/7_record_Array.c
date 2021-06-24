#include <stdio.h>
#include <stdlib.h>

int main(){
    FILE *fptr;
    int num[3], i, number[3];

    fptr = fopen("dummy.txt", "w+b");
    if(fptr == NULL){
        printf("File cannot be opened");
        exit(0);
    }

    printf("Reading the array \n");
    for(i = 0; i < 3; i++){
        printf("Enter number #%d : ", i + 1);
        scanf("%d", &num[i]);
    }

    // fwrite(&ptr, sizeof(array), num of array, fptr);
    fwrite(&num, sizeof(num), 1, fptr);

    rewind(fptr);
    
    // fread(&ptr, sizeof(array), num of array, fptr);
    fread(&number, sizeof(num), 1, fptr);

    printf("Reading the number \n");
    for(i = 0; i < 3; i++){
        printf("%d \n", number[i]);
    }

    fclose(fptr);
    return 0;
}