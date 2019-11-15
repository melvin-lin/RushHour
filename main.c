#include "stm32l476xx.h"
#include "lcd.h"
#include <stdlib.h>

void delay(void);
void movingString(uint8_t* str, uint8_t len);

int main(void){
	LCD_Initialization();
	LCD_Display_Name();
	//while (1) {
	//	movingString((uint8_t *)"EE-14 is fuN", 12);
		//movingString((uint8_t *)"Health NetworkiNg GPA", 21);
	//}
}

// Delay loop - M
void delay(void) {
	int i;
	for (i = 0; i < 100000; i++);
}

// Scrolls a string across the LCD - M
void movingString(uint8_t* str, uint8_t len) {
	int i; 
	uint8_t* expanded = (uint8_t*) malloc(2*len+1);
	
	// Copy string twice 
	for (i = 0; i < len; i++) {
		expanded[i] = str[i];
	}
	expanded[len] = ' '; 
	for (i = 0; i < len; i++) {
		expanded[i+len+1] = str[i]; 
	}
	
	// Display the scrolling string
	for (i = 0; i < len; i++) {
		LCD_DisplayString(expanded);
		expanded++;
		delay();
	}
}
