#pragma once

#include <cstdint>
#include <algorithm>

#pragma pack(push, 1)

//Val must be fixed size
template<typename Val>
struct Var 
{
    static_assert(std::is_pod<Val>::value, "Val of Var must be pod type");

    uint32_t size_;
    Val data_[0];
    
    Var()
        : size_(0)
    {
    }
    
    inline uint32_t size() const 
    {
        return size_;
    }
    
    inline void resize(uint32_t s)
    {
        size_ = s;
    }
    
    inline void push_back(const Val & val)
    {
        data_[size_] = val;
        ++size_;
    }
    
    inline Val * data()
    {
        return & data_[0];
    }
    
    inline const Val * data() const
    {
        return & data_[0];
    }
    
    inline uint32_t bytes() const
    {
        return sizeof( size_ ) + size_ * sizeof(data_[0]);
    }
    
    inline Val * begin()
    {
        return &data_[0];
    }
    
    inline const Val * begin() const
    {
        return &data_[0];
    }
    
    inline Val * end()
    {
        return &data_[size_];
    }
    
    inline const Val * end() const
    {
        return &data_[size_];
    }
    
    inline Val & at(uint32_t i)
    {
        return data_[i];
    }
    
    inline const Val & at(uint32_t i) const
    {
        return data_[i];
    }
};

template<typename T>
struct VarVar
{
    uint32_t size_;
    Var<T> value_[0];
};

struct SrcHeader
{
    uint32_t version;    //文件头版本号
    uint32_t bodyCount;  //文件体数据结构的个数
    uint32_t bodyLength; //文件体数据总长度

    SrcHeader(uint32_t ver = 0,uint32_t cnt = 0,uint32_t len = 0)
        : version(ver)
        , bodyCount(cnt)
        , bodyLength(len)
    {
    }
};


#pragma pack(pop)

template<typename T>
const Var<T> * ParseVar(const uint8_t * data, uint32_t size, uint32_t & offset)
{
    const Var<T> * result;
    if(offset + sizeof(*result) > size)
        return 0;
    result = reinterpret_cast<const Var<T> *>(data+ offset);
    offset += result->bytes();
    if(offset > size)
        return 0;
    return result;
}

template<typename T>
inline Var<T> * CreateVar(char * buf)
{
    return new(buf)Var<T>{};
}

template<typename T, typename Iter>
inline uint32_t FillVar(Var<T> * out, Iter begin, Iter end)
{
    for(;begin != end; ++begin)
    {
        out->push_back(*begin);
    }
    return out->bytes();
}

template<typename T, typename Containter>
inline uint32_t FillVar(Var<T> * out, const Containter & in)
{
    return FillVar(out, std::begin(in), std::end(in));
}

template<typename T>
uint32_t FillVar(Var<T> * out, const std::string & in)
{
    return FillVar(out, in.begin(), in.end() + 1);
}

template<typename T>
uint32_t FillVar(Var<T> * out, const char * in)
{
    return FillVar(out, in, in + strlen(in) + 1);
}

template<typename T>
inline bool ParserSection(uint8_t * data, uint32_t len, uint32_t & offset, uint32_t & size, T * & value)
{
    // get size
    if(offset + sizeof(uint32_t) > len)
        return false;
    size = *(const uint32_t *)(data + offset);
    offset += sizeof(uint32_t);
    
    if(size != 0)
    {
        value = (T *)(data + offset);
    }
    
    offset += size * sizeof(value[0]);
    if(offset > len)
        return false;
    return true;
}

template<typename Iterator, typename T>
inline Iterator binary_find(Iterator first,Iterator last, const T & val)
{
    first = std::lower_bound(first, last, val);
    return (first != last && !(val < *first)) ? first : last;
}

template<typename Iterator, typename T, typename Compare>
inline Iterator binary_find(Iterator first,Iterator last, const T & val, Compare comp)
{
    first = std::lower_bound(first, last, val, comp);
    return (first != last && !comp(val, *first)) ? first : last;
}
