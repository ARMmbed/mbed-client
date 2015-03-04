#ifndef M2M_STATE_MACHINE_OBSERVER_H__
#define M2M_STATE_MACHINE_OBSERVER_H__

/**
 * @brief Observer class for informing State changes to M2MInterfaceImpl.
 */

class M2MStateMachineObserver
{
public :

    /**
     * @brief Inform the state is done.
     */
    virtual void done() = 0;

};

#endif // M2M_STATE_MACHINE_OBSERVER_H__

