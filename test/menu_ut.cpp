#include "ssd1306.hpp"
#include "oled_io.hpp"
#include <iostream>
#include "stdlib.h"
#include "stdio.h"
#include "fonts.h"

class TestOLED : public IO::SSD1306::OLED<uint32_t>
{
    public:
    void printout()
    {
        for (auto i = 0; i < IO::SSD1306::GDDRAM_PAGE_NUM; i++)
        {
            for (auto j = 0; j < IO::SSD1306::GDDRAM_PAGE_DATA_SIZE; j++)
            {
                for(auto k=0; k < IO::SSD1306::GDDRAM_PAGE_SIZE; k++)
                {
                    if((buf.page[i][k] & (1<<j)) == 0)
                        std::cout<<"*";
                    else
                        std::cout<<" ";
                }
                std::cout<<std::endl;
            }
        }
    }
    void print_buff()
    {
        std::cout<<std::endl<<std::endl;
        for (auto i = 0; i < IO::SSD1306::GDDRAM_PAGE_NUM; i++)
        {
            for (auto j = 0; j < IO::SSD1306::GDDRAM_PAGE_SIZE; j++)
            {
                if (j % 32 == 0)
                    std::cout<<std::endl;

                printf("0x%02x ", buf.page[i][j]);
                
            }
            std::cout<<std::endl;
        }
    }
};

namespace {

    TestOLED oled;

    typedef void (*oledScroll)(void);

    typedef IO::Entry<const char*, oledScroll> oledMenuEntry;

    void oled_scroll()
    {
        ;
    }

    void print_lol()
    {
        std::cout << "LOL" << std::endl;
    }

    

};

int Menu_Actions()
{
    oledMenuEntry entry1("Menu Start", print_lol);
    oledMenuEntry entry2("Menu 2", oled_scroll, &entry1.link);
    oledMenuEntry entry3("Menu 3", oled_scroll, &entry2.link);
    oledMenuEntry entry4("Menu 4", oled_scroll, &entry3.link, &entry1.link);

    IO::SSD1306Menu<FontDef_t, TestOLED> myMenu(font::Font_11x18, &oled);
    myMenu.active = &entry1.link;
    
    printf("Active: 0x%X : Prev:0x%0x Next:0x%0x\n", myMenu.active, myMenu.active->prev, myMenu.active->next);
    printf("Offset: 0x%X\n", myMenu.active->offset);
    oledMenuEntry* ptr = (oledMenuEntry*)(myMenu.active - myMenu.active->offset);

    printf("Entry1: 0x%X : Prev:0x%0x Next:0x%0x\n", &entry1, entry1.link.prev, entry1.link.next);
    printf("Entry2: 0x%X : Prev:0x%0x Next:0x%0x\n", &entry2, entry2.link.prev, entry2.link.next);
    printf("Entry3: 0x%X : Prev:0x%0x Next:0x%0x\n", &entry3, entry3.link.prev, entry3.link.next);
    printf("Entry4: 0x%X : Prev:0x%0x Next:0x%0x\n", &entry4, entry4.link.prev, entry4.link.next);

    if (ptr->data != entry1.data)
    {
        std::cout << "ERROR" << std::endl;
        return 1;
    }

    myMenu.up();
    printf("Active: 0x%X : Prev:0x%0x Next:0x%0x\n", myMenu.active, myMenu.active->prev, myMenu.active->next);
    ptr = (oledMenuEntry*)(myMenu.active - myMenu.active->offset);
    if (ptr->data != entry4.data)
    {
        printf("Active: 0x%X\n", myMenu.active);
        std::cout << "ERROR" << std::endl;
        return 1;
    }

    myMenu.down();
    ptr = (oledMenuEntry*)(myMenu.active - myMenu.active->offset);
    if (ptr->data != entry1.data)
    {
        std::cout << "ERROR" << std::endl;
        return 1;
    }

    
    std::cout << ptr->data << std::endl;

}

int main() 
{
    
    oled.Init();
    oled.Clear();
    oled.Putc('B', &font::Font_7x10, IO::SSD1306::Color::WHITE);
    oled.Puts("Badabing", &font::Font_7x10, IO::SSD1306::Color::WHITE);
    
    oled.print_buff();

    oled.printout();

    if(Menu_Actions())
    {
        return 1;
    }

    

    return 0;
}