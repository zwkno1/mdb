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
        auto result = db_.open(config_);
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

    Database * db()
    {
        return &db_;
    }

private:

    DatabaseConfig config_;

    Database db_;
};

class ExampleDatabaseReader
{
public:
    ExampleDatabaseReader(ExampleDatabase * db)
        : snapshot_(db->db())
    {
    }

    const char * query(int i)
    {
        return reinterpret_cast<const char *>(snapshot_.table(i)->data);
    }

private:
    Snapshot snapshot_;

};
