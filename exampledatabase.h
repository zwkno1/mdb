#pragma once

#include <database.h>
#include <snapshot.h>

class ExampleDatabase
{
public:
    ExampleDatabase(DatabaseConfig & config)
        : config_(config)
    {
    }

    bool open()
    {
        auto result = db_.open(config_.name.c_str());
        if(!result)
            return false;
        return true;
    }

    bool create()
    {
        if(!db_.create(config_))
            return false;

        // validate table

        return true;
    }

    DatabaseMeta * meta()
    {
        return db_.meta();
    }

private:

    DatabaseConfig config_;

    Database db_;
};

class ExampleDatabaseReader
{
public:
    ExampleDatabaseReader(ExampleDatabase * db)
        : snapshot_(db->meta())
    {

    }
private:
    Snapshot snapshot_;

};
