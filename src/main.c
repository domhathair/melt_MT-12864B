#include <libopencm3/cm3/cortex.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/timer.h>

#define MELT_PIN(port, pin, state) ((state) ? (gpio_set(port, pin)) : gpio_clear(port, pin))
#define MELT_RES(state) MELT_PIN(GPIOA, GPIO4, state)
#define MELT_A0(state) MELT_PIN(GPIOA, GPIO6, state)
#define MELT_CSI(state) MELT_PIN(GPIOC, GPIO4, state)

#define SPI_TX_IS_EMPTY (SPI_SR(SPI1) & SPI_SR_TXE)
#define SPI_DATA_REGISTER (SPI_DR(SPI1))

#include "melt.h"
#include <math.h>

extern void melt_hardware(void) {
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO4);       /* PA4: LCD_RES */
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO5); /* PA5: LCD_SCL */
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO6);       /* PA6: LCD_A0 */
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO7); /* PA7: LCD_SI */
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO4);       /* PC4: LCD_CSI */

    SPI1_CR1 |= SPI_CR1_CPHA | SPI_CR1_CPOL | SPI_CR1_SSI | SPI_CR1_SSM | SPI_CR1_MSTR | SPI_CR1_SPE;
}

extern void melt_send(uint8_t data, uint8_t a0) {
    MELT_A0(a0);

    while (!(SPI_TX_IS_EMPTY));

    SPI_DATA_REGISTER = data;
}

extern void melt_init(void) {
    static const uint8_t cmd[] = {0x2F, 0x27, 0x89, 0x00, 0x81, 0x30, 0x40, 0xA6, 0xC8, 0xA0, 0xAF};
    enum { TIMEOUT = USHRT_MAX };
    melt_hardware();

    MELT_RES(0);
    MELT_CSI(1);
    for (size_t i = 0; i < TIMEOUT; i++);
    MELT_RES(1);
    MELT_CSI(0);
    for (size_t i = 0; i < sizeof_array(cmd); i++) /*  */
        melt_send_command(cmd[i]);
    melt_pour_all(MODE_CLEAR);
}

static void init_tim4(void) {
    TIM_PSC(TIM4) = (F_CPU / 2000) - 1;
    TIM_ARR(TIM4) = 1000 * 2;
    TIM_CNT(TIM4) = 0;
    TIM_DIER(TIM4) |= TIM_DIER_UIE;
    TIM_CR1(TIM4) |= TIM_CR1_CEN;

    nvic_enable_irq(NVIC_TIM4_IRQ);
}

extern int main(void) {
    cm_disable_interrupts();

    rcc_periph_clock_enable(RCC_TIM4);
    rcc_periph_clock_enable(RCC_SPI1);
    rcc_periph_clock_enable(RCC_AFIO);
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOC);

    init_tim4();
    melt_init();

    melt_draw_rectangle(MODE_DRAW, 0, 0, 64, 128);
    melt_draw_rectangle(MODE_DRAW, 2, 2, 60, 124);
    melt_draw_rectangle(MODE_DRAW, 22, 63, 23, 58);
    melt_draw_rectangle(MODE_DRAW, 20, 61, 27, 62);

    melt_printf(MODE_DRAW, 24, 65, u8"Счётчик:");

    melt_draw_circle(MODE_DRAW, 32, 32, 24);
    melt_draw_circle(MODE_DRAW, 32, 32, 22);

    cm_enable_interrupts();

    while (1);

    return 0;
}

extern void tim4_isr(void) {
#ifndef M_PI
#define M_PI 3.1415927f
#endif /* M_PI */

    static uint32_t tick = 0;
    static int32_t deg_sec = 271, deg_min = 271, deg_hour = 271;
    static size_t sec_len = 20, min_len = 18, hour_len = 16;

    TIM_SR(TIM4) &= ~TIM_SR_UIF;

    melt_pour_area(MODE_CLEAR, 35, 64, 9, 56);
    melt_printf(MODE_DRAW, 36, 64, u8"%02lu:%02lu:%02lu", (tick / 3600), ((tick / 60) % 60), (tick % 60));

    melt_draw_line_arbitrary(MODE_CLEAR, 32, 32, 32 + sec_len * cos(deg_sec % 60 / 30.0 * M_PI),
                             32 + sec_len * sin(deg_sec % 60 / 30.0 * M_PI));
    deg_sec--;
    if (!(tick % 60)) {
        melt_draw_line_arbitrary(MODE_CLEAR, 32, 32, 32 + min_len * cos(deg_min % 60 / 30.0 * M_PI),
                                 32 + min_len * sin(deg_min % 60 / 30.0 * M_PI));
        deg_min--;
    }
    if (!(tick % 720)) {
        melt_draw_line_arbitrary(MODE_CLEAR, 32, 32, 32 + hour_len * cos(deg_hour % 60 / 30.0 * M_PI),
                                 32 + hour_len * sin(deg_hour % 60 / 30.0 * M_PI));
        deg_hour--;
    }

    melt_draw_line_arbitrary(MODE_DRAW, 32, 32, 32 + hour_len * cos(deg_hour % 60 / 30.0 * M_PI),
                             32 + hour_len * sin(deg_hour % 60 / 30.0 * M_PI));
    melt_draw_line_arbitrary(MODE_DRAW, 32, 32, 32 + min_len * cos(deg_min % 60 / 30.0 * M_PI),
                             32 + min_len * sin(deg_min % 60 / 30.0 * M_PI));
    melt_draw_line_arbitrary(MODE_DRAW, 32, 32, 32 + sec_len * cos(deg_sec % 60 / 30.0 * M_PI),
                             32 + sec_len * sin(deg_sec % 60 / 30.0 * M_PI));

    tick++;
}
