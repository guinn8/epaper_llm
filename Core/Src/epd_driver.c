#include "epd_driver.h"
#include "driver_esp8266.h"
#include "main.h"
#include "DEV_Config.h"
#include "GUI_Paint.h"
#include "EPD_2in13_V3.h"

#define Imagesize ((EPD_2in13_V3_WIDTH % 8 == 0)? (EPD_2in13_V3_WIDTH / 8 ): (EPD_2in13_V3_WIDTH / 8 + 1)) * EPD_2in13_V3_HEIGHT

static uint8_t BlackImage[Imagesize] = {0};

void epd_initialize(void) {
    DEV_Module_Init();
    EPD_2in13_V3_Init();
    EPD_2in13_V3_Clear();

    Paint_NewImage(BlackImage, EPD_2in13_V3_WIDTH, EPD_2in13_V3_HEIGHT, 270, WHITE); 
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
}

void epd_display_image(const uint8_t* image) {
    Paint_SelectImage(image);
    EPD_2in13_V3_Display(image);
}

void epd_display_line(uint8_t x, uint8_t y, const char* line) {
    Paint_SelectImage(BlackImage);
    Paint_DrawString_EN(x, y, line, &Font20, WHITE, BLACK);
    EPD_2in13_V3_Display_Partial(BlackImage);
    EPD_2in13_V3_Clear();
    EPD_2in13_V3_Sleep();
}



void display_input_char(char input) {
    static uint8_t x = 0, y = 0;
    const uint8_t char_height = Font20.Height;
    static char line_buffer[128];
    static int buffer_index = 0;

    if (input == '\r') {
        line_buffer[buffer_index] = '\0';
        epd_display_line(x, y, line_buffer);
        buffer_index = 0;
        y += char_height;
        x = 0;
    } else {
        if (buffer_index < sizeof(line_buffer) - 1) {
            line_buffer[buffer_index++] = input;
        }
    }
}

void epd_test(void) {
    epd_initialize();

    char input;
    while ((input = __io_getchar()) != EOF) {
        __io_putchar(input);
        display_input_char(input);
    }

    EPD_2in13_V3_Clear();
    EPD_2in13_V3_Sleep();
    DEV_Delay_ms(2000); // Important, at least 2s delay
    DEV_Module_Exit();
}
