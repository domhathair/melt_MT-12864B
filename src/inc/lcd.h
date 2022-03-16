/*#include "mcu_model.h"*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LCD_HEIGHT 64
#define LCD_WIDTH 128

/*Указать макросы для управления уровнем на пинах*/
#define LCD_RES_HIGH /*GP_PP HIGH*/
#define LCD_RES_LOW  /*GP_PP LOW*/

#define LCD_SCL_HIGH /*GP_PP HIGH*/
#define LCD_SCL_LOW  /*GP_PP LOW*/

#define LCD_A0_HIGH  /*GP_PP HIGH*/
#define LCD_A0_LOW   /*GP_PP LOW*/

#define LCD_CSI_HIGH /*GP_PP HIGH*/
#define LCD_CSI_LOW  /*GP_PP LOW*/

/*Указать макросы для проверки состояния SPI и буфера данных*/
#define SPI_CHECK    /*SPI TX EMPTY*/
#define SPI_SEND     /*SPI DATA REGISTER*/

/**/

typedef enum {
    DRAW, /*Заполнить поле*/
    CLEAR, /*Очистить поле*/
    INVERT /*Инвертировать значение поля*/
} MODE;

/**/

extern uint8_t SCREEN[LCD_HEIGHT / 8][LCD_WIDTH];
extern uint8_t FONT[];

/**/

union uint64_R {
    struct {
        uint8_t byte_0;
        uint8_t byte_1;
        uint8_t byte_2;
        uint8_t byte_3;
        uint8_t byte_4;
        uint8_t byte_5;
        uint8_t byte_6;
        uint8_t byte_7;
    } bytes;
    struct {
        uint32_t dword_0;
        uint32_t dword_1;
    } dword;
};

/**/

typedef struct {
    void (*All)(MODE mode);
    void (*Area)(MODE mode, uint8_t str, uint8_t col, uint8_t height, uint8_t width);
    void (*Bitmap)(MODE mode, uint8_t str, uint8_t col, uint8_t *image, uint8_t height, uint8_t width);
} LCD_Draw_Fill_Struct;

typedef struct {
    void (*Horizontal)(MODE mode, uint8_t str, uint8_t col, uint8_t length);
    void (*Vertical)(MODE mode, uint8_t str, uint8_t col, uint8_t length);
    void (*Arbitrary)(MODE mode, uint8_t str_from, uint8_t col_from, uint8_t str_to, uint8_t col_to);
} LCD_Draw_Line_Struct;

typedef struct {
    LCD_Draw_Fill_Struct Fill;
    void (*Pixel)(MODE mode, uint8_t str, uint8_t col);
    LCD_Draw_Line_Struct Line;
    void (*Rectangle)(MODE mode, uint8_t str, uint8_t col, uint8_t height, uint8_t width);
    void (*Circle)(MODE mode, uint8_t str, uint8_t col, uint8_t rad);
} LCD_Draw_Struct;

typedef struct {
    void (*Init)(void);
    LCD_Draw_Struct Draw;
    void (*Print)(MODE mode, uint8_t str, uint8_t col, char *string);
} LCD_Struct;

extern LCD_Struct LCD;

/**/

void LCD_Send(uint8_t data, uint8_t address0);

void LCD_Send_Command(uint8_t cmd);

void LCD_Send_Data(uint8_t data);

void LCD_Address(uint8_t address);

void LCD_Init_Hardware(void);

void LCD_Init(void);

void LCD_Draw_Fill_All(MODE mode);

void LCD_Draw_Pixel(MODE mode, uint8_t str, uint8_t col);

void LCD_Draw_Line_Horizontal(MODE mode, uint8_t str, uint8_t col, uint8_t length);

void LCD_Draw_Line_Vertical(MODE mode, uint8_t str, uint8_t col, uint8_t length);

void LCD_Draw_Line_Arbitrary(MODE mode, uint8_t str_from, uint8_t col_from, uint8_t str_to, uint8_t col_to);

void LCD_Draw_Rectangle(MODE mode, uint8_t str, uint8_t col, uint8_t height, uint8_t width);

void LCD_Draw_Fill_Area(MODE mode, uint8_t str, uint8_t col, uint8_t height, uint8_t width);

void LCD_Draw_Circle(MODE mode, uint8_t str, uint8_t col, uint8_t rad);

void LCD_Draw_Fill_Bitmap(MODE mode, uint8_t str, uint8_t col, uint8_t *image, uint8_t height, uint8_t width);

void LCD_UTF8_Decode(char *string);

void LCD_Print(MODE mode, uint8_t str, uint8_t col, char *string);
