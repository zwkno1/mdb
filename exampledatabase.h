#pragma once

#include <database.h>
#include <databaseconfig.h>

class ExampleDatabase
{
public:
    ExampleDatabase(DatabaseConfig & config)
        : config_(config)
    {
    }

    bool create()
    {
        auto result = db_.create(config_);
        if(!result.first)
            return false;

        if(result.second)
        {
            // validate all tables
        }
        else
        {
            // initialize all tables
        }

        return true;
    }

private:

    DatabaseConfig config_;

    Database db_;
};
