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

#define SENSOR_LIST_ITEM_WIDTH  (128)
#define SENSOR_LIST_ITEM_HEIGHT (128)

class SensorWidget {
    private:
        struct {
            SensorConnection* device         = nullptr;
            LineRecorder*     recorder       = nullptr;
            SidebarWidget*    sidebar_widget = nullptr;

            RectEx rc_widget;
            
            int  taskbar_indent = 1;
            bool request_redraw = true;

            struct {
                GtkWidget* row;
                GtkWidget* grid;
                GtkWidget* sidebar;
                GtkWidget* image;
                GtkWidget* label;
                GtkWidget* bar;
                GtkWidget* bar_label;
            } gtk;

            union {
                uint8_t data = 0;
                struct {
                    uint8_t fShiftKeyDown : 1;
                    uint8_t fCtrlKeyDown : 1;
                    uint8_t fAltKeyDown : 1;

                    uint8_t fLeftBtnDown : 1;
                    uint8_t fMidBtnDown : 1;
                    uint8_t fRightBtnDown : 1;

                    uint8_t fDoubleClick : 1;
                    uint8_t fTrippleClick : 1;
                } flag;
                struct {
                    uint8_t fAuxKeyDown : 3;
                    uint8_t fBtnDown    : 3;
                    uint8_t fMultiClick : 2;
                } group;
            } event;
            
            float eventTime_s = 0.0f;
            uint32_t eventTimeStamp_ms = 0;
            PointF eventPt;
            PointF currentPt;

        } m;

        void init(SensorConnection* _device, LineRecorder* _recorder);
        void cleanup(void);

        static gboolean _eventHandler_ChannelWidget(GtkWidget* _widget, GdkEvent* _event, gpointer _data);
        static gboolean _eventHandler_SidebarWidget(GtkWidget* _widget, GdkEvent* _event, gpointer _data);

        void eventHandler(int _type, void* _holder, GdkEvent* _event);
        void handleMouseButtonEvent(int type, void* holder, GdkEventButton* btnEvent);
        void handleScrollEvent(GdkEventScroll* _scroll_event);
        void handleEnterLeave(int _type, ChannelWidget* _holder, GdkEventCrossing* _event);
        void handleMouseMoveEvent(int _type, ChannelWidget* _holder, GdkEventMotion* _motion_event);
        void processEventState(guint state);
        void select_all_channels(bool _hilighted, bool _selected, uint8_t _flags = 0x03);
        bool processEventTime(uint32_t timeValue);
        bool processMouseClicks(int type, void* holder);
        bool processKeyStates(GdkEventButton* btnEvent);
        bool handleButtonRelease(void);
        bool selectChannel(ChannelWidget* holder);
        int  create_label(void);
        int  create_sidebar(void);
        int  create_channel_list(void);
        int  update_channel_list(void);

    public:
        SensorWidget(SensorConnection* _device, LineRecorder* _recorder = nullptr) {
            init(_device, _recorder);
        }
        
        ~SensorWidget() {
            cleanup();
        }

        void set_sidebar_number(int _number) {
            if (m.sidebar_widget != nullptr) {
                m.sidebar_widget->set_sidebar_number(_number);
            }
        }

        GtkWidget* get_row()       { return (m.gtk.row);       }
        GtkWidget* get_grid()      { return (m.gtk.grid);      }
        GtkWidget* get_sidebar()   { return (m.gtk.sidebar);   }
        GtkWidget* get_image()     { return (m.gtk.image);     }
        GtkWidget* get_label()     { return (m.gtk.label);     }
        GtkWidget* get_bar()       { return (m.gtk.bar);       }
        GtkWidget* get_bar_label() { return (m.gtk.bar_label); }
};

