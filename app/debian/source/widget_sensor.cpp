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

#include "includes.h"

#define MLI_EVENT_TYPE_SIDEBAR (0)
#define MLI_EVENT_TYPE_CHANNEL (1)

void SensorWidget::init(SensorConnection* _device, int _side_bar_number) {
    m.device   = _device;

    m.rc_widget.clr();
    m.gtk.row = gtk_list_box_row_new();
    m.gtk.grid = gtk_grid_new();
    gtk_container_set_border_width(GTK_CONTAINER(m.gtk.grid), 4);
    gtk_grid_set_column_spacing(GTK_GRID(m.gtk.grid), 8);
    gtk_widget_set_halign(m.gtk.grid, GtkAlign::GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(m.gtk.row), m.gtk.grid);

    int sidebar_width_px  = create_sidebar();
    int label_width_px    = create_label();
    int channels_width_px = create_channel_list();

    set_sidebar_number(_side_bar_number);

    int widget_cx = sidebar_width_px + (int)std::max(label_width_px, channels_width_px);
    m.rc_widget.set(0, 0, std::max(widget_cx, SENSOR_LIST_ITEM_WIDTH), SENSOR_LIST_ITEM_HEIGHT);
}

void SensorWidget::cleanup(void) {
    std::vector<Scale*> channels = m.device->get_channels();
    for (Scale*& channel_data : channels) {
        ChannelWidget* channel_widget = (ChannelWidget*)channel_data->get_userdata();
        if (channel_widget != nullptr) {
            channel_data->set_userdata(nullptr);
            delete (channel_widget);
        }
    }
}

gboolean SensorWidget::_eventHandler_ChannelWidget(GtkWidget* _widget, GdkEvent* _event, gpointer _data) {
    ChannelWidget* channel_widget = OBJ_PTR(ChannelWidget, _data);
    (channel_widget->get_parent())->eventHandler(MLI_EVENT_TYPE_CHANNEL, channel_widget, _event);
    return (false);
}

gboolean SensorWidget::_eventHandler_SidebarWidget(GtkWidget* _widget, GdkEvent* _event, gpointer _data) {
    OBJ_PTR(SensorWidget, _data)->eventHandler(MLI_EVENT_TYPE_SIDEBAR, nullptr, _event);
    return (false);
}

void SensorWidget::eventHandler(int _type, void* _holder, GdkEvent* _event) {
    switch (_event->type) {
        case GdkEventType::GDK_MOTION_NOTIFY: {
            handleMouseMoveEvent(_type, (ChannelWidget*)_holder, (GdkEventMotion*)_event);;
        } break;

        case GdkEventType::GDK_ENTER_NOTIFY:
        case GdkEventType::GDK_LEAVE_NOTIFY: {
            handleEnterLeave(_type, (ChannelWidget*)_holder, (GdkEventCrossing*)_event);;
        } break;

        case GdkEventType::GDK_SCROLL: {
            handleScrollEvent((GdkEventScroll*)_event);;
        } break;

        case GdkEventType::GDK_DOUBLE_BUTTON_PRESS:
        case GdkEventType::GDK_TRIPLE_BUTTON_PRESS:
        case GdkEventType::GDK_BUTTON_PRESS:
        case GdkEventType::GDK_BUTTON_RELEASE: {
            handleMouseButtonEvent(_type, _holder, (GdkEventButton*)_event);
        } break;

        default: {
        } break;
    }
}

void SensorWidget::handleScrollEvent(GdkEventScroll* _scroll_event) {
    float eventTime_s = std::min(1.0f, std::max(0.0f, (float)(_scroll_event->time - m.eventTimeStamp_ms) / 1000.0f));
    m.currentPt.set(_scroll_event->x, _scroll_event->y);
    processEventState(_scroll_event->state);
}


void SensorWidget::handleEnterLeave(int _type, ChannelWidget* _holder, GdkEventCrossing* _event) {
    m.currentPt.set(_event->x, _event->y);
    select_all_channels(false, false, 0x01);
}

void SensorWidget::handleMouseMoveEvent(int _type, ChannelWidget* _holder, GdkEventMotion* _motion_event) {
    float eventTime_s = std::min(1.0f, std::max(0.0f, (float)(_motion_event->time - m.eventTimeStamp_ms) / 1000.0f));
    m.currentPt.set(_motion_event->x, _motion_event->y);
    if (_type == MLI_EVENT_TYPE_CHANNEL) {
        select_all_channels(false, false, 0x01);
        if (_holder != nullptr) {
            _holder->set_state(true, false, 0x01);
        }
    }
}

int SensorWidget::create_sidebar(void) {
    m.sidebar_widget = new SidebarWidget(m.device);

    m.gtk.sidebar = m.sidebar_widget->get();
    gtk_grid_attach(GTK_GRID(m.gtk.grid), m.gtk.sidebar, 0, 0, 1, 3);

    GtkTool::set_mouse_button_event_list(m.gtk.sidebar, G_CALLBACK(SensorWidget::_eventHandler_SidebarWidget), this);

    int sideBarWidth = m.sidebar_widget->get_width();
    return (sideBarWidth);
}

int SensorWidget::create_label(void) {
    const ProductID* pid = m.device->get_pid();
    char string[1024]{ 0 };
    snprintf(string, sizeof (string), "%s %s V%s (%s), SN %s, RSSI %s",
        pid->manufacturer, pid->product, pid->firmware_version, pid->firmware_date, pid->device_serial_number, pid->rssi);

    m.gtk.label = gtk_label_new(string);
    gtk_widget_set_halign(m.gtk.label, GtkAlign::GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(m.gtk.grid), m.gtk.label, 1, 0, 4, 1);
    int labelWidth = strlen(string) * 16;
    return (labelWidth);
}

int SensorWidget::create_channel_list(void) {
    m.gtk.bar = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(m.gtk.bar), 8);
    gtk_grid_set_column_homogeneous(GTK_GRID(m.gtk.bar), true);
    gtk_grid_attach(GTK_GRID(m.gtk.grid), m.gtk.bar, m.taskbar_indent, 1, 4, 1);

    return (update_channel_list());
}

int SensorWidget::update_channel_list(void) {
    gtk_grid_remove_row(GTK_GRID(m.gtk.bar), 0);

    std::vector<Scale*> channels = m.device->get_channels();
    int n = 0;
    for (Scale*& channel_data : channels) {
        ChannelWidget* channel_widget = (ChannelWidget*)channel_data->get_userdata();
        if (channel_widget != nullptr) {
            channel_data->set_userdata(nullptr);
            delete (channel_widget);
        }
        
        channel_widget = new ChannelWidget(n++, -1, -1, channel_data, this);
        channel_data->set_userdata(channel_widget);

        GtkWidget* item = channel_widget->get_widget();
        if (item != nullptr) {
            GtkTool::set_mouse_button_event_list(item, G_CALLBACK(SensorWidget::_eventHandler_ChannelWidget), channel_widget);
            gtk_grid_attach(GTK_GRID(m.gtk.bar), item, n++, 0, 1, 1);
        }
    }

    for (int i = n; i < 6; i++) { // @todo: until channel max
        gtk_grid_attach(GTK_GRID(m.gtk.bar), gtk_label_new(" "), i, 0, 1, 1);
    }

    int taskbar_width = n * SENSOR_LIST_ITEM_WIDTH;
    return (taskbar_width);
}

void SensorWidget::set_sidebar_number(int _side_bar_number) {
    if (m.sidebar_widget != nullptr) {
        m.sidebar_widget->set_sidebar_number(_side_bar_number);
    }
}

bool SensorWidget::processEventTime(uint32_t timeValue) {
    if (m.eventTimeStamp_ms == 0) {
        m.eventTimeStamp_ms = (uint32_t)std::max(0, (int)timeValue - 1000);
    }
    m.eventTime_s = std::min(1.0f, std::max(0.0f, (float)(timeValue - m.eventTimeStamp_ms) / 1000.0f));
    m.eventTimeStamp_ms = timeValue;
    return (false);
}

bool SensorWidget::processMouseClicks(int _type, void* _holder) {
    bool mustUpdate = false;

    if ((m.event.group.fAuxKeyDown == 0) && (m.event.group.fMultiClick == 0) && (m.eventTime_s > 0.5f)) {
        if (m.event.flag.fLeftBtnDown == 1) {
            if (_type == MLI_EVENT_TYPE_CHANNEL) {
                mustUpdate |= selectChannel((ChannelWidget*)_holder);
            }
        } else if (m.event.flag.fRightBtnDown == 1) {

        } else if (m.event.flag.fMidBtnDown == 1) {
            
        }
    } else if ((m.event.flag.fLeftBtnDown == 1) && (m.event.flag.fDoubleClick == 1)) {
        mustUpdate = true; // @todo: processParameterRequest(type, _holder);
    }

    return (mustUpdate);
}

void SensorWidget::select_all_channels(bool _hilighted, bool _selected, uint8_t _flags) {
    std::vector<Scale*> channels = m.device->get_channels();
    for (Scale*& channel : channels) {
        ChannelWidget* widget = (ChannelWidget*)channel->get_userdata();
        if (widget != nullptr) {
            widget->set_state(_hilighted, _selected, _flags);
        }
    }
}

bool SensorWidget::selectChannel(ChannelWidget* _holder) {
    select_all_channels(false, false, 0x02);

    if (_holder != nullptr) {
        _holder->set_state(false, true, 0x02);
        // @todo: Control the selected channel in line recorder from here.
    }

    return (false);
}

void SensorWidget::processEventState(guint state) {
    m.event.flag.fShiftKeyDown = (state & KEY_STATE_EX_SHIFT)   ? 1 : 0;
    m.event.flag.fCtrlKeyDown  = (state & KEY_STATE_EX_CONTROL) ? 1 : 0;
    m.event.flag.fAltKeyDown   = (state & KEY_STATE_EX_ALT)     ? 1 : 0;
}

bool SensorWidget::processKeyStates(GdkEventButton* btnEvent) {
    bool mustUpdate = false;

    processEventState(btnEvent->state);

    if (btnEvent->type == GdkEventType::GDK_BUTTON_PRESS) {
        m.event.flag.fLeftBtnDown  = ((int)btnEvent->button == KEY_BUTTON_MOUSE_LEFT)   ? 1 : 0;
        m.event.flag.fMidBtnDown   = ((int)btnEvent->button == KEY_BUTTON_MOUSE_MIDDLE) ? 1 : 0;
        m.event.flag.fRightBtnDown = ((int)btnEvent->button == KEY_BUTTON_MOUSE_RIGHT)  ? 1 : 0;
        if (m.eventTime_s > 0.5f) {
            m.event.flag.fDoubleClick  = 0;
            m.event.flag.fTrippleClick = 0;
        }
        m.eventPt.set(btnEvent->x, btnEvent->y);
    } else if (btnEvent->type == GdkEventType::GDK_BUTTON_RELEASE) {
        m.currentPt.set(btnEvent->x, btnEvent->y);
        mustUpdate |= handleButtonRelease();
    } else if (btnEvent->type == GdkEventType::GDK_DOUBLE_BUTTON_PRESS) {
        m.event.flag.fDoubleClick  = 1;
        m.event.flag.fTrippleClick = 0;
    } else if (btnEvent->type == GdkEventType::GDK_TRIPLE_BUTTON_PRESS) {
        m.event.flag.fDoubleClick  = 0;
        m.event.flag.fTrippleClick = 1;
    }

    return (mustUpdate);
}

void SensorWidget::handleMouseButtonEvent(int type, void* holder, GdkEventButton* btnEvent) {
    bool mustUpdate = false;

    mustUpdate |= processEventTime((uint32_t)(btnEvent->time));
    mustUpdate |= processKeyStates(btnEvent);
    mustUpdate |= processMouseClicks(type, holder);

    if (mustUpdate) {

    }
}

bool SensorWidget::handleButtonRelease(void) {
    m.event.flag.fLeftBtnDown = 0;
    m.event.flag.fMidBtnDown = 0;
    m.event.flag.fRightBtnDown = 0;
    if (m.eventTime_s > 0.5f) {
        m.event.flag.fDoubleClick = 0;
        m.event.flag.fTrippleClick = 0;
    }
    return (true);
}


