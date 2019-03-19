#pragma once

#include "Database.h"
#include <atomic>

class DatabaseReader
{
public:
    DatabaseReader(const DatabaseReader &) = delete;
    DatabaseReader & operator=(const DatabaseReader &) = delete;

    DatabaseReader(Database * db);

    ~DatabaseReader();

private:
    Database * db_;

    int32_t readIndex_;
};

