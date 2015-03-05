#ifndef M2MCONSTANTS_H
#define M2MCONSTANTS_H

#include "m2mconfig.h"


const int MAX_VALUE_LENGTH = 256;
const int BUFFER_LENGTH = 1024;
const String COAP = "coap://";
const uint64_t MINIMUM_REGISTRATION_TIME = 60; //in seconds
const uint64_t ONE_SECOND_TIMER = 1;

// values per: draft-ietf-core-observe-16
// OMA LWM2M CR ref.
#define START_OBSERVATION 0
#define STOP_OBSERVATION 1

// PUT attributes to be checked from server
const String EQUAL = "=";
const String AMP = "&";
const String PMIN = "pmin";
const String PMAX = "pmax";
const String CANCEL = "cancel";


//LWM2MOBJECT NAME/ID
const String M2M_SECURITY_ID = "0";
const String M2M_SERVER_ID = "1";
const String M2M_ACCESS_CONTROL_ID = "2";
const String M2M_DEVICE_ID = "3";

const String M2M_CONNECTIVITY_MONITOR_ID = "4";
const String M2M_FIRMWARE_ID = "5";
const String M2M_LOCATION_ID = "6";
const String M2M_CONNECTIVITY_STATISTICS_ID = "7";
const String RESERVED_ID = "8";

//OBJECT RESOURCE TYPE
const String DEVICE_TYPE = "device_type";
const String SECURITY_TYPE = "security_type";
const String SERVER_TYPE = "server_type";

//DEVICE RESOURCES
const String DEVICE_MANUFACTURER = "0";
const String DEVICE_DEVICE_TYPE = "17";
const String DEVICE_MODEL_NUMBER = "1";
const String DEVICE_SERIAL_NUMBER = "2";
const String DEVICE_HARDWARE_VERSION = "18";
const String DEVICE_FIRMWARE_VERSION = "3";
const String DEVICE_SOFTWARE_VERSION = "19";
const String DEVICE_REBOOT = "4";
const String DEVICE_FACTORY_RESET = "5";
const String DEVICE_AVAILABLE_POWER_SOURCES = "6";
const String DEVICE_POWER_SOURCE_VOLTAGE = "7";
const String DEVICE_POWER_SOURCE_CURRENT = "8";
const String DEVICE_BATTERY_LEVEL = "9";
const String DEVICE_BATTERY_STATUS = "20";
const String DEVICE_MEMORY_FREE = "10";
const String DEVICE_MEMORY_TOTAL = "21";
const String DEVICE_ERROR_CODE = "11";
const String DEVICE_RESET_ERROR_CODE = "12";
const String DEVICE_CURRENT_TIME = "13";
const String DEVICE_UTC_OFFSET = "14";
const String DEVICE_TIMEZONE = "15";
const String DEVICE_SUPPORTED_BINDING_MODE = "16";

const String BINDING_MODE_UDP = "U";
const String BINDING_MODE_UDP_QUEUE = "UQ";
const String BINDING_MODE_SMS = "S";
const String BINDING_MODE_SMS_QUEUE = "SQ";

const String ERROR_CODE_VALUE = "0";



//SECURITY RESOURCES
const String SECURITY_M2M_SERVER_URI = "0";
const String SECURITY_BOOTSTRAP_SERVER = "1";
const String SECURITY_SECURITY_MODE = "2";
const String SECURITY_PUBLIC_KEY = "3";
const String SECURITY_SERVER_PUBLIC_KEY = "4";
const String SECURITY_SECRET_KEY = "5";
const String SECURITY_SMS_SECURITY_MODE = "6";
const String SECURITY_SMS_BINDING_KEY = "7";
const String SECURITY_SMS_BINDING_SECRET_KEY = "8";
const String SECURITY_M2M_SERVER_SMS_NUMBER = "9";
const String SECURITY_SHORT_SERVER_ID = "10";
const String SECURITY_CLIENT_HOLD_OFF_TIME = "11";

//SERVER RESOURCES
const String SERVER_SHORT_SERVER_ID= "0";
const String SERVER_LIFETIME = "1";
const String SERVER_DEFAULT_MIN_PERIOD = "2";
const String SERVER_DEFAULT_MAX_PERIOD = "3";
const String SERVER_DISABLE = "4";
const String SERVER_DISABLE_TIMEOUT = "5";
const String SERVER_NOTIFICATION_STORAGE = "6";
const String SERVER_BINDING = "7";
const String SERVER_REGISTRATION_UPDATE= "8";

#endif // M2MCONSTANTS_H

