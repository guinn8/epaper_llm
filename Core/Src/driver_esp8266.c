/**
 * @file driver_esp8266.c
 * @author Gavin Guinn
 * @brief Functions to control an ESP8266 over a uart interface.
 * @date 2024-06-28
 *
 */

#include "driver_esp8266.h"
#include <stdio.h>
#include <string.h>

static void uart_transceive(void *huart, uint8_t *message, uint8_t *resp, uint16_t resp_len) {
  uint16_t index = 0;
  while (*message) {
    (void)HAL_UART_Transmit(huart, message, 1, 10);

    if (index < resp_len - 1) {
      (void)HAL_UART_Receive(huart, &resp[index], 1, 1000);
      index++;
    }

    message++;
  }
  resp[index] = '\0';
}

uint8_t *AT_Command(void *huart, uint8_t *command, uint8_t *response, uint16_t response_size) {
  uint8_t fmt_cmd[128] = {0};
  snprintf((char *)fmt_cmd, sizeof(fmt_cmd), "%s\r\n", (char *)command);

  uint8_t echo[128] = {0};
  uart_transceive(huart, fmt_cmd, echo, sizeof(echo));
  if (strncmp((char *)command, (char *)echo, strlen((char *)command)) != 0) {
    return NULL;
  }

  uint8_t interstitial_control[3] = {0};
  if (HAL_OK != HAL_UART_Receive(huart, interstitial_control, 3, 1000)) {
    return NULL;
  } else if (strncmp("\n\r\n", (char *)interstitial_control, sizeof(interstitial_control))) {
    return NULL;
  }

  uint16_t index = 0;
  uint8_t prev_ch = 0; // Initialize prev_ch
  while (index < response_size - 1) {
    uint8_t ch;
    if (HAL_UART_Receive(huart, &ch, 1, 1000) != HAL_OK) {
      return response;
    }

    if (ch == '\r') {
      prev_ch = ch;
      continue;
    } else if (ch == '\n' && prev_ch == '\r') {
      response[index] = '\0';
      return response;
    }

    response[index++] = ch;
  }

  return NULL;
}


uint8_t *AT_Command2(void *huart, uint8_t *command, uint8_t *response, uint16_t response_size) {
  uint8_t fmt_cmd[128] = {0};
  snprintf((char *)fmt_cmd, sizeof(fmt_cmd), "%s\r\n", (char *)command);

  uint8_t echo[128] = {0};
  uart_transceive(huart, fmt_cmd, echo, sizeof(echo));
  if (strncmp((char *)command, (char *)echo, strlen((char *)command)) != 0) {
    return NULL;
  }


  uint16_t index = 0;
  uint8_t prev_ch = 0; // Initialize prev_ch
  while (index < response_size - 1) {
    uint8_t ch;
    if (HAL_UART_Receive(huart, &ch, 1, 10000) != HAL_OK) {
      return response;
    }

    response[index++] = ch;
  }

  return NULL;
}