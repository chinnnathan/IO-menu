#include "oled_io.hpp"
#include "menu.hpp"

namespace {

    typedef void (*oledScroll)(void);

    typedef IO::Entry<const char*, oledScroll> oledMenuEntry;

    void oled_scroll()
    {
        ;
    }

    oledMenuEntry entry1("Menu Start", oled_scroll);
    oledMenuEntry entry2("Menu 2", oled_scroll, &entry1);
    oledMenuEntry entry3("Menu 3", oled_scroll, &entry2);
    oledMenuEntry entry4("Menu 4", oled_scroll, &entry3);

};

/*

#include "menu.hpp"
#include "menu_api.h"
#if OLED_SSD1306
    #include "oled_io.h"
#endif
#include <stdio.h>

constexpr size_t numMenu = 4;

typedef uint16_t (*printOledType)(uint8_t* pbuff, size_t len);
typedef void (*oledScroll)(void);
typedef menu::entry<char*, printOledType> oledMenuEntry;

uint16_t printClicks(uint8_t* pbuff, size_t len);

namespace 
{
    uint8_t toggle = 0;
    
    menu::flat<char*, printOledType, oledScroll, numMenu> mainMenu("MenuStart", printClicks, menu_next, menu_prev);
    // menu::flat<char*, printOledType, oledScroll, numMenu> mainMenu(menu_next, menu_prev);
}

void print_menu(oledMenuEntry *menuEntry)
{
    print_oled(OLED_INFO, menuEntry->data);
    print_oled(OLED_DATA, menuEntry->next->data);
    print_oled(OLED_SUBDATA, menuEntry->next->next->data);
    print_oled(OLED_BOTTOM, menuEntry->next->next->next->data); 
}

void menu_init()
{
    mainMenu.add("Menu1", printClicks);
    mainMenu.add("Menu2", printClicks);
    print_menu(mainMenu.active);
       
    print_oled(OLED_INFO, mainMenu.v[0].data);
    print_oled(OLED_DATA, mainMenu.v[1].data);
    print_oled(OLED_SUBDATA, mainMenu.v[2].data);
    print_oled(OLED_BOTTOM, mainMenu.v[3].data); 
        // print_menu(mainMenu.active);

    
    // mainMenu.add("Menu3", printClicks);
    // mainMenu.add("Menu4", printClicks);
    
    // print_oled(OLED_INFO, menuEntry->data);

//    print_menu(mainMenu.active);
}


void menu_next()
{
    auto menuEntry = mainMenu.next();
    print_menu(menuEntry);
}

void menu_prev()
{
    auto menuEntry = mainMenu.prev();
    print_menu(menuEntry);
}

uint16_t menu_entry(uint8_t* pbuff, size_t len)
{
    return mainMenu.active->action(pbuff, len);    
}

uint16_t printClicks(uint8_t* pbuff, size_t len)
{
    toggle ^= 1;
    snprintf((char*)pbuff, len, "Num ticks:%lu", (TIM1->CNT)>>2);
    print_oled(OLED_SUBDATA, (char*)pbuff);
    return toggle;
}
*/