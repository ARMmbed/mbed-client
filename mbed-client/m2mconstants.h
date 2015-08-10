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
const int BUFFER_LENGTH = 1024;
extern const String COAP;
const int32_t MINIMUM_REGISTRATION_TIME = 60; //in seconds
const uint64_t ONE_SECOND_TIMER = 1;
const uint16_t ENDPOINT_LENGTH = 64;
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
extern const String EQUAL;
extern const String AMP;
extern const String PMIN;
extern const String PMAX;
extern const String GT;
extern const String LT;
extern const String ST;
extern const String CANCEL;

//LWM2MOBJECT NAME/ID
extern const String M2M_SECURITY_ID;
extern const String M2M_SERVER_ID;
extern const String M2M_ACCESS_CONTROL_ID;
extern const String M2M_DEVICE_ID;

extern const String M2M_CONNECTIVITY_MONITOR_ID;
extern const String M2M_FIRMWARE_ID;
extern const String M2M_LOCATION_ID;
extern const String M2M_CONNECTIVITY_STATISTICS_ID;
extern const String RESERVED_ID;

//OBJECT RESOURCE TYPE
extern const String OMA_RESOURCE_TYPE;

//DEVICE RESOURCES
extern const String DEVICE_MANUFACTURER;
extern const String DEVICE_DEVICE_TYPE;
extern const String DEVICE_MODEL_NUMBER;
extern const String DEVICE_SERIAL_NUMBER;
extern const String DEVICE_HARDWARE_VERSION;
extern const String DEVICE_FIRMWARE_VERSION;
extern const String DEVICE_SOFTWARE_VERSION;
extern const String DEVICE_REBOOT;
extern const String DEVICE_FACTORY_RESET;
extern const String DEVICE_AVAILABLE_POWER_SOURCES;
extern const String DEVICE_POWER_SOURCE_VOLTAGE;
extern const String DEVICE_POWER_SOURCE_CURRENT;
extern const String DEVICE_BATTERY_LEVEL;
extern const String DEVICE_BATTERY_STATUS;
extern const String DEVICE_MEMORY_FREE;
extern const String DEVICE_MEMORY_TOTAL;
extern const String DEVICE_ERROR_CODE;
extern const String DEVICE_RESET_ERROR_CODE;
extern const String DEVICE_CURRENT_TIME;
extern const String DEVICE_UTC_OFFSET;
extern const String DEVICE_TIMEZONE;
extern const String DEVICE_SUPPORTED_BINDING_MODE;

extern const String BINDING_MODE_UDP;
extern const String BINDING_MODE_UDP_QUEUE;
extern const String BINDING_MODE_SMS;
extern const String BINDING_MODE_SMS_QUEUE;

extern const String ERROR_CODE_VALUE;



//SECURITY RESOURCES
extern const String SECURITY_M2M_SERVER_URI;
extern const String SECURITY_BOOTSTRAP_SERVER;
extern const String SECURITY_SECURITY_MODE;
extern const String SECURITY_PUBLIC_KEY;
extern const String SECURITY_SERVER_PUBLIC_KEY;
extern const String SECURITY_SECRET_KEY;
extern const String SECURITY_SMS_SECURITY_MODE;
extern const String SECURITY_SMS_BINDING_KEY;
extern const String SECURITY_SMS_BINDING_SECRET_KEY;
extern const String SECURITY_M2M_SERVER_SMS_NUMBER;
extern const String SECURITY_SHORT_SERVER_ID;
extern const String SECURITY_CLIENT_HOLD_OFF_TIME;

//SERVER RESOURCES
extern const String SERVER_SHORT_SERVER_ID;
extern const String SERVER_LIFETIME;
extern const String SERVER_DEFAULT_MIN_PERIOD;
extern const String SERVER_DEFAULT_MAX_PERIOD;
extern const String SERVER_DISABLE;
extern const String SERVER_DISABLE_TIMEOUT;
extern const String SERVER_NOTIFICATION_STORAGE;
extern const String SERVER_BINDING;
extern const String SERVER_REGISTRATION_UPDATE;

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

const int COAP_CONTENT_OMA_TLV_TYPE = 99;
const int COAP_CONTENT_OMA_JSON_TYPE = 100;

#endif // M2MCONSTANTS_H
