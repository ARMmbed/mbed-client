#ifndef M2M_SINGLETON_H
#define M2M_SINGLETON_H

/**
 * Base class for singletons.
 *
 * This construction relies on the fact that function-static objects
 * are only initialized when the function is first being called upon;
 * thus, we maintain the benefit of dynamic initialization.
 *
 * To use this, inherit from Singleton<YourClass> and add
 * \code
 *  friend class Singleton<YourClass>;
 * \endcode
 * in the private part of your class.
 *
 * To call a method on the singleton class...
 * \code
 *  YourClass::GetInstance()->method();
 * \endcode
 */
template <class T>
class M2MSingleton {
public:
    /**
     * \return The singleton instance for this class.
     */
    static T *get_instance() {
        static T _instance;
        return &_instance;
    }
    
protected:
    M2MSingleton() {}
    virtual ~M2MSingleton() {}
};

#endif //M2M_SINGLETON_H

