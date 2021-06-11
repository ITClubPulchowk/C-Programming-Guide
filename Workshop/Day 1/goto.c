int main()
{
	int i=0;
	int inconditional = 0;
	if (i == 1)
	{
	here:
		inconditional = 1;
		goto there;
	}
	goto here;
	there:
	return 0;
}