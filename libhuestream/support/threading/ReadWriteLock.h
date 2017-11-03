/*******************************************************************************
Copyright (C) 2017 Philips Lighting Holding B.V.
All Rights Reserved.
********************************************************************************/
#ifndef THREADING_READWRITELOCK_H_
#define THREADING_READWRITELOCK_H_

#include <mutex>
#include <condition_variable>

namespace huesdk_lib {
    
    class ReadWriteLock{
    public:
        /**
        constructor
        */
        ReadWriteLock();

        /**
        destructor
        */
        ~ReadWriteLock();
        
        /**
         obtain a read lock
         will block while write lock is active
         */
        void read_lock();

        /**
         release a read lock
         */
        void read_unlock();
        
        /**
         obtain a write lock
         will block while a read lock or write lock is active
         */
        void write_lock();
        
        /**
         release a write lock
         */
        void write_unlock();

    private:
        int  _reader_count;
        bool _has_write_lock;
        bool _stop;
        bool _busy;
        std::mutex _mutex;
        std::condition_variable _unlocked_condition;
        std::condition_variable _busy_condition;
    };
    
    class UniqueReadLock {
    public:
        explicit UniqueReadLock(ReadWriteLock* lock);
        ~UniqueReadLock();
    private:
        ReadWriteLock* _lock;
    };
    
    class UniqueWriteLock {
    public:
        explicit UniqueWriteLock(ReadWriteLock* lock);
        ~UniqueWriteLock();
    private:
        ReadWriteLock* _lock;
    };
    
}  // namespace huesdk_lib

#endif  // THREADING_READWRITELOCK_H_
