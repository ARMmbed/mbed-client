/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef M2M_DEVICE_H
#define M2M_DEVICE_H

#include "m2mobject.h"

// FORWARD DECLARATION
class M2MResource;

/**
 *  @brief M2MDevice.
 *  This class represents the Device Object model of LWM2M framework.
 *  This class will provides interface for handling the device object
 *  and all its corresponding resources.There can be only one instance
 *  of Device Object.
 */
class  M2MDevice : public M2MObject {

friend class M2MInterfaceFactory;

public:

     /**
      * @brief Enum defining all the resources associdated with
      * Device Object in LWM2M framework.
      */
    typedef enum {
        Manufacturer,
        DeviceType,
        ModelNumber,
        SerialNumber,
        HardwareVersion,
        FirmwareVersion,
        SoftwareVersion,
        Reboot,
        FactoryReset,
        AvailablePowerSources,
        PowerSourceVoltage,
        PowerSourceCurrent,
        BatteryLevel,
        BatteryStatus,
        MemoryFree,
        MemoryTotal,
        ErrorCode,
        ResetErrorCode,
        CurrentTime,
        UTCOffset,
        Timezone,
        SupportedBindingMode
    }DeviceResource;

private:

    /**
     * Constructor
     */
    M2MDevice();

    // Prevents the use of assignment operator.
    M2MDevice& operator=( const M2MDevice& /*other*/ );

    // Prevents the use of copy constructor
    M2MDevice( const M2MDevice& /*other*/ );

public:

    /**
     * Destructor
     */
    virtual ~M2MDevice();

    /**
     * @brief Creates a new resource for given resource enum.
     * @param resource, List of resource names which can be created using this function are
     * 'Manufacturer', 'DeviceType','ModelNumber','SerialNumber',
     * 'HardwareVersion', 'FirmwareVersion', 'SoftwareVersion',
     *  'UTCOffset', 'Timezone', 'SupportedBindingMode'.
     * @param value, Value to be set on the resource, in String format
     * @return M2MResource if created successfully else NULL.
     */
    M2MResource* create_resource(DeviceResource resource, const String &value);

    /**
     * @brief Creates a new resource for given resource enum.
     * @param resource, List of resource names which can be created using this function are
     * 'AvailablePowerSources','PowerSourceVoltage','PowerSourceCurrent',
     * 'BatteryLevel', 'BatteryStatus', 'MemoryFree', 'MemoryTotal',
     * 'ErrorCode', 'CurrentTime'.
     * @param value, Value to be set on the resource, in Integer format
     * @return M2MResource if created successfully else NULL.
     */
    M2MResource* create_resource(DeviceResource resource, uint32_t value);

    /**
     * @brief Creates a new resource for given resource name.
     * @param resource, List of resource names which can be created using this function are
     * 'ResetErrorCode','FactoryReset'.
     * @return M2MResource if created successfully else NULL.
     */
    M2MResource* create_resource(DeviceResource resource);

    /**
     * @brief Deletes the resource with the given resource enum.
     * It cannot not delete the mandatory resources.
     * @param resource, Name of the resource to be deleted.
     * @param instance_id, Instance Id of the resource, default is 0
     * @return True if deleted else false.
     */
    bool delete_resource(DeviceResource resource,
                         uint16_t instance_id = 0);

    /**
     * @brief Sets the value of the given resource enum.
     * @param resource, List of resource name for which value can be set
     * using this function are
     * 'Manufacturer', 'DeviceType','ModelNumber','SerialNumber',
     * 'HardwareVersion', 'FirmwareVersion', 'SoftwareVersion',
     *  'UTCOffset', 'Timezone', 'SupportedBindingMode'.
     * @param value, Value to be set on the key, in String format
     * @param instance_id, Instance Id of the resource, default is 0
     * @return True if successfully set else false.
     */
    bool set_resource_value(DeviceResource resource,
                            const String &value,
                            uint16_t instance_id = 0);

    /**
     * @brief Sets the value of the given resource enum.
     * @param resource, List of resource name for which value can be set
     * using this function are
     * 'AvailablePowerSources','PowerSourceVoltage','PowerSourceCurrent',
     * 'BatteryLevel', 'BatteryStatus', 'MemoryFree', 'MemoryTotal',
     * 'ErrorCode', 'CurrentTime'.
     * @param value, Value to be set on the key, in Integer format
     * @param instance_id, Instance Id of the resource, default is 0
     * @return True if successfully set else false.
     */
    bool set_resource_value(DeviceResource resource,
                            uint32_t value,
                            uint16_t instance_id = 0);

    /**
     * @brief Returns the value of the given resource enum, in String
     * @param resource, List of resource names which can return value using this function are
     * 'Manufacturer', 'DeviceType','ModelNumber','SerialNumber',
     * 'HardwareVersion', 'FirmwareVersion', 'SoftwareVersion',
     *  'UTCOffset', 'Timezone', 'SupportedBindingMode'.
     * @param instance_id, Instance Id of the resource, default is 0
     * @return Value associated with that key, if key is not valid it returns NULL.
     */
    String resource_value_string(DeviceResource resource,
                                 uint16_t instance_id = 0) const;

    /**
     * @brief Returns the value of the given resource key name, in Integer
     * @param resource, List of resource names which can return value using this function are
     * 'AvailablePowerSources','PowerSourceVoltage','PowerSourceCurrent',
     * 'BatteryLevel', 'BatteryStatus', 'MemoryFree', 'MemoryTotal',
     * 'ErrorCode', 'CurrentTime'.
     * @param instance_id, Instance Id of the resource, default is 0
     * @return Value associated with that key, if key is not valid it returns -1.
     */
    uint32_t resource_value_int(DeviceResource resource,
                               uint16_t instance_id = 0) const;

    /**
     * @brief Returns if the resource instance with given resource enum exists or not
     * @param resource, resource enum.
     * @return True if at least one instance exists else false.
     */
    bool is_resource_present(DeviceResource resource)const;

    /**
     * @brief Returns number of resources for whole device object
     * @return Total umber of resources belonging to device object.
     */
    uint16_t total_resource_count()const;

    /**
     * @brief Returns number of resources for given resource enum
     * @param resource, resource enum.
     * @return Number of resources for a given resource enum returns 1 for
     * mandatory resources else can be 0 as well if no instance exists for an
     * optional resource.
     */
    uint16_t per_resource_count(DeviceResource resource)const;

private:

    M2MResource* get_resource(DeviceResource res,
                              uint16_t instance_id = 0) const;

    String resource_name(DeviceResource resource) const;

private :

    M2MObjectInstance*    _device_instance;

    friend class Test_M2MDevice;
};

#endif // M2M_DEVICE_H

