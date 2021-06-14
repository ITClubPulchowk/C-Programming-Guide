int factorial( int n)
{
    if(n==1)return 1;
    return n*factorial(n-1);
}

int main()
{
    int facto;

    facto = factorial(4);

    return 0;
}