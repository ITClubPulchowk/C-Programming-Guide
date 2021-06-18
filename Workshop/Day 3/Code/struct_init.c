struct Student {
    int roll;
    int marks[5];
    float percent;
};

int main() {
    struct Student a = {
        2,{33,44,55,66,77} };
    int total = 0;
    int b = a.roll;
    int c = a.marks[0];
    float d = a.percent;
    a.roll = 1;
  // cant initialize matrix like a.marks={10,20,30,40,50}
      for (int i = 0; i < 5; i++) {
          total += a.marks[i]; //total= total+a.marks[i] 
      }
      a.percent = total / 5.0f;

    return 0;
}
















