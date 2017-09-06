#include "stupidscanf.h"

void stupidscanf(const char* expression, int *a, int *b) {
	int ta = 0, tb = 0, i, state = 1;
	unsigned len = strlen(expression);

	for (i = 0; i < len; i++) {
		if (state) {
			switch (expression[i]) {
				case '+':
				case '-':
				case '*':
				case '/':
					state = 0;
					break;
				default:
					ta *= 10;
					ta += (expression[i] - '0');
					break;
			}
		}
		else {
			tb *= 10;
			tb += (expression[i] - '0');
		}
	}

	*a = ta;
	*b = tb;
}
