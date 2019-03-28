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
        return db_.open(config_);
    }

    bool create()
    {
        return db_.create(config_);
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

    uint64_t active()
    {
        return snapshot_.active();
    }
private:
    Snapshot snapshot_;

};
