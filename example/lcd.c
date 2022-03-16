#include "lcd.h"
#include "init.h"

/**
  * @brief Обращение к функциям рекомендуется совершать через данную структуру
  */
 
LCD_Struct LCD = {
    &LCD_Init, /*LCD.Init() - операция инициализации дисплея*/
    /*Данная функция ничего не принимает*/
    { /*<LCD_Draw_Struct>*/
        { /*<LCD_Draw_Fill_Struct>*/
            /**/
            &LCD_Draw_Fill_All, /*LCD.Draw.Fill.All(...) - операция заполнения экрана целиком*/
            /*mode - режим рисования (DRAW/CLEAR/INVERT)*/
            &LCD_Draw_Fill_Area, /*LCD.Draw.Fill.Area(...) - операция заполнения области экрана*/
            /*mode, str - строчка начала, col - столбик начала, height - высота области, width - ширина области*/
            &LCD_Draw_Fill_Bitmap /*LCD.Draw.Fill.Bitmap(...) - операция рисования битового поля*/
            /*mode, str, col, *image - указатель на битовое поле, height, width*/
            /**/
        }, /*</LCD_Draw_Fill_Struct>*/
        /**/
        &LCD_Draw_Pixel, /*LCD.Draw.Pixel(...) - операция рисования единичного пикселя*/
        /*mode, str - координата по строчке, col - координата по столбику*/
        /**/
        { /*<LCD_Draw_Line_Struct>*/
            /**/
            &LCD_Draw_Line_Horizontal, /*LCD.Draw.Line.Horizontal(...) - операция рисования горизонтальной линии*/
            /*mode, str, col, length - длина линии по горизонтали*/
            &LCD_Draw_Line_Vertical, /*LCD.Draw.Line.Vertical(...) - операция рисования вертикальной линии*/
            /*mode, str, col, length - длина линии по вертикали*/
            &LCD_Draw_Line_Arbitrary /*LCD.Draw.Line.Arbitrary(...) - операция рисования произвольной линии*/
            /*mode, str_from - начальная координата по строчке, col_from - начальная координата по столбику,
            str_to - конечная координата по строчке, col_to - конечная координата по столбику*/
            /**/
        }, /*</LCD_Draw_Line_Struct>*/
        /**/
        &LCD_Draw_Rectangle, /*LCD.Draw.Rectangle(...) - операция рисования квадратика*/
        /*mode, координаты верхнего левого угла квадратика по: str - строчке, col - столбцу, height, width*/
        &LCD_Draw_Circle /*LCD.Draw.Circle(...) - операция рисования кружочка*/
        /*mode, координаты цента кружочка по: str, col, rad - радиус*/        
        /**/
    }, /*</LCD_Draw_Struct>*/
    /**/
    &LCD_Print /*LCD.Print(...) - операция вывода текста*/
    /*mode, координаты верхнего левого угла окна текста по: str, col, *string - указатель на строку формата UTF8*/
    /**/
}; /*LCD*/

/**/

void LCD_Init_Hardware(void) {
#ifdef __STM32F2xx_H
    Init_Port_Mode_BF(GP_PP, PB, 7); /* PB7:    LCD_RES */
    Init_Port_Mode_BF(AF_PP, PB, 3); /* PB3:    LCD_SCL */
    Init_Port_Mode_AF(AF5, PB, 3);
    Init_Port_Mode_BF(GP_PP, PB, 4); /* PB4:    LCD_A0 */
    Init_Port_Mode_BF(AF_PP, PB, 5); /* PB5:    LCD_SI */
    Init_Port_Mode_AF(AF5, PB, 5);
    Init_Port_Mode_BF(GP_PP, PB, 6); /* PB6:    LCD_CSI */
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    SPI1->CR1 |= SPI_CR1_CPHA | SPI_CR1_CPOL | SPI_CR1_SSI |
                 SPI_CR1_SSM | SPI_CR1_MSTR | SPI_CR1_SPE; /* Enable SPI1 */
#endif
} /*LCD_Init_Hardware*/

/**/

void LCD_Send(uint8_t data, uint8_t address0) {
    if (!address0) {
        LCD_A0_LOW;
    } else {
        LCD_A0_HIGH;
    }
    while (!(SPI_CHECK));
    SPI_SEND = data;
} /*LCD_Send*/

/**/

void LCD_Send_Data(uint8_t data) {
    LCD_Send(data, 1);
} /*LCD_Send_Data*/

/**/

void LCD_Send_Command(uint8_t cmd) {
    LCD_Send(cmd, 0);
} /*LCD_Send_Command*/

/**/
 
void LCD_Address(uint8_t address) {
    LCD_Send_Command(0x10 | (address >> 4));
    LCD_Send_Command(address & 0xF);
} /*LCD_Address*/

/**/

void LCD_Init(void) {
    size_t count;
    LCD_Init_Hardware();

    LCD_RES_LOW;
    LCD_CSI_HIGH;
    for (count = 0; count < 0xFFFF; count++);
    LCD_RES_HIGH;
    LCD_CSI_LOW;
    LCD_Send_Command(0x2F);
    LCD_Send_Command(0x27);
    LCD_Send_Command(0x89);
    LCD_Send_Command(0x00);
    LCD_Send_Command(0x81);
    LCD_Send_Command(0x30);
    LCD_Send_Command(0x40);
    LCD_Send_Command(0xA6);
    LCD_Send_Command(0xC8);
    LCD_Send_Command(0xA0);
    LCD_Send_Command(0xAF); /*Display driver enable*/
    LCD_Draw_Fill_All(CLEAR);
} /*LCD_Init*/

/**/

void LCD_Draw_Fill_All(MODE mode) {
    uint8_t str, col;
    for (str = 0; str < (LCD_HEIGHT / 8); str++) {
        LCD_Send_Command(0xB0 + str); /*String select*/
        LCD_Send_Command(0x10); /*4 higher bits (16px per sector)*/
        LCD_Send_Command(0x00); /*4 lower bits*/
        for (col = 0; col < LCD_WIDTH; col++) {
            switch (mode) {
                case DRAW:
                    SCREEN[str][col] = 0xFF;
                break;
                case CLEAR:
                    SCREEN[str][col] = 0x00;
                break;
                case INVERT:
                    SCREEN[str][col] ^= 0xFF;
                break;
            }
            LCD_Send_Data(SCREEN[str][col]);
        }
    }
} /*LCD_Draw_Fill_All*/

/**/

void LCD_Draw_Pixel(MODE mode, uint8_t str, uint8_t col) {
    uint8_t local;
    if (str < LCD_HEIGHT && col < LCD_WIDTH) {
        local = str / 8;
        LCD_Send_Command(0xB0 + local);
        LCD_Send_Command(0x10 + (col / 16));
        LCD_Send_Command(col % 16);
        switch (mode) {
            case DRAW:
                SCREEN[local][col] |= 0x1 << (str % 8);
            break;
            case CLEAR:
                SCREEN[local][col] &= ~(0x1 << (str % 8));
            break;
            case INVERT:
                SCREEN[local][col] ^= 0x1 << (str % 8);
            break;
        }
        LCD_Send_Data(SCREEN[local][col]);
    }
} /*LCD_Draw_Pixel*/

/**/

void LCD_Draw_Line_Horizontal(MODE mode, uint8_t str, uint8_t col, uint8_t length) {
    uint8_t add;
    for (add = 0; (add < length) && (col + add < LCD_WIDTH); add++)
        LCD_Draw_Pixel(mode, str, col + add);
} /*LCD_Draw_Line_Horizontal*/

/**/

void LCD_Draw_Line_Vertical(MODE mode, uint8_t str, uint8_t col, uint8_t length) {
    uint8_t add;
    for (add = 0; (add < length) && (str + add < LCD_HEIGHT); add++)
        LCD_Draw_Pixel(mode, str + add, col);
} /*LCD_Draw_Line_Vertical*/

/**/

void LCD_Draw_Line_Arbitrary(MODE mode, uint8_t str_from, uint8_t col_from, uint8_t str_to, uint8_t col_to) {
    int8_t it;
    uint8_t first_add, second_add;
    int8_t dx = col_to - col_from;
    int8_t dy = str_to - str_from;
    int8_t mx = (abs(dx) > abs(dy)) ? dx : dy;
    int32_t tang = (abs(dx) > abs(dy)) ? (dy << 10) / dx : (dx << 10) / dy;

    for (it = 0; it != mx; (mx > 0) ? it++ : it--) {
        first_add = (abs(dx) > abs(dy)) ? str_from + ((it * tang) >> 10) : str_from + it;
        second_add = (abs(dx) > abs(dy)) ? col_from + it : col_from + ((it * tang) >> 10);
        LCD_Draw_Pixel(mode, first_add, second_add);
    }
} /*LCD_Draw_Line_Arbitrary*/

/**/

void LCD_Draw_Rectangle(MODE mode, uint8_t str, uint8_t col, uint8_t height, uint8_t width) {
    LCD_Draw_Line_Horizontal(mode, str, col, width);
    LCD_Draw_Line_Vertical(mode, str + 1, col + width - 1, height - 1);
    LCD_Draw_Line_Vertical(mode, str + 1, col, height - 1);
    LCD_Draw_Line_Horizontal(mode, str + height - 1, col + 1, width - 2);
} /*LCD_Draw_Rectangle*/

/**/

void LCD_Draw_Fill_Area(MODE mode, uint8_t str, uint8_t col, uint8_t height, uint8_t width) {
    uint8_t add;
    for (add = 0; (add < height) && (str + add < LCD_HEIGHT); add++)
        LCD_Draw_Line_Horizontal(mode, str + add, col, width);
} /*LCD_Draw_Fill_Area*/

/**/

void LCD_Draw_Circle(MODE mode, uint8_t str, uint8_t col, uint8_t rad) {
    int8_t magic, x, y;

    for (x = 0, y = rad, magic = 3 - 2 * rad;;) {
        LCD_Draw_Pixel(mode, y + str, x + col);
        LCD_Draw_Pixel(mode, -y + str, x + col);
        LCD_Draw_Pixel(mode, -y + str, -x + col);
        LCD_Draw_Pixel(mode, y + str, -x + col);
        LCD_Draw_Pixel(mode, x + str, y + col);
        LCD_Draw_Pixel(mode, -x + str, y + col);
        LCD_Draw_Pixel(mode, -x + str, -y + col);
        LCD_Draw_Pixel(mode, x + str, -y + col);

        if (x >= y)
            break;

        if (magic <= 0)
            magic += 4 * x + 6;
        else {
            magic += 4 * (x - y) + 10;
            y--;
        }

        x++;
    }
} /*LCD_Draw_Circle*/

/**/

void LCD_Draw_Fill_Bitmap(MODE mode, uint8_t str, uint8_t col, uint8_t *image, uint8_t height, uint8_t width) {
    uint8_t x, y, local, shift;
    if (str < LCD_HEIGHT && col < LCD_WIDTH) {
        union uint64_R Full_col = {RESET};

        for (x = 0; (x < width) && (col + x < LCD_WIDTH); x++) {
            local = col + x;
            for (y = 0; (y < height) && ((y * 8) < LCD_HEIGHT); y++) {
                shift = str + (y * 8);
                if (shift < 32)
                    Full_col.dword.dword_0 |= image[(y * width) + x] << shift;
                if (shift > 24)
                    Full_col.dword.dword_1 |= (shift > 32) ? image[(y * width) + x] << (shift - 32) :
                                              image[(y * width) + x] >> (32 - shift);
            }
            for (y = 0; y < (LCD_HEIGHT / 8); y++) {
                LCD_Send_Command(0xB0 + y);
                LCD_Send_Command(0x10 + (local / 16));
                LCD_Send_Command(local % 16);
                switch (mode) {
                    case DRAW:
                        SCREEN[y][local] |= *(&(Full_col.bytes.byte_0) + y);
                    break;
                    case CLEAR:
                        SCREEN[y][local] &= ~(*(&(Full_col.bytes.byte_0) + y));
                    break;
                    case INVERT:
                        SCREEN[y][local] ^= *(&(Full_col.bytes.byte_0) + y);
                    break;
                }
                LCD_Send_Data(SCREEN[y][local]);
            }
            Full_col.dword.dword_0 = RESET;
            Full_col.dword.dword_1 = RESET;
        }
    }
} /*LCD_Draw_Fill_Bitmap*/

/**/

void LCD_UTF8_Decode(char *string) {
    size_t count, sub_count, size;
    
    size = strlen(string) + 1;
    for (count = 0; count < size; count++) {
        if (string[count] & 0xC0) {
            if(string[++count] & 0x80) {
                switch (string[count - 1]) {
                    case (char)0xD0:
                        string[count] = (string[count - 1] & 0x1E) * 0x0B + (string[count] & 0x3F);
                    break;
                    case (char)0xD1:
                        string[count] = (string[count - 1] & 0x1E) * 0x0F + (string[count] & 0x3F);
                    break;
                }
                for (sub_count = count--; sub_count < size; sub_count++) 
                    string[sub_count - 1] = string[sub_count];
                size--;
            }
        }
    }
} /*LCD_UTF8_Decode*/

/**/

void LCD_Print(MODE mode, uint8_t str, uint8_t col, char *string) {

    size_t count = 0;
    size_t size = 0;
    
    LCD_UTF8_Decode(string);
    
    size = strlen(string);
    if (str < LCD_HEIGHT && col < LCD_WIDTH) {
        for (count = 0; count < size; count++) {
            LCD_Draw_Fill_Bitmap(mode, str, col + (7 * count), &FONT[string[count] * 6], 1, 6);
        }
    }
} /*LCD_Print*/
