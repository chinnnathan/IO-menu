#include "oled_io.hpp"
#include "menu.hpp"
#include "menu_api.h"

#if STM32F103
#include "stm32f1xx_hal.h"
#endif
#if STMF407
#include "stm32f4xx_hal.h"
#endif

void enter_submenu();
void enter_mainmenu();



namespace {

    typedef void (*oledScroll)(void);

    typedef IO::Entry<const char*, oledScroll> oledMenuEntry;

    void oled_scroll()
    {
        ;
    }

    class F103OLED : public IO::SSD1306::OLED<I2C_HandleTypeDef>
    {
        public:

        F103OLED()
        {
            bus.addr = 0x78;
        }

        void i2cInit() 
        {
            //MX_I2C1_Init();
            uint32_t p = 250000;
            while(p>0)
                p--;
            //HAL_I2C_DeInit(&hi2c1);
            //p = 250000;
            //while(p>0)
            //	p--;
            //MX_I2C1_Init();
        }

        uint16_t i2cReady(uint16_t address, uint16_t devAddr, uint16_t timeout) 
        {
            if (HAL_I2C_IsDeviceReady(bus.handle, address, devAddr, timeout) != HAL_OK) {
            /* Return false */
                return 0;
            }
            return 1;
        }

        void i2cWriteMulti(uint8_t address, uint8_t reg, uint8_t* data, uint16_t count) 
        {
            uint8_t dt[256];
            dt[0] = reg;
            uint8_t i;
            for(i = 0; i < count; i++)
            dt[i+1] = data[i];
            HAL_I2C_Master_Transmit(bus.handle, address, dt, count+1, 10);
        };

        void i2cWrite(uint8_t address, uint8_t reg, uint8_t data) 
        {
            uint8_t dt[2];
            dt[0] = reg;
            dt[1] = data;
            HAL_I2C_Master_Transmit(bus.handle, address, dt, 2, 10);
        };
    };

    F103OLED oled;

    oledMenuEntry mainMenuEntries[] =
    {
        oledMenuEntry("Menu1", blink_led_pattern_1),
        oledMenuEntry("Menu2", blink_led_pattern_2, &mainMenuEntries[0].link),
        oledMenuEntry("Menu3", blink_led_pattern_3, &mainMenuEntries[1].link),
        oledMenuEntry("Enter SubMenu", enter_submenu, &mainMenuEntries[2].link, &mainMenuEntries[0].link),
    };

    oledMenuEntry subMenuEntries[] =
    {
        oledMenuEntry("Sub Menu1", blink_led_pattern_4),
        oledMenuEntry("Sub Menu2", blink_led_pattern_3, &subMenuEntries[0].link),
        oledMenuEntry("Sub Menu3", blink_led_pattern_2, &subMenuEntries[1].link),
        oledMenuEntry("Exit SubMenu", enter_mainmenu, &subMenuEntries[2].link, &subMenuEntries[0].link),
    };

    void linkEntryArray(oledMenuEntry* arr, size_t n)
    {
        uint16_t i = 1;
        for(; i < n; i++)
        {
            arr[i].link.p(&arr[i-1].link);
        }
        arr[i].link.n(&arr[0].link);
    }

    

    IO::SSD1306Menu<FontDef_t, IO::SSD1306::OLED<I2C_HandleTypeDef>> MainMenu(&font::Font_7x10, &oled);
};

void enter_mainmenu()
{
    // linkEntry Array function not quite working :'(
    // linkEntryArray(mainMenuEntries, len(mainMenuEntries));
    MainMenu.active = &mainMenuEntries[0].link;
    MainMenu.draw<oledMenuEntry>();
}

void enter_submenu()
{
    MainMenu.active = &subMenuEntries[0].link;
    MainMenu.draw<oledMenuEntry>();
}


void menu_init(I2C_HandleTypeDef* hi2c)
{
    // linkEntry Array function not quite working :'(
    // linkEntryArray(mainMenuEntries, len(mainMenuEntries));
	MainMenu.active = &mainMenuEntries[0].link;

    oled.bus.handle = hi2c;

	oled.Init();
	oled.Clear();

    oled.GotoXY(0,0);
	oled.Puts("Initialized", &font::Font_7x10, IO::SSD1306::Color::WHITE);
    oled.UpdateScreen();

    enter_mainmenu();
}

void menu_up()
{
	MainMenu.up();
    MainMenu.draw<oledMenuEntry>();
}

void menu_next()
{
	MainMenu.up();
    MainMenu.draw<oledMenuEntry>();
}

void menu_prev()
{
	MainMenu.down();
    MainMenu.draw<oledMenuEntry>();
}

uint16_t menu_entry(uint8_t* pbuff, size_t len)
{
    MainMenu.getActiveEntry<oledMenuEntry>()->action();
    return 0;
}


