#ifndef M2M_TIMER_IMPL_LINUX_H
#define M2M_TIMER_IMPL_LINUX_H

#include <pthread.h>
#include <stdint.h>

class M2MTimerObserver;
/**
 * @brief M2MTimerImpl
 * Private implementation class for timer, this can be
 * modified based on platform on which mbed client needs
 * to be used.
 */
class M2MTimerImpl
{
public:

    // Prevents the use of assignment operator
    M2MTimerImpl& operator=(const M2MTimerImpl& other);

    // Prevents the use of copy constructor
    M2MTimerImpl(const M2MTimerImpl& other);

    /**
    * Constructor.
    */
    M2MTimerImpl(M2MTimerObserver& _observer);

    /**
    * Destructor.
    */
    virtual ~M2MTimerImpl();

    /**
     * Starts timer
     * @param interval Timer's interval in milliseconds
    * @param single_shot defines if timer is ticked
    * once or is it restarted everytime timer is expired.
    */
    void start_timer(uint64_t interval, bool single_shot = true);

    /**
    * Stops timer.
    * This cancels the ongoing timer.
    */
    void stop_timer();

    /**
    * Callback function for timer completion.
    */
    void timer_expired();

    /**
    * Callback function for thread handling.
    */
    void thread_function();

private:

    M2MTimerObserver&   _observer;
    bool                _single_shot;
    uint64_t            _interval;
    uint64_t            _counter;
    pthread_t           _coap_exec_thread; /* Thread for coap_exec-function */

};

#endif // M2M_TIMER_IMPL_LINUX_H
