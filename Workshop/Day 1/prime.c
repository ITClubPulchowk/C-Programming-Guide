int main()
{
	int num = 6;
	int isPrime = 1;
	for (int i = 2; i <= num - 1; i++)
	{
		if (num % i == 0)
		{
			isPrime = 0;
			break;

		}
		else
			continue;
	}
}