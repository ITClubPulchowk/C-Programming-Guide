void main()
{
	int n = 1;
	int isodd;
	START:
	if (n <= 5)
	{
		if (n % 2 == 0)
			goto EVEN;
		goto ODD;
	}
	goto end;
EVEN:
	// 0 means even
	isodd = 0;
	n++;
	goto START;
ODD:
	// 1 is odd
	isodd = 1;
	n++;
	goto START;
end:
	return 0;
}