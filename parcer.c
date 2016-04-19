#include "parser.h"

char *substring(char *src, int start, int length) {
	char *res = (char *)malloc(sizeof(char) * (length + 1));
	if (res == NULL)
	{
		return NULL;
	}

	memcpy(res, src + start, length);
	res[length] = 0;

	return res;
}

void nextChar(char *expression) {
	pch = ch;
	ch = (++pos < (int)strlen(expression)) ? expression[pos] : -1;
}

int eat(char *expression, int charToEat) {
	while (ch == ' ' || ch == '\t') nextChar(expression);
	
	if (ch == charToEat) {
		nextChar(expression);
		return 1;
	}
	
	return 0;
}

double internalParse(char *expression) {
	nextChar(expression);
	double x = parseExpression(expression);
	if (pos < (int)strlen(expression)) return 0.001; // TODO throws errors
	return x;
}

double parseExpression(char *expression) {
	double x = parseTerm(expression);
	
	for (;;) {
		if (eat(expression, '+')) x += parseTerm(expression); // addition
		else if (eat(expression, '-')) x -= parseTerm(expression); // subtraction
		else return x;
	}
}

double parseTerm(char *expression) {
	double x = parseFactor(expression);
	for (;;) {
		if (eat(expression, '*')) x *= parseFactor(expression); // multiplication
		else if (eat(expression, '/')) x /= parseFactor(expression); // division
		else return x;
	}
}

double parseFactor(char *expression) {
	if (eat(expression, '+')) return parseFactor(expression); // unary plus
	if (eat(expression, '-')) return -parseFactor(expression); // unary minus

	double x = 0.0;
	int startPos = pos;
	if (eat(expression, '(')) { // parentheses
		x = parseExpression(expression);
		eat(expression, ')');
	} else if (isdigit(ch) || ch == '.' || (ch == 'e' && isdigit(pch)) || (ch == '-' && pch == 'e')) { // numbers
		while (isdigit(ch) || ch == '.' || (ch == 'e' && isdigit(pch)) || (ch == '-' && pch == 'e')) nextChar(expression);
		char *sub = substring(expression, startPos, pos);

		x = atof(sub);

		free(sub);
	} else if (isalpha(ch)) { // functions
		while (isalpha(ch)) nextChar(expression);
		char *func = substring(expression, startPos, pos);

		if (!strcmp(func, "e")) {
			x = M_E;
		}
		else if (!strcmp(func, "pi")) {
			x = M_PI;
		}
		else {
			x = parseFactor(expression);

			if (!strcmp(func, "sqrt")) x = sqrt(x);
			else if (!strcmp(func, "sin")) x = sin(x);
			else if (!strcmp(func, "arcsin")) x = asin(x);
			else if (!strcmp(func, "arccos")) x = acos(x);
			else if (!strcmp(func, "arctg")) x = atan(x);
			else if (!strcmp(func, "cos")) x = cos(x);
			else if (!strcmp(func, "tg")) x = tan(x);
			else if (!strcmp(func, "ln")) x = log(x);
			else if (!strcmp(func, "floor")) x = floor(x);
			else if (!strcmp(func, "ceil")) x = ceil(x);
			else {
				// TODO UNKNOWN_FUNCTION
			}
		}

		free(func);
	}
	else {
		// TODO CANNOT_PARSE
	}

	if (eat(expression, '^')) x = pow(x, parseFactor(expression)); // exponentiation

	return x;
}

int parse(char *expression, double *res) {
	pos = -1;
	*res = internalParse(expression);
	
	return 0;
}
