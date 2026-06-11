/*
 * ==============================================================================
 *
 *  PROJECT:     "Rase" Radio Sensor Project,    Measuring and Config Application
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

#define SELF                    ((void*)(this))
#define OBJ_PTR(cast, ptr)      (reinterpret_cast<cast*>(ptr))
#define APP_PTR(ptr)            (OBJ_PTR(Application, ptr))

class CallbackParameter {
    private:
        void* this_ptr  = nullptr;
        void* parameter = nullptr;
        void* widget    = nullptr;
        void* auxWidget = nullptr;

    public:
        CallbackParameter(void* _this_ptr, void* _parameter, void* _widget = nullptr, void* _auxWidget = nullptr) {
            this_ptr  = _this_ptr;
            parameter = _parameter;
            widget    = _widget;
            auxWidget = _auxWidget;
        }

        ~CallbackParameter() {
        }

        void* get_this(void) { return (this_ptr); }
        void* get_pointer(void)  { return (parameter); }

        GtkWidget* get_widget(void)    { return ((GtkWidget*)widget); }
        GtkWidget* get_aux_widget(void) { return ((GtkWidget*)auxWidget); }
        
        void set_Widget(GtkWidget* _widget, GtkWidget* _auxWidget = nullptr) {
            widget = (void*)_widget;
            if (_auxWidget != nullptr) {
                auxWidget = _auxWidget;
            }
        }

};

#define ON_ITEM(item)           ((void*)new CallbackParameter(this, (void*)((uint64_t)item)))
#define CALLBACK_PARAMETER(ptr) (reinterpret_cast<CallbackParameter*>(ptr))
