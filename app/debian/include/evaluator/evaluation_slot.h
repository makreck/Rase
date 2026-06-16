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

class EvaluationSlot {
    private:
        struct {
            int           fd;
            LogFile*      logfile;
            LogWindow     window;
            EvalPt*       channel[LOG_SLOT_MAX];
            pthread_t     thread_handle = INVALID_THREAD_HANDLE;
            volatile bool data_ready = false;
        } m;

        static void* _evaluation_thread(void* _object);
        void evaluation_thread(void);

        void init(int _fd, LogFile* _logfile, LogWindow* _window);
        void cleanup(void);

    public:
        EvaluationSlot(int _fd, LogFile* _logfile, LogWindow* _window) {
            init(_fd, _logfile, _window);
        }

        ~EvaluationSlot() {
            cleanup();
        }

        LogWindow* get_window(void);
        EvalPt* create_channel(void);
        EvalPt* get_channel(int index, bool auto_create);
        void perform_sync(void);
        void perform_async(void);
        bool is_data_ready(void);
        bool wait_for_data_ready(void);

};
