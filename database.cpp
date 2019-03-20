#include "database.h"
#include <iostream>

bool Database::open(const char * name)
{
    if (!shm_.open(name))
    {
        return false;
    }
    return true;
}

std::pair<bool, bool> Database::create(const DatabaseConfig & config)
{
    uint64_t size = config.getDatabaseSize();
    if (!shm_.create(config.name.c_str(), size))
    {
        return std::make_pair(false, false);
    }

    assert(shm_.size() == size);
    if (shm_.size() != size)
    {
        return std::make_pair(false, false);
    }

    // try load table
    if((meta()->version == DatabaseMeta::DATABASE_VERSION) && (meta()->table_count == config.tables.size()*2) && loadTable())
    {
        return std::make_pair(true, true);
    }

    // load table failed because of shared memory not initialized, build it
    tables_.clear();
    tables_.reserve(config.tables.size()*2);
    // init db meta
    new(shm_.address()) DatabaseMeta{ static_cast<uint32_t>(config.tables.size() * 2) };
    uint64_t offset = sizeof(DatabaseMeta);
    for(int i = 0; i < 2; ++i)
    {
        for(auto & i : config.tables)
        {
            // init table meta
            TableMeta * t = new (reinterpret_cast<uint8_t *>(shm_.address()) + offset) TableMeta{ i.name.c_str(), i.filePath.c_str(), i.size };
            tables_.push_back(t);
            offset += i.size + sizeof(TableMeta);
        }
    }

    return std::make_pair(true, false);
}

bool Database::loadTable()
{
    // valiadate size
    if(shm_.size() < sizeof(DatabaseMeta))
        return false;

    tables_.clear();
    tables_.reserve(meta()->table_count);
    uint64_t offset = sizeof(DatabaseMeta);
    for (uint32_t i = 0; i < meta()->table_count; ++i)
    {
        if(offset + sizeof(TableMeta) > shm_.size())
            return false;

        TableMeta * t = reinterpret_cast<TableMeta *>(reinterpret_cast<uint8_t *>(shm_.address()) + offset);
        if(offset + t->size + sizeof(TableMeta) > shm_.size())
            return false;
        tables_.push_back(t);
        offset += t->size + sizeof (TableMeta);
    }

    return true;
}
