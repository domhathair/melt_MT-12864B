#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <math.h>
#include "stm32f2xx.h"

#define SYS_CLOCK 96000000U

#ifndef bool
#define bool _Bool
#endif

extern uint32_t TICK;
