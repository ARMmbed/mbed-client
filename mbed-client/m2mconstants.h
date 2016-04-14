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
#ifndef M2MCONSTANTS_H
#define M2MCONSTANTS_H

#include <stdint.h>
#include "m2mconfig.h"

const int MAX_VALUE_LENGTH = 256;
const int BUFFER_LENGTH = 1152;
extern const char* COAP;
extern const char* COAPS;
const int32_t MINIMUM_REGISTRATION_TIME = 60; //in seconds
const uint64_t ONE_SECOND_TIMER = 1;
const uint16_t MAX_ALLOWED_STRING_LENGTH = 64;
const uint16_t OPTIMUM_LIFETIME = 3600;
const uint16_t REDUCE_LIFETIME = 900;
const float REDUCTION_FACTOR = 0.75f;

const int RETRY_COUNT = 2;
const int RETRY_INTERVAL = 5;

// values per: draft-ietf-core-observe-16
// OMA LWM2M CR ref.
#define START_OBSERVATION 0
#define STOP_OBSERVATION 1

// PUT attributes to be checked from server
extern const char* EQUAL;
extern const char* AMP;
extern const char* PMIN;
extern const char* PMAX;
extern const char* GT;
extern const char* LT;
extern const char* ST;
extern const char* STP;
extern const char* CANCEL;

//LWM2MOBJECT NAME/ID
extern const char* M2M_SECURITY_ID;
extern const char* M2M_SERVER_ID;
extern const char* M2M_ACCESS_CONTROL_ID;
extern const char* M2M_DEVICE_ID;

extern const char* M2M_CONNECTIVITY_MONITOR_ID;
extern const char* M2M_FIRMWARE_ID;
extern const char* M2M_LOCATION_ID;
extern const char* M2M_CONNECTIVITY_STATISTICS_ID;
extern const char* RESERVED_ID;

//OBJECT RESOURCE TYPE
extern const char* OMA_RESOURCE_TYPE;

//DEVICE RESOURCES
extern const char* DEVICE_MANUFACTURER;
extern const char* DEVICE_DEVICE_TYPE;
extern const char* DEVICE_MODEL_NUMBER;
extern const char* DEVICE_SERIAL_NUMBER;
extern const char* DEVICE_HARDWARE_VERSION;
extern const char* DEVICE_FIRMWARE_VERSION;
extern const char* DEVICE_SOFTWARE_VERSION;
extern const char* DEVICE_REBOOT;
extern const char* DEVICE_FACTORY_RESET;
extern const char* DEVICE_AVAILABLE_POWER_SOURCES;
extern const char* DEVICE_POWER_SOURCE_VOLTAGE;
extern const char* DEVICE_POWER_SOURCE_CURRENT;
extern const char* DEVICE_BATTERY_LEVEL;
extern const char* DEVICE_BATTERY_STATUS;
extern const char* DEVICE_MEMORY_FREE;
extern const char* DEVICE_MEMORY_TOTAL;
extern const char* DEVICE_ERROR_CODE;
extern const char* DEVICE_RESET_ERROR_CODE;
extern const char* DEVICE_CURRENT_TIME;
extern const char* DEVICE_UTC_OFFSET;
extern const char* DEVICE_TIMEZONE;
extern const char* DEVICE_SUPPORTED_BINDING_MODE;

extern const char* BINDING_MODE_UDP;
extern const char* BINDING_MODE_UDP_QUEUE;
extern const char* BINDING_MODE_SMS;
extern const char* BINDING_MODE_SMS_QUEUE;

extern const char* ERROR_CODE_VALUE;



//SECURITY RESOURCES
extern const char* SECURITY_M2M_SERVER_URI;
extern const char* SECURITY_BOOTSTRAP_SERVER;
extern const char* SECURITY_SECURITY_MODE;
extern const char* SECURITY_PUBLIC_KEY;
extern const char* SECURITY_SERVER_PUBLIC_KEY;
extern const char* SECURITY_SECRET_KEY;
extern const char* SECURITY_SMS_SECURITY_MODE;
extern const char* SECURITY_SMS_BINDING_KEY;
extern const char* SECURITY_SMS_BINDING_SECRET_KEY;
extern const char* SECURITY_M2M_SERVER_SMS_NUMBER;
extern const char* SECURITY_SHORT_SERVER_ID;
extern const char* SECURITY_CLIENT_HOLD_OFF_TIME;

//SERVER RESOURCES
extern const char* SERVER_SHORT_SERVER_ID;
extern const char* SERVER_LIFETIME;
extern const char* SERVER_DEFAULT_MIN_PERIOD;
extern const char* SERVER_DEFAULT_MAX_PERIOD;
extern const char* SERVER_DISABLE;
extern const char* SERVER_DISABLE_TIMEOUT;
extern const char* SERVER_NOTIFICATION_STORAGE;
extern const char* SERVER_BINDING;
extern const char* SERVER_REGISTRATION_UPDATE;

//FIRMWARE RESOURCES
extern const char* FIRMWARE_PACKAGE;
extern const char* FIRMWARE_PACKAGE_URI;
extern const char* FIRMWARE_UPDATE;
extern const char* FIRMWARE_STATE;
extern const char* FIRMWARE_UPDATE_SUPPORTED_OBJECTS;
extern const char* FIRMWARE_UPDATE_RESULT;
extern const char* FIRMWARE_PACKAGE_NAME;
extern const char* FIRMWARE_PACKAGE_VERSION;

// TLV serializer / deserializer
const uint8_t TYPE_RESOURCE = 0xC0;
const uint8_t TYPE_MULTIPLE_RESOURCE = 0x80;
const uint8_t TYPE_RESOURCE_INSTANCE = 0x40;
const uint8_t TYPE_OBJECT_INSTANCE = 0x0;

const uint8_t ID8 = 0x0;
const uint8_t ID16 = 0x20;

const uint8_t LENGTH8 = 0x08;
const uint8_t LENGTH16 = 0x10;
const uint8_t LENGTH24 = 0x18;

const uint8_t COAP_CONTENT_OMA_TLV_TYPE = 99;
const uint8_t COAP_CONTENT_OMA_JSON_TYPE = 100;
const uint8_t COAP_CONTENT_OMA_OPAQUE_TYPE = 42;

const uint16_t MAX_UNINT_16_COUNT = 65535;

#endif // M2MCONSTANTS_H
