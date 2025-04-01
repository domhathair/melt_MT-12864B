#ifndef melt_h
#define melt_h

#include <limits.h>
#include <stdint.h>

enum { MELT_HEIGHT = 64, MELT_WIDTH = 128, FONT_SYMBOLS = 1536 };

#if !defined(MELT_PIN) || !defined(MELT_RES) || !defined(MELT_A0) || !defined(MELT_CSI)
#warning "Укажите макросы для работы с уровнем выводов MELT"
#endif /* MELT_MACRO */

#if !defined(SPI_TX_IS_EMPTY) || !defined(SPI_DATA_REGISTER)
#warning "Укажите макросы для проверки состояния SPI и буфера данных"
#endif /* SPI_MACRO */

#ifndef sizeof_array
#define sizeof_array(array) (sizeof(array) / sizeof(*array))
#endif /* sizeof_array */

typedef enum mode {
    MODE_DRAW,  /*Заполнить поле*/
    MODE_CLEAR, /*Очистить поле*/
    MODE_INVERT /*Инвертировать значение поля*/
} mode_t;

extern uint8_t screen[MELT_HEIGHT / CHAR_BIT][MELT_WIDTH];
extern uint8_t font[FONT_SYMBOLS];

union qword {
    uint8_t byte[sizeof(uint64_t) / sizeof(uint8_t)];
    uint16_t word[sizeof(uint64_t) / sizeof(uint16_t)];
    uint32_t dword[sizeof(uint64_t) / sizeof(uint32_t)];
};

extern void melt_send(uint8_t, uint8_t);
extern void melt_send_command(uint8_t);
extern void melt_send_data(uint8_t);
extern void melt_address(uint8_t);
extern void melt_hardware(void);
extern void melt_init(void);
extern void melt_pour_all(mode_t);
extern void melt_pour_area(mode_t, uint8_t, uint8_t, uint8_t, uint8_t);
extern void melt_pour_bitmap(mode_t, uint8_t, uint8_t, uint8_t *, uint8_t, uint8_t);
extern void melt_draw_pixel(mode_t, uint8_t, uint8_t);
extern void melt_draw_line_horizontal(mode_t, uint8_t, uint8_t, uint8_t);
extern void melt_draw_line_vertical(mode_t, uint8_t, uint8_t, uint8_t);
extern void melt_draw_line_arbitrary(mode_t, uint8_t, uint8_t, uint8_t, uint8_t);
extern void melt_draw_rectangle(mode_t, uint8_t, uint8_t, uint8_t, uint8_t);
extern void melt_draw_circle(mode_t, uint8_t, uint8_t, uint8_t);
extern void melt_printf(mode_t, uint8_t, uint8_t, const uint8_t *, ...);

#endif /* melt_h */
