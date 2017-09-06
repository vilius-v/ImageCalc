#include <stdio.h>
#include <xio.h>

//#include "platform.h"
#include "xparameters.h"
#include "cam_ctrl_header.h"
#include "vmodcam_header.h"
#include "xgpio.h"
#include "xstatus.h"

#include "processImage.h"
#include "determineChar.h"
#include "stupidscanf.h"

#define blDvmaCR		0x00000000 // Control Reg Offset
#define blDvmaFWR		0x00000004 // Frame Width Reg Offset
#define blDvmaFHR		0x00000008 // Frame Height Reg Offset
#define blDvmaFBAR		0x0000000c // Frame Base Addr Reg Offset
#define blDvmaFLSR		0x00000010 // Frame Line Stride Reg Offeset
#define blDvmaHSR		0x00000014 // H Sync Reg Offset
#define blDvmaHBPR		0x00000018 // H Back Porch Reg Offset
#define blDvmaHFPR		0x0000001c // H Front Porch Reg Offset
#define blDvmaHTR		0x00000020 // H Total Reg Offset
#define blDvmaVSR		0x00000024 // V Sync Reg Offset
#define blDvmaVBPR		0x00000028 // V Back Porch Reg Offset
#define blDvmaVFPR		0x0000002c // V Front Porch Reg Offset
#define blDvmaVTR		0x00000030 // V Total Reg Offset

#define EXPRESSION_SIZE 10
XGpio switches;

void printAnswer(const char* expression, u8 num_elem, char operator) {
	int a = 0, b = 1, result = 0;
	char err = 0;

	switch (operator) {
		case '+':
			stupidscanf(expression, &a, &b);
			result = a+b;
			break;
		case '-':
			stupidscanf(expression, &a, &b);
			result = a-b;
			break;
		case '*':
			stupidscanf(expression, &a, &b);
			result = a*b;
			break;
		case '/':
			stupidscanf(expression, &a, &b);
			if (b == 0) {
				xil_printf("Cannot divide by zero.\n\r");
				result = 0;
				err = 1;
			}
			else
				result = a/b;
			break;
		default:
			xil_printf("Error: unknown operator.\n\r");
			err = 1;
	}
	if (!err)
		xil_printf("Your answer is: %d\n\r", result);
}

int main() {
	u32 lDvmaBaseAddress = XPAR_DVMA_0_BASEADDR;
	int posX, posY;
    //init_platform();

	// Initialize switches
    XGpio_Initialize(&switches, XPAR_DIP_SWITCHES_8BIT_DEVICE_ID);
    XGpio_SetDataDirection(&switches, 1, 7);

	for(posX = 0; posX<2560; posX++)
		for(posY = 0; posY<720; posY++) // 720
			XIo_Out16(XPAR_DDR2_SDRAM_MPMC_BASEADDR + 2*(posY*2560+posX), 0);

	for(posX = 0; posX<2560; posX++)
		for(posY = 0; posY<720; posY++) // 720
			XIo_Out16(XPAR_DDR2_SDRAM_MPMC_BASEADDR + 2*(posY*2560+posX), (posX/40)<<4);

	XIo_Out32(lDvmaBaseAddress + blDvmaHSR, 40); 	// hsync
	XIo_Out32(lDvmaBaseAddress + blDvmaHBPR, 260); 	// hbpr
	XIo_Out32(lDvmaBaseAddress + blDvmaHFPR, 1540); // hfpr
	XIo_Out32(lDvmaBaseAddress + blDvmaHTR, 1650); 	// htr
	XIo_Out32(lDvmaBaseAddress + blDvmaVSR, 5); 	// vsync
	XIo_Out32(lDvmaBaseAddress + blDvmaVBPR, 25); 	// vbpr
	XIo_Out32(lDvmaBaseAddress + blDvmaVFPR, 745); 	// vfpr
	XIo_Out32(lDvmaBaseAddress + blDvmaVTR, 750); 	// vtr
		
	XIo_Out32(lDvmaBaseAddress + blDvmaFWR, 0x00000500); // frame width 0x00000500
	XIo_Out32(lDvmaBaseAddress + blDvmaFHR, 0x000002D0); // frame height 0x000002D0
	XIo_Out32(lDvmaBaseAddress + blDvmaFBAR, XPAR_DDR2_SDRAM_MPMC_BASEADDR); // frame base addr
	XIo_Out32(lDvmaBaseAddress + blDvmaFLSR, 0x00000A00); // frame line stride 0x00000A00
	XIo_Out32(lDvmaBaseAddress + blDvmaCR, 0x00000003); // dvma enable, dfl enable

	CamIicCfg(XPAR_CAM_IIC_0_BASEADDR, 0);
	CamIicCfg(XPAR_CAM_IIC_1_BASEADDR, 0);
	CamCtrlInit(XPAR_CAM_CTRL_0_BASEADDR, 0, 2560);
	CamCtrlInit(XPAR_CAM_CTRL_1_BASEADDR, 0, 0);

	u8 switch_val = 0;
    u8 capture_data = 0;
    u8 old_capture_data = 0;
    u8 count = 0;

	u8 num_vals = 0;
	u8 get_answer = 0;
	u8 clear = 0;
	u8 has_been_cleared = 1;

	short captured_chars[5];
	char expression[EXPRESSION_SIZE]= {0};
	char op = '$';

	while(1) {
		for(posX = 530; posX<751; posX++){
			for(posY = 250; posY<261; posY++){
				XIo_Out16(XPAR_DDR2_SDRAM_MPMC_BASEADDR + 2*(posY*2560+posX), 0xf00);
			}
		}
		for(posX = 530; posX<751; posX++){
			for(posY = 460; posY<471; posY++){
				XIo_Out16(XPAR_DDR2_SDRAM_MPMC_BASEADDR + 2*(posY*2560+posX), 0xf00);
			}
		}

		for(posX = 530; posX<541; posX++){
			for(posY = 260; posY<461; posY++){
				XIo_Out16(XPAR_DDR2_SDRAM_MPMC_BASEADDR + 2*(posY*2560+posX), 0xf00);
			}
		}

		for(posX = 740; posX<751; posX++){
			for(posY = 260; posY<461; posY++){
				XIo_Out16(XPAR_DDR2_SDRAM_MPMC_BASEADDR + 2*(posY*2560+posX), 0xf00);
			}
		}

		old_capture_data = capture_data;

		switch_val = XGpio_DiscreteRead(&switches, 1);
		capture_data = switch_val & 0x1;
		get_answer = switch_val & 0x2;
		clear = switch_val & 0x4;

		if (clear && !has_been_cleared) {
			op = '$';
			num_vals = 0;
		    capture_data = 0;
		    old_capture_data = 0;
			int i = 0;
			for (; i < EXPRESSION_SIZE; i++)
				expression[i] = 0;
			xil_printf("Your expression has been cleared.\n\r");
			has_been_cleared = 1;
		}
		// Switch OFF -> ON
		else if(!old_capture_data && capture_data){
			// Start processing the grid blocks
			count = 0;
			captured_chars[0] = processImage(540, 260);
			count++;
		}
		// Switch ON -> ON
		else if (old_capture_data && capture_data) {
			if (count < 5) {
				captured_chars[count] = processImage(540, 260);
				count++;
			}
		}
		// Switch ON -> OFF
		else if (old_capture_data && !capture_data) {
			char result = determineChar(captured_chars, count);
			if (result == '?') {
				xil_printf("Please re-enter number or operator, unable to scan correctly.\n\r");
				continue;
			}
			if (result == '+' || result == '-' || result == '*' || result == '/') {
				if (op == '$') {
					if (num_vals == 0) {
						xil_printf("First input cannot be an operator.\n\r");
						continue;
					}
					op = result;
				}
				else {
					xil_printf("Cannot have multiple operators in an expression.\n\r");
					continue;
				}
			}
			expression[num_vals] = result;
			num_vals++;

			xil_printf("%s\n\r", expression);
			has_been_cleared = 0;
		}
		// Compute answer
		else if (get_answer && (num_vals > 2)) {
			expression[num_vals] = '\0';
			printAnswer(expression, num_vals, op);

		    capture_data = 0;
		    old_capture_data = 0;
			op = '$';
			num_vals = 0;
			int i = 0;
			for (; i < EXPRESSION_SIZE; i++)
				expression[i] = 0;
			xil_printf("Your expression has been cleared.\n\r");
		}
	}

	return 0;
}
