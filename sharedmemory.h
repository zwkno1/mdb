#pragma once

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <limits>

class SharedMemory
{
public:
    static void remove(const char * name)
    {
        boost::interprocess::shared_memory_object::remove(name);
    }

    SharedMemory()
    {
    }

    bool open(const char * name)
    {
        try
        {
            shm_ = boost::interprocess::shared_memory_object(boost::interprocess::open_or_create, name, boost::interprocess::read_write);
            boost::interprocess::offset_t size = 0;
            if((!shm_.get_size(size)) || (size <= 0))
            {
                return false;
            }
            region_ = boost::interprocess::mapped_region {shm_, boost::interprocess::read_write, 0, size_t(size)};
        }
        catch(boost::interprocess::interprocess_exception & e)
        {
            return false;
        }
        return true;
    }

    bool create(const char * name, uint64_t size)
    {
        try
        {
            shm_ = boost::interprocess::shared_memory_object(boost::interprocess::open_or_create, name, boost::interprocess::read_write);
            boost::interprocess::offset_t oldSize = 0;
            if(!shm_.get_size(oldSize))
            {
                return false;
            }

            if(size > std::numeric_limits<boost::interprocess::offset_t>::max())
            {
                return false;
            }

            if(static_cast<uint64_t>(oldSize) != size)
            {
                shm_.truncate(static_cast<boost::interprocess::offset_t>(size));
            }
            region_ = boost::interprocess::mapped_region {shm_, boost::interprocess::read_write, 0, size};
        }
        catch(boost::interprocess::interprocess_exception & e)
        {
            return false;
        }
        return true;
    }

    const char * name() const
    {
        return shm_.get_name();
    }

    void * address() const
    {
        return region_.get_address();
    }

    size_t size() const
    {
        return region_.get_size();
    }

private:
    boost::interprocess::shared_memory_object shm_;

    boost::interprocess::mapped_region region_;
};
