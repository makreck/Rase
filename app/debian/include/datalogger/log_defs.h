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

class LogHeader;
class LogInventory;
class LogRegistry;
class LogRegistryChunk;
class LogRange;
class LogWindow;
class Scale;

#define LOG_CHUNK_DIR_MAX       (7164)                                          // # Maximum number of entries for the log file directory
#define LOG_SLOT_MAX            (16)                                            // # Maximum number of supported slots (channel keys) in the log file
#define LOG_USERDATA_MAX        (4096)                                          // # Size of the user-space (e.g. report text) in LogInventory structure

#define LOG_FILE_TITEL          "Data log file, V1.0.0.0 - 2026-06-04\r\n\0"    // # Displayable titel string at the beginning of the log file
#define LOG_FILE_MAGIC          (0x20260604)                                    // # Magic ID for identify the file type
#define LOG_FILE_VERSION        (0x01000000)                                    // # file format version
#define LOG_FILE_UPDATE_MS      (30000)                                         // # Intwerval, in milliseconds, for automatiocally updating/writing the file header
#define LOG_FILE_DEF_FOLDER     "/recordings"                                   // # Default sub-folder for storing datalogger files
#define LOG_FILE_EXTENSION      ".rsl"                                          // # Default file type extension for datalogger files

#define LOG_FILE_POS_HEADER     (0)
#define LOG_FILE_POS_INVENTORY  (0 + sizeof (LogHeader))
#define LOG_FILE_POS_REGISTRY   (0 + sizeof (LogHeader) + sizeof (LogInventory))
#define LOG_FILE_POS_DATA       (0 + sizeof (LogHeader) + sizeof (LogInventory) + sizeof (LogRegistry))
