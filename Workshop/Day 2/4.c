int main()
{
    static int a=1;
    { 
        int a=2;

        a += 1;
    }
    return 0;
}