#include "database.h"
#include <iostream>

bool Database::open(const DatabaseConfig & config)
{
    if (!shm_.open(config.name.c_str()))
    {
        return false;
    }

    if(!loadTable(config))
        return false;

    return true;
}

bool Database::create(const DatabaseConfig & config)
{
    uint64_t size = config.getDatabaseSize();
    if (!shm_.create(config.name.c_str(), size))
    {
        return false;
    }

    assert(shm_.size() == size);
    if (shm_.size() != size)
    {
        return false;
    }

    DatabaseMeta * d = meta();
    // try load table
    if((d->version == DatabaseMeta::DATABASE_VERSION) && (d->table_count == config.tables.size()*2) && loadTable(config))
    {
        return true;
    }

    // load table failed because of shared memory not initialized, build it
    tables_.clear();
    tables_.reserve(config.tables.size()*2);
    uint64_t offset = sizeof(DatabaseMeta);
    for(int i = 0; i < 2; ++i)
    {
        for(auto & j : config.tables)
        {
            // init table meta
            TableMeta * t = new (reinterpret_cast<uint8_t *>(shm_.address()) + offset) TableMeta{ j.name.c_str(), j.path.c_str(), j.lockfile.c_str(), j.size };
            // load table 0
            if(i == 0)
            {
                if(!t->load(false))
                    return false;
            }
            tables_.push_back(t);
            offset += j.size + sizeof(TableMeta);
        }
    }

    // init db meta
    new(shm_.address()) DatabaseMeta{ static_cast<uint32_t>(config.tables.size() * 2) };

    return true;
}

bool Database::loadTable(const DatabaseConfig & config)
{
    // valiadate size
    if(shm_.size() < sizeof(DatabaseMeta))
        return false;

    DatabaseMeta * d = meta();
    uint64_t offset = sizeof(DatabaseMeta);

    tables_.clear();
    tables_.reserve(d->table_count);
    for (uint32_t i = 0; i < d->table_count; ++i)
    {
        if(offset + sizeof(TableMeta) > shm_.size())
            return false;

        TableMeta * t = reinterpret_cast<TableMeta *>(reinterpret_cast<uint8_t *>(shm_.address()) + offset);
        if(offset + t->size + sizeof(TableMeta) > shm_.size())
            return false;
        tables_.push_back(t);
        offset += t->size + sizeof (TableMeta);
    }

    if(offset != shm_.size())
        return false;

    // check table
    if(tables_.size() != config.tables.size() * 2)
        return false;

    for(size_t i = 0; i < config.tables.size(); ++i)
    {
        for(size_t j = 0; j < 2; ++j)
        {
            if(config.tables[i] != *tables_[i + j*config.tables.size()])
                return false;
        }
    }

    return true;
}

size_t Database::update()
{
    size_t count = 0;
    size_t begin = (1ull - meta()->active & 1ull)*(tables_.size()/2);
    for(size_t i = 0; i < tables_.size()/2; ++i)
    {
        if(tables_[begin+i]->load())
            ++count;
    }
    meta()->trigge();
    return count;
}
