/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef THREADING_THREAD_H_
#define THREADING_THREAD_H_

#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <utility>

using std::function;
using std::thread;

namespace huesdk_lib {

    //
    typedef function<void()> ThreadTask;

    class Thread {
     public:
        /**
         Construct thread without task.
         This constructor doesn't enforce to initialize it when declared as member
         variable
         */
        Thread() = default;
        
        /**
         Construct thread with task
         @param task The task which needs to be executed in the thread
         */
        explicit Thread(ThreadTask task);

        /**
         Construct thread with task and name.
         @param name Name to be applied to the thread
         @param task The task which needs to be executed in the thread
         */
        explicit Thread(std::string name, ThreadTask task);
    
        /**
         Destructor, which will block until the thread is finished
         */
        ~Thread();
    
        /**
         Move constructor
         VS2013 error C2610: is not a special member function which can be defaulted
         */
        Thread(Thread&& thread) : _thread(move(thread._thread)) {
        }
        /**
        Move operator
        VS2013 error C2610: is not a special member function which can be defaulted
        */
        Thread& operator=(Thread&& thread) {
            _thread = move(thread._thread);
            return *this;
        }
    
        /**
         Whether the thread is joinable or not
         @return true when joinable, false otherwise
         */
        bool joinable() const;
    
        /** 
         Join the thread
         */
        void join();
    
        /**
         Detach the thread
         */
        void detach();

        /**
         @return the id of the underlying thread
         */
        std::thread::id get_id() const;

        struct Self {
            /**
             Sets the system name for the current thread
             @param name name of the thread
             */
            static void set_name(const std::string& name);
            /**
             Returns the system name of the current thread
             @return name of the thread
             */
            static std::string name();
        };
    
     private:
        /** instance of the STL thread */
        thread _thread;
    };

}  // namespace huesdk_lib

#endif  // THREADING_THREAD_H_
