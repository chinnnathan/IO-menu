#ifndef INC_MENU_HPP_
#define INC_MENU_HPP_
#include <stddef.h>
#include "stdint.h"

template <typename T, size_t N>
constexpr size_t len(T(&)[N]) { return N; }

namespace IO
{

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
    };
    

    template<typename ValueType, typename ActionType>
    struct Entry
    {
        Link        link;
        ValueType   data;    //< The IO info that gets displayed in some way. Usually char[n]
        ActionType  action;  //< The IO action that is undertaken when this is activated

        Entry(ValueType v, ActionType a, Link*p=nullptr, Link* n=nullptr) : data(v), action(a), link(p, n) 
        {   
            link.get_offset<Entry<ValueType,ActionType>>();
        }
    };

    

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

        virtual void draw() { }
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
