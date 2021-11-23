#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lcd.h"

/*
    Инициализация SPI контроллера 
         Требуемые данные:
         
Пины:   GP_PP    -> LCD_RES
        SPI_CLK  -> LCD_SCL
        GP_PP    -> LCD_A0
        SPI_MOSI -> LCD_SI
        GP_PP    -> LCD_CSI
        
Режим SPI: 1   -   1
           ---- ----
           CPOL CPHA
           ---- ----
           Пол. Фаза
           
Выбор слейва за контроллером
Направление работы - мастер
                           
*/

void Init_LCD_Hardware(void) {
    /* GP_PP:      LCD_RES */
    /* SPI_CLK:  LCD_SCL */
    /* GP_PP:      LCD_A0 */
    /* SPI_MOSI: LCD_SI */
    /* GP_PP:     LCD_CSI */

    /* Enable SPI */
    return;
} /*Init_LCD_Hardware*/

/**/

void LCD_Send(uint8_t data, uint8_t address0) {
    if (!address0) {
        LCD_A0_LOW;
    } else {
        LCD_A0_HIGH;
    }
    while (!(SPI_CHECK)) {
    };
    SPI_SEND = data;
    return;
} /*LCD_Send*/

/**/

void LCD_Send_Command(uint8_t cmd) {
    LCD_Send(cmd, 0);
    return;
} /*LCD_Send_Command*/

/**/

void LCD_Send_Data(uint8_t data) {
    LCD_Send(data, 1);
    return;
} /*LCD_Send_Data*/

/**/

void LCD_Address(uint8_t address) {
    LCD_Send_Command(0x10 | (address >> 4));
    LCD_Send_Command(address & 0xF);
    return;
} /*LCD_Address*/

/**/

void Init_LCD(void) {
    size_t count;
    Init_LCD_Hardware();

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
    return;
} /*Init_LCD*/

/**/

void LCD_Screen(MODE mode) {
    uint8_t str, col;
    for (str = 0; str < 8; str++) {
        LCD_Send_Command(0xB0 + str); /*String select*/
        LCD_Send_Command(0x10);          /*4 higher bits (16px per sector)*/
        LCD_Send_Command(0x00);          /*4 lower bits*/
        for (col = 0; col < LCD_WIDTH; col++) {
            if (mode == DRAW)
                SCREEN[str][col] = 0xFF;
            else if (mode == CLEAR)
                SCREEN[str][col] = 0x00;
            else if (mode == INVERT)
                SCREEN[str][col] ^= 0xFF;
            LCD_Send_Data(SCREEN[str][col]);
        }
    }
    return;
} /*LCD_Screen*/

/**/

void LCD_Pixel(MODE mode, uint8_t str, uint8_t col) {
    uint8_t local;
    if (str < LCD_HEIGHT && col < LCD_WIDTH) {
        local = str / 8;
        LCD_Send_Command(0xB0 + local);
        LCD_Send_Command(0x10 + (col / 16));
        LCD_Send_Command(col % 16);
        if (mode == DRAW)
            SCREEN[local][col] |= 0x1 << (str % 8);
        else if (mode == CLEAR)
            SCREEN[local][col] &= ~(0x1 << (str % 8));
        else if (mode == INVERT)
            SCREEN[local][col] ^= 0x1 << (str % 8);
        LCD_Send_Data(SCREEN[local][col]);
    }
    return;
} /*LCD_Pixel*/

/**/

void LCD_HLine(MODE mode, uint8_t str, uint8_t col, uint8_t length) {
    uint8_t add;
    for (add = 0; (add < length) && (col + add < LCD_WIDTH); add++)
        LCD_Pixel(mode, str, col + add);
    return;
} /*LCD_HLine*/

/**/

void LCD_VLine(MODE mode, uint8_t str, uint8_t col, uint8_t length) {
    uint8_t add;
    for (add = 0; (add < length) && (str + add < LCD_HEIGHT); add++)
        LCD_Pixel(mode, str + add, col);
    return;
} /*LCD_VLine*/

/**/

void LCD_Line(MODE mode, uint8_t str_From, uint8_t col_From, uint8_t str_To, uint8_t col_To) {
    int8_t it;
    uint8_t first_add, secon_add;
    int8_t dx = col_To - col_From;
    int8_t dy = str_To - str_From;
    int8_t mx = (abs(dx) > abs(dy)) ? dx : dy;
    float tang = (abs(dx) > abs(dy)) ? (float) dy / (float) dx : (float) dx / (float) dy;

    for (it = 0; it != mx; (mx > 0) ? it++ : it--) {
        first_add = (abs(dx) > abs(dy)) ? str_From + (it * tang) : str_From + it;
        secon_add = (abs(dx) > abs(dy)) ? col_From + it : col_From + (it * tang);
        LCD_Pixel(mode, first_add, secon_add);
    }
    return;
} /*LCD_Line*/

/**/

void LCD_Rectangle(MODE mode, uint8_t str, uint8_t col, uint8_t height, uint8_t width) {
    LCD_HLine(mode, str, col, width);
    LCD_VLine(mode, str + 1, col + width - 1, height - 1);
    LCD_VLine(mode, str + 1, col, height - 1);
    LCD_HLine(mode, str + height - 1, col + 1, width - 2);
    return;
} /*LCD_Rectangle*/

/**/

void LCD_Fill(MODE mode, uint8_t str, uint8_t col, uint8_t height, uint8_t width) {
    uint8_t add;
    for (add = 0; (add < height) && (str + add < LCD_HEIGHT); add++)
        LCD_HLine(mode, str + add, col, width);
    return;
} /*LCD_Fill*/

/**/

void LCD_Circle(MODE mode, uint32_t str, uint32_t col, uint32_t rad) {
    int32_t d, x, y;

    for (x = 0, y = rad, d = 3 - 2 * rad;;) {
        LCD_Pixel(mode, y + str, x + col);
        LCD_Pixel(mode, -y + str, x + col);
        LCD_Pixel(mode, -y + str, -x + col);
        LCD_Pixel(mode, y + str, -x + col);
        LCD_Pixel(mode, x + str, y + col);
        LCD_Pixel(mode, -x + str, y + col);
        LCD_Pixel(mode, -x + str, -y + col);
        LCD_Pixel(mode, x + str, -y + col);

        if (x >= y)
            break;

        if (d <= 0)
            d += 4 * x + 6;
        else {
            d += 4 * (x - y) + 10;
            y--;
        }

        x++;
    }
} /*LCD_Circle*/

/**/

void LCD_Bitmap(MODE mode, uint8_t str, uint8_t col, uint8_t *image, uint8_t height, uint8_t width) {
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
            for (y = 0; y < 8; y++) {
                LCD_Send_Command(0xB0 + y);
                LCD_Send_Command(0x10 + (local / 16));
                LCD_Send_Command(local % 16);
                if (mode == DRAW)
                    SCREEN[y][local] |= *(&(Full_col.bytes.byte_0) + y);
                else if (mode == CLEAR)
                    SCREEN[y][local] &= ~(*(&(Full_col.bytes.byte_0) + y));
                else if (mode == INVERT)
                    SCREEN[y][local] ^= *(&(Full_col.bytes.byte_0) + y);
                LCD_Send_Data(SCREEN[y][local]);
            }
            Full_col.dword.dword_0 = RESET;
            Full_col.dword.dword_1 = RESET;
        }
    }
    return;
} /*LCD_Bitmap*/

/**/

void LCD_Print(MODE mode, uint8_t str, uint8_t col, char *string) {

    size_t count = 0;

    size_t it = 0, size = 0;
    size = strlen(string);
    char *local_String = (char *) calloc(size + 1, sizeof(char));
    for (; count < size; count++, it++) {
#ifdef __CYRILLIC
        switch (string[count])
        {
        case (char)0xD0:
            local_String[it] = string[++count] + 0xD0 - 0xA0;
            break;
        case (char)0xD1:
            local_String[it] = string[++count] + 0xD1 - 0x61;
            break;
        default:
#endif
        local_String[it] = string[count];
#ifdef __CYRILLIC
        break;
    }
#endif
    }
    if (str < LCD_HEIGHT && col < LCD_WIDTH) {
        for (count = 0; count < strlen(local_String); count++) {
            LCD_Bitmap(mode, str, col + (7 * count), &FONT[local_String[count] * 6], 1, 6);
        }
    }
    free(local_String);
    return;
} /*LCD_Print*/
