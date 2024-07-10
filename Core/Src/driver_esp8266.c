/**
 * @file driver_esp8266.c
 * @author Gavin Guinn
 * @brief Functions to control an ESP8266 over a uart interface.
 * @date 2024-06-28
 *
 */

#include "driver_esp8266.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "DEV_Config.h"
#include "EPD_2in13_V3.h"
#include "GUI_Paint.h"
#include "driver_esp8266.h"
#include "epd_driver.h"
#include "main.h"

#define PORT "7000"

extern UART_HandleTypeDef huart1;
static UART_HandleTypeDef *modem_uart = &huart1;
static char resp[5024] = {0};
static uint16_t read_index = 0;
volatile uint16_t write_index = 0;
static uint16_t start_at_cmd = 0;

void copy_string_from_circular_buffer(char *response, size_t response_len) {
  size_t len =
      (write_index >= start_at_cmd) ? (write_index - start_at_cmd) : (sizeof(resp) - start_at_cmd + write_index);
  len = (len > response_len - 1) ? (response_len - 1) : len;
  strncpy(response, &resp[start_at_cmd], len);
  response[len] = '\0';
}

bool send_at_command_and_check_response(char *cmd, char *expected_response, char *response, size_t response_len) {
  HAL_UART_Transmit(modem_uart, (uint8_t *)cmd, strlen(cmd), HAL_MAX_DELAY);

  uint32_t start_tick = HAL_GetTick();
  start_at_cmd = read_index;
  while ((HAL_GetTick() - start_tick) < 20000) {
    write_index = (sizeof(resp) - __HAL_DMA_GET_COUNTER(modem_uart->hdmarx)) % sizeof(resp);
    while (read_index != write_index) {
      putchar(resp[read_index]);
      if (strncmp(&resp[read_index], expected_response, strlen(expected_response)) == 0) {
        for (size_t i = 0; i < strlen(expected_response); i++)
        {
          putchar(resp[read_index + i + 1]);
        }
        
        printf("\n\r");
        read_index += strlen(expected_response);
        copy_string_from_circular_buffer(response, response_len);
        return true;
      }
      read_index = (read_index + 1) % sizeof(resp);
    }
    HAL_Delay(10);
  }

  // printf("Command failed: %s \r\n", cmd);
  return false;
}

void server_communication(void) {
  char response[1024] = {0};
  int num_chars;
  char message[1024] = {0};
  
  while (1)
  {
    if (send_at_command_and_check_response("AT+CIPSTART=\"TCP\", \"iot.espressif.cn\", 8000 \r\n", "OK", response, sizeof(response))) {
      break;
    }
  }
  

  epd_initialize();
  

 
  epd_display_line(0, 0, response);

}


typedef struct {
  char ap_ip[16];
  char ap_mac[18];
  char sta_ip[16];
  char sta_mac[18];
} NetworkInfo;

int parse_cifsr_response(const char *response, NetworkInfo *info) {
  const char *ap_ip_str = "+CIFSR:APIP,\"";
  const char *ap_mac_str = "+CIFSR:APMAC,\"";
  const char *sta_ip_str = "+CIFSR:STAIP,\"";
  const char *sta_mac_str = "+CIFSR:STAMAC,\"";

  const char *ap_ip_start = strstr(response, ap_ip_str);
  const char *ap_mac_start = strstr(response, ap_mac_str);
  const char *sta_ip_start = strstr(response, sta_ip_str);
  const char *sta_mac_start = strstr(response, sta_mac_str);

  if (!ap_ip_start || !ap_mac_start || !sta_ip_start || !sta_mac_start) {
    return -1; // Parsing failed
  }

  sscanf(ap_ip_start + strlen(ap_ip_str), "%[^\"]", info->ap_ip);
  sscanf(ap_mac_start + strlen(ap_mac_str), "%[^\"]", info->ap_mac);
  sscanf(sta_ip_start + strlen(sta_ip_str), "%[^\"]", info->sta_ip);
  sscanf(sta_mac_start + strlen(sta_mac_str), "%[^\"]", info->sta_mac);

  return 0; // Parsing successful
}

#include <stdio.h>
#include <string.h>

void ping_pong_communication(void) {
    char response[1024] = {0};

    if (!send_at_command_and_check_response("AT+CIPSEND\r\n", ">", response, sizeof(response))) {
        return;
    }

    const char eot[] = "<|eot_id|>";
    while (1) {
        memset(response, NULL, sizeof(response));
        printf("trying to prompt llm server...\n\r");
        if (send_at_command_and_check_response("give me only 16 jazzy words: zip zop zoop!\n\r", "<|eot_id|>", response, 100)) {
            printf("\r\nSuccessful ping-pong message!\n\r");
            printf("\n got response = \"%s\"\n\r", response);
            epd_initialize();
            char* pos = strstr(response, "<|");
            if (pos != NULL) {
                *pos = '\0';
            }
            epd_display_line(0, 0, response);
        }
        else{
        printf("\n\rError got response = \"%s\"\n\r", response);

        }
        HAL_Delay(5000);
    }
}


void setup_network(void){
    HAL_UART_Receive_DMA(modem_uart, (uint8_t *)resp, sizeof(resp));

    char response[1024] = {0};

    if (send_at_command_and_check_response("AT+RST\r\n", "ready", response, sizeof(response))) {
        printf("Module reset.\n\r");
    }

    if (send_at_command_and_check_response("AT\r\n", "OK", response, sizeof(response))) {
        printf("AT OK.\n\r");
    }

    if (send_at_command_and_check_response("AT+CWMODE_CUR=3\r\n", "OK", response, sizeof(response))) {
        printf("WiFi mode set.\n\r");
    }

    if (send_at_command_and_check_response("AT+CWJAP_CUR=\"tiglath\",\"thedog123\"\r\n", "OK", response, sizeof(response))) {
        printf("Connected to WiFi.\n\r");
    }

    if (send_at_command_and_check_response("AT+CIFSR\r\n", "OK", response, sizeof(response))) {
        printf("Got IP address.\n\r");
    }

    if (send_at_command_and_check_response("AT+CIPMUX=0\r\n", "OK", response, sizeof(response))) {
        printf("Single connection mode set.\n\r");
    }

    while (1) {
        if (send_at_command_and_check_response("AT+CIPSTART=\"TCP\",\"10.0.0.201\","PORT"\r\n", "OK", response, sizeof(response))) {
            printf("Connected to server.\n\r");
            break;
        }
        HAL_Delay(10000);
    }

   if (send_at_command_and_check_response("AT+CIPMODE=1\r\n", "OK", response, sizeof(response))) {
    }

    

    ping_pong_communication();
}
