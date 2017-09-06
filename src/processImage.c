#include "processImage.h"
#include "xio.h"
#include "xparameters.h"

/*
        0    1    2
+----+----+----+----+----+
|    |  0 |  1 |  2 |    |  0
+----+----+----+----+----+
|    |  3 |  4 |  5 |    |  1
+----+----+----+----+----+
|    |  6 |  7 |  8 |    |  2
+----+----+----+----+----+
|    |  9 | 10 | 11 |    |  3
+----+----+----+----+----+
|    | 12 | 13 | 14 |    |  4
+----+----+----+----+----+

*/


int processGridBlock(char blockno, int xBase, int yBase, short *thresholdRGB) {
	int xStart = blockno % 3;
	int yStart = 0;
	if (blockno < 3) yStart = 0;
	else if (blockno < 6) yStart = 1;
	else if (blockno < 9) yStart = 2;
	else if (blockno < 12) yStart = 3;
	else yStart = 4;

	int posY = yBase + yStart*GRID_LENGTH;
	int yEnd = posY + GRID_LENGTH;
	int posX = xBase + GRID_WIDTH + xStart*GRID_WIDTH;
	xStart = posX;
	int xEnd = posX + GRID_WIDTH;


	// Average the pixel color of the block
	short numPixels = GRID_WIDTH * GRID_LENGTH;
	short avgR = 0;
	short avgG = 0;
	short avgB = 0;
	u16 color = 0;

	for ( ; posY < yEnd; posY++) {
		for (posX = xStart ; posX < xEnd; posX++) {
			color = XIo_In16(XPAR_DDR2_SDRAM_MPMC_BASEADDR + 2*(posY*2560+posX));
			avgB += (color&0x000f);
			avgG += ((color&0x00f0)>>4);
			avgR += ((color&0x0f00)>>8);
		}
	}

	avgR /= numPixels;
	avgG /= numPixels;
	avgB /= numPixels;

	//xil_printf("Block number: %d, value of R: %d \n\r", blockno, avgR);
	//xil_printf("Block number: %d, value of G: %d \n\r", blockno, avgG);
	//xil_printf("Block number: %d, value of B: %d \n\r", blockno, avgB);

	u8 sum = 0;
	if(avgR < thresholdRGB[0]-2){
		sum += 1;
	}
	if(avgG < thresholdRGB[1]-2){
		sum += 1;
	}
	if(avgB < thresholdRGB[2]-2){
		sum += 1;
	}

	if (sum >= 2)
		return 1;

	return 0;
}

void calcThreshold(int xBase, int yBase, short *thresholdRGB) {
	int xStart = xBase-60; // the +1 is to avoid the red boxes, not sure if needed
	int yStart = yBase-60; // ^^
	int posX, posY;
	int xEnd = xStart + GRID_WIDTH;
	int yEnd = yStart + GRID_LENGTH;

	// Average the pixel color of the block
	short numPixels = GRID_WIDTH * GRID_LENGTH;
	short avgR = 0;
	short avgG = 0;
	short avgB = 0;
	u16 color = 0;

    for (posY = yStart; posY < yEnd; posY++) {
    	for (posX = xStart ; posX < xEnd; posX++) {
			color = XIo_In16(XPAR_DDR2_SDRAM_MPMC_BASEADDR + 2*(posY*2560+posX));
			avgB += (color&0x000f);
			avgG += ((color&0x00f0)>>4);
			avgR += ((color&0x0f00)>>8);
    	}
    }

	avgR /= numPixels;
	avgG /= numPixels;
	avgB /= numPixels;

	thresholdRGB[0] = avgR;
	thresholdRGB[1] = avgG;
	thresholdRGB[2] = avgB;
}

short processImage(int xBase, int yBase) {
	short num = 0;
	short threshold[3];
	char n;

	calcThreshold(xBase, yBase, threshold);
	//xil_printf("Threshold 0: %d \n\r", threshold[0]);
	//xil_printf("Threshold 1: %d \n\r", threshold[1]);
	//xil_printf("Threshold 2: %d \n\r", threshold[2]);

	int val;
	for (n = 0; n < NUM_BLOCKS; n++) {
		val = processGridBlock(n, xBase, yBase, threshold);
		if (val) {
			num |= (BASE_MASK >> n);
		}
		// xil_printf("Val of block: %d is %d \n\r", n, val);
	}

	return num;
}
