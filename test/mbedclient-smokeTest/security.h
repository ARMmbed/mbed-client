

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

#ifndef __SECURITY_H__

#define __SECURITY_H__

 

#include <inttypes.h>

 

#define MBED_DOMAIN "eeforester"

#define MBED_ENDPOINT_NAME "3a2d81ae-14f6-48e1-8545-857db0d76631"

 

const unsigned char PSK[] = {0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x30,0x31,0x32,0x33,0x34,0x35,0x36};

const unsigned char PSK_IDENTITY[] = {0x0F,0x0F};

 

const uint8_t SERVER_CERT[] = "-----BEGIN CERTIFICATE-----\r\n"

"MIIBmDCCAT6gAwIBAgIEVUCA0jAKBggqhkjOPQQDAjBLMQswCQYDVQQGEwJGSTEN\r\n"

"MAsGA1UEBwwET3VsdTEMMAoGA1UECgwDQVJNMQwwCgYDVQQLDANJb1QxETAPBgNV\r\n"

"BAMMCEFSTSBtYmVkMB4XDTE1MDQyOTA2NTc0OFoXDTE4MDQyOTA2NTc0OFowSzEL\r\n"

"MAkGA1UEBhMCRkkxDTALBgNVBAcMBE91bHUxDDAKBgNVBAoMA0FSTTEMMAoGA1UE\r\n"

"CwwDSW9UMREwDwYDVQQDDAhBUk0gbWJlZDBZMBMGByqGSM49AgEGCCqGSM49AwEH\r\n"

"A0IABLuAyLSk0mA3awgFR5mw2RHth47tRUO44q/RdzFZnLsAsd18Esxd5LCpcT9w\r\n"

"0tvNfBv4xJxGw0wcYrPDDb8/rjujEDAOMAwGA1UdEwQFMAMBAf8wCgYIKoZIzj0E\r\n"

"AwIDSAAwRQIhAPAonEAkwixlJiyYRQQWpXtkMZax+VlEiS201BG0PpAzAiBh2RsD\r\n"

"NxLKWwf4O7D6JasGBYf9+ZLwl0iaRjTjytO+Kw==\r\n"

"-----END CERTIFICATE-----\r\n";

 

const uint8_t CERT[] = "-----BEGIN CERTIFICATE-----\r\n"

"MIIBizCCATCgAwIBAgIEQnSlKTAMBggqhkjOPQQDAgUAMBAxDjAMBgNVBAMTBW5z\r\n"

"cDAxMB4XDTE1MDgyNDA1NTIyNloXDTE2MDgyMzA1NTIyNlowgYcxODA2BgNVBAMT\r\n"

"LzNhMmQ4MWFlLTE0ZjYtNDhlMS04NTQ1LTg1N2RiMGQ3NjYzMS9lZWZvcmVzdGVy\r\n"

"MQwwCgYDVQQLEwNBUk0xEjAQBgNVBAoTCW1iZWQgdXNlcjENMAsGA1UEBxMET3Vs\r\n"

"dTENMAsGA1UECBMET3VsdTELMAkGA1UEBhMCRkkwWTATBgcqhkjOPQIBBggqhkjO\r\n"

"PQMBBwNCAATt6HjQ4w0k7r7rtIFrr3LM7W/xKNmZEdBQJx2O/MAjTaKSgfetFmdb\r\n"

"ykoTycVu7Kf/egDt5eM/rcP2tfI6cmO7MAwGCCqGSM49BAMCBQADRwAwRAIgPsE/\r\n"

"89DVaM6zmMIJxT9xtakGthSCoHKWsqXGh9GjG70CIHmP3S1xhW+LchpEPYLkqFSe\r\n"

"rG99o6C/ieT5ax2zPr27\r\n"

"-----END CERTIFICATE-----\r\n";

 

const uint8_t KEY[] = "-----BEGIN PRIVATE KEY-----\r\n"

"MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQgMTR1KaMFLa41mR8D\r\n"

"F7mQ2lzd5rdu3pfUEkJHk+BtQzChRANCAATt6HjQ4w0k7r7rtIFrr3LM7W/xKNmZ\r\n"

"EdBQJx2O/MAjTaKSgfetFmdbykoTycVu7Kf/egDt5eM/rcP2tfI6cmO7\r\n"

"-----END PRIVATE KEY-----\r\n";

 

#endif //__SECURITY_H__
