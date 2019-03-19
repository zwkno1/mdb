/*************************************************
  File name:  CommSrc.h
  Version:
  Description: 通用内存数据定义
  Function List:
*************************************************/
#ifndef __COMM_SRC_H_
#define __COMM_SRC_H_
#include <stdint.h>
#include <string.h>
#include <vector>
#include <exception>
#include <string>
#include <algorithm>

using namespace std;


#define CHECKLENGTH(a,b) if ((a) > (b)) return false


namespace data
{

#pragma pack(push, 1)

const uint32_t  MAX_FREQCNTL_KEY_LEN = 16;  // 频次最大长度
const uint32_t  MAX_SCENE_NUM = 6;          // 场景定向的最大类型数
const uint32_t  MAX_AGE_NUM=16;              // 年龄定向的最大段数
const uint32_t  MAX_ZONE_NUM=1024;              // 空间定向的最大段数
const uint32_t  MAX_OS_NUM=256;              // 年龄定向的最大段数
const uint32_t  MAX_BROWSER_NUM=256;              // 空间定向的最大段数
const uint32_t  MAX_AUDIENCE_KEY_LEN=64;    // 人群定向人群id最大长度





const string  ADS_LOCATION_FILE_NAME("ads_location_info.src");
const string  ADS_ORDER_FILE_NAME("ads_order_info.src");
const string  ADS_LOCATION_INDEX_FILE_NAME("ads_location_index_info.src");
const string  ADS_ADTYPE_KEY_FILE_NAME("ads_adtype_key.src");
const string  ADS_ORDER_INDEX_FILE_NAME("ads_order_index_info.src");
const string  ADS_ADTYPE_KEY_INDEX_FILE_NAME("ads_adtype_index.src");
const string  ADS_TAG_FILE_NAME("ads_tag_info.src");                     //标签定向信息
const string  ADS_TAG_INDEX_FILE_NAME("ads_tag_index_info.src");         //标签定向索引
const string  ADS_MEDIA_BLACKLIST_FILE_NAME("ads_media_blacklist_info.src");                      //媒体黑白名单
const string  ADS_MEDIA_ADVERTISER_LIST_FILE_NAME("ads_media_advertiser_list.src");                      //媒体广告主黑白名单
const string  ADS_MEDIA_ADVERTISER_LIST_INDEX_FILE_NAME("ads_media_advertiser_list_index.src");          //媒体广告主黑白名单索引
const string  ADS_ADVERTISER_MEDIA_LIST_FILE_NAME("ads_advertiser_media_list.src");                      //广告主媒体黑白名单
const string  ADS_ADVERTISER_MEDIA_LIST_INDEX_FILE_NAME("ads_advertiser_media_list_index.src");          //广告主媒体黑白名单索引
const string  ADS_CLIENT_CONFIG_FILE_NAME("ads_client_config.src");               // dmp
const string  ADS_ALGO_FLOW_FILE_NAME("ads_algo_flow_info.src");               // algo flow
const string  ADS_LBS_FILE_NAME("ads_lbs_info.src");               // lbs
const string  ADS_IPINFO_DATA_FILE("ipinfo_uds.dat");               // IPinfo 转换为UDS后的数据文件名
const string  ADS_MAXMIND_IPINFO_DATA_FILE("ipinfo_maxmind.dat");   // Maxmind IPinfo 
const string  ADS_IPBLACKLIST_FILE_NAME("ip_blacklist.src");               // IP黑名单

const string  ADS_TARGET_FILE_NAME("ads_target_info.src");                     //target定向信息
const string  ADS_TARGET_INDEX_FILE_NAME("ads_target_index_info.src");         //target定向索引

const string  ADS_SMOOTH_FILE_NAME("ads_smooth_info.src");         //平滑控制

const string  ADS_INDUSTRY_CODE_FILE_NAME("industry_code.src");               // 行业编码映射
const string  ADS_MEDIA_INDUSTRY_LIST_FILE_NAME("ads_media_industry_list.src");                      //媒体广告主黑白名单


#define _ADS_SEM_RECVD_KEY_                     0x00010003                     // 分发任务队列使用信号量key


//Val must be fixed size
template<typename Val>
struct Var 
{
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

typedef struct _ADS_VAR_UINT8_
{
    uint32_t m_u32Cnt;
    uint8_t  m_u8Value[0];
} VarU8;

typedef struct _ADS_VAR_UINT8_UINT8_
{
    uint32_t m_u32Cnt;
    VarU8    m_stValue[0];
} VarU8U8;


typedef struct _ADS_VAR_UINT32_
{
    uint32_t m_u32Cnt;
    uint32_t m_u32Value[0];
} VarU32;


typedef struct _ADS_VAR_UINT32_UINT32_
{
    uint32_t m_u32Cnt;
    VarU32   m_stValue[0];
} VarU32U32;


typedef struct _ADS_PRICE_LIMIT_
{
    uint32_t m_u32Mode;
    float    m_fPrice;
} PriceLimit;

typedef struct _ADS_VAR_PRICE_LIMIT_
{
    uint32_t       m_u32Cnt;
    PriceLimit     m_stValue[0];
} VarPriceLimit;

typedef struct _ADS_FREQUENCE_CNTL_
{
    char     m_acFrequenceKey[MAX_FREQCNTL_KEY_LEN];
    uint32_t m_ulFreqLimitNum;
    uint32_t m_begin;
    uint32_t m_end;

    void init()
    {
      memset(m_acFrequenceKey,0,MAX_FREQCNTL_KEY_LEN);
      m_ulFreqLimitNum = 3;
      m_begin = 0;
      m_end = 0;
    }
} AdsFreqCntl;

typedef struct _ADS_VAR_FREQUENCE_
{
    uint32_t       m_u32Cnt;
    AdsFreqCntl    m_stFreqCnt[0];
} VarFreq;

struct Audience
{
    uint32_t id;
    uint32_t type;
};

//资源文件文件头
struct AdsSrcHeader
{
    uint32_t m_u32Version;    //文件头版本号
    uint32_t m_u32BodyCount;  //文件体数据结构的个数
    uint32_t m_u32BodyLength; //文件体数据总长度

    void init(uint32_t ver =0,uint32_t cnt =0,uint32_t len =0) {
      m_u32Version = ver;
      m_u32BodyCount = cnt;
      m_u32BodyLength = len;
    }

    uint32_t body_size()
    {
        return this->m_u32BodyLength;
    }

    uint32_t body_cnt()
    {
        return this->m_u32BodyCount;
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

string FormatTime(time_t t, char const* format = "%F %T");

//偏移指针以及check 长度
inline bool RemoveAndCheck(uint8_t *& p, uint32_t& u32Offset, uint32_t u32MvLen, uint32_t u32TolLen)
{
    u32Offset += u32MvLen;
    if(u32Offset > u32TolLen) 
        return false;
    p += u32MvLen;
    return true;
}


inline void FillSectionLen(char *& pTmp,uint32_t u32SrcCnt)
{
    uint32_t u32TmpLen = sizeof(uint32_t);;
    *(uint32_t *)pTmp = u32SrcCnt;
    pTmp += u32TmpLen;
}

inline void FillString(char * & pTmp, const std::string & str)
{
    FillSectionLen(pTmp, str.size() + 1);
    strcpy(pTmp, &str[0]);
    pTmp += str.size() + 1;
}

/*************************************************************************
* Function: 处理协议中长度+ 值的类型流
* Param:  uint8_t const &* pTmp   [in][out] --> 流地址
*             uint32_t & u32FixedOffset  [in] [out] --> 偏移
               uint32_t u32SrcLen [in] --> 总长度
               uint32_t &u32SrcCnt[in] [out] --> Section 个数
               T & value[in] [out] --> Section 头指针
 * Return:
               true --> 成功
               false --> 解析错误
*************************************************************************/
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

}

#endif

