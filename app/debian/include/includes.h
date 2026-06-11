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

#include "stdafx.h"

#include "typedefs.h"
#include "version.h"
#include "static_parameter.h" 
#include "app_error.h"
#include "micro_json.h"
#include "times.h"
#include "color.h"
#include "pointf.h"
#include "rect_ex.h"
#include "callback.h"
#include "svg_image.h"
#include "gtk_tool.h"
#include "resource.h"
#include "files.h"
#include "config.h"
#include "scale.h"
#include "scale_drawing.h"
#include "product_id.h"
#include "sensor_connection.h"
#include "sensor_bus.h"

#include "datalogger/log_defs.h"
#include "datalogger/log_frame.h"
#include "datalogger/log_inventory.h"
#include "datalogger/log_registry.h"
#include "datalogger/log_header.h"
#include "datalogger/log_range.h"
#include "datalogger/log_window.h"
#include "datalogger/log_file.h"
#include "datalogger.h"

#include "evaluator/evaluation_defs.h"
#include "evaluator/evaluation_point.h"
#include "evaluator/evaluation_slot.h"
#include "evaluator.h"

#include "line_recorder.h"
#include "widget_sidebar.h"
#include "widget_channel.h"
#include "widget_sensor.h"
#include "app.h"
