#include "stm32l476xx.h"
#include "LED.h"
#include "LCD.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

volatile uint32_t TimeDelay;
volatile bool pushedUp = 0;
volatile bool pushedDown = 1;
bool DONE = 0;
int counter = 0;
int SPEED = 50;
int MAXSPEED = 20;
int HARDTIME = 30000;

uint8_t OBSUP[] = "°";
uint8_t OBSDOWN[] = "%";
uint8_t OBSBLANK[] = " ";
uint8_t obstacles[] = "     ";

void SysTick_Initialize(uint32_t ticks);
void SysTick_Handler(void);

void joyStick(void);
void shiftAndAdd(int randNum);
bool isGameover(void);
void startGame(void);
void playGame(void);
void endGame(void);
void movingString(uint8_t* str, uint8_t len);

int main(void) {
	LED_Init();
	uint32_t ticks = 4000;
	SysTick_Initialize(ticks);
	LCD_Initialization();
	Joypad_Initilization();

	startGame();
	Red_LED_Off();
	Green_LED_On();
	playGame();
	Red_LED_On();
	Green_LED_Off();
	endGame();
}

// Start the game
void startGame() {
		volatile uint32_t down;
		while (counter % 1000 != 0) {
			movingString((uint8_t*) "TAP TAP REVOLUTION", 18);
			down = GPIOA-> IDR & GPIO_IDR_IDR_5;
			if (down) {
					break;
			}
		}
		while (1) {
			movingString((uint8_t*) "PRESS DOWN TO START GAME", 24);
			down = GPIOA-> IDR & GPIO_IDR_IDR_5;
			if (down) {
					break;
			}
		}
}

// Play the game
void playGame(void) {
	while (!isGameover()) {
		joyStick();
		if (pushedUp) {
				if ((obstacles[0] == OBSUP[0]) && pushedUp) {
						LCD_Display_Car_Up_Full();
				}	else {
						LCD_Display_Car_Up();
				}
		} else if (pushedDown) {
				if ((obstacles[0] == OBSDOWN[0]) && pushedDown) {
						LCD_Display_Car_Down_Full();
				}	else {
						LCD_Display_Car_Down();
				}
		}
		if (!isGameover()) {
				if (counter % SPEED == 0) {
						shiftAndAdd(rand() % 3);
				}
		}
	}
}

// End the game and print some messages
void endGame(void) {
		volatile uint32_t center;
		// Stay on death screen for a bit before game over
		while (counter % 200 != 0) {
				center = GPIOA-> IDR & GPIO_IDR_IDR_0;
				LCD_Display_Obstacle(obstacles);
				if (center) {
					break;
				}
		}
		while (1) {
				movingString((uint8_t*) "GAME OVER PRESS RESET", 21);
		}
}

// Shift obstacles string left and add one char to be back
void shiftAndAdd(int randNum) {
		int i;
		// Shift left
		for (i = 0; i < 4; i++) {
				obstacles[i] = obstacles[i+1];
		}
		// Logics of what to add to the obstacle
		bool breakTime = (counter >= HARDTIME) && (counter < HARDTIME+5000);
		bool addDown = (randNum == 1) && (obstacles[3] != OBSUP[0])
					&& (obstacles[2] != OBSUP[0]) && !breakTime;
		bool addUp = (randNum == 2) && (obstacles[3] != OBSDOWN[0])
					&& (obstacles[2] != OBSDOWN[0]) && !breakTime;
		
		// Add obstacle to the end
		if (addDown) {
				obstacles[4] = OBSDOWN[0];
		} else if (addUp) {
				obstacles[4] = OBSUP[0];
		} else if ((breakTime) && (counter >= HARDTIME+2000)) {
				Red_LED_Toggle();
				obstacles[4] = OBSBLANK[0];
	  } else if ((!breakTime) || (counter >= HARDTIME+3000)){
				Red_LED_Off();
				obstacles[4] = OBSBLANK[0];
		}
		LCD_Display_Obstacle(obstacles);
}

// Return true if game ends, false otherwise
bool isGameover(void) {
		// Obstacle on us is not blank
		if ((obstacles[0] != OBSBLANK[0])) {
				// Obstacle is up while we are down
				if ((obstacles[0] == OBSUP[0]) && pushedDown) {
						return 1;
				}	// Obstacle is down while we are up
				else if ((obstacles[0] == OBSDOWN[0]) && pushedUp) {
						return 1;
				}					
		}
		return 0;
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
	// Increase speed after some time
	if (counter > HARDTIME) {
			if (SPEED > MAXSPEED) {
					SPEED--;
			}
	}
	counter++;
}

// Get input from the Joystick
void joyStick(void)
{
		volatile uint32_t up = GPIOA-> IDR & GPIO_IDR_IDR_3;
		volatile uint32_t down = GPIOA-> IDR & GPIO_IDR_IDR_5;
	  // Update booleans based on joypad status
		if (up) {
				pushedUp = 1;
				pushedDown = 0;		
		}
		else if (down) {
				pushedUp = 0;
				pushedDown = 1;
		}
}

// Scrolls a string across the LCD
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
	volatile uint32_t down;
	for (i = 0; i < len; i++) {
			LCD_DisplayString(expanded);
			expanded++;
			Red_LED_Toggle();
			while(counter % 200 != 0);
			down = GPIOA-> IDR & GPIO_IDR_IDR_5;
			if (down) {
					break;
			}	
			Green_LED_Toggle();			
	}
	free (expanded);
}
