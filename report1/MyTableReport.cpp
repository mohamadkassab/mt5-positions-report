#include "pch.h"
#include "MyTableReport.h"
#include <iostream>
#include <vector>
#include <codecvt>
#include <map>
#include <tuple>
#include <string>

//+------------------------------------------------------------------+
//| Plugin description structure                                       |
//+------------------------------------------------------------------+
const MTReportInfo CMyTableReport::s_info =
{
 100,                                        // Updated version number
 MTReportAPIVersion,                        // API version remains the same
 MTReportInfo::IE_VERSION_ANY,              // IE version requirement
 L"Positions Report ",                    // Updated report name
 L"Copyright 2024, Your Company Name",      // Updated copyright
 L"Positions report filtered by groups and login",  // Updated description
 MTReportInfo::SNAPSHOT_POSITIONS_FULL,      // Example snapshot mode
 MTReportInfo::TYPE_TABLE,                  // Report type (unchanged)
 { 0 },                                     // Reserved initialization
 {

     { MTReportParam::TYPE_GROUPS, MTAPI_PARAM_GROUPS, L"*" }, 
     { MTReportParam::TYPE_INT, L"Login", 0},

   },2                                  // Updated number of parameters
};

//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CMyTableReport::CMyTableReport(void)
{
}
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CMyTableReport::~CMyTableReport(void)
{
}
//+------------------------------------------------------------------+
//| Plugin release method                                            |
//+------------------------------------------------------------------+
void CMyTableReport::Release(void)
{
    delete this;
}
//+------------------------------------------------------------------+
//| Report generation method                                         |
//+------------------------------------------------------------------+
MTAPIRES CMyTableReport::Generate(const UINT type, IMTReportAPI* api)
{
    IMTDatasetColumn* column = nullptr;
    UINT64* logins = nullptr;
    IMTPositionArray* positions = nullptr;
    IMTPosition* current_position = nullptr;
    IMTConParam* login_param = nullptr;

    MTAPIRES          res;
    UINT              total_logins = 0;
    UINT total_positions = 0;
    std::map<std::string, std::tuple<UINT32, double, double, double, double>> summary;
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    double current_volume = 0.0;
    double current_price = 0.0;
    LPCWSTR current_symbol;

    try {
        //--- Checking for a pointer
        if (!api)
            return(MT_RET_ERR_PARAMS);
        //--- Checking the type of a requested report
        if (type != MTReportInfo::TYPE_TABLE)
            return(MT_RET_ERR_PARAMS);
        //--- creating a column object
        if ((column = api->TableColumnCreate()) == NULL)
        {
            return(MT_RET_ERR_MEM);
        }
        if ((positions = api->PositionCreateArray()) == NULL)
        {
            column->Release();
            return(MT_RET_ERR_MEM);
        }
        if ((current_position = api->PositionCreate()) == NULL)
        {
            column->Release();
            positions->Release();
            return(MT_RET_ERR_MEM);
        }
        if ((login_param = api->ParamCreate()) == NULL)
        {
            column->Release();
            positions->Release();
            return(MT_RET_ERR_MEM);
        }

        //--- Preparing the column 1
        column->Clear();
        column->Name(L"Symbol");
        column->ColumnID(1);
        column->Offset(offsetof(TableRecord, symbol));
        column->Type(IMTDatasetColumn::TYPE_STRING);
        column->Size(MtFieldSize(TableRecord, symbol));
        if ((res = api->TableColumnAdd(column)) != MT_RET_OK)
        {
            column->Release();
            positions->Release();
            login_param->Release();
            return(res);
        }
        //---

        //--- preparing the column 2
        column->Clear();
        column->Name(L"Total Positions");
        column->ColumnID(2);
        column->Offset(offsetof(TableRecord, total_positions));
        column->Type(IMTDatasetColumn::TYPE_UINT32);
        if ((res = api->TableColumnAdd(column)) != MT_RET_OK)
        {
            column->Release();
            positions->Release();
            login_param->Release();
            return(res);
        }
        //---
    
        //--- preparing the column 3
        column->Clear();
        column->Name(L"Buy Volumne");
        column->ColumnID(3);
        column->Offset(offsetof(TableRecord, buy_volume));
        column->Type(IMTDatasetColumn::TYPE_DOUBLE);
        if ((res = api->TableColumnAdd(column)) != MT_RET_OK)
        {
            column->Release();
            positions->Release();
            login_param->Release();
            return(res);
        }
        //---
    
        //--- preparing the column 4
        column->Clear();
        column->Name(L"Buy Price");
        column->ColumnID(4);
        column->Offset(offsetof(TableRecord, buy_price));
        column->Type(IMTDatasetColumn::TYPE_DOUBLE);
        if ((res = api->TableColumnAdd(column)) != MT_RET_OK)
        {
            column->Release();
            positions->Release();
            login_param->Release();
            return(res);
        }
        //---
   
        //--- preparing the column 5
        column->Clear();
        column->Name(L"Sell Volume");
        column->ColumnID(5);
        column->Offset(offsetof(TableRecord, sell_volume));
        column->Type(IMTDatasetColumn::TYPE_DOUBLE);
        if ((res = api->TableColumnAdd(column)) != MT_RET_OK)
        {
            column->Release();
            positions->Release();
            login_param->Release();
            return(res);
        }
        //---

        //--- preparing the column 6
        column->Clear();
        column->Name(L"Sell Price");
        column->ColumnID(6);
        column->Offset(offsetof(TableRecord, sell_price));
        column->Type(IMTDatasetColumn::TYPE_DOUBLE);
        if ((res = api->TableColumnAdd(column)) != MT_RET_OK)
        {
            column->Release();
            positions->Release();
            login_param->Release();
            return(res);
        }
        //---

        //--- preparing the column 7
        column->Clear();
        column->Name(L"Net Volume");
        column->ColumnID(7);
        column->Offset(offsetof(TableRecord, net_volume));
        column->Type(IMTDatasetColumn::TYPE_DOUBLE);
        if ((res = api->TableColumnAdd(column)) != MT_RET_OK)
        {
            column->Release();
            positions->Release();
            login_param->Release();
            return(res);
        }
        //---
   
        if ((res = api->ParamLogins(logins, total_logins)) != MT_RET_OK)
        {
            column->Release();
            positions->Release();
            login_param->Release();
            return(MT_RET_ERR_NOTFOUND);
        }
        if ((res = api->ParamGet(L"Login", login_param)) != MT_RET_OK)
        {
            column->Release();
            positions->Release();
            login_param->Release();
            return(MT_RET_ERR_NOTFOUND);
        }


        if (logins && total_logins)
        {
            for (UINT i = 0; i < total_logins; i++)
            { 
                if (login_param->ValueInt() == 0 || login_param->ValueInt() == logins[i]) {

                    if (res = api->PositionGet(logins[i], positions) == MT_RET_OK) {              
                        total_positions = positions->Total();
                        current_volume = 0.0;
                        current_price = 0.0;

                        if (positions && total_positions) {
                            for (UINT i = 0; i < total_positions; i++) {   
                                current_position = positions->Next(i);
                                if (current_position == nullptr) {
                                    continue;
                                }
                                current_symbol = current_position->Symbol();
                                if (current_symbol) {
                                    std::string key = converter.to_bytes(current_position->Symbol());
                                    if (summary.find(key) == summary.end())
                                    {
                                        summary[key] = std::make_tuple(0, 0.0, 0.0, 0.0, 0.0);
                                    }
                                    std::get<0>(summary[key]) += 1;

                                    current_volume = current_position->Volume() / 10000.0;
                                    current_price = current_position->PriceOpen();
                                    if (current_position->Action() == 0) {

                                        std::get<1>(summary[key]) += current_volume;
                                        std::get<2>(summary[key]) += current_price;
                                    }
                                    else if (current_position->Action() == 1) {
                                        std::get<3>(summary[key]) += current_volume;
                                        std::get<4>(summary[key]) += current_price;
                                    }
                                }
                               
                            }
                        }
                    }
                }         
            }

            for (const auto& pair : summary) {
                const std::string& key = pair.first;
                std::wstring wKey(key.begin(), key.end()); 
                LPCWSTR lpwKey = wKey.c_str();
                const auto& t = pair.second;
                TableRecord record = { 0 };
                CMTStr::Copy(record.symbol, lpwKey);
                record.total_positions = std::get<0>(t);
                record.buy_volume = std::get<1>(t);
                record.buy_price = std::get<2>(t);
                record.sell_volume = std::get<3>(t);
                record.sell_price = std::get<4>(t);
                record.net_volume = std::get<1>(t) - std::get<3>(t);
                res = api->TableRowWrite(&record, sizeof(record));         
            }

            column->Release();              
            positions->Release(); 
            login_param->Release();
            api->Free(logins);

            return(MT_RET_OK);
        }
        else {
            return(MT_RET_ERR_NOTFOUND);
        }
    }
    catch (const std::exception& e) {
        if (column != nullptr) {
            column->Release();
        }
        if (positions != nullptr) {
            positions->Release();
        }
        if (login_param != nullptr) {
            login_param->Release();
        }

        api->Free(logins);
        if (api) {
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter; 
            std::wstring errorMsg = L"An error occurred: " + converter.from_bytes(e.what()); 
            api->LoggerOut(2, errorMsg.c_str()); 
        }
    }
}

//+------------------------------------------------------------------+
