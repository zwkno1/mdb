#pragma once

#include <atomic>
#include <boost/interprocess/sync/named_semaphore.hpp>

#include <database.h>

class DatabaseService
{
public:
    DatabaseService(Database * db, const char * key)
        : db_(db)
        , semaphore_(boost::interprocess::open_or_create, key, 0)
        , stop_(false)
    {
    }

    void run()
    {
        while(!stop_)
        {
            semaphore_.wait();
            db_->update();
        }
    }

    void stop()
    {
        stop_ = true;
    }

private:
    Database * db_;

    boost::interprocess::named_semaphore semaphore_;

    std::atomic<bool> stop_;
};
