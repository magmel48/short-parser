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

int checkBrackets(char *expression) {
	int balance = 0;
	int i = 0;
	while (i < (int)strlen(expression)) {
		if (expression[i] == '(' && balance >= 0) ++balance;
		if (expression[i] == ')') --balance;
		++i;
	}

	return balance == 0;
}

int firstIndexOf(char *expression, char symbol) {
	int i = 0;

	while (i < (int)strlen(expression)) {
		if (expression[i] == symbol) {
			return i;
		}

		++i;
	}

	return -1;
}

int lastIndexOf(char *expression, char symbol) {
	int i = (int)strlen(expression) - 1;
	
	while (i >= 0) {
		if (expression[i] == symbol) {
			return i;
		}

		--i;
	}

	return -1;
}

void nextChar(char *expression) {
	pch = ch;
	ch = (++pos < (int)strlen(expression)) ? expression[pos] : -1;
	nch = (pos + 1 < (int)strlen(expression)) ? expression[pos + 1] : -1;
	n2ch = (pos + 2 < (int)strlen(expression)) ? expression[pos + 2] : -1;
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
	int error = parseFactor(expression, res, 0);
	
	for (;;) {
		if (error) {
			return error;
		}

		if (eat(expression, '*')) {
			error = parseFactor(expression, &x, 0); // multiplication
			*res *= x;
		} else if (eat(expression, '/')) {
			error = parseFactor(expression, &x, 0); // division
			
			if (!error) {
				if (fabs(x) < 0.00001) {
					error = DIVISION_BY_ZERO;
				}
				else {
					*res /= x;
				}
			}
		} else {
			return error;
		}
	}
}

int isNumber() {
	return isdigit(ch) ||
		(ch == '.' && (isdigit(pch) || isdigit(nch))) ||
		((ch == 'e' || ch == 'E') && (isdigit(pch) || pch == '.') && (((nch == '-' || nch == '+') && (isdigit(n2ch))) || isdigit(nch))) ||
		((ch == '-' || ch == '+') && (pch == 'e' || pch == 'E') && isdigit(nch));
}

int parseFactor(char *expression, double *res, int isFunction) {
	int error = 0;
	int startPos = 0;

	double x = 0.0;
	double y = 0.0;

	int isInitialized = 0;

	if (eat(expression, '+')) {
		error = parseFactor(expression, res, 0); // unary plus
		*res = +(*res);
		
		return error;
	}
	
	if (eat(expression, '-')) {
		error = parseFactor(expression, res, 0); // unary minus
		*res = -(*res);
		
		return error;
	}

	startPos = pos;

	if (eat(expression, '(')) { // parentheses
		error = parseExpression(expression, &x);
		isInitialized = 1;

		if (isFunction) {
			if (eat(expression, ')')) {
				*res = x;
				return error;
			}
		}
		else {
			eat(expression, ')');
		}
	} else if (isNumber() && !isFunction) { // numbers
		while (isNumber()) {
			nextChar(expression);
		}
		
		char *sub = substring(expression, startPos, pos - startPos);
		if (sub == NULL) {
			return MEMORY_ERROR;
		}

		if (strstr(sub, "e") && strstr(sub, ".")) {
			if (lastIndexOf(sub, 'e') < lastIndexOf(sub, '.')) {
				free(sub);
				return CANNOT_PARSE;
			}
		}

		if (firstIndexOf(sub, 'e') != lastIndexOf(sub, 'e')) {
			free(sub);
			return CANNOT_PARSE;
		}

		if (firstIndexOf(sub, '.') != lastIndexOf(sub, '.')) {
			free(sub);
			return CANNOT_PARSE;
		}

		x = atof(sub);
		isInitialized = 1;

		free(sub);
	} else if (isalpha(ch) && !isFunction) { // functions
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
			error = parseFactor(expression, &x, 1);
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
					x = 1.0 / tan(x);
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

		isInitialized = 1;

		free(func);
	}
	else {
		error = CANNOT_PARSE;
	}

	if (eat(expression, '^') && !isFunction && !error && isInitialized) {
		error = parseFactor(expression, &y, 0);
		if (x < 0 && fabs(y - (int)y) > 0.00001) {
			error = ILLEGAL_EXP_ARG;
		} if (x == 0 && y <= 0) {
			error = ILLEGAL_EXP_ARG;
		} else {
			x = pow(x, y); // exponentiation
		}
	}

	*res = x;

	return error;
}

int parse(char *expression, double *res) {
	pch = -1;
	ch = -1;
	nch = -1;
	n2ch = -1;

	pos = -1;
	
	if (checkBrackets(expression)) {
		return internalParse(expression, res);
	}

	return CANNOT_PARSE;
}
