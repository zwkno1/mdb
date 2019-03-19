#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <string.h>
#include <PosixSharedMemory.h>

struct TableMeta
{
    TableMeta(const char * n, uint64_t s)
        : size(s)
    {
        memset(name, 0, sizeof(name));
        strncpy(name, n, 31);
    }

    uint64_t size;
    char name[64];
};

struct ResourceMeta : public TableMeta
{
    static const int PATH_MAX = 4096;

    /**
     * 数据头结构体大小
     */
    uint32_t header_size;

    /**
     * 数据项结构体大小
     */
    uint32_t item_size;

    /**
     * 最大数据长度。通常在启动时读取配置决定。
     */
    uint32_t capacity;

    /**
     * 资源文件路径
     */
    char path[PATH_MAX];

    /**
     * 资源文件加载时间
     */
    uint64_t utime;

    /**
     * 当前数据个数，在加载文件时获取。
     */
    uint32_t item_count;
};

struct DatabaseMeta
{
    volatile uint32_t version;
    volatile uint32_t table_meta_size;
    volatile uint32_t table_count;
    volatile int32_t read_index;
    volatile uint64_t update_time;
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

class Database
{
public:
    Database(const Database &) = delete;
    Database & operator=(const Database &) = delete;

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
