#pragma once

#include "database.h"
#include <boost/noncopyable.hpp>

class Snapshot : private boost::noncopyable
{
public:
    Snapshot(DatabaseMeta * db)
        : db_(db)
        , readIndex_(db_->read_index)
    {
        db_->increaseRef(readIndex_);
    }

    ~Snapshot()
    {
        db_->decreaseRef(readIndex_);
    }

private:
    DatabaseMeta * db_;

    uint32_t readIndex_;
};

