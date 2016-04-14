/*
 * Copyright (c) 2015 ARM Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "mbed-client/m2mconstants.h"
const char* COAP = "coap://";
const char* COAPS = "coaps://";

// PUT attributes to be checked from server
const char *EQUAL = "=";
const char *AMP = "&";
const char *PMIN = "pmin";
const char *PMAX = "pmax";
const char *GT = "gt";
const char *LT = "lt";
const char *ST = "st";
const char *STP = "stp";
const char* CANCEL = "cancel";


//LWM2MOBJECT NAME/ID
const char* M2M_SECURITY_ID = "0";
const char* M2M_SERVER_ID = "1";
const char* M2M_ACCESS_CONTROL_ID = "2";
const char* M2M_DEVICE_ID = "3";

const char* M2M_CONNECTIVITY_MONITOR_ID = "4";
const char* M2M_FIRMWARE_ID = "5";
const char* M2M_LOCATION_ID = "6";
const char* M2M_CONNECTIVITY_STATISTICS_ID = "7";
const char* RESERVED_ID = "8";

//OMA RESOURCE TYPE
const char* OMA_RESOURCE_TYPE = ""; //oma.lwm2m

//DEVICE RESOURCES
const char* DEVICE_MANUFACTURER = "0";
const char* DEVICE_DEVICE_TYPE = "17";
const char* DEVICE_MODEL_NUMBER = "1";
const char* DEVICE_SERIAL_NUMBER = "2";
const char* DEVICE_HARDWARE_VERSION = "18";
const char* DEVICE_FIRMWARE_VERSION = "3";
const char* DEVICE_SOFTWARE_VERSION = "19";
const char* DEVICE_REBOOT = "4";
const char* DEVICE_FACTORY_RESET = "5";
const char* DEVICE_AVAILABLE_POWER_SOURCES = "6";
const char* DEVICE_POWER_SOURCE_VOLTAGE = "7";
const char* DEVICE_POWER_SOURCE_CURRENT = "8";
const char* DEVICE_BATTERY_LEVEL = "9";
const char* DEVICE_BATTERY_STATUS = "20";
const char* DEVICE_MEMORY_FREE = "10";
const char* DEVICE_MEMORY_TOTAL = "21";
const char* DEVICE_ERROR_CODE = "11";
const char* DEVICE_RESET_ERROR_CODE = "12";
const char* DEVICE_CURRENT_TIME = "13";
const char* DEVICE_UTC_OFFSET = "14";
const char* DEVICE_TIMEZONE = "15";
const char* DEVICE_SUPPORTED_BINDING_MODE = "16";

const char* BINDING_MODE_UDP = "U";
const char* BINDING_MODE_UDP_QUEUE = "UQ";
const char* BINDING_MODE_SMS = "S";
const char* BINDING_MODE_SMS_QUEUE = "SQ";

const char* ERROR_CODE_VALUE = "0";



//SECURITY RESOURCES
const char* SECURITY_M2M_SERVER_URI = "0";
const char* SECURITY_BOOTSTRAP_SERVER = "1";
const char* SECURITY_SECURITY_MODE = "2";
const char* SECURITY_PUBLIC_KEY = "3";
const char* SECURITY_SERVER_PUBLIC_KEY = "4";
const char* SECURITY_SECRET_KEY = "5";
const char* SECURITY_SMS_SECURITY_MODE = "6";
const char* SECURITY_SMS_BINDING_KEY = "7";
const char* SECURITY_SMS_BINDING_SECRET_KEY = "8";
const char* SECURITY_M2M_SERVER_SMS_NUMBER = "9";
const char* SECURITY_SHORT_SERVER_ID = "10";
const char* SECURITY_CLIENT_HOLD_OFF_TIME = "11";

//SERVER RESOURCES
const char* SERVER_SHORT_SERVER_ID = "0";
const char* SERVER_LIFETIME = "1";
const char* SERVER_DEFAULT_MIN_PERIOD = "2";
const char* SERVER_DEFAULT_MAX_PERIOD = "3";
const char* SERVER_DISABLE = "4";
const char* SERVER_DISABLE_TIMEOUT = "5";
const char* SERVER_NOTIFICATION_STORAGE = "6";
const char* SERVER_BINDING = "7";
const char* SERVER_REGISTRATION_UPDATE = "8";

//FIRMWARE RESOURCES
const char* FIRMWARE_PACKAGE = "0";
const char* FIRMWARE_PACKAGE_URI = "1";
const char* FIRMWARE_UPDATE = "2";
const char* FIRMWARE_STATE = "3";
const char* FIRMWARE_UPDATE_SUPPORTED_OBJECTS = "4";
const char* FIRMWARE_UPDATE_RESULT = "5";
const char* FIRMWARE_PACKAGE_NAME = "6";
const char* FIRMWARE_PACKAGE_VERSION = "7";
