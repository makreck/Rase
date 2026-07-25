/*
 * ==============================================================================
 *
 *  PROJECT:     "Rase" Radio Sensor Project,      Preliminary Configuration Tool
 *  COPYRIGHT:   (C)2025-2026 KKS-Elektronik,  M. Kreck, <makreck@googlemail.com>
 *
 *  This program is free software: you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free Software
 *  Foundation, either version 3 of the License, or (at your option) any later
 *  version.
 *
 *  This program is distributed in the hope that it will be useful,   but WITHOUT
 *  ANY WARRANTY, without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE, see the GNU General Public License for details.
 *
 *  You should have received a copy of the  GNU General Public License along with
 *  this program. If not, see <https://www.gnu.org/licenses/>.
 *
 * ==============================================================================
 */

#pragma once

#define SIZEOFARRAY(a)      (sizeof(a) / sizeof((a)[0]))

#define SELF                ((void*)(this))
#define OBJ_PTR(cast, ptr)  (reinterpret_cast<cast*>(ptr))
#define APP_PTR(ptr)        (OBJ_PTR(App, ptr))

class CallbackParameter {
    private:
        void*   this_ptr  = nullptr;
        int     item_id   = -1;
        void*   parameter = nullptr;
        void*   widget    = nullptr;
        void*   auxWidget = nullptr;

    public:
        CallbackParameter(void* _this_ptr, void* _parameter, void* _widget = nullptr, void* _auxWidget = nullptr) {
            this_ptr  = _this_ptr;
            parameter = _parameter;
            widget    = _widget;
            auxWidget = _auxWidget;
        }

        CallbackParameter(void* _this_ptr, int _item_id, void* _parameter = nullptr) {
            this_ptr  = _this_ptr;
            item_id   = _item_id;
            parameter = _parameter;
        }

        ~CallbackParameter() {
        }

        void* get_this(void)      { return (this_ptr);  }
        void* get_parameter(void) { return (parameter); }
        int   get_item_id(void)   { return (item_id);   }

        GtkWidget* get_widget(void)    { return ((GtkWidget*)widget); }
        GtkWidget* get_aux_widget(void) { return ((GtkWidget*)auxWidget); }
        
        void set_Widget(GtkWidget* _widget, GtkWidget* _auxWidget = nullptr) {
            widget = (void*)_widget;
            if (_auxWidget != nullptr) {
                auxWidget = _auxWidget;
            }
        }

};

#define ON_ITEM(instance, item) ((void*)new CallbackParameter((instance), (int)(item)))
#define ON_ITEM_PAR(instance, item, par) ((void*)new CallbackParameter((instance), (int64_t)(item), (void*)(par)))
#define CALLBACK_PARAMETER(ptr) (reinterpret_cast<CallbackParameter*>(ptr))
