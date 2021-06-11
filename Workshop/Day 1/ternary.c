
int main()
{
	//ternery operator
	int a = 1;
	int b = 3;
	int c = 4;
	int greatest = (a > b) ? (a > c ? a : c) : (b > c ? b : c);

	//if else equivalent
	//if (a > b)
	//{
	//	if (a > c)
	//		greatest = a;
	//	else
	//		greatest = c; 
	//}
	//else
	//{
	//	if (b > c)
	//		greatest = b;
	//	else
	//		greatest = c;
	//}
}