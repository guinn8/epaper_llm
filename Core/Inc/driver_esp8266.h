#include <stdint.h>
#include "main.h"
#include "stm32f4xx_hal_uart.h"

uint8_t *AT_Command(void *huart, uint8_t *command, uint8_t *response, uint16_t response_size);
