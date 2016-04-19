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

int internalParse(char *expression, double *res) {
	int error = 0;
	
	nextChar(expression);
	error = parseExpression(expression, res);
	if (pos < (int)strlen(expression) && !error) error = CANNOT_PARSE;
	
	return error;
}

int parseExpression(char *expression, double *res) {
	double x = *res;
	int error = parseTerm(expression, res);

	for (;;) {
		if (error) {
			return error;
		}

		if (eat(expression, '+')) {
			error = parseTerm(expression, &x); // addition
			*res += x;
		} else if (eat(expression, '-')) {
			error = parseTerm(expression, &x); // subtraction
			*res -= x;
		} else {
			return error;
		}
	}
}

int parseTerm(char *expression, double *res) {
	double x = *res;
	int error = parseFactor(expression, res);
	
	for (;;) {
		if (error) {
			return error;
		}

		if (eat(expression, '*')) {
			error = parseFactor(expression, &x); // multiplication
			*res *= x;
		} else if (eat(expression, '/')) {
			error = parseFactor(expression, &x); // division
			if (fabs(x) < 0.00001) {
				error = DIVISION_BY_ZERO;
			} else {
				*res /= x;
			}
		} else {
			return error;
		}
	}
}

int parseFactor(char *expression, double *res) {
	int error = 0;
	int startPos = 0;

	double x = 0.0;
	double y = 0.0;

	if (eat(expression, '+')) {
		error = parseFactor(expression, res); // unary plus
		*res = +(*res);
		
		return error;
	}
	
	if (eat(expression, '-')) {
		error = parseFactor(expression, res); // unary minus
		*res = -(*res);
		
		return error;
	}

	startPos = pos;

	if (eat(expression, '(')) { // parentheses
		error = parseExpression(expression, &x);
		eat(expression, ')');
	} else if (isdigit(ch) || ch == '.' || (ch == 'e' && isdigit(pch)) || (ch == '-' && pch == 'e')) { // numbers
		while (isdigit(ch) || ch == '.' || (ch == 'e' && isdigit(pch)) || (ch == '-' && pch == 'e')) {
			nextChar(expression);
		}
		
		char *sub = substring(expression, startPos, pos - startPos);
		if (sub == NULL) {
			return MEMORY_ERROR;
		}

		x = atof(sub);

		free(sub);
	} else if (isalpha(ch)) { // functions
		while (isalpha(ch)) nextChar(expression);
		char *func = substring(expression, startPos, pos - startPos);
		if (func == NULL) {
			return MEMORY_ERROR;
		}

		if (!strcmp(func, "e")) {
			x = M_E;
		}
		else if (!strcmp(func, "pi")) {
			x = M_PI;
		}
		else {
			error = parseFactor(expression, &x);
			if (error) {
				free(func);
				return error;
			}

			if (!strcmp(func, "sqrt")) {
				if (x < 0) {
					error = ILLEGAL_SQRT_ARG;
				} else {
					x = sqrt(x);
				}
			}
			else if (!strcmp(func, "sin")) x = sin(x);
			else if (!strcmp(func, "arcsin")) {
				if (x < -1 || x > 1) {
					error = ILLEGAL_ASIN_ARG;
				} else {
					x = asin(x);
				}
			}
			else if (!strcmp(func, "arccos")) {
				if (x < -1 || x > 1) {
					error = ILLEGAL_ACOS_ARG;
				} else {
					x = acos(x);
				}
			}
			else if (!strcmp(func, "arctg")) x = atan(x);
			else if (!strcmp(func, "cos")) x = cos(x);
			else if (!strcmp(func, "tg")) {
				if (fabs(cos(x)) < 0.00001) {
					error = ILLEGAL_TG_ARG;
				} else {
					x = tan(x);
				}
			}
			else if (!strcmp(func, "ctg")) {
				if (fabs(sin(x)) < 0.00001) {
					error = ILLEGAL_CTG_ARG;
				} else {
					x = 1 / tan(x);
				}
			}
			else if (!strcmp(func, "ln")) {
				if (x <= 0) {
					error = ILLEGAL_LOG_ARG;
				} else {
					x = log(x);
				}
			}
			else if (!strcmp(func, "floor")) x = floor(x);
			else if (!strcmp(func, "ceil")) x = ceil(x);
			else {
				error = UNKNOWN_FUNCTION;
			}
		}

		free(func);
	}
	else {
		error = CANNOT_PARSE;
	}

	if (eat(expression, '^')) {
		error = parseFactor(expression, &y);
		x = pow(x, y); // exponentiation
	}

	*res = x;

	return error;
}

int parse(char *expression, double *res) {
	pos = -1;
	return internalParse(expression, res);
}
