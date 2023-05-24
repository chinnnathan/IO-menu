#pragma once
#include "ssd1306.hpp"

#include "menu.hpp"
#if OLED_SSD106
#include "ssd1306.hpp"
#include "fonts.h"
#endif


namespace IO
{
    constexpr uint16_t numLines = 4;
    constexpr uint16_t offset = 18;

    template<typename FontType, typename OledType>
    class SSD1306Menu : public IO::Menu 
    {
        public:
        FontType font;
        OledType *oled;

        SSD1306Menu(FontType f, OledType *o) : font(f), oled(o)
        {
            // if (!oled->Initialized)
            // {
            //     oled->Init();
            // }
        }

        void writeline(char* str, uint8_t line)
        {
            if (line < numLines)
            {
                oled->GotoXY(0, line * (offset));
                oled->Puts(str, font, SSD1306::Color::WHITE);
            }
        }

        template<typename T>
        void draw()
        {
            T* ptr = (T*)(active - active->offset);
            oled->Fill(SSD1306::Color::BLACK);
            for(auto i = 0; i < numLines; i ++)
            {
                writeline(ptr->data, i);
                ptr = (T*)(ptr->link->next - ptr->link->offset);
            }
            oled->UpdateScreen();
        }

        void up()
        {
            active = active->prev;
        }
        
        void down()
        {
            active = active->next;
        }
    };

};