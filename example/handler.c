#include "main.h"
#include "init.h"
#include "lcd.h"

int32_t deg_sec = 271, deg_min = 270;

void TIM6_DAC_IRQHandler(void) {

    TIM6->SR &= ~TIM_SR_UIF;

    char *string = (char *) calloc(64, 1);
    LCD_Fill(CLEAR, 30, 64, 8, 56);
    snprintf(string, 64, "%02d:%02d:%02d", (TICK / 3600), ((TICK / 60) % 60), (TICK % 60));
    LCD_Print(DRAW, 30, 64, string);

    free(string);

    LCD_Line(CLEAR, 32, 32,
             32 + 20 * cos(deg_sec % 60 / 30.0 * 3.141592653) /*x coor*/,
             32 + 20 * sin(deg_sec % 60 / 30.0 * 3.141592653) /*y coor*/
    );
    deg_sec--;
    if (!(TICK % 60)) {
        LCD_Line(CLEAR, 32, 32,
                 32 + 18 * cos(deg_min % 60 / 30.0 * 3.141592653),
                 32 + 18 * sin(deg_min % 60 / 30.0 * 3.141592653));
        deg_min--;
    }
    LCD_Line(DRAW, 32, 32,
             32 + 18 * cos(deg_min % 60 / 30.0 * 3.141592653),
             32 + 18 * sin(deg_min % 60 / 30.0 * 3.141592653));
    LCD_Line(DRAW, 32, 32,
             32 + 20 * cos(deg_sec % 60 / 30.0 * 3.141592653),
             32 + 20 * sin(deg_sec % 60 / 30.0 * 3.141592653));

    TICK++;
    return;
}
