#pragma once

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <limits>

class SharedMemory
{
public:
    SharedMemory()
    {
    }

    bool open(const char * name)
    {
        try
        {
            shm_ = boost::interprocess::shared_memory_object(boost::interprocess::open_or_create, name, boost::interprocess::read_only);
            boost::interprocess::offset_t size = 0;
            if((!shm_.get_size(size)) || (size <= 0))
            {
                return false;
            }
            region_ = boost::interprocess::mapped_region {shm_, boost::interprocess::read_only, 0, size_t(size)};
        }
        catch(boost::interprocess::interprocess_exception & e)
        {
            return false;
        }
        return true;
    }

    bool create(const char * name, uint64_t capacity)
    {
        try
        {
            shm_ = boost::interprocess::shared_memory_object(boost::interprocess::open_or_create, name, boost::interprocess::read_write);
            boost::interprocess::offset_t size = 0;
            if((!shm_.get_size(size)) || (size <= 0))
            {
                return false;
            }

            if(capacity > std::numeric_limits<boost::interprocess::offset_t>::max())
            {
                return false;
            }

            if(size < 0)
            {
                return false;
            }

            if(static_cast<uint64_t>(size) != capacity)
            {
                shm_.truncate(static_cast<boost::interprocess::offset_t>(capacity));
            }
            region_ = boost::interprocess::mapped_region {shm_, boost::interprocess::read_write, 0, capacity};
        }
        catch(boost::interprocess::interprocess_exception & e)
        {
            return false;
        }
        return true;
    }

    void remove()
    {
        shm_.remove(shm_.get_name());
        region_ = boost::interprocess::mapped_region{};
        shm_ = boost::interprocess::shared_memory_object{};
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
