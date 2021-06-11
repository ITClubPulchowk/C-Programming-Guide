int main()
{
	char letter = 'u';
	int isVowel = 0;
	//is vowel is 0 if letter is consonant
	switch(letter)
	{
		case 'a':
			isVowel = 1;
			break;
		case 'e':
			isVowel = 1;
			break;
		case 'i':
			isVowel = 1;
			break;
		case 'o':
			isVowel = 1;
			break;
		case 'u':
			isVowel = 1;

		default:
			isVowel = 0;
			break;
	}

	return 0;
}