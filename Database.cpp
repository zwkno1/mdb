#include "Database.h"
#include <iostream>

bool Database::open()
{
    if (shared_memory_.open())
    {
        return false;
    }

    if (buildTable())
    {
        return false;
    }

    //// 验证表数据
    //if (validateTables())
    //{
    //    return false;
    //}

    return true;
}

bool Database::create(uint64_t capacity)
{
    if (!shared_memory_.create(capacity))
    {
        return false;
    }

    if (shared_memory_.capacity() != capacity)
    {
        return false;
    }

    meta().version = version_;

    //char * bytes = reinterpret_cast<char *>(&(meta().data));
    //if (initializeTables(bytes))
    //    return false;

    // 建立表索引
    if (buildTable())
        return false;

    //// 验证表数据
    //if (validateTables())
    //    return false;

    return true;
}

bool Database::buildTable()
{
    // valiadate size
    if(shared_memory_.capacity() < sizeof(DatabaseMeta))
        return false;

    // validate meta data
    if(sizeof(TableMeta) != meta().table_meta_size)
        return false;

    tables_.clear();
    tables_.reserve(meta().table_count);
    uint64_t offset = sizeof(DatabaseMeta);
    for (uint32_t i = 0; i < meta().table_count; ++i)
    {
        if(offset + sizeof(TableMeta) > shared_memory_.capacity())
            return false;

        TableMeta * t = reinterpret_cast<TableMeta *>(shared_memory_.address() + offset);
        if(offset + t->size > shared_memory_.capacity())
            return false;
        tables_.push_back(t);
        offset += t->size;
    }

    return true;
}
