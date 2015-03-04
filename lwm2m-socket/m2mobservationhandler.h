#ifndef M2M_OBSERVATION_HANDLER_H
#define M2M_OBSERVATION_HANDLER_H


//FORWARD DECLARATION
class M2MBase;

/**
 * @brief An interface to handle the observation
 * callbacks from different objects.
 *
 */
class M2MObservationHandler
{
  public:

    /**
     * @brief Observation callback to be sent to the
     * server because some of the observed parameter has
     * changed.
     * @param object, Observed object whose information
     * has to be sent.
     */
    virtual void observation_to_be_sent(M2MBase *object) = 0;

    /**
     * @brief NSDL resource needs to be deleted.
     * @param object, Reference of the observed object whose information
     * needs to be deleted.
     */
    virtual void resource_to_be_deleted(const M2MBase &object) = 0;

};


#endif // M2M_OBSERVATION_HANDLER_H
