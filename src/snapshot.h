#pragma once

#include "database.h"
#include <boost/noncopyable.hpp>

class Snapshot : private boost::noncopyable
{
public:
    Snapshot(Database * db)
        : db_(db)
        , tableCount_(db->meta()->table_count/2)
        , active_(db->meta()->active)
    {
        db_->meta()->increaseRef(active_ & 1);
    }

    ~Snapshot()
    {
        db_->meta()->decreaseRef(active_ & 1);
    }

    TableMeta * table(size_t index)
    {
        if(index > tableCount_)
            return nullptr;
        return db_->table((active_ & 1ull) * tableCount_ + index);
    }

    uint64_t active()
    {
        return active_;
    }
private:
    Database * db_;

    uint32_t tableCount_;

    uint64_t active_;
};

