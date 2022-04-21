#include "stm32f2xx.h"

#ifndef bool
#define bool _Bool
#endif

/**/

typedef enum {
    PA = 0,
    PB = 1,
    PC = 2,
    PD = 3,
    PE = 4,
    PF = 5,
    PG = 6,
    PH = 7,
    PI = 8
} Port_Enum;

/**/

typedef enum {
    GP_PP = 0x2C,     /*01 0 11 00*/
    GP_PP_PU = 0x2D, /*01 0 11 01*/
    GP_PP_PD = 0x2E, /*01 0 11 10*/
    GP_OD = 0x3C,     /*01 1 11 00*/
    GP_OD_PU = 0x3D, /*01 1 11 01*/
    GP_OD_PD = 0x3C, /*01 1 11 10*/
    IN_FL = 0x00,     /*00 0 00 00*/
    IN_PU = 0x01,     /*00 0 00 01*/
    IN_PD = 0x02,     /*00 0 00 10*/
    IN_AN = 0x60,     /*11 0 00 00*/
    AF_PP = 0x4C,     /*10 0 11 00*/
    AF_PP_PU = 0x4D, /*10 0 11 01*/
    AF_PP_PD = 0x4E, /*10 0 11 10*/
    AF_OD = 0x5C,     /*10 1 11 00*/
    AF_OD_PU = 0x5D, /*10 1 11 01*/
    AF_OD_PD = 0x5E     /*10 1 11 10*/
} Port_Mode_Enum;

/**/

typedef enum {
    AF0 = 0x0,
    AF1 = 0x1,
    AF2 = 0x2,
    AF3 = 0x3,
    AF4 = 0x4,
    AF5 = 0x5,
    AF6 = 0x6,
    AF7 = 0x7,
    AF8 = 0x8,
    AF9 = 0x9,
    AF10 = 0xA,
    AF11 = 0xB,
    AF12 = 0xC,
    AF13 = 0xD,
    AF14 = 0xE,
    AF15 = 0xF
} Port_Mode_AF_Enum;

/**/

typedef enum {
    LOW = 0,
    HIGH = !LOW
} Port_Value;

/**/

typedef enum {
    MODER = 0,
    OTYPER = 1,
    OSPEEDR = 2,
    PUPDR = 3,
    IDR = 4,
    ODR = 5,
    BSRR = 6,
    LCKR = 7,
    AFRL = 8,
    AFRH = 9
} GPIO_Register_Enum;

/**/

typedef union {
    struct {
        uint8_t pupdr: 2;
        uint8_t ospeedr: 2;
        uint8_t otyper: 1;
        uint8_t moder: 2;
    } Part;
    Port_Mode_Enum Full;
} GPIO_Mode_Setup;

/**/

typedef enum {
    OFF = 0,
    ON = !OFF
} Functional_State;

/**/

typedef struct {
	void (*BF)(Port_Mode_Enum mode, Port_Enum address, uint8_t number);
	void (*AF)(Port_Mode_AF_Enum af_Mode, Port_Enum address, uint8_t number);
} Init_Port_Mode_Struct;

typedef struct {
	void (*RCControl)(Port_Enum address, Functional_State tumbler);
	Init_Port_Mode_Struct Mode;
	void (*Level)(Port_Enum address, uint8_t number, Port_Value value);
} Init_Port_Struct;

typedef struct {
	void (*Common)(void);
} Init_TIM_Struct;

typedef struct {
	void (*PLL)(void);
	Init_Port_Struct Port;
	Init_TIM_Struct TIM;
} Init_Struct;

extern Init_Struct Init;

/**/

extern void Init_PLL(void);

extern void Init_Port_RCControl(Port_Enum address, Functional_State tumbler);

extern void Init_Port_Mode_BF(Port_Mode_Enum mode, Port_Enum address, uint8_t number);

extern void Init_Port_Mode_AF(Port_Mode_AF_Enum af_Mode, Port_Enum address, uint8_t number);

extern void Init_Port_Level(Port_Enum address, uint8_t number, Port_Value value);

extern void Init_TIM_Common(void);
