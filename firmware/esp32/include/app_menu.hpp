/*
 * ==============================================================================
 *
 *  PROJECT:     "Rase" Radio Sensor Project,    ESP32-S3 Station Device Firmware
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

#define IDM_TITLE               (0)
#define IDM_EXIT                (1)
#define IDM_MAIN                (2)
#define IDM_LAYOUT              (3)
#define IDM_DISPLAY             (4)
#define IDM_LED_INTENSITY       (5)
#define IDM_SENSOR_SELECT       (6)
#define IDM_FACTORY_RESET       (7)
#define IDM_REBOOT              (8)
#define IDM_CONFIG              (9)

#define IDM_CONTRAST            (10)
#define IDM_DISPLAY_OFF         (11)
#define IDM_ROTATE  	        (12)
#define IDM_CONFIG_INTERFACE    (13)

#define IDM_DISPLAY_OFF_BASE    (20)
#define IDM_DISPLAY_OFF_NEVER   (IDM_DISPLAY_OFF_BASE + 0)
#define IDM_DISPLAY_OFF_10SEC   (IDM_DISPLAY_OFF_BASE + 1)
#define IDM_DISPLAY_OFF_1MIN    (IDM_DISPLAY_OFF_BASE + 2)
#define IDM_DISPLAY_OFF_5MIN    (IDM_DISPLAY_OFF_BASE + 3)
#define IDM_DISPLAY_OFF_15MIN   (IDM_DISPLAY_OFF_BASE + 4)
#define IDM_DISPLAY_OFF_30MIN   (IDM_DISPLAY_OFF_BASE + 5)
#define IDM_DISPLAY_OFF_LAST    (IDM_DISPLAY_OFF_BASE + 9)

#define IDM_CONTRAST_BASE       (30)
#define IDM_CONTRAST_HIGH       (IDM_CONTRAST_BASE + 0)
#define IDM_CONTRAST_MEDIUM     (IDM_CONTRAST_BASE + 1)
#define IDM_CONTRAST_LOW        (IDM_CONTRAST_BASE + 2)
#define IDM_CONTRAST_LAST       (IDM_CONTRAST_BASE + 9)

#define IDM_LAYOUT_BASE         (40)
#define IDM_LAYOUT_VALUE_PAGE   (IDM_LAYOUT_BASE + 0)
#define IDM_LAYOUT_DETAILS_PAGE (IDM_LAYOUT_BASE + 1)
#define IDM_LAYOUT_INFO_PAGE    (IDM_LAYOUT_BASE + 2)
#define IDC_LAYOUT_PAGE_LAST    (IDM_LAYOUT_BASE + 9) 

#define IDM_LED_INTENSITY_BASE  (50)
#define IDM_LED_INTENSITY_100   (IDM_LED_INTENSITY_BASE + 0)
#define IDM_LED_INTENSITY_75    (IDM_LED_INTENSITY_BASE + 1)
#define IDM_LED_INTENSITY_50    (IDM_LED_INTENSITY_BASE + 2)
#define IDM_LED_INTENSITY_25    (IDM_LED_INTENSITY_BASE + 3)
#define IDM_LED_INTENSITY_10    (IDM_LED_INTENSITY_BASE + 4)
#define IDM_LED_INTENSITY_1     (IDM_LED_INTENSITY_BASE + 5)
#define IDM_LED_INTENSITY_0     (IDM_LED_INTENSITY_BASE + 6)
#define IDM_LED_INTENSITY_LAST  (IDM_LED_INTENSITY_BASE + 9)

#define IDM_SENSOR_BASE         (60)
#define IDM_SENSOR_LAST         (IDM_SENSOR_BASE + APP_DRV_LIST_MAX)

class MenuItem {
    public:
        int id;
        const char* string;

        void set(int givenID, const char* givenString) {
            id = givenID;
            string = givenString;
        }
};

