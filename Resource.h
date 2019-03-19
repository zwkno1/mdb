/*************************************************
  File name: Resource.h
  Version:        
  Description:基础资源表定义
  Function List:  
*************************************************/


#pragma once
#ifndef __RESOURCE_H_
#define __RESOURCE_H_

#include <sys/stat.h>
#include <fstream>
#include "Database.h"
#include "AdsProperties.h"
#include "CommSrc.h"

namespace data {

struct ResourceMeta: public TableMeta {

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
    time_t utime;

    /**
     * 资源文件最后修改时间
     */
    struct stat filestat;

    /**
     * 当前数据个数，在加载文件时获取。
     */
    uint32_t item_count;

    ResourceMeta(char const* n = "", uint32_t hsize = 0, uint32_t isize = 0)
        : TableMeta(n), header_size(hsize), item_size(isize), capacity(0),
            utime(0), item_count(0) {
      memset(path, 0, sizeof(path));
      memset(&filestat, 0, sizeof(filestat));
    }

    int Init(AdsProperties const& config, char const* section) {
      std::string key(this->name);
      key.append("Capacity");

      uint32_t cap = 0;
      cap = config.Getd(section, key.c_str(), 0);
      if (cap > 0)
        this->set_capacity(cap);
      return 0;
    }

    bool Validate(ResourceMeta const* resource, bool is_validate_size) const {
      return this->header_size == resource->header_size
          //&& this->item_size == resource->item_size
          && 0 == strcmp(this->name, resource->name)
          && (is_validate_size ?
              (this->capacity == resource->capacity
                  && this->size == resource->size) : true);
    }

    char * CopyTo(char * bytes) const {
      memcpy(bytes, this, sizeof(ResourceMeta));
      return bytes + this->size;
    }

    void DumpStatus(std::ostream & os) const {
      os << "{";
      os << "\"size\": " << size;
      os << ", \"name\": \"" << name << "\"";
      os << ", \"header_size\": " << header_size;
      os << ", \"item_size\": " << item_size;
      os << ", \"capacity\": " << capacity;
      os << ", \"path\": \"" << path << "\"";
      os << ", \"utime\": \"" << FormatTime(utime) << "\"";
      os << ", \"file_size\": " << filestat.st_size;
      os << ", \"file_mtime\": \"" << FormatTime(filestat.st_mtime) << "\"";
      os << ", \"item_count\": " << item_count;
      os << "}";
    }

    void set_capacity(uint32_t c) {
      this->capacity = c;
      this->size = sizeof(ResourceMeta) + c;
    }

    void set_path(std::string const& p) {
      if (p != path) {
        strncpy(path, p.c_str(), sizeof(path) - 1);

        // after change path, data need reload. So reset filestat
        memset(&(filestat), 0, sizeof(filestat));
      }
    }
};

inline std::ostream & operator <<(std::ostream & os, ResourceMeta const& obj) {
  obj.DumpStatus(os);
  return os;
}

template<typename H, typename T>
  std::pair<int, std::string> LoadResource(ResourceMeta * meta, H * header,
      T * data) {
    using namespace std;
    std::stringstream ss;

    uint32_t capacity = meta->capacity;
    char const* path = meta->path;

    if(0 == capacity) 
    {
        ss << "数据文件被禁用: " << path;
        return make_pair(0, ss.str());
    }

    struct stat * filestat = &(meta->filestat);
    if (::stat(path, filestat)) {
      ss << "数据文件无法访问: " << path;
      return make_pair(-1, ss.str());
    }

    time_t mtime = filestat->st_mtime;
    size_t file_size = filestat->st_size;

    // 读取文件头
    if (file_size < sizeof(header)) {
        ss << "数据文件格式错误: " << path << ", mtime=" << mtime <<  ", size=" << file_size;
        return make_pair(-1, ss.str());
    }
    cout<<" path:"<<path<<endl<<"sizeof header:"<<sizeof(H)<<endl;


    ifstream is(path, ios_base::in | ios_base::binary);
    is.read(reinterpret_cast<char *>(header), sizeof(H));
    if (!is) {
        ss << "数据文件头读取错误: " << path << ", mtime=" << mtime;
        return make_pair(-2, ss.str());
    }

    cout<<"header: body size:"<< header->body_size()<<" header->body_cnt():"<<header->body_cnt()<<endl;
    size_t body_size = header->body_size();
    if (file_size  > capacity) {
      ss << "数据文件数据超过限额: " << path << ", mtime=" << mtime <<  ", body_size = " << body_size << ", capacity = " << capacity;
      return make_pair(-3, ss.str());
    }

    if (file_size != sizeof(H) + body_size) {
        ss << "数据文件头不正确: " << path << "mtime=" << mtime << ", file_size("<< file_size << ") != header(" << sizeof(H) << ") + body_size(" << body_size << ")";
        return make_pair(-4, ss.str());
    }

    // 读取文件体
    is.read(reinterpret_cast<char *>(data), body_size);
    if (!is) {
        ss << "数据文件读取数据错误: " << path << ", mtime=" << mtime << ", body_size = " << body_size;
        return make_pair(-5, ss.str());
    }

    ss <<  "数据文件读取成功: " << path <<", mtime=" << mtime << ", body_size = " << body_size;
    meta->item_count = header->body_cnt();
    return make_pair(meta->item_count, ss.str());
  }

template<typename H, typename T>
  struct Resource: public ResourceMeta {

      typedef H HeaderType;
      typedef T DataType;

      H header;

      T items[0];

      Resource(char const* n = "")
          : ResourceMeta(n, sizeof(H), sizeof(T)), header() {
      }

      std::pair<int, std::string> LoadData(time_t now) {
        this->utime = now;
        return LoadResource<H, T>(this, &header, items);
      }

      T const* begin() const {
        return &items[0];
      }

      T const* end() const {
        return items + item_count;
      }
  };

/**
 * 文件名不变的资源文件
 */
template<typename H, typename T>
  struct StaticResource: public Resource<H, T> {
      typedef Resource<H, T> Super;

      StaticResource(char const* n = "")
          : Super(n) {
      }

      std::pair<int, std::string> IsChanged(time_t now) const {
        using namespace std;

        char const* loading_path = this->path;
        struct stat loading_filestat = {0};
        std::stringstream ss;

        // 获取数据文件信息
        if (::stat(loading_path, &loading_filestat)) {
          ss << "无法访问数据文件: " << loading_path;
          return make_pair(-1, ss.str());
        }

        // 根据文件变更时间判断是否更新
        if (loading_filestat.st_mtime == this->filestat.st_mtime) {
          ss << "数据文件未更新: " << loading_path << ", mtime=" << this->filestat.st_mtime;
          return make_pair(0, ss.str());
        } else {
          ss <<  "数据文件已更新: " << loading_path << ", last_mtime=" << this->filestat.st_mtime << ", mtime= " << loading_filestat.st_mtime;
          return make_pair(1, ss.str());
        }
      }

      std::pair<int, std::string> Load(time_t now) {
        std::pair<int, std::string> rc = this->IsChanged(now);
        if (1 == rc.first) {
          rc = this->LoadData(now);
        }
        return rc;
      }
  };

} /* namespace data */
#endif /* RESOURCE_H_ */
