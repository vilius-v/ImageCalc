#include "processImage.h"
#include "xio.h"

char determineChar(short *arr, int len) {
	float sum[15] = {0};
	int i, j;

	for (i = 0; i < len; i++) {
		short bitcode = arr[i];
		for (j = 0; j < NUM_BLOCKS; j++) {
			if (bitcode & (BASE_MASK >> j)) {
				sum[j]++;
		  	}
		}
	}

	float avg;
	short num = 0;
	short printval;
	for (j = 0; j < NUM_BLOCKS; j++) {
		avg = sum[j] / len;
		if (avg >= 0.5) {
			num |= (BASE_MASK >> j);
		}
	}

	// Division detection
	if ((~num & MASK_DIV1) == MASK_DIV1 && (num & MASK_DIV2)) {
     // checks if blocks 0 - 3 and 11 -14 rows are not filled
     // && if blocks 5 and 9 are filled
		return '/';
	}

	switch (num) {
		case MASK0: return '0';
		case MASK1: return '1';
		case MASK2: return '2';
		case MASK3: return '3';
		case MASK4: return '4';
		case MASK5: return '5';
		case MASK6: return '6';
		case MASK7: return '7';
		case MASK8: return '8';
		case MASK9: return '9';
		case MASK_ADD: return '+';
		case MASK_MULT: return '*';
		case MASK_SUB: return '-';
		default: return '?';
	}
}
