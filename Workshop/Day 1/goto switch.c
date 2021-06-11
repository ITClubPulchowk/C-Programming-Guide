int main()
{
	int num = 8;
	int isodd;

	if (num % 2 == 0)
		goto even;
	else
		goto odd;
even:
	isodd = 0;
	goto end;
odd:
	isodd = 1;
	goto end;
end:
	return 0;
}