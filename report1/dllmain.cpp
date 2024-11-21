// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "MyTableReport.h"

//+------------------------------------------------------------------+
//| Entry point MTReportCreate                                       |
//+------------------------------------------------------------------+
MTAPIENTRY MTAPIRES  MTReportCreate(const UINT index, const UINT apiversion, IMTReportContext** context)
{
    //--- Check of parameters
    if (!context) return(MT_RET_ERR_PARAMS);
    //--- checking the report index
    if (index == 0)
    {
        //--- Creation of a copy
        if ((*context = new(std::nothrow) CMyTableReport()) == NULL)
            return(MT_RET_ERR_MEM);
        //--- Successful
        return(MT_RET_OK);
    }
    //--- Not found
    return(MT_RET_ERR_NOTFOUND);
}
//+------------------------------------------------------------------+

//+------------------------------------------------------------------+
//| The MTReportAbout entry point                                    |
//+------------------------------------------------------------------+
MTAPIENTRY MTAPIRES MTReportAbout(const UINT index, MTReportInfo& info)
{
    //--- Checking the index
    if (index == 0)
    {
        CMyTableReport::Info(info);
        return(MT_RET_OK);
    }
    //--- Not found
    return(MT_RET_ERR_NOTFOUND);
}
//+------------------------------------------------------------------+

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

