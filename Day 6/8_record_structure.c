#include <stdio.h>

struct movie{
    char name[50], genre[20];
    int year_released; 
};

int main(){
    struct movie m[2], m2[2];
    
    FILE *fptr = fopen("movie.txt", "w+b");

    for(int i = 0; i < 2; i++){
        printf("Enter the movie name : ");
        scanf("%s", m[i].name);
        printf("Enter the genre : ");
        scanf("%s", m[i].genre);
        printf("Enter the year released : ");
        scanf("%d", &m[i].year_released);
    }

    // Syntax : fwrite(&ptr, sizeof(structure), num of structure, fptr);
    fwrite(&m, sizeof(struct movie), 2, fptr);

    rewind(fptr);
    
    // Syntax : fread(&ptr, sizeof(structure), num of structure, fptr);
    fread(&m2, sizeof(struct movie), 2, fptr);

    // Use the fread data to print
    for(int i = 0; i < 2; i++){
        printf("Movie #%d \n", i+1);
        printf("name : %s \n", m2[i].name);
        printf("genre : %s \n", m2[i].genre);
        printf("year released : %d \n", m2[i].year_released);
        printf("\n");
    }

    return 0;
}
