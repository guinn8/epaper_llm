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
    HAL_Delay(1000);
}

void epd_display_image(const uint8_t* image) {
    Paint_SelectImage(image);
    EPD_2in13_V3_Display(image);
}

void epd_display_line(uint8_t x, uint8_t y, const char* line) {
    Paint_SelectImage(BlackImage);
    Paint_DrawString_EN(x, y, line, &Font20, WHITE, BLACK);
    EPD_2in13_V3_Display_Partial(BlackImage);
}

void epd_tty_demo(const char* text) {
    uint8_t x = 0, y = 0;
    const uint8_t char_height = Font20.Height;

    Paint_NewImage(BlackImage, EPD_2in13_V3_WIDTH, EPD_2in13_V3_HEIGHT, 90, WHITE); 
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);

    const char* line_start = text;
    for (const char* p = text; ; ++p) {
        if (*p == '\n' || *p == '\0') {
            char line[EPD_2in13_V3_WIDTH / Font20.Width + 1];
            int length = p - line_start;
            strncpy(line, line_start, length);
            line[length] = '\0';

            epd_display_line(x, y, line);

            y += char_height;
            line_start = p + 1;
        }
        if (*p == '\0') break;
    }
}

void epd_test(void) {
    epd_initialize();

    epd_tty_demo("Hello, this is a\nTTY-like display.\nIt streams chars\nline by line.\nEnjoy!");

    EPD_2in13_V3_Clear();
    EPD_2in13_V3_Sleep();
    DEV_Delay_ms(2000); // Important, at least 2s delay
    DEV_Module_Exit();
}
