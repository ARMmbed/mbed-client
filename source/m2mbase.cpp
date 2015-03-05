#include "lwm2m-client/m2mbase.h"
#include "lwm2m-client/m2mobservationhandler.h"
#include "lwm2m-client/m2mconstants.h"
#include "include/m2mtimer.h"

M2MBase& M2MBase::operator=(const M2MBase& other)
{
    if (this != &other) { // protect against invalid self-assignment
        _operation = other._operation;
        _mode = other._mode;
        _name = other._name;
        _resource_type = other._resource_type;
        _interface_description = other._interface_description;
        _coap_content_type = other._coap_content_type;
        _instance_id = other._instance_id;
        _observable = other._observable;
        _observation_handler = other._observation_handler;
        _observation_number = other._observation_number;
        _pmax = other._pmax;
        _pmin = other._pmin;
        _pmin_exceeded = other._pmin_exceeded;
        _pmax_exceeded = other._pmax_exceeded;
        _report_scheduled = other._report_scheduled;
        if(other._pmin_timer) {
            _pmin_timer = new M2MTimer(*other._pmin_timer);
        }
        if(other._pmax_timer) {
            _pmax_timer = new M2MTimer(*other._pmax_timer);
        }
        _value_length = other._value_length;
        if(other._value) {
            _value = (uint8_t *)malloc(other._value_length);
            memcpy((uint8_t *)_value, (uint8_t *)other._value, other._value_length);
        }
    }
    return *this;
}

M2MBase::M2MBase(const M2MBase& other)
{
    *this = other;
}

M2MBase::M2MBase(const String & resource_name,
                 M2MBase::Mode mde)
: _operation(M2MBase::NOT_ALLOWED),
  _mode(mde),
  _observation_handler(NULL),
  _name(resource_name),
  _instance_id(0),
  _observable(false),
  _under_observation(false),
  _observation_number(0),
  _pmax(0.0f),
  _pmin(0.0f),
  _pmin_exceeded(false),
  _pmax_exceeded(false),
  _report_scheduled(false),
  _pmin_timer(new M2MTimer(*this)),
  _pmax_timer(new M2MTimer(*this)),
  _value(NULL),
  _value_length(0)
{ 
}

M2MBase::~M2MBase()
{
    if(_value) {
        free(_value);
        _value = NULL;
    }
    if(_observation_handler) {
        _observation_handler->resource_to_be_deleted(*this);
    }
    if(_pmax_timer) {
        delete _pmax_timer;
    }
    if(_pmin_timer) {
    delete _pmin_timer;
    }
}

void M2MBase::set_operation(M2MBase::Operation opr)
{
    // If the mode is Static, there is only GET_ALLOWED
   // supported.
    if(M2MBase::Static == _mode) {
            _operation = M2MBase::GET_ALLOWED;
    } else {
        _operation = opr;
    }
}

void M2MBase::set_interface_description(const String &desc)
{
    _interface_description = desc;
}

void M2MBase::set_resource_type(const String &res_type)
{
    _resource_type = res_type;
}

void M2MBase::set_coap_content_type(const uint8_t con_type)
{
    _coap_content_type = con_type;
}

void M2MBase::set_observable(bool observable)
{
   _observable = observable;
}

void M2MBase::set_under_observation(bool observed,
                                    M2MObservationHandler *handler)
{
    _observation_handler = handler;
    _under_observation = observed;
    _report_scheduled = false;
    if(_under_observation) {
        report();
    }
}

void M2MBase::set_observation_token(const String &token)
{
    _token = token;
}

void M2MBase::set_instance_id(const uint16_t inst_id)
{
    _instance_id = inst_id;
}

bool M2MBase::set_value(const uint8_t *value, const uint32_t value_length)
{
    bool success = false;
    if(_value) {
         free(_value);
         _value = NULL;
         _value_length = 0;
    }

    if( value != NULL && value_length > 0 ) {
        success = true;
       _value = (uint8_t *)malloc(value_length);
        memcpy((uint8_t *)_value, (uint8_t *)value, value_length);
        _value_length = value_length;
        // schedule reporting
        schedule_report();
    }
    return success;
}

M2MBase::Operation M2MBase::operation() const
{
    return _operation;
}

const String& M2MBase::name() const
{
    return _name;
}

uint16_t M2MBase::instance_id() const
{
    return _instance_id;
}

const String& M2MBase::interface_description() const
{
    return _interface_description;
}

const String& M2MBase::resource_type() const
{
    return _resource_type;
}

uint8_t M2MBase::coap_content_type() const
{
    return _coap_content_type;
}

bool M2MBase::is_observable() const
{
    return _observable;
}

void M2MBase::get_value(uint8_t *&value, uint32_t &value_length)
{
    value_length = 0;
    if(value) {
        free(value);
        value = NULL;
    }
    value = (uint8_t *)malloc(_value_length);
    value_length = _value_length;
    memcpy((uint8_t *)value, (uint8_t *)_value, value_length);
}

bool M2MBase::is_under_observation() const
{
    return _under_observation;
}

const String& M2MBase::observation_token() const
{
    return _token;
}

M2MBase::Mode M2MBase::mode() const
{
    return _mode;
}

uint8_t M2MBase::observation_number() const
{
    return _observation_number;
}

bool M2MBase::parse_notification_attribute(char *&query)
{
    bool success = false;
    char *query_option = strtok(query, "&");// split the string
    char query_options[5][20];
    uint8_t num_options = 0;
    while (query_option != NULL){
        strcpy(query_options[num_options++], query_option);
        query_option = strtok(NULL, "&");// next query option
    }
    for (int option = 0; option < num_options; option++) {
        if(set_notification_attribute(query_options[option])) {
            success = true;
        }
    }
    if(success) {
        // initializes and sends an update if observing is on, don't change observing state
        // allows cancel to turn off observing and updte state without sending a notification
        _report_scheduled = true;
        report();
    }
    return success;
}

void M2MBase::timer_expired(M2MTimerObserver::Type type)
{
    switch(type) {
        case M2MTimerObserver::PMinTimer: {
            if (_report_scheduled){
                report();
            }
            else{
                _pmin_exceeded = true;
            }
        }
        break;
        case M2MTimerObserver::PMaxTimer: {
            _pmax_exceeded = true;
            report();
        }
        break;
        default:
            break;
    }
}

bool M2MBase::set_notification_attribute(char* option)
{
    bool success = false;
    char* attribute = strtok(option, EQUAL.c_str()); // first token
    char* value = strtok(NULL, EQUAL.c_str()); // next token

    if (strcmp(attribute, PMIN.c_str()) == 0) {
        sscanf(value, "%f", &_pmin);
        success = true;
    }
    else if(strcmp(attribute, PMAX.c_str()) == 0) {
        sscanf(value, "%f", &_pmax);
        success = true;
    }
    else if(strcmp(attribute, CANCEL.c_str()) == 0) {
        set_under_observation(false,NULL);
        success = true;
    }
    return success;
}

void M2MBase::report()
{
    uint64_t time_interval = (uint64_t)(_pmin * 1000);
    _pmin_timer->stop_timer();
    _pmin_timer->start_timer(time_interval,
                             M2MTimerObserver::PMinTimer,
                             true);

    time_interval = (uint64_t)(_pmax * 1000);
    _pmax_timer->stop_timer();
    _pmax_timer->start_timer(time_interval,
                             M2MTimerObserver::PMaxTimer,
                             true);

    if(_under_observation) {
        if (_pmax_exceeded) {
            _pmax_exceeded = false;
        }
        if(_pmin_exceeded) {
            _pmin_exceeded = false;
        }
        _observation_number++;
        if(_observation_handler && _report_scheduled) {
            _report_scheduled = false;
            _observation_handler->observation_to_be_sent(this);
        }
    }
}

void M2MBase::schedule_report()
{
    if(_under_observation) {
        _report_scheduled = true;
        if (_pmin_exceeded || _pmax_exceeded) {
            report();
        }
    }
}
