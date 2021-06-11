int main()

{
    int isVowel = 0;
    char letter = 'i';

    if (letter == 'a')
        goto a;
    if (letter == 'e')
        goto e;
    if (letter == 'i')
        goto i;
    if (letter == 'o')
        goto o;
    if (letter == 'u')
        goto u;

    goto end;
a:
    isVowel = 1;
    goto end;
e:
    isVowel = 1;
    goto end;
i:
    isVowel = 1;
    goto end;
o:
    isVowel = 1;
    goto end;
u:
    isVowel = 1;
    goto end;
end:
    return 0;
}