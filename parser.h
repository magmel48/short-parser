#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#define M_E 2.71828182845904523536
#define M_PI 3.14159265358979323846

typedef enum {
	MEMORY_ERROR = 1,
	UNKNOWN_VARIABLE,
	UNKNOWN_FUNCTION,
	ILLEGAL_LOG_ARG,
	ILLEGAL_TG_ARG,
	ILLEGAL_CTG_ARG,
	ILLEGAL_ASIN_ARG,
	ILLEGAL_ACOS_ARG,
	ILLEGAL_SQRT_ARG,
	ILLEGAL_NUMBER,
	ILLEGAL_EXP_ARG,
	NO_CLOSING_BRACKET,
	DIVISION_BY_ZERO,
	CANNOT_PARSE
} ERROR;

static int pos = -1;

static char pch;
static char ch;
static char nch;
static char n2ch;

char* substring(char*, int, int);

void nextChar();
int parse(char *, double *);
int internalParse();
int parseExpression();
int parseTerm();
int parseFactor();
