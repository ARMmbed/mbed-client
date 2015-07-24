#include <string.h>
#include <stdio.h>
#include <stdarg.h>

//stack
#include "eventOS_event.h"
#include "eventOS_scheduler.h"

//mbed-client-libservice
#include "randLIB.h"
#include "nsdynmemLIB.h"
#include "ns_trace.h"
#include "ns_cmdline.h"


//application
#include "cmd_commands.h"

#define APP_DEV_HEAP_SIZE 30000

void eventOS_scheduler_idle(void)
{
    eventOS_scheduler_wait();
}

/*Global variables*/

uint8_t app_defined_stack_heap[APP_DEV_HEAP_SIZE];

void app_heap_error_handler(heap_fail_t event)
{
    switch (event)
    {
        case NS_DYN_MEM_NULL_FREE:
            tracef(TRACE_LEVEL_ERROR, "mem", "Dyn mem error:NULL_FREE");
            break;
        case NS_DYN_MEM_DOUBLE_FREE:
            tracef(TRACE_LEVEL_ERROR, "mem", "Dyn mem error:DOUBLE_FREE");
            break;

        case NS_DYN_MEM_ALLOCATE_SIZE_NOT_VALID:
            tracef(TRACE_LEVEL_ERROR, "mem", "Dyn mem error:SIZE_NOT_VALID");
            break;
        case NS_DYN_MEM_POINTER_NOT_VALID:
            tracef(TRACE_LEVEL_ERROR, "mem", "Dyn mem error:POINTER_NOT_VALID");
            break;

        case NS_DYN_MEM_HEAP_SECTOR_CORRUPTED:
            tracef(TRACE_LEVEL_ERROR, "mem", "Dyn mem error:SECTOR_CORRUPTED");
            break;

        case NS_DYN_MEM_HEAP_SECTOR_UNITIALIZED:
            tracef(TRACE_LEVEL_ERROR, "mem", "Dyn mem error:SECTOR_UNITIALIZED");
            break;
        default:
      tracef(TRACE_LEVEL_ERROR, "mem", "Dyn mem error:UNKNOWN!");
            break;
    }
    while(1);
}

int app_time_i=0;
char* time_now(size_t size)
{
  static char str[10] = {0};
  sprintf(str, "[%04d]", app_time_i++);
  return str;
}

void trace_printer(const char* str)
{
  printf("%s\r\n", str);
  cmd_output();
  fflush(stdout);
}
void cmd_printer(const char *str)
{
  cmd_printf("%s", str);
  fflush(stdout);
}

void custom_cmd_response_out(const char* fmt, va_list ap)
{
  vprintf(fmt, ap);
  fflush(stdout);
}

mem_stat_t  memory_heap_stat;
/**
 * \brief Application infinite loop.
 */
int main(void)
{
    ns_dyn_mem_init(app_defined_stack_heap, APP_DEV_HEAP_SIZE, app_heap_error_handler, &memory_heap_stat);
    eventOS_scheduler_init();
    trace_init();
    set_trace_print_function( trace_printer );
    set_trace_cmdprint_function( cmd_printer );
    set_trace_prefix_function( time_now );
    set_trace_config(TRACE_MODE_COLOR|TRACE_ACTIVE_LEVEL_DEBUG|TRACE_CARRIAGE_RETURN);
    cmd_init( &custom_cmd_response_out );
    initialize_app_commands(0);
    eventOS_scheduler_run();
    return 0;
}
