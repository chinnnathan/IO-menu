/**
 * original author:  Tilen Majerle<tilen@majerle.eu>
 * modification for STM32f10x: Alexander Lutsai<s.lyra@ya.ru>
 * C++ generics: Nathan Chinn

   ----------------------------------------------------------------------
   	Copyright (C) Nathan Chinn, 2023
   	Copyright (C) Alexander Lutsai, 2016
    Copyright (C) Tilen Majerle, 2015
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.
     
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
   ----------------------------------------------------------------------
 */
#pragma once

#include "fonts.h"

#include "stdlib.h"
#include "stdint.h"
#include "string.h"

using namespace font; // undo this laziness

/* Absolute value */
#define ABS(x)   ((x) > 0 ? (x) : -(x))

namespace IO::SSD1306 {

constexpr uint16_t WIDTH                                = 128;
constexpr uint16_t HEIGHT                               = 64;
constexpr uint16_t GDDRAM_PAGE_NUM                      = 8;
constexpr uint16_t GDDRAM_PAGE_DATA_SIZE                = 8; // uint8_t byte for each data page element
constexpr uint16_t BUFFER_SIZE                          = (WIDTH * HEIGHT) / GDDRAM_PAGE_DATA_SIZE;
constexpr uint16_t WRITE_SIZE                           = BUFFER_SIZE/WIDTH;
constexpr uint16_t GDDRAM_PAGE_SIZE                     = BUFFER_SIZE/GDDRAM_PAGE_NUM;
constexpr uint16_t RIGHT_HORIZONTAL_SCROLL              = 0x26;
constexpr uint16_t LEFT_HORIZONTAL_SCROLL               = 0x27;
constexpr uint16_t VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL = 0x29;
constexpr uint16_t VERTICAL_AND_LEFT_HORIZONTAL_SCROLL  = 0x2A;
constexpr uint16_t DEACTIVATE_SCROLL                    = 0x2E; // Stop scroll
constexpr uint16_t ACTIVATE_SCROLL                      = 0x2F; // Start scroll
constexpr uint16_t SET_VERTICAL_SCROLL_AREA             = 0xA3; // Set scroll range
constexpr uint16_t NORMALDISPLAY                        = 0xA6;
constexpr uint16_t INVERTDISPLAY                        = 0xA7;

template<typename T>
struct i2c
{
    T* handle;
    uint16_t addr = 0x78; //0x7A
};

enum class Color : uint8_t 
{
    BLACK = 0x00,
    WHITE = 0x01
};

union ScreenBuffer
{
    uint8_t page[GDDRAM_PAGE_NUM][GDDRAM_PAGE_SIZE];
    uint8_t array[BUFFER_SIZE];
};

template<typename T>
class OLED
{   
    public:
    ScreenBuffer buf;
    i2c<T> bus;
    
    OLED()
    {
        CurrentX = 0;
        CurrentY = 0;
        Inverted = 0;
        Initialized = 0;   
    }

    uint8_t Init(void) {

        /* Init I2C */
        i2cInit();
        
        /* Check if LCD connected to I2C */
        if (!i2cReady(bus.handle, bus.addr, 1, 20000))
            return 1;
        
        /* A little delay */
        uint32_t p = 2500;
        while(p>0)
            p--;

        /* Init LCD */
        uint8_t cmd[] = {
            0xAE, //display off
            0x20, //Set Memory Addressing Mode   
            0x10, //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
            0xB0, //Set Page Start Address for Page Addressing Mode,0-7
            0xC8, //Set COM Output Scan Direction
            0x00, //---set low column address
            0x10, //---set high column address
            0x40, //--set start line address
            0x81, //--set contrast control register
            0xFF,
            0xA1, //--set segment re-map 0 to 127
            0xA6, //--set normal display
            0xA8, //--set multiplex ratio(1 to 64)
            0x3F, //
            0xA4, //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
            0xD3, //-set display offset
            0x00, //-not offset
            0xD5, //--set display clock divide ratio/oscillator frequency
            0xF0, //--set divide ratio
            0xD9, //--set pre-charge period
            0x22, //
            0xDA, //--set com pins hardware configuration
            0x12,
            0xDB, //--set vcomh
            0x20, //0x20,0.77xVcc
            0x8D, //--set DC-DC enable
            0x14, //
            0xAF, //--turn on SSD1306 panel
        };
        
        WriteCommandSet(cmd);
        
        

        WriteCommand(DEACTIVATE_SCROLL);

        /* Clear screen */
        Fill(Color::BLACK);
        
        /* Update screen */
        UpdateScreen();
        
        /* Set default values */
        CurrentX = 0;
        CurrentY = 0;
        
        /* Initialized OK */
        Initialized = 1;
        
        /* Return OK */
        return 0;
    }

    void ScrollRight(uint8_t start_row, uint8_t end_row)
    {
        uint8_t cmd[] = {
            RIGHT_HORIZONTAL_SCROLL,
            0x00,
            start_row,
            0x00,
            end_row,
            0x01,
            ACTIVATE_SCROLL
        };
        WriteCommandSet(cmd);
    }


    void ScrollLeft(uint8_t start_row, uint8_t end_row)
    {
        uint8_t cmd[] = {
            LEFT_HORIZONTAL_SCROLL,
            0x00,
            start_row,
            0x00,
            end_row,
            0x01,
            ACTIVATE_SCROLL
        };
        WriteCommandSet(cmd);
    }

    void Scrolldiagright(uint8_t start_row, uint8_t end_row)
    {
        uint8_t cmd[] = {
            SET_VERTICAL_SCROLL_AREA,
            0x00,
            HEIGHT,
            VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL,
            0x00,
            start_row,
            0x00,
            end_row,
            0x01,
            ACTIVATE_SCROLL
        };
        WriteCommandSet(cmd);
    }


    void Scrolldiagleft(uint8_t start_row, uint8_t end_row)
    {
        uint8_t cmd[] = {
            SET_VERTICAL_SCROLL_AREA,
            0x00,
            HEIGHT,
            VERTICAL_AND_LEFT_HORIZONTAL_SCROLL,
            0x00,
            start_row,
            0x00,
            end_row,
            0x01,
            ACTIVATE_SCROLL
        };
        WriteCommandSet(cmd);
    }


    void Stopscroll(void)
    {
        WriteCommand(DEACTIVATE_SCROLL);
    }



    void InvertDisplay (int i)
    {
        if (i) WriteCommand(INVERTDISPLAY);

        else WriteCommand(NORMALDISPLAY);
    }


    void DrawBitmap(int16_t x, int16_t y, const unsigned char* bitmap, int16_t w, int16_t h, uint16_t color)
    {

        int16_t byteWidth = (w + GDDRAM_PAGE_NUM-1) / GDDRAM_PAGE_NUM; // Bitmap scanline pad = whole byte
        uint8_t byte = 0;

        for(int16_t j=0; j<h; j++, y++)
        {
            for(int16_t i=0; i<w; i++)
            {
                if(i & 7)
                {
                byte <<= 1;
                }
                else
                {
                byte = (*(const unsigned char *)(&bitmap[j * byteWidth + i / GDDRAM_PAGE_NUM]));
                }
                if(byte & 0x80) DrawPixel(x+i, y, color);
            }
        }
    }

    

    void UpdateScreen(void) {
        uint8_t m;
        
        for (m = 0; m < GDDRAM_PAGE_NUM; m++) {
            uint8_t cmd[] = {0xB0 + m, 0x00, 0x10};
            WriteCommandSet(cmd);

            i2cWriteMulti(bus.addr, 0x40, buf.page[m], WIDTH);
        }
    }

    void ToggleInvert(void) {
        uint16_t i;
        
        /* Toggle invert */
        Inverted = !Inverted;
        
        /* Do memory toggle */
        for (i = 0; i < sizeof(buf.array); i++) {
            buf.array[i] = ~buf.array[i];
        }
    }

    void Fill(Color color) {
        /* Set memory */
        memset(buf.array, (color == Color::BLACK) ? 0x00 : 0xFF, sizeof(buf.array));
    }

    void DrawPixel(uint16_t x, uint16_t y, Color color) {
        if (
            x >= WIDTH ||
            y >= HEIGHT
        ) {
            /* Error */
            return;
        }
        
        /* Check if pixels are inverted */
        // if (Inverted) {
        //     color = (Color)!color;
        // }
        
        /* Set color */
        if (color == Color::WHITE) {
            buf.array[x + (y / GDDRAM_PAGE_NUM) * WIDTH] |= 1 << (y % GDDRAM_PAGE_NUM);
        } else {
            buf.array[x + (y / GDDRAM_PAGE_NUM) * WIDTH] &= ~(1 << (y % GDDRAM_PAGE_NUM));
        }
    }

    void DrawLine(uint16_t lineIdx, Color c)
    {
        if (lineIdx > HEIGHT)
        {
            return;
        }
        uint8_t *page = buf.page[lineIdx / GDDRAM_PAGE_NUM];
        
        if( c == Color::WHITE)
        {
            for (auto i = 0; i < sizeof(page); i++)
            {
                buf.page[lineIdx][i] |= (1 << (lineIdx % GDDRAM_PAGE_NUM));
            }
        }
        else
        {
            for (auto i = 0; i < sizeof(page); i++)
            {
                buf.page[lineIdx][i] &= ~(1 << (lineIdx % GDDRAM_PAGE_NUM));
            }
        }
    }

    void FillPage(uint16_t page, uint8_t data = 0x00)
    {
        if (page < GDDRAM_PAGE_NUM)
        {
            memset(buf.page[page], data, sizeof(buf.page[page]));
        }
    }

    void GotoXY(uint16_t x, uint16_t y) {
        /* Set write pointers */
        CurrentX = x;
        CurrentY = y;
    }

    char Putc(char ch, FontDef_t* Font, Color color) {
        uint32_t i, b, j;
        
        /* Check available space in LCD */
        if (
            WIDTH <= (CurrentX + Font->FontWidth) ||
            HEIGHT <= (CurrentY + Font->FontHeight)
        ) {
            /* Error */
            return 0;
        }
        
        /* Go through font */
        for (i = 0; i < Font->FontHeight; i++) {
            b = Font->data[(ch - 32) * Font->FontHeight + i];
            for (j = 0; j < Font->FontWidth; j++) {
                if ((b << j) & 0x8000) {
                    DrawPixel(CurrentX + j, (CurrentY + i), color);
                } 
                // else {
                //     DrawPixel(CurrentX + j, (CurrentY + i), (uint8_t)!color);
                // }
            }
        }
        
        /* Increase pointer */
        CurrentX += Font->FontWidth;
        
        /* Return character written */
        return ch;
    }

    char Puts(char* str, FontDef_t* Font, Color color) {
        /* Write characters */
        while (*str) {
            /* Write character by character */
            if (Putc(*str, Font, color) != *str) {
                /* Return error */
                return *str;
            }
            
            /* Increase string pointer */
            str++;
        }
        
        /* Everything OK, zero should be returned */
        return *str;
    }
    
    void Clear (void)
    {
        Fill (Color::BLACK);
        UpdateScreen();
    }
    void ON(void) {
        uint8_t cmd[] = { 0x8D, 0x14, 0xAF };
        WriteCommandSet(cmd);
    }
    void OFF(void) {
        uint8_t cmd[] = { 0x8D, 0x10, 0xAE };
        WriteCommandSet(cmd);
    }

    inline void WriteCommand(uint16_t data)
    {
        i2cWrite(bus.addr, 0x00, data);
    }

    inline void WriteData(uint16_t data)
    {
        i2cWrite(bus.addr, 0x40, data);
    }

    template <size_t N>
    inline void WriteDataSet(const uint8_t data[N])
    {
        i2cWriteMulti(bus.addr, 0x40, data, N);
    }

    // template <size_t N>
    inline void WriteCommandSet(const uint8_t data[])
    {
        i2cWriteMulti(bus.addr, 0x00, (uint8_t*)data, sizeof(data));
    }

    virtual void        i2cInit() {};
    virtual uint16_t    i2cReady(T* i2cHandle, uint16_t address, uint16_t devAddr, uint16_t timeout) { return 0; }
    virtual void        i2cWriteMulti(uint8_t address, uint8_t reg, uint8_t* data, uint16_t count) {};
    virtual void        i2cWrite(uint8_t address, uint8_t reg, uint8_t data) {};

    // private:
    uint16_t CurrentX;
	uint16_t CurrentY;
	uint8_t Inverted;
	uint8_t Initialized;
};
    

}
