#ifndef INC_MENU_HPP_
#define INC_MENU_HPP_
#include <stddef.h>
#include "stdint.h"

#include <stdio.h>

namespace IO
{
    union MenuInput
    {
        struct
        {
            uint8_t up          : 1;
            uint8_t down        : 1;
            uint8_t right       : 1;
            uint8_t left        : 1;
            uint8_t enter       : 1;
            uint8_t escape      : 1;
            uint8_t interrupt   : 1;
            uint8_t error       : 1;
        };
        
        uint8_t all;
    };

    struct Link
    {
        public:
        Link* next; //init to nullptr
        Link* prev;
        size_t offset;

        Link(Link* p=nullptr, Link*n=nullptr) : next(n), prev(p)
        {
            if (n)
                n->prev = this;
            if(p)
                p->next = this;
        }

        // DLL
        Link* get_head()
        {
            Link* ret = this;
            while(ret->prev)
            {
                ret = ret->prev;
            }
            return ret;
        }

        Link* get_tail()
        {
            Link* ret = this;
            while(ret->next)
            {
                ret = ret->next;
            }
            return ret;
        }

        Link* n(Link *ptr=nullptr)
        {
            if (ptr)
            {
                next = ptr;
                ptr->prev = this;
            }
            return next;
        }

        Link* p(Link *ptr=nullptr)
        {
            if (ptr)
            {
                prev = ptr;
                ptr->next = this;
            }
            return prev;
        }

        template<typename StructType>
        size_t get_offset()
        {
            offset = offsetof(StructType, link);
            return offset;
        }

        template <typename T>
        T* get_entry()
        {
           return (T*)((size_t)this - (size_t)offset);
        }

    };
    

    template<typename ValueType, typename ActionType>
    struct Entry
    {
        ValueType   data;    //< The IO info that gets displayed in some way. Usually char[n]
        ActionType  action;  //< The IO action that is undertaken when this is activated
        Link        link;

        Entry(ValueType v, ActionType a, Link*p=nullptr, Link* n=nullptr) : data(v), action(a), link(p, n) 
        {   
            link.get_offset<Entry<ValueType,ActionType>>();
        }
    };

    template<typename EntryType, size_t N>
    void linkEntryArray(EntryType(&arr)[N])
    {
        for (uint16_t i = 1; i < N; i++)
        {
            arr[i].link.p(&arr[i-1].link);
        }
        arr[N-1].link.n(&arr[0].link);
    }
  

    /**
     * @brief The Menu object will be used to manage the IO linked Entries
     *        It will not store the data itself. Each element will have to be created and added.
     *        By doing this, we should ensure the smallest memory footprint possible, as each entry will
     *        be defined as the minimum size necessary
    */
    class Menu
    {
        public:
        Link *active;

        Menu() {}
        Menu(Link* a) : active(a) {}

        Link* getActive() { return active; }

        template <typename T>
        T* getActiveEntry() { return (T*)(active - active->offset); }

        virtual void up() { }
        virtual void down() { }
        virtual void left() { }
        virtual void right() { }
        virtual void escape() { }
        virtual void enter() { }
        virtual void interrupt() { }
        virtual void error() { }


        template <typename T>
        void draw() { }

        template <typename T>
        void task(MenuInput &in)
        {
            if (in.all)
            {
                if (in.down)
                {
                    down();
                    in.down = 0;
                }
                if (in.up)
                {
                    up();
                    in.up = 0;
                }
                if (in.left)
                {
                    left();
                    in.left = 0;
                }
                if (in.right)
                {
                    right();
                    in.right = 0;
                }
                if (in.enter)
                {
                    enter();
                    in.enter = 0;
                }
                if (in.escape)
                {
                    escape();
                    in.escape = 0;
                }
                if (in.interrupt)
                {
                    interrupt();
                    in.interrupt = 0;
                }
                if (in.error)
                {
                    error();
                    in.error = 0;
                }
            }
            draw<T>();
        }
    };

  /*  template<typename ValueType, typename ActionType, typename ScrollActionType, size_t NumberMenuEntries>
    struct menu
    {
        typedef entry<ValueType, ActionType> EntryType;

        flat(ValueType val, ActionType action, ScrollActionType next, ScrollActionType prev)
        {
            
            scrollNext = next;
            scrollPrev = prev;

            emptyList = v;

            

            // EntryType *last = &v[NumberMenuEntries-1];

            for(size_t i = 1; i < NumberMenuEntries; i++)
            {
                emptyList->next = &v[i];
                v[i].prev = emptyList;
                emptyList = emptyList->next;
            }
            emptyList = &v[0];
            emptyList->prev = nullptr;
            v[NumberMenuEntries-1].next = nullptr;

            active = &v[0];
            emptyList = emptyList->next;
            active->data = val;
            active->action = action;
            active->next = active;
            active->prev = active;
            // add(val, action);
        }

        flat(ScrollActionType next, ScrollActionType prev)
        {
            scrollNext = next;
            scrollPrev = prev;
            for(size_t i = 1; i < NumberMenuEntries; i++)
            {
                v[i-1].next = &v[i];
            }
            emptyList = &v[0];
        }

        EntryType v[NumberMenuEntries];

        ScrollActionType scrollNext;
        ScrollActionType scrollPrev;

        uint16_t add(ValueType val, ActionType action)
        {
            if (active)
            {
                auto tv = &v[activeIndex];
                tv->prev = &v[activeIndex-1];
            }
            else
            {
                
            }
            activeEntries++;


            emptyList->prev = emptyList->next;
            temp->next = emptyList;
            emptyList = emptyList->next;

            temp->next->next = nullptr;

            temp->next->data = val;
            temp->next->action = action;
        }

        EntryType* prev()
        {
            active = active->prev;
            return active;
        }

        EntryType* next()
        {
            active = active->next;
            return active;
        }

        public:
            EntryType* active = nullptr;

        private:
            size_t activeEntries = 0;
            size_t activeIndex = 0;

            EntryType* emptyList = nullptr;
            
    };
    */

    // template<typename ValueType, typename ActionType>
    // struct flat
    // {
    //     // TODO: Figure out how not to rely on void pointers
    //     void* next;     //< Entry next to the current
    //     void* prev;     //< Entry before the current

        
    //     entry<ValueType, ActionType> v;

    //     bool isMenuFlat()
    //     {
    //         return this->isFlat;
    //     }

    //     flat* add(ValueType val, ActionType action, flat* itemPtr)
    //     {
    //         itemPtr->next = this->next;

    //         this->next = itemPtr;
    //         itemPtr->prev = this;
            
    //         this->v.data = val;
    //         this->v.action = action;

    //         return itemPtr;
    //     }

    //     private:
    //     bool isFlat = 1;
    // };

    


    
}



#endif /* INC_MENU_HPP_ */
