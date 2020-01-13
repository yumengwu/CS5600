#include <stdio.h> 

int checkNumber(char * str)
{
    while (*str != '\0')
    {
        if (!(*str >= '0' && *str <= '9'))
        {
            return 0;
        }
        ++str;
    }
    return 1;
}

int checkOp(char * str)
{
    if (!(*str == '+' || *str == '-' || *str == '*' || *str == '/'))
    {
        return 0;
    }
    ++str;
    return *str == '\0' ? 1 : 0;
}

int calc(int a, char * op, int b)
{
    switch (*op)
    {
        case '+':
            return a + b;
        case '-':
            return a - b;
        case '*':
            return a * b;
        case '/':
            return a / b;
    }
}

int main(int argc, char ** argv)
{
    if (argc != 4)
    {
        printf("Usage:\n  ./ccalc N op N\n");
        return 1;
    }
    if (!(checkNumber(argv[1]) && checkNumber(argv[3]) && checkOp(argv[2])))
    {
        printf("Usage:\n  ./ccalc N op N\n");
        return 1;
    }
    for (int i = 1; i < argc; ++i)
    {
        printf("%s ", argv[i]);
    }
    printf("= %d\n", calc(atoi(argv[1]), argv[2], atoi(argv[3])));
    return 0;
}
