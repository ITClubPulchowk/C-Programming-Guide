int string_length(char* string)
{
	char* beginning = string;
	for(; *string++ != '\0';);
	return (int)(string - beginning) - 1;
}

int string_compare2(char* string1, char* string2)
{
	int result = *string1 - *string2;
	while(*string1++ != '\0' && *string2++ != '\0')
	{
		result = *string1 - *string2;
		if (result != 0)
			break;
	}
	return result > 0 ?  1 : result < 0 ? -1 : 0;
}

int string_compare(char* string1, char* string2)
{
	int result = *string1 - *string2;
	while((*string1 != '\0') && (*string2 != '\0') && result == 0)
		result = *string1++ - *string2++;
	return  result > 0 ? 1 : result < 0 ? -1 : 0;
}

// Size of destination should be enough to accomodate both source1 and source2
void string_concatinate(char* destination, char* source1, char* source2)
{
	while (*source1 != '\0')
		*destination++ = *source1++;
	while (*source2 != '\0')
		*destination++ = *source2++;
	*destination = '\0';
}

// The size of destination should be large enough to accomodate the characters in size (i.e. sizeof(source) == sizeof(destination) at the least
void string_copy(char* destination, char* source)
{
	while ((*destination++ = *source++) != '\0');
}

int main()
{
	char* name = "Hari";
	char* other_name = "Harihar";
	char* other_nameee = "Hariharr";
	int length = string_length(name);
	int same_string = string_compare2(name, other_name);
	int same_string2 = string_compare2("HallO", "Hallo");
	int same_string3 = string_compare2("", "");
	char arr[100];
	string_copy(arr, "");
	return 0;
}