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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
#include <math.h>
#include <errno.h>
#include <math.h>
#include <float.h>

#include <vector>

#include <nvs.h>
#include <nvs_flash.h>

#include <mutex>

#include <sys/param.h>
#include <sys/socket.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <sys/param.h>

#include <nvs_flash.h>
#include <arpa/inet.h>

#include <soc/uart_reg.h>
#include <soc/uart_struct.h>
#include <soc/rmt_struct.h>
#include <soc/dport_reg.h>
#include <soc/gpio_sig_map.h>
#include <soc/soc_caps.h>
#include <soc/io_mux_reg.h>

#include <driver/gpio.h>
#include <driver/sdmmc_host.h>
#include <driver/i2c.h>
#include <driver/uart.h>
#include <driver/sdmmc_host.h>
#include <driver/usb_serial_jtag.h>
#include <driver/ledc.h>

// RMT is completely incompatible from one IDF version to another!
// #include <driver/rmt_tx.h>
// #include <driver/rmt_encoder.h>
#define CONFIG_RMT_SUPPRESS_DEPRECATE_WARN 1
#include <driver/rmt.h>

#include <esp_system.h>
#include <esp_console.h>
#include <esp_pm.h>
#include <esp_wifi.h>
#include <esp_wps.h>
#include <esp_event.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_mac.h>
#include <esp_vfs_fat.h>
#include <esp_netif.h>
#include <esp_http_server.h>
#include <esp_timer.h>
#include <esp_task_wdt.h>
#include <esp_intr_alloc.h>
#include <esp_vfs_fat.h>
#include <esp_partition.h>
#include <esp_app_desc.h>
#include <esp_flash_partitions.h>
#include <esp_image_format.h>
#include <esp_secure_boot.h>
#include <esp_flash_encrypt.h>
#include <esp_ota_ops.h>
#include <esp_system.h>
#include <esp_efuse.h>
#include <esp_attr.h>
#include <esp_flash.h>
#include <esp_sntp.h>

#include <lwip/err.h>
#include <lwip/sockets.h>
#include <lwip/sys.h>
#include <lwip/netdb.h>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <freertos/queue.h>

#include <cJSON.h>

#define TAG "-------> RASE ------->"
