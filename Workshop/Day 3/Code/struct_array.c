struct Student {
    int roll;
    int marks[5];
    float percent;
};

int fun(struct Student a) {
   
}

int main() {
    struct Student a[5] = { {1,{95,95,95,97,87}},{2,{34,45,66,78,88}} };
    int i, total = 0;
    for (i = 0; i < 5; i++) {
        total += a[0].marks[i];
    }
    a[0].percent = total / 5.0f;
    return 0;
}