#pragma once

#include "database.h"
#include <boost/noncopyable.hpp>

class Snapshot : private boost::noncopyable
{
public:
    Snapshot(Database * db)
        : db_(db)
        , tableCount_(db->meta()->table_count/2)
        , readIndex_(db->meta()->read_index)
    {
        db_->meta()->increaseRef(readIndex_);
    }

    ~Snapshot()
    {
        db_->meta()->decreaseRef(readIndex_);
    }

    TableMeta * table(size_t index)
    {
        if(index > tableCount_)
            return nullptr;
        return db_->table(readIndex_*tableCount_ + index);
    }

private:
    Database * db_;

    uint32_t tableCount_;

    uint32_t readIndex_;
};

