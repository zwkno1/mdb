#pragma once

#include "database.h"
#include <boost/noncopyable.hpp>

class DatabaseReader : private boost::noncopyable
{
public:
    DatabaseReader(Database * db)
        : db_(db)
        , readIndex_(db_->meta()->read_index)
    {
        db_->meta()->increaseRef(readIndex_);
    }

    ~DatabaseReader()
    {
        db_->meta()->decreaseRef(readIndex_);
    }

private:
    Database * db_;

    uint32_t readIndex_;
};

