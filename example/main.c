#include "main.h"
#include "init.h"
#include "lcd.h"

uint8_t SCREEN[LCD_HEIGHT / 8][LCD_WIDTH];
uint32_t TICK = RESET;

int main(void) {
    __disable_irq();

    Init.PLL();
    Init.Port.RCControl(PA, ON);
    Init.Port.RCControl(PB, ON);

    LCD.Init();

    Init.TIM.Common();
    
    LCD.Draw.Rectangle(DRAW, 0, 0, 64, 128);
    LCD.Draw.Rectangle(DRAW, 2, 2, 60, 124);
    LCD.Draw.Rectangle(DRAW, 28, 63, 11, 58);
    LCD.Draw.Rectangle(DRAW, 26, 61, 15, 62);

    LCD.Draw.Circle(DRAW, 32, 32, 24);
    LCD.Draw.Circle(DRAW, 32, 32, 22);

    __enable_irq();

    while (1);
} /*main*/
