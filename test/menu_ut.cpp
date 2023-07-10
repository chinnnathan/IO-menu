#include "ssd1306.hpp"
#include "oled_io.hpp"
#include <iostream>
#include "stdlib.h"
#include "stdio.h"
#include "fonts.h"



void enter_mainmenu();

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

    void enter_submenu()
    {

    }

    

};

int Menu_Actions()
{
    oledMenuEntry entry1("Menu Start", print_lol);
    oledMenuEntry entry2("Menu 2", oled_scroll, &entry1.link);
    oledMenuEntry entry3("Menu 3", oled_scroll, &entry2.link);
    oledMenuEntry entry4("Menu 4", oled_scroll, &entry3.link, &entry1.link);

    IO::SSD1306Menu<FontDef_t, TestOLED> myMenu(&font::Font_11x18, &oled);
    myMenu.active = &entry1.link;

    printf("Active: 0x%X : Prev:0x%0x Next:0x%0x\n", myMenu.active, myMenu.active->prev, myMenu.active->next);
    printf("Offset: 0x%X\n", myMenu.active->offset);
    printf("Active-Offset: 0x%X\n", myMenu.active -  myMenu.active->offset);
    oledMenuEntry* ptr = myMenu.active->get_entry<oledMenuEntry>();
    printf("ptr: 0x%X : Prev:0x%0x Next:0x%0x\n", ptr, ptr->link.prev, ptr->link.next);

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
    ptr = myMenu.active->get_entry<oledMenuEntry>(); 
    if (ptr->data != entry4.data)
    {
        printf("Active: 0x%X\n", myMenu.active);
        std::cout << "ERROR 3->4" << std::endl;
        return 1;
    }

    myMenu.down();
    ptr = myMenu.active->get_entry<oledMenuEntry>();
    if (ptr->data != entry1.data)
    {
        std::cout << "ERROR4->1" << std::endl;
        return 1;
    }

    
    std::cout << ptr->data << std::endl;
    return 0;
}

void linkEntryArray(oledMenuEntry* arr, size_t s)
{
    uint16_t i = 1;
    // printf("Entry[%d]: 0x%X : Prev:0x%0x Next:0x%0x\n", 0, &arr[0], arr[0].link.prev, arr[0].link.next);
    for(; i < s; i++)
    {
        arr[i].link.p(&arr[i-1].link);

        printf("Entry[%d]: 0x%X : Prev:0x%0x Next:0x%0x\t| Prev->next:0x%0x\n", i, &arr[i], arr[i].link.prev, arr[i].link.next, arr[i].link.prev->next);

    }
    arr[i-1].link.n(&arr[0].link);
    // arr[0].link.prev = &arr[i].link;
    // arr[i].link.next = &arr[0].link;
    printf("Entry[%d]: 0x%X : Prev:0x%0x Next:0x%0x\n", i, &arr[i], arr[i].link.prev, arr[i].link.next);

}

int menu_linking()
{
    oledMenuEntry mainMenuEntries[] =
    {
        oledMenuEntry("Menu1", oled_scroll),
        oledMenuEntry("Menu2", oled_scroll, &mainMenuEntries[0].link),
        oledMenuEntry("Menu3", oled_scroll, &mainMenuEntries[1].link),
        oledMenuEntry("Enter SubMenu", enter_submenu, &mainMenuEntries[2].link, &mainMenuEntries[0].link),
    };

    oledMenuEntry copyMenuEntry[] =
    {
        oledMenuEntry("Menu1", oled_scroll),
        oledMenuEntry("Menu2", oled_scroll),
        oledMenuEntry("Menu3", oled_scroll),
        oledMenuEntry("Enter SubMenu", enter_submenu),
    };

    IO::linkEntryArray(copyMenuEntry);
    // linkEntryArray(copyMenuEntry, len(copyMenuEntry));

    if(len(mainMenuEntries) != 4)
    {
        std::cout << "Error!! len(mainMenuEntries != 4): " << len(mainMenuEntries) << std::endl;
        return 1; 
    }

    for(uint32_t i = 0; i < len(mainMenuEntries); i++)
    {
        if (strcmp(mainMenuEntries[i].data, copyMenuEntry[i].data))
        {
            std::cout << "Str unequal" << std::endl;
            return 1;
        }
        // if(mainMenuEntries[i].link)
    }
    printf("Entry0: 0x%X : Prev:0x%0x Next:0x%0x\n", &copyMenuEntry[0], copyMenuEntry[0].link.prev, copyMenuEntry[0].link.next);
    printf("Entry1: 0x%X : Prev:0x%0x Next:0x%0x\n", &copyMenuEntry[1], copyMenuEntry[1].link.prev, copyMenuEntry[1].link.next);
    printf("Entry2: 0x%X : Prev:0x%0x Next:0x%0x\n", &copyMenuEntry[2], copyMenuEntry[2].link.prev, copyMenuEntry[2].link.next);
    printf("Entry3: 0x%X : Prev:0x%0x Next:0x%0x\n", &copyMenuEntry[3], copyMenuEntry[3].link.prev, copyMenuEntry[3].link.next);

    return 0;
}

int main() 
{
    
    oled.Init();
    oled.Clear();
    oled.Putc('B', &font::Font_7x10, IO::SSD1306::Color::WHITE);
    oled.Puts((char*)"Badabing", &font::Font_7x10, IO::SSD1306::Color::WHITE);
    
    oled.print_buff();

    oled.printout();

    std::cout<<std::endl << "Begin Menu_linking test" <<std::endl;
    if(menu_linking())
    {
        return 1;
    }
    std::cout<<std::endl << "Begin Menu Action test" <<std::endl ;
    if(Menu_Actions())
    {
        return 1;
    }
    

    

    return 0;
}