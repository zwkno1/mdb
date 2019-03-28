#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <atomic>
#include <iostream>
#include <string.h>
#include <boost/noncopyable.hpp>
#include <boost/interprocess/sync/file_lock.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/filesystem.hpp>
#include <boost/interprocess/exceptions.hpp>
#include <sharedmemory.h>
#include <chrono>

struct TableMeta
{
    enum : uint32_t
    {
        MAX_NAME_SIZE = 64,
        MAX_PATH_SIZE = 4096,
    };

    TableMeta(const char * n, const char * p, const char * l, uint64_t s)
        : size(s)
        , utime(0)
    {
        memset(name, 0, sizeof(name));
        memset(path, 0, sizeof(path));
        memset(lockfile, 0, sizeof(lockfile));
        strncpy(name, n, MAX_NAME_SIZE-1);
        strncpy(path, p, MAX_PATH_SIZE-1);
        strncpy(lockfile, l, MAX_PATH_SIZE-1);
    }

    bool load(bool isTry = true)
    {
        try
        {
            // check up to date
            time_t lastWriteTime = boost::filesystem::last_write_time(path);
            if(lastWriteTime <= utime)
            {
                std::cout << "updatetime: " << utime << ",lastWriteTime: " << lastWriteTime << std::endl;
                return false;
            }

            // lock
            boost::interprocess::file_lock flock{lockfile};
            boost::interprocess::scoped_lock<boost::interprocess::file_lock> lock{flock, boost::interprocess::defer_lock};

            if(isTry)
            {
                lock.try_lock();
            }
            else
            {
                lock.lock();
            }

            if(!lock.owns())
            {
                std::cout << "lock " << lockfile << " failed" << std::endl;
                return false;
            }

            // check file size
            auto fileSize = boost::filesystem::file_size(path);
            if(fileSize > size || fileSize == 0)
            {
                std::cout << "file " << path << " size: " << fileSize << std::endl;
                return false;
            }

            // load data
            boost::interprocess::file_mapping file(path, boost::interprocess::read_only);
            boost::interprocess::mapped_region region(file, boost::interprocess::read_only);
            memcpy(&data[0], region.get_address(), region.get_size());
            utime = lastWriteTime;
        }
        catch(boost::interprocess::interprocess_exception & e)
        {
            std::cout << e.what() << std::endl;
            return false;
        }
        catch(boost::filesystem::filesystem_error & e)
        {
            std::cout << e.what() << std::endl;
            return false;
        }

        return true;
    }

    // table name
    char name[MAX_NAME_SIZE];

    // resource file path
    char path[MAX_PATH_SIZE];

    // lock file
    char lockfile[MAX_PATH_SIZE];

    // size of table's data
    uint64_t size;

    // update time
    time_t utime;

    uint8_t data[0];
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
        , active{0}
        , refcount{{0}, {0}}
    {
    }

    void trigge()
    {
        active += 1;
    }

    std::atomic<uint32_t> version;
    std::atomic<uint32_t> table_count;
    std::atomic<uint64_t> active;
    std::atomic<uint32_t> refcount[2];

    void increaseRef(uint32_t index)
    {
        ++refcount[index];
    }

    void decreaseRef(uint32_t index)
    {
        --refcount[index];
    }
};

struct TableConfig
{
    uint64_t size;
    std::string name;
    std::string path;
    std::string lockfile;

    bool operator==(const TableMeta & tm) const
    {
        if(size != tm.size)
            return false;
        if(name != tm.name)
            return false;
        if(path != tm.path)
            return false;
        if(lockfile != tm.lockfile)
            return false;
        return true;
    }

    bool operator !=(const TableMeta & tm) const
    {
        return ! operator==(tm);
    }
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

    bool open(const DatabaseConfig & config);

    bool create(const DatabaseConfig & config);

    size_t update();

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

private:
    bool loadTable(const DatabaseConfig & config);

    SharedMemory shm_;

    std::vector<TableMeta *> tables_;
};
