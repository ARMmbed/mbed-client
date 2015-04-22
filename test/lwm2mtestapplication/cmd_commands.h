#ifndef CMD_COMMANDS_H_
#define CMD_COMMANDS_H_

#include <pthread.h>

#define EVENT_TYPE_CMDLINE          5
#define EVENT_CMDLINE_CONSOLE       5

void initialize_app_commands(int8_t rf_driver_id);

#endif
