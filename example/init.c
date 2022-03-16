#include "init.h"
#include "main.h"

Init_Struct Init = {
    &Init_PLL, /*Init.PLL*/
    { /*<Init_Port_Struct>*/
        &Init_Port_RCControl,
        { /*<Init_Port_Mode_Struct>*/
            &Init_Port_Mode_BF,
            &Init_Port_Mode_AF
        }, /*</Init_Port_Mode_Struct>*/
        &Init_Port_Level
    }, /*</Init_Port_Struct>*/
    { /*<Init_TIM_Struct>*/    
        &Init_TIM_Common
    } /*</Init_TIM_Struct>*/ 
}; /*Init*/
            
            
void Init_PLL(void) {
    uint32_t count = 0, current = 0;
    RCC->CR |= RCC_CR_HSEON;
    do {
        current = RCC->CR & RCC_CR_HSERDY;
    } while ((current == 0) && (count++ != 0xFFFF));
    if (current == 0)
        exit(-1);
    count = 0;
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE; /*HSE как источник PLL множителя частоты*/
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM;
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLM_3 | RCC_PLLCFGR_PLLM_4;
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN;
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_7 | RCC_PLLCFGR_PLLN_8;
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLQ;
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLQ_1;
    RCC->CR |= RCC_CR_PLLON; /*Включение множителя, итого при кварце 12МГц получаем 96МГц*/
    do {
        current = RCC->CR & RCC_CR_PLLRDY;
    } while ((current == 0) && (count++ != 0xFFFF));
    if (current == 0)
        exit(-1);
    count = 0;
    FLASH->ACR &= ~FLASH_ACR_LATENCY; /*Для частот 90..120 МГц нужно пропускать четыре цикла*/
    FLASH->ACR |= FLASH_ACR_LATENCY_3WS;
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL; /*PLL как базовое тактирование*/
    do {
        current = RCC->CFGR & RCC_CFGR_SWS_PLL;
    } while ((current == 0) && (count++ != 0xFFFF));
    if (current == 0)
        exit(-1);
} /*Init_PLL*/

/**/

void Init_Port_RCControl(Port_Enum address, Functional_State tumbler) {
    RCC->AHB1ENR = (tumbler == DISABLE) ? RCC->AHB1ENR & ~(1 << address) : RCC->AHB1ENR | (1 << address);
} /*Init_Port_RCControl*/

/**/

void Init_Port_Mode_BF(Port_Mode_Enum mode, Port_Enum address, uint8_t number) {
    GPIO_Mode_Setup GPIO_Mode_Register = {0};
    GPIO_Mode_Register.Full = mode;
    uint32_t *GPIO_Address = (uint32_t * )(AHB1PERIPH_BASE + (address << 10));
    GPIO_Address[MODER] &= ~(0x3 << (number * 2));
    GPIO_Address[MODER] |= GPIO_Mode_Register.Part.moder << (number * 2);
    GPIO_Address[OTYPER] &= ~(0x3 << (number));
    GPIO_Address[OTYPER] |= GPIO_Mode_Register.Part.otyper << (number);
    GPIO_Address[OSPEEDR] &= ~(0x3 << (number * 2));
    GPIO_Address[OSPEEDR] |= GPIO_Mode_Register.Part.ospeedr << (number * 2);
    GPIO_Address[PUPDR] &= ~(0x3 << (number * 2));
    GPIO_Address[PUPDR] |= GPIO_Mode_Register.Part.pupdr << (number * 2);
} /*Init_Port_Mode_BF*/

/**/

void Init_Port_Mode_AF(Port_Mode_AF_Enum af_Mode, Port_Enum address, uint8_t number) {
    uint32_t *GPIO_Address = (uint32_t * )(AHB1PERIPH_BASE + (address << 10));
    if (number < 8) {
        GPIO_Address[AFRL] &= ~(0xF << (number * 4));
        GPIO_Address[AFRL] |= af_Mode << (number * 4);
    } else {
        GPIO_Address[AFRH] &= ~(0xF << ((number - 8) * 4));
        GPIO_Address[AFRH] |= af_Mode << ((number - 8) * 4);
    }
} /*Init_Port_Mode_AF*/

/**/

void Init_Port_Level(Port_Enum address, uint8_t number, Port_Value value) {
    uint32_t *GPIO_Address = (uint32_t * )(AHB1PERIPH_BASE + (address << 10));
    GPIO_Address[BSRR] |= 1 << number << ((value == LOW) ? 16 : 0);
} /*Init_Port_Level*/

/**/

void Init_TIM_Common(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
    TIM6->PSC = (SYS_CLOCK / 2000) - 1;
    TIM6->ARR = 1000 * 2;
    TIM6->CNT = 0;
    TIM6->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM6_DAC_IRQn);
    NVIC_SetPriority(TIM6_DAC_IRQn, 3);
    TIM6->CR1 |= TIM_CR1_CEN;
    NVIC_SetPendingIRQ(TIM6_DAC_IRQn);
} /*Init_TIM_Common*/
