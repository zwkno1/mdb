#pragma once

#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string>

class PosixSharedMemory
{
public:
    PosixSharedMemory(PosixSharedMemory &) = delete;

    PosixSharedMemory & operator=(const PosixSharedMemory &) = delete;

    PosixSharedMemory(std::string const & name);

    ~PosixSharedMemory();

    bool open();

    bool create(size_t capacity);

    bool createOnly(size_t capacity);

    void close();

    bool isOpen() const;

    const std::string & name() const;

    size_t capacity() const;

    mode_t mode() const;

    const uint8_t * address() const;

    uint8_t * address();

    int remove();

    bool isChanged() const;

    int error() const;

private:
    bool open(int oflag, mode_t mode, size_t capacity);

    std::string name_;

    std::string path_;

    uint8_t * address_;

    size_t capacity_;

    int fd_;

    int flag_;

    mode_t mode_;

    int error_;
};
