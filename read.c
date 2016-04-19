#include <crtdbg.h>
#include "parser.h"

char* readLn(FILE *input)
{
	int n;
	int i, j;
	char c;
	char* s, *q;
	n = 2;
	i = 0;
	c = 0;
	s = (char*)malloc(sizeof(char) * n);
	if (s == NULL)
	{
		printf("ERROR can not allocate memory for new string");
		exit(1);
	}

	for (j = 0; j < n; j++)
	{
		s[j] = 0;
	}

	for (;;)
	{
		c = (char)fgetc(input);
		if (c == '\n' || c == '\r')
		{
			break;
		}
		else if (c == EOF)
		{
			if (strlen(s) == 0)
			{
				free(s);
				return NULL;
			}

			break;
		}

		if (i == n - 1)
		{
			++n;
			q = (char*)realloc(s, sizeof(char) * n);
			if (q == NULL)
			{
				printf("ERROR can not re-allocate memory for new characters");
				free(s);
				exit(1);
			}
			else
			{
				s = q;
			}
		}

		s[n - 1] = 0;
		s[i] = c;
		++i;
	}

	return s;
}

int isWhiteSpace(char symbol)
{
	if (symbol == ' ' || symbol == '\t')
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int isComment(char *string)
{
	int i;
	i = 0;
	while (string[i] != '/')
	{
		if (isWhiteSpace(string[i]))
		{
			i++;
		}
		else
		{
			return 0;
		}
	}

	if (string[i + 1] == '/')
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int isEmpty(char *string)
{
	int i;
	i = 0;

	if (string[0] == '\0')
	{
		return 1;
	}
	while (string[i] != '\0')
	{
		if (!isWhiteSpace(string[i]))
		{
			return 0;
		}
		else
		{
			++i;
		}
	}

	return 1;
}

void out(char *string, double res)
{
	printf("%s == %g\n", string, res);
}

int main(int argc, char* argv[])
{
	FILE *input;
	char *string;
	int error;
	double res = 0;

	if (argc == 1)
	{
		input = stdin;
	}
	else if (argc == 2)
	{
		error = fopen_s(&input, argv[1], "r");
		if (error != 0 || input == NULL)
		{
			printf("ERROR not correct document");
			exit(1);
		}
	}
	else
	{
		printf("ERROR not correct arguments");
		exit(1);
	}

	for (;;)
	{
		string = readLn(input);

		if (string == NULL)
		{
			break;
		}
		if (isComment(string) || isEmpty(string))
		{
			puts(string);
		}
		else
		{
			error = parse(string, &res);
			if (!error) {
				out(string, res);
			}
			else {
				switch (error) {
				case MEMORY_ERROR:
					printf("%s == ERROR memory error\n", string);
					break;
				case UNKNOWN_VARIABLE:
					printf("%s == ERROR unknown variable\n", string);
					break;
				case UNKNOWN_FUNCTION:
					printf("%s == ERROR unknown function\n", string);
					break;
				case ILLEGAL_LOG_ARG:
					printf("%s == ERROR illegal log argument\n", string);
					break;
				case ILLEGAL_TG_ARG:
					printf("%s == ERROR illegal tan argument\n", string);
					break;
				case ILLEGAL_CTG_ARG:
					printf("%s == ERROR illegal ctan argument\n", string);
					break;
				case ILLEGAL_ASIN_ARG:
					printf("%s == ERROR illegal asin argument\n", string);
					break;
				case ILLEGAL_ACOS_ARG:
					printf("%s == ERROR illegal acos argument\n", string);
					break;
				case ILLEGAL_SQRT_ARG:
					printf("%s == ERROR illegal square root argument\n", string);
					break;
				case ILLEGAL_EXP_ARG:
					printf("%s == ERROR illegal exponention argument\n", string);
					break;
				case ILLEGAL_NUMBER:
					printf("%s == ERROR illegal number format\n", string);
					break;
				case NO_CLOSING_BRACKET:
					printf("%s == ERROR no closing bracket\n", string);
					break;
				case DIVISION_BY_ZERO:
					printf("%s == ERROR division by zero\n", string);
					break;
				case CANNOT_PARSE:
					printf("%s == ERROR cannot parse\n", string);
					break;
				}
			}
		}
		free(string);
	}

	fclose(input);
	_CrtDumpMemoryLeaks();
	return 0;
}