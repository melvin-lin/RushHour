#include "stm32l476xx.h"
#include "LED.h"
#include "LCD.h"

volatile uint32_t TimeDelay;
void SysTick_Initialize(uint32_t ticks);
void LCD_Display_Car_Up(void);
void LCD_Display_Car_Down(void);
void SysTick_Handler(void);
void delay(void);
void joyStick(void);

volatile bool pushedUp = 0;
volatile bool pushedDown = 1;
volatile bool toggle_move = 0;
int counter = 0; 

int main(void) {
	LED_Init();
	uint32_t ticks = 4000;
	SysTick_Initialize(ticks);
	LCD_Initialization(); 
	
	while (1) {
		if (counter % 10 >= 0) {
				joyStick();
				Red_LED_Toggle();
				if (pushedUp)
					LCD_Display_Car_Up();
				if (pushedDown)
					LCD_Display_Car_Down();
		}
	}		
}

// ticks: number of ticks between two interrupts
void SysTick_Initialize(uint32_t ticks) {
	
	// Disable SysTick IRQ and SysTick counter
	SysTick->CTRL = 0;
	
	// Set reload register
	SysTick->LOAD = ticks - 1;
	
	// Set interrupt priority of SysTick
	// Make SysTick least urgent (i.e. highest priority number)
	// __NVIC_PRIO_BITS: number of bits for priority levels, defined in CMSIS
	NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
	
	// Reset the SysTick counter value
	SysTick->VAL = 0;
	
	// Select processor clock
	// 1 = processor clock; 0 = external clock
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
	
	// Enable SysTick exception request
	// 1 = counting down to zero asserts the SysTick exception request
	// 0 = counting down to zero does no assert the SysTick exception request
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	
	// Enable SysTick timer
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

// SysTick interrupt service routine
void SysTick_Handler(void) {
	if (TimeDelay > 0) {
		TimeDelay--;
	}

	counter++;
}

void joyStick(void)
{
		uint32_t value = GPIOA-> IDR & GPIO_IDR_IDR_3;
		uint32_t value1 = GPIOA-> IDR & GPIO_IDR_IDR_5;
		if (value) {
					pushedUp = !pushedUp;
					pushedDown = !pushedUp;
		}
		if (value1) {
					pushedDown = !pushedDown;
					pushedUp = !pushedDown;
		}
}
