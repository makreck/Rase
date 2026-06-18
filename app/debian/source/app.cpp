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

app_err_t Application::init(int argc, char* argv[]) {
    m.argumentsCount = argc;
    m.argumentsList = argv;

    m.cfg = new Config();
    m.bus = new SensorBus(Application::_sensor_bus_callback, this);

    startUI();

    return (ok);
}

app_err_t Application::cleanup(void) {
    m.fTerminate = true;

    stop_update();

    if (m.lineRecorder != nullptr) {
        delete (m.lineRecorder);
    }

    if (m.gtkApp != nullptr) {
        g_object_unref(m.gtkApp);
    }

    if (m.bus != nullptr) {
        delete (m.bus);
        m.bus = nullptr;
    }
    
    if (m.cfg != nullptr) {
        delete (m.cfg);
        m.cfg = nullptr;
    }
    
    return (ok);
}

app_err_t Application::run(void) {
    return ((app_err_t)g_application_run(G_APPLICATION(m.gtkApp), m.argumentsCount, m.argumentsList));
}

void Application::start_update(void) {
    if (m.updateThread == 0) {
        pthread_create(&m.updateThread, nullptr, Application::_updateThreadProc, this);
    }
}

void Application::stop_update(void) {
    if (m.updateThread != 0) {
        pthread_cancel(m.updateThread);
        pthread_join(m.updateThread, nullptr);
        m.updateThread = 0;
    }
}

app_err_t Application::_sensor_bus_callback(void* _user_param, SensorBusMessageCode _message, void* _arg1, void* _arg2) {
    return ((APP_PTR(_user_param))->sensor_bus_callback(_message, _arg1, _arg2));
}
app_err_t Application::sensor_bus_callback(SensorBusMessageCode message, void* arg1, void* arg2) {

    switch (message) {
        case SensorBusMessageCode::new_measurement: {
        } break;

        case SensorBusMessageCode::scan_begin: {
            std::vector<SensorConnection*>* device_list = (std::vector<SensorConnection*>*)arg1;
            char string[1024]{ 0 };
            snprintf(string, sizeof (string), "Scan_begin, currently %d devices.", (int)device_list->size());
            set_status_text(string);
        } break;

        case SensorBusMessageCode::scan_update: {
        } break;

        case SensorBusMessageCode::scan_complete: {
            std::vector<SensorConnection*>* device_list = (std::vector<SensorConnection*>*)arg1;
            char string[1024]{ 0 };
            snprintf(string, sizeof (string), "Scan complete, %d devices found.", (int)device_list->size());
            set_status_text(string);

            gdk_threads_add_idle(Application::_busUpdate, this);
        } break;

        case SensorBusMessageCode::device_added: {
            SensorConnection* sc = (SensorConnection*)arg1;
            const ProductID* pid = sc->get_pid();
            char string[1024]{ 0 };
            snprintf(string, sizeof (string), "Device added: IP %s, name=\"%s %s %s\".",
                sc->get_path(), pid->manufacturer, pid->product, pid->device_serial_number);
            set_status_text(string);
        } break;

        case SensorBusMessageCode::device_on_remove: {
            SensorConnection* sc = (SensorConnection*)arg1;
            const ProductID* pid = sc->get_pid();
            char string[1024]{ 0 };
            snprintf(string, sizeof (string), "Removing device: IP %s, name=\"%s %s %s\".",
                sc->get_path(), pid->manufacturer, pid->product, pid->device_serial_number);
            set_status_text(string);
        } break;

        case SensorBusMessageCode::device_removed: {
            SensorConnection* sc = (SensorConnection*)arg1;
            const ProductID* pid = sc->get_pid();
            char string[1024]{ 0 };
            snprintf(string, sizeof (string), "Device removed: IP %s, name=\"%s %s %s\".",
                sc->get_path(), pid->manufacturer, pid->product, pid->device_serial_number);
            set_status_text(string);
        } break;

        case SensorBusMessageCode::query_start: {
        } break;

        case SensorBusMessageCode::query_started: {
        } break;

        case SensorBusMessageCode::query_stop: {
        } break;

        case SensorBusMessageCode::query_stopped: {
        } break;

        case SensorBusMessageCode::rescan_request: {
            set_status_text("Rescan request...");
            gdk_threads_add_idle(Application::_post_function_task, new StaticParameter(this, (int64_t)SensorBusMessageCode::rescan_request));
        } break;

        case SensorBusMessageCode::recording_started: {
            const char* logging_path = (const char*)arg1;
            if ((logging_path != nullptr) && (m.lineRecorder != nullptr)) {
                m.lineRecorder->add_evaluation(logging_path);
            }
        } break;

        case SensorBusMessageCode::recording_ended: {
        } break;

        default: {
        } break;
    }


    return (0);
}
