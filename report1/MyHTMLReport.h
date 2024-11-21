#pragma once
#include "C:\MetaTrader5SDK\Include\MT5APIReport.h"
class CMyHTMLReport :
    public IMTReportContext
{
private:
    static const MTReportInfo s_info;            // Report data
    static const LPCWSTR s_template;             // HTML template
public:
    //--- Constructor/destructor
    CMyHTMLReport(void);
    ~CMyHTMLReport(void);
    //--- Get the report data
    static void       Info(MTReportInfo& info) { info = s_info; }
    //--- 
    virtual void      Release(void);
    //--- Report generation method
    virtual MTAPIRES  Generate(const UINT type, IMTReportAPI* api);
};

