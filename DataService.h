#pragma once

#include "OrderBlacklist.h"
#include "IpInfo.h"
#include "DSPSrc.h"
#include "IpBlacklist.h"
#include "IndustryCode.h"
#include <memory>

namespace data 
{
    
class DataService
{
public:
    DataService(DSPSrcDatabase * dsp_src_db, IPInfoDatabase * ip_info_db ,OrderBlacklistDatabase * order_blacklist_db, IpBlacklistDatabase * ip_blacklist_db, IndustryCodeDatabase * industry_code_db)
        : dsp_src_(dsp_src_db)
        , ip_info_(ip_info_db)
        , order_blacklist_(order_blacklist_db)
        , ip_blacklist_(ip_blacklist_db)
        , industry_code_(industry_code_db)
    {        
    }
    
    inline time_t update_time() 
    {
        return dsp_src_.snapshot().toggle_time();
    }
    
    inline int QueryIPInfo(const Address & addr, uint32_t & zone) 
    {
        return ip_info_.Query(addr, zone);
    }
    
    inline int QueryGeoIPInfo(const Address & addr, uint64_t & zone) 
    {
        return ip_info_.QueryGeoIP(addr, zone);
    }
    
    inline void FindLocation(Location & location) 
    {
        dsp_src_.FindLocation(location);
    }
    
    inline Tag FindTag(uint32_t id) 
    {
        return dsp_src_.FindTag(id);
    }
    
    inline Order GetOrder(uint32_t orderId)
    {
        return dsp_src_.GetOrder(orderId);
    }
    
    //根据广告位查询订单列表(基础查询接口，UserInfo决定是否进行用户信息索引匹配)
    inline bool GetOrders(bool debug, uint32_t test, uint32_t & testResult, std::mt19937 & engine, uint8_t plat, Location const& location ,OrderArray & orders, uint32_t maxOrderNum = 1000)
    {
        return dsp_src_.GetOrders(debug, test, testResult, engine, plat, location, orders, order_blacklist_, maxOrderNum);
    }
    
    inline bool IsOrderEnabled(uint32_t id)
    {
        return order_blacklist_.IsEnabled(id); 
    }
    
    inline bool IsIpInBlacklist(const std::string & ip)
    {
        return ip_blacklist_.IsInBlacklist(ip);
    }
    
    inline TargetLengthSrc * FindTarget(uint32_t targetId)
    {
        return dsp_src_.FindTarget(targetId);
    }
    
    inline uint32_t GetSmoothRate(uint32_t orderid)
    {
        return dsp_src_.GetSmoothRate(orderid);
    }
    
    inline const char * ConvertIndustryCode(const char * code)
    {
        return industry_code_.convertIndustryCode(code);
    }
    
    inline uint32_t GetLBSId(uint64_t hash, uint32_t * & id)
    {
        return dsp_src_.GetLBSId(hash, id);
    }
    
private:
    DSPSrcReader dsp_src_;
    
    IPInfoReader ip_info_;
    
    OrderBlacklistReader order_blacklist_;
    
    IpBlacklistReader ip_blacklist_;
    
    IndustryCodeReader industry_code_;
};

}  // namespace 
