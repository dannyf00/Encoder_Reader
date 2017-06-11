#include "config.h"						//configuration words
#include "gpio.h"                           //we use gpio functions
#include "delay.h"                          //we use software delays

//hardware configuration
#define ENC_PORT		GPIO
#define ENC_DDR			TRISIO
#define ENC_A			(1<<4)
#define ENC_B			(1<<5)

#define OUT_PORT		GPIO				//shadow variable
#define OUT_DDR			TRISIO
#define OUT(x)			OUT_PORT = (x)
#define OUT_UP			(1<<0)				//1->up, 0->down
#define OUT_DN			(1<<1)				//1->dn, 0->up
#define OUT_PULSE		(1<<2)				//output pulse
//end hardware configuration

//global defines

//global variables
uint8_t sGPIO = 0;							//gpio shadow variable
//determine increment / decrement of the encoder
const int8_t encoder_dir[]={0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};

//encoder initialization
void enc_init(void) {
	IO_IN(ENC_DDR, ENC_A |  ENC_B);			//enc a/b as input
	
	IO_OUT(OUT_DDR, OUT_UP | OUT_DN | OUT_PULSE);
	
	//sGPIO = GPIO;							//save shadow variable
}

//read encoder
inline void enc_read(void) {
	static uint8_t enc_state = 0;			//previous state, organized as A..B
	uint8_t tmp;
	int8_t tmp1;
	
	tmp = ENC_PORT;							//read the port
	//update the encoder state
	enc_state = enc_state << 2;
	if (IO_GET(tmp, ENC_A)) enc_state |= (1<<1);
	if (IO_GET(tmp, ENC_B)) enc_state |= (1<<0);
	
	//look up the direction
	tmp1 = encoder_dir[enc_state & 0x0f];
	
	//update the output
	if (tmp1 == +1)	IO_SET(tmp, OUT_UP); else IO_CLR(tmp, OUT_UP);
	if (tmp1 == -1)	IO_SET(tmp, OUT_DN); else IO_CLR(tmp, OUT_DN);
	if (tmp1 !=  0) {IO_FLP(tmp, OUT_PULSE); OUT(tmp);}		
}
		
int main(void) {

	mcu_init();							    //initialize the mcu
	enc_init();								//reset the encoder
	while (1) {
		enc_read();							//read the encoder and generate the output
	}	
}

