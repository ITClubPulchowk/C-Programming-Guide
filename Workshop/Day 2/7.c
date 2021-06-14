// difference between global and static variables

int a = 5;

int fun()
{
    a = 7;
    return a;
}

int main()
{
    int b;

    b = fun();

    return 0;
}