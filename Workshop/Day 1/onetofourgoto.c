int main()
{
	int i = 1;
	HERE:
	if (i <= 5)
	{
		i++;
		goto HERE;
	}
	return 0;
}