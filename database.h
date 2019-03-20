#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <string.h>
#include <boost/noncopyable.hpp>
#include <sharedmemory.h>

struct TableMeta
{
    enum : uint32_t
    {
        MAX_NAME_SIZE = 64,
        MAX_PATH_SIZE = 4096,
    };

    TableMeta(const char * n, const char * p, uint64_t s)
        : size(s)
        , utime(0)
    {
        memset(name, 0, sizeof(name));
        memset(path, 0, sizeof(path));
        strncpy(name, n, MAX_NAME_SIZE-1);
        strncpy(path, p, MAX_PATH_SIZE-1);
    }

    // table name
    char name[MAX_NAME_SIZE];

    // resource file path
    char path[MAX_PATH_SIZE];

    // size of table's data
    uint64_t size;

    // update time
    uint64_t utime;
};

struct DatabaseMeta
{
    enum : uint32_t
    {
        DATABASE_VERSION = 1,
    };

    DatabaseMeta(uint32_t tc)
        : version{DATABASE_VERSION}
        , table_count{tc}
        , read_index{1}
        , refcount{0}
    {
    }

    volatile uint32_t version;
    volatile uint32_t table_count;
    volatile uint32_t read_index;
    volatile uint32_t refcount[2];

    void increaseRef(uint32_t index)
    {

    }

    void decreaseRef(uint32_t index)
    {

    }
};

struct TableConfig
{
    bool enable;
    uint64_t size;
    std::string name;
    std::string filePath;
    std::string lockPath;
};

struct DatabaseConfig
{
    std::string name;

    std::vector<TableConfig> tables;

    uint64_t getDatabaseSize() const
    {
        uint64_t size = sizeof(DatabaseMeta);
        for(const auto & i : tables)
        {
            size += (i.size + sizeof(TableMeta)) * 2;
        }
        return size;
    }
};

class Database : private boost::noncopyable
{
public:
    Database()
    {
    }

    bool open(const char * name);

    std::pair<bool, bool> create(const DatabaseConfig & config);

    const char * name() const
    {
        return shm_.name();
    }

    const TableMeta * table(size_t index) const
    {
        return tables_[index];
    }

    TableMeta * table(size_t index)
    {
        return tables_[index];
    }

    DatabaseMeta * meta() const
    {
        return reinterpret_cast<DatabaseMeta *>(shm_.address());
    }

    void * address()
    {
        return shm_.address();
    }

    uint64_t size() const
    {
        return shm_.size();
    }

    bool loadTable();

private:
    std::string name_;

    SharedMemory shm_;

    std::vector<TableMeta *> tables_;
};
