#include "main.h"
#include "init.h"
#include "lcd.h"

uint32_t SCREEN[8][128];
uint32_t TICK = RESET;

int main(void) {
    __disable_irq();

    Init_PLL();
    Init_Port_RCC(PA, ON);
    Init_Port_RCC(PB, ON);

    Init_LCD();
    LCD_Screen(CLEAR);

    Init_TIM_Common();

    LCD_Rectangle(DRAW, 0, 0, 64, 128);
    LCD_Rectangle(DRAW, 2, 2, 60, 124);
    LCD_Rectangle(DRAW, 28, 63, 11, 58);
    LCD_Rectangle(DRAW, 26, 61, 15, 62);

    LCD_Circle(DRAW, 32, 32, 24);
    LCD_Circle(DRAW, 32, 32, 22);

    __enable_irq();

    while (1);
} /*main*/
