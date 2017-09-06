static short BASE_MASK = 0x4000;

#define LENGTH 200
#define WIDTH 200
#define GRID_LENGTH 40
#define GRID_WIDTH 40
#define NUM_BLOCKS 15

#define MASK0 0x7B6F
#define MASK1 0x2492
#define MASK2 0x73E7
#define MASK3 0x73CF
#define MASK4 0x5BC9
#define MASK5 0x79CF
#define MASK6 0x79EF
#define MASK7 0x7249
#define MASK8 0x7BEF
#define MASK9 0x7BC9
#define MASK_ADD 0x05D0
#define MASK_SUB 0x01C0
#define MASK_MULT 0x0FF8
#define MASK_DIV1 0x780F 
#define MASK_DIV2 0x0220 

short processImage(int xBase, int yBase);
void calcThreshold(int xBase, int yBase, short *thresholdRGB);
int processGridBlock(char blockno, int xBase, int yBase, short *thresholdRGB);
