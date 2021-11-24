#include "stm32f2xx.h"

/*Закомментировать, если в среде разработки кириллица работает без ошибок*/
#define __CYRILLIC 1

/**/

#define LCD_HEIGHT 64
#define LCD_WIDTH 128

/*Указать макросы для управления уровнем на пинах*/
#define LCD_RES_HIGH GPIOB->BSRR = GPIO_BSRR_BS7
#define LCD_RES_LOW GPIOB->BSRR = GPIO_BSRR_BR7

#define LCD_SCL_HIGH GPIOB->BSRR = GPIO_BSRR_BS3
#define LCD_SCL_LOW GPIOB->BSRR = GPIO_BSRR_BR3

#define LCD_A0_HIGH GPIOB->BSRR = GPIO_BSRR_BS4
#define LCD_A0_LOW GPIOB->BSRR = GPIO_BSRR_BR4

#define LCD_CSI_HIGH GPIOB->BSRR = GPIO_BSRR_BS5
#define LCD_CSI_LOW GPIOB->BSRR = GPIO_BSRR_BR5

/*Указать макросы для проверки состояния SPI и буфера данных*/
#define SPI_CHECK SPI1->SR & SPI_SR_TXE
#define SPI_SEND SPI1->DR

/**/

typedef enum {
    DRAW,
    CLEAR,
    INVERT
} MODE;

/**/

extern uint32_t SCREEN[8][128];
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

void LCD_Send(uint8_t data, uint8_t address0);

void LCD_Send_Command(uint8_t cmd);

void LCD_Send_Data(uint8_t data);

void LCD_Address(uint8_t address);

void Init_LCD_Hardware(void);

void Init_LCD(void);

void LCD_Screen(MODE mode);

void LCD_Pixel(MODE mode, uint8_t str, uint8_t col);

void LCD_HLine(MODE mode, uint8_t str, uint8_t col, uint8_t length);

void LCD_VLine(MODE mode, uint8_t str, uint8_t col, uint8_t length);

void LCD_Line(MODE mode, uint8_t str_From, uint8_t col_From, uint8_t str_To, uint8_t col_To);

void LCD_Rectangle(MODE mode, uint8_t str, uint8_t col, uint8_t height, uint8_t width);

void LCD_Fill(MODE mode, uint8_t str, uint8_t col, uint8_t height, uint8_t width);

void LCD_Circle(MODE mode, uint32_t str, uint32_t col, uint32_t rad);

void LCD_Bitmap(MODE mode, uint8_t str, uint8_t col, uint8_t *image, uint8_t height, uint8_t width);

void LCD_Print(MODE mode, uint8_t str, uint8_t col, char *string);
