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

#include "line_recorder/line_recorder_find.h"

#define LR_MAX_TASK                     (6)
#define LV_CURVE_MAX                    (4096)
#define LV_CURVE_AUTO_SIZE              (0)
#define LV_CURVE_LENGTH_EXPAND_FACTOR   (2.0)
#define LV_EVALUATION_TIME_OVERHEAD     (0.5)
#define LV_CATCH_PT_RADIUS              (15.0f)
#define LV_MAX_SECONDS_GAP              (60.0)
#define LV_POINT_REGISTER_SIZE          (32)

#define LR_SCALE_BOX_HEIGHT             (80)
#define LR_SCALE_INDENT_X_LEFT          (16)
#define LR_SCALE_INDENT_X_RIGHT         (16)
#define LR_SCALE_INDENT_Y_TOP           (4)
#define LR_SCALE_INDENT_Y_BOTTOM        (0)

#define LR_SCALE_BORDER_HEIGHT          (4)
#define LR_PAPER_ROLLER_HEIGHT          (16)
#define LR_PAPER_GRADIENT_THRESHOLD     (int)((float)LR_PAPER_ROLLER_HEIGHT / 2.5f)
#define LR_PAPER_SEGMENT_HEIGHT         (64)

#define LR_INFO_BOX_HEIGHT              (48)
#define LR_SUPEND_TO_RESTART_DEF_S      (60.0)
#define LR_ZOOM_RECT_MIN_SIZE           (16)

#define LR_INIT_TIMESPAN                (1.0 / TC_MINUTES_PER_DAY)

typedef bool (*LREventCallback)(LRFindResult* _event_result, void* _user_param);

class LineRecorder {
    private:
        struct {
            LREventCallback callback_proc = nullptr;
            void* user_param = nullptr;

            GtkWidget* area = nullptr;
            cairo_surface_t* surface = nullptr;

            Scale default_scale;
            std::vector<ScaleStep> scale_steps;

            struct {
                Evaluator* device = nullptr;
                EvalCurve* node   = nullptr;
                std::string headline;
            } select;

            std::vector<Evaluator*> evaluations;

            cairo_surface_t* segment = nullptr; 
            double paper_shift = 0.0;
            double update_speed_s = 0.050;
            LogWindow window;

            float event_time_s = 0.0f;
            LRFindResult event_result;
            uint32_t event_timestamp_ms = 0;
            PointF pt_event;
            PointF pt_current;
            LogWindow event_window;

            pthread_t update_thread_handle = INVALID_THREAD_HANDLE;
            bool update_pending = false;
            uint64_t suspend_timestamp = 0;

            union {
                RectEx rect[15];
                struct {
                    RectEx surface;
                    RectEx segment;

                    RectEx scaleBox;
                    RectEx scale;
                    RectEx scaleBorder;
                        
                    RectEx paperBox;
                    RectEx paperRollTop;
                    RectEx paperRollBottom;
                    RectEx paper;
                        
                    RectEx infoBox;
                    RectEx infoBorder;
                    RectEx info;
                    RectEx infoFile;
                    RectEx infoWnd;
                    RectEx infoSel;
                } rc;
            };
            RectEx rc_zoom;

            union {
                ColorRef c[8];
                struct {
                    ColorRef scaleBkg;
                    ColorRef scaleText;
                    ColorRef paper;
                    ColorRef paperText;
                    ColorRef gridFine;
                    ColorRef gridMain;
                    ColorRef infoTextDef;
                    ColorRef infoTextHi;
                } color;
            };

            union {
                cairo_pattern_t* patternList[4]{ nullptr };
                struct {
                    cairo_pattern_t* scaleBorder;
                    cairo_pattern_t* paperTopRoll;
                    cairo_pattern_t* paperBottomRoll;
                    cairo_pattern_t* infoBorder;
                } pattern;
            };

            union {
                uint64_t flags = 0;
                struct {
                    uint64_t fShiftKeyDown : 1;
                    uint64_t fCtrlKeyDown : 1;
                    uint64_t fAltKeyDown : 1;
                    uint64_t fLeftBtnDown : 1;
                    uint64_t fMidBtnDown : 1;
                    uint64_t fRightBtnDown : 1;
                    uint64_t fDoubleClick : 1;
                    uint64_t fTrippleClick : 1;

                    uint64_t fZoomRectangle : 1;
                    uint64_t fZoomScale : 1;
                    uint64_t fZoomPaper : 1;
                    uint64_t fAutoScroll : 1;
                    uint64_t fReserved1 : 4;

                    uint64_t fReservedA2 : 8;
                    uint64_t fReservedA3 : 8;
                    uint64_t fReservedA4 : 8;
                    uint64_t fReservedA5 : 8;
                    uint64_t fReservedA6 : 8;
                    uint64_t fReservedA7 : 8;
                };
                
                struct {
                    uint64_t fAuxKeyDown : 3;
                    uint64_t fBtnDown : 3;
                    uint64_t fMultiClick : 2;

                    uint64_t fZoom : 3;
                    uint64_t fControls : 5;

                    uint64_t fReservedB2 : 8;
                    uint64_t fReservedB3 : 8;
                    uint64_t fReservedB4 : 8;
                    uint64_t fReservedB5 : 8;
                    uint64_t fReservedB6 : 8;
                    uint64_t fReservedB7 : 8;
                };
            };
        } m;

        void init(LREventCallback callback_proc, void* user_param);
        void cleanup(void);
        
        void init_colors(void);
        void init_times(void);
        void init_drawing_area(void);

        static void* _update_thread(void* _object);
        void update_thread(void);

        static gboolean _update_callback(gpointer _object);
        void update_callback(void);

        static gboolean _configure(GtkWidget* _widget, cairo_t* _cr, gpointer _data);
        void configure(GtkWidget* widget, cairo_t* cr);

        static gboolean _draw_function(GtkWidget* _area, cairo_t* _cr, gpointer _data);
        void draw_function(GtkWidget* area, cairo_t* cr);

        static gboolean _event_handler(GtkWidget* _widget, GdkEvent* _event, gpointer _data);
        void event_handler(GdkEvent* event);

        void set_event_callback(LREventCallback _callbackProc, void* _user_param);
        void notify_event(void);
        bool process_event_time(uint32_t _time_value);
        bool process_mouse_clicks(void);
        void process_event_state(guint _state);
        bool process_key_states(GdkEventButton* _btn_event);
        bool handle_button_release(void);
        void handle_mouse_button_event(GdkEventButton* _btn_event);
        void handle_mouse_move_event(GdkEventMotion* _motion_event);
        void handle_scroll_event(GdkEventScroll* _scroll_event);
        bool begin_mouse_zoom(void);
        void process_zoom_rect(void);
        bool end_mouse_zoom(void);

        double top_of_paper(void);
        double paper2now(void);
        double scroll_paper(double timeOffset);
        void update(void);
        void redraw(void);
        void suspend_autoscroll(float seconds);
        void resume_autoscroll(void);
        void scroll_paper_by_dots(double _dotsToScroll);
        void create_rectangles(int x, int y, int width, int height);
        void create_segment(void);
        void destroy_segment(void);
        void create_gradient_patterns(void);
        void destroy_gradient_patterns(void);
        bool find_element(double _x, double _y);
        void update_segment(void);
        void window_update(void);
        bool select_channel(void);

        bool set_found_on_scale(double _x, double _y);
        bool set_found_on_info(double _x, double _y);
        bool set_found_on_paper(double _x, double _y);

        void draw_scale(cairo_t* cr);
        void draw_paper(cairo_t* cr);
        void draw_info(cairo_t* cr);
        void draw_channels(cairo_t* cr);
        void draw_control_helpers(cairo_t* cr);
        void draw_selection_info(cairo_t *cr, RectEx& rc, LRFindResult& result);
        void draw_info_section(cairo_t* cr, int x, int y, int& max_x, const char* hint, const char* part1, const char* part2, const char* part3);

    public:
        LineRecorder(LREventCallback _callbackProc, void* _user_param) {
            init(_callbackProc, _user_param);
        }

        ~LineRecorder() {
            cleanup();
        }

        GtkWidget* get(void) { return (m.area); }
        
        bool add_evaluation(const char* _path);

};
