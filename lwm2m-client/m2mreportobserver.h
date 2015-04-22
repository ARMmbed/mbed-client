#ifndef M2MREPORTOBSERVER_H
#define M2MREPORTOBSERVER_H

/**
 * @brief An interface to report to the base class
 * when to send the report to the server.
 *
 */
class M2MReportObserver
{
  public:

    /**
     * @brief Observation callback to be sent to the
     * server because some of the observed parameter has
     * changed.
     */
    virtual void observation_to_be_sent() = 0;

};

#endif // M2MREPORTOBSERVER_H
