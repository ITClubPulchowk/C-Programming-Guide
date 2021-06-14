// Static variables inside a block

int main()
{
    int val = 0;
    for(int i=0; i<2; i++)
    {
        static int k = 1;
        k++;
        val = k;
    }

    return 0;
}