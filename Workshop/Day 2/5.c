// static variables

int count = 0;

int fun()
{
    static int count = 0;
    count++;
    return count;
}

int main()
{
    int val;
    
    val = 1;
    
    val = fun();

    val = fun();

    return 0;
}