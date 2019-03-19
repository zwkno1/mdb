#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <string.h>
#include <boost/noncopyable.hpp>
#include <PosixSharedMemory.h>

struct TableMeta
{
    enum class : uint32_t
    {
        MAX_NAME_SIZE = 64,
        MAX_PATH_SIZE = 4096,
    };

    TableMeta(const char * n, const char * p, uint64_t cap)
        : capacity(cap)
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

    // capacity of table include table meta
    uint64_t capacity;

    // update time
    uint64_t utime;
};

struct DatabaseMeta
{
    volatile uint32_t version;
    volatile uint32_t table_count;
    volatile int32_t read_index;
    volatile uint32_t refcount[2];

    void increaseRef(int index)
    {

    }

    void decreaseRef(int index)
    {

    }
};

struct ResourceConfig
{
    bool enable;
    std::string filePath;
    std::string fileLockPath;

};

class Database : private boost::noncopyable
{
public:

    Database(const std::string & name, uint32_t version)
        : version_(version)
        , shared_memory_(name)
    {
    }

    bool open();

    bool create(uint64_t capacity);

    const std::string & name() const
    {
        return shared_memory_.name();
    }

    uint64_t capacity() const
    {
        return shared_memory_.capacity();
    }

    const TableMeta * table(size_t index) const
    {
        return tables_[index];
    }

    TableMeta * table(size_t index)
    {
        return tables_[index];
    }

    const DatabaseMeta & meta() const
    {
        return *(reinterpret_cast<DatabaseMeta const*>(shared_memory_.address()));
    }

    DatabaseMeta & meta()
    {
        return *(reinterpret_cast<DatabaseMeta *>(shared_memory_.address()));
    }

    bool buildTable();

private:
    uint32_t version_;

    PosixSharedMemory shared_memory_;

    std::vector<TableMeta *> tables_;
};
