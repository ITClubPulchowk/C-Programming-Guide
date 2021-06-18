int main() {
    int i,j,a,matrix_reloaded[3][4] = { {1, 2, 3, 4},
                                        {5, 6, 7, 8},
                                        {9,10,11,12}};
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 4; j++) {
            matrix_reloaded[i][j] = matrix_reloaded[i][j]*2;
        }
    }
    return 0;
}