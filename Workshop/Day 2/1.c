// need for functions.


int is_prime(int number);


int main()
{
    int number = 17;

    int prime_ho_ki_nai;

    prime_ho_ki_nai = is_prime(17);

    int another_no = 97;

    prime_ho_ki_nai = is_prime(another_no);

    return 0;
}


int is_prime(int number)
{
    int is_prime = 1;

    for(int i=2;i<number;i++)
    {
        if(number % i == 0)
        {
            is_prime = 0;
            break;
        }
    }
    return is_prime;
}