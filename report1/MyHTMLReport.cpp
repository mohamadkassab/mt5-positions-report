#include "pch.h"
#include "MyHTMLReport.h"

//+------------------------------------------------------------------+
//| Module description structure                                     |
//+------------------------------------------------------------------+
const MTReportInfo CMyHTMLReport::s_info =
{
 100,
 MTReportAPIVersion,
 MTReportInfo::IE_VERSION_ANY,
 L"My HTML Report",
 L"Copyright 2001-2011, MetaQuotes Software Corp.",
 L"MetaTrader 5 Report API plug-in",
 0,
 MTReportInfo::TYPE_HTML,
   { 0 },
   {              // params
    { MTReportParam::TYPE_GROUPS, MTAPI_PARAM_GROUPS, L"*" },
   },1            // params_total
};
//+------------------------------------------------------------------+

//+------------------------------------------------------------------+
//| HTML template                                                    |
//+------------------------------------------------------------------+
const LPCWSTR CMyHTMLReport::s_template =
L"<html>"
L"<head><title>My HTML Report</title></head>"
L"<body>"
L"<table width=100% border=1>"
L"<tr><td width=50%>Name</td><td width=50%>Leverage</td></tr>"
L"<mt5:users>"
L"<tr> <td><mt5:name/></td> <td>1 : <mt5:leverage/></td> </tr>"
L"</mt5:users>"
L"</table>"
L"</body>"
L"</html>";
//+------------------------------------------------------------------+

//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CMyHTMLReport::CMyHTMLReport(void)
{
}
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CMyHTMLReport::~CMyHTMLReport(void)
{
}
//+------------------------------------------------------------------+
//| Plugin release method                                            |
//+------------------------------------------------------------------+
void CMyHTMLReport::Release(void)
{
    delete this;
}

//+------------------------------------------------------------------+
//| Report generation method                                         |
//+------------------------------------------------------------------+
MTAPIRES CMyHTMLReport::Generate(const UINT type, IMTReportAPI* api)
{
    IMTDatasetColumn* column = NULL;
    IMTUser* user = NULL;
    UINT64* logins = NULL;
    UINT              logins_total = 0, counter = 0;
    MTAPISTR          tag = { 0 };
    MTAPIRES          res;
    //--- Check of pointer
    if (!api)
        return(MT_RET_ERR_PARAMS);
    //--- Checking the type of a requested report
    if (type != MTReportInfo::TYPE_HTML)
        return(MT_RET_ERR_PARAMS);
    //--- Template loading
    if ((res = api->HtmlTplLoad(s_template)) != MT_RET_OK) return(res);
    //--- Get the list of users
    if ((res = api->ParamLogins(logins, logins_total)) != MT_RET_OK)
        return(res);
    //--- Create an object of a client record
    if ((user = api->UserCreate()) == NULL)
    {
        api->Free(logins);
        return(MT_RET_ERR_MEM);
    }
    //--- Get the next macros for processing
    //--- The number of the macros processings are placed in counter.
    //--- The macros is processed using the HtmlTplProcess method
    while ((res = api->HtmlTplNext(tag, &counter)) == MT_RET_OK)
    {
        //--- Clients loop macros
        //--- This macros is used to display the number of table rows appropriate to the number of the received clients
        if (CMTStr::CompareNoCase(tag, L"users") == 0)
        {
            //--- Are there any more clients?
            if (logins && counter < logins_total)
            {
                //--- If there are, they must be displayed by processing embedded tags:
                //--- L"<tr> <td><mt5:name/></td> <td>1 : <mt5:leverage/></td> </tr>"
                //--- To do this, call HtmlTplProcess that gives the order
                //--- To process the construction embedded in <mt5:users>...</mt5:users>.
                //--- Embedded macros will be received during the next reference to HtmlTplNext
                if ((res = api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE)) != MT_RET_OK)
                {
                    if (logins) api->Free(logins);
                    user->Release();
                    return(res);
                }
                //--- Get the current client description (the client for the current row)
                //--- from its entry we will get the information on the name and the leverage
                if ((res = api->UserGet(logins[counter], user)) != MT_RET_OK)
                {
                    if (logins) api->Free(logins);
                    user->Release();
                    return(res);
                }
            }
            continue;
        }
        //--- Process the macros belonging to the current row
        //--- All client information is already present in user
        //--- The name of a client
        if (CMTStr::CompareNoCase(tag, L"name") == 0)
        {
            //--- Use the HtmlWriteSafe method to screen
            //--- possible special symbols in the client's name (e.g., < or >)
            if ((res = api->HtmlWriteSafe(user->Name(), IMTReportAPI::HTML_SAFE_USENOBSP)) != MT_RET_OK)
            {
                if (logins) api->Free(logins);
                user->Release();
                return(res);
            }
            continue;
        }
        //--- Leverage
        if (CMTStr::CompareNoCase(tag, L"leverage") == 0)
        {
            //--- Formatting the number
            if ((res = api->HtmlWrite(L"%u", user->Leverage())) != MT_RET_OK)
            {
                if (logins) api->Free(logins);
                user->Release();
                return(res);
            }
            continue;
        }
    }
    //---
    if (logins) api->Free(logins);
    user->Release();
    //--- Check if the template processing has been finished successfully
    if (res != MT_RET_REPORT_TEMPLATE_END) return(res);
    //--- Successful
    return(MT_RET_OK);
}
//+------------------------------------------------------------------+