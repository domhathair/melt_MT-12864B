#include "main.h"
#include "init.h"
#include "lcd.h"

int32_t deg_sec = 271, deg_min = 271, deg_hour = 271;
char string[64] = {0};

size_t sec_len = 20, min_len = 18, hour_len = 16;

void TIM6_DAC_IRQHandler(void) {

    TIM6->SR &= ~TIM_SR_UIF;

    LCD.Draw.Fill.Area(CLEAR, 30, 64, 8, 56);
    snprintf(string, 64, "%02d:%02d:%02d", (TICK / 3600), ((TICK / 60) % 60), (TICK % 60));
    LCD.Print(DRAW, 30, 64, string);

    LCD.Draw.Line.Arbitrary(CLEAR, 32, 32,
             32 + sec_len * cos(deg_sec % 60 / 30.0 * 3.141592653) /*x coor*/,
             32 + sec_len * sin(deg_sec % 60 / 30.0 * 3.141592653) /*y coor*/
    );
    deg_sec--;
    if (!(TICK % 60)) {
        LCD.Draw.Line.Arbitrary(CLEAR, 32, 32,
                 32 + sec_len * cos(deg_min % 60 / 30.0 * 3.141592653),
                 32 + sec_len * sin(deg_min % 60 / 30.0 * 3.141592653));
        deg_min--;
    }
    if (!(TICK % 720)) {
		LCD.Draw.Line.Arbitrary(CLEAR, 32, 32,
                 32 + hour_len * cos(deg_hour % 60 / 30.0 * 3.141592653),
                 32 + hour_len * sin(deg_hour % 60 / 30.0 * 3.141592653));
        deg_hour--;
    }
	
    LCD.Draw.Line.Arbitrary(DRAW, 32, 32,
             32 + hour_len * cos(deg_hour % 60 / 30.0 * 3.141592653),
             32 + hour_len * sin(deg_hour % 60 / 30.0 * 3.141592653));
    LCD.Draw.Line.Arbitrary(DRAW, 32, 32,
             32 + sec_len * cos(deg_min % 60 / 30.0 * 3.141592653),
             32 + sec_len * sin(deg_min % 60 / 30.0 * 3.141592653));
    LCD.Draw.Line.Arbitrary(DRAW, 32, 32,
             32 + sec_len * cos(deg_sec % 60 / 30.0 * 3.141592653),
             32 + sec_len * sin(deg_sec % 60 / 30.0 * 3.141592653));

    TICK++;
}
