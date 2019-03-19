#include <errno.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>

#include "PosixSharedMemory.h"

PosixSharedMemory::PosixSharedMemory(std::string const& name)
    : name_(name)
    , path_("/dev/shm/" + name)
    , address_(MAP_FAILED)
    , capacity_(0)
    , fd_(-1)
    , flag_(0)
    , mode_(0)
    , error_(0)
{
}

PosixSharedMemory::~PosixSharedMemory()
{
    close();
}

bool PosixSharedMemory::open()
{
    return open(O_RDONLY, 0, 0);
}

bool PosixSharedMemory::create(size_t size)
{
    return open(O_CREAT, S_IRWXU, size);
}

bool PosixSharedMemory::createOnly(size_t size)
{
    return open(O_CREAT| O_EXCL, S_IRWXU, size);
}

bool PosixSharedMemory::open(int flag, mode_t mode, size_t capacity)
{
    // the shared memory has mapped
    if (isOpen())
        return true;

    fd_ = shm_open(name_.c_str(), flag, mode);
    if (fd_ < 0)
    {
        error_ = errno;
        return false;
    }

    // 获取Shared Memory状态（大小）
    struct stat mstat;
    if (fstat(fd_, &mstat) != 0)
    {
        error_ = errno;
        close();
        return false;
    }

    // 设置Shared Memory大小
    if ((capacity != 0) && (mstat.st_size != capacity))
    {
        if (ftruncate64(fd_, capacity) != 0)
        {
            error_ = errno;
            close();
            return  false;
        }
    }

    // 将Shared Memory映射进地址空间
    int prot = (flag | O_RDWR) ? PROT_READ | PROT_WRITE : PROT_READ;
    address_ = mmap(nullptr, capacity_, prot, MAP_SHARED, fd_, 0);
    if (address_ == MAP_FAILED)
    {
        error_ = errno;
        close();
        return false;
    }

    capacity_ = (capacity == 0 ? mstat.st_size : capacity);
    mode_ = mstat.st_mode;

    return true;
}

void PosixSharedMemory::close()
{
    if(munmap(address_, capacity_) != 0)
    {
        error_ = errno;
    }
    address_ = MAP_FAILED;

    if(fd_ != -1)
    {
        ::close(fd_);
        fd_ = -1;
    }
}

const std::string & PosixSharedMemory::name() const
{
    return name_;
}

size_t PosixSharedMemory::capacity() const
{
    return capacity_;
}

mode_t PosixSharedMemory::mode() const
{
    return mode_;
}

uint8_t const* PosixSharedMemory::address() const
{
    return address_;
}

uint8_t * PosixSharedMemory::address()
{
    return address_;
}

int PosixSharedMemory::error() const
{
    return error_;
}

int PosixSharedMemory::remove()
{
    return shm_unlink(path_.c_str());
}

bool PosixSharedMemory::isOpen() const
{
    return address_ != MAP_FAILED;
}

bool PosixSharedMemory::isChanged() const
{
    struct stat mtat = {0};
    if (fstat(fd_, &mtat))
    {
        return false;
    }

    struct stat fstat = {0};
    if (stat(path_.c_str(), &fstat))
    {
        return false;
    }

    return mtat.st_dev == fstat.st_dev && mtat.st_ino == fstat.st_ino;
}
