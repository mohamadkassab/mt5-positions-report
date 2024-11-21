#pragma once
#include "C:\MetaTrader5SDK\Include\MT5APIReport.h"

class CMyTableReport :
    public IMTReportContext
{
private:
    static const MTReportInfo s_info;            // Report data
    //--- table record
    #pragma pack(push,1)
        struct TableRecord
        {
            wchar_t        symbol[32];
            UINT32         total_positions;
            double         buy_volume;
            double         buy_price;
            double         sell_volume;
            double         sell_price;
            double         net_volume;

        };
    #pragma pack(pop)     
        //--- ...

public:
    //--- Constructor/destructor
    CMyTableReport(void);
    virtual          ~CMyTableReport(void);
    //--- Get the report data
    static void       Info(MTReportInfo& info) { info = s_info; }
    //--- 
    virtual void      Release(void);
    //--- Report generation method
    virtual MTAPIRES  Generate(const UINT type, IMTReportAPI* api);

};

