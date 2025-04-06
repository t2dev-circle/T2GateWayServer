/*
 * MIT License
 *
 * Copyright (c) 2025 t2dev-circle
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the Software), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * Software.
 */
#include "ApiHelper.h"
#include "util/tc_encoder.h"
#include "util/tc_timeprovider.h"

using namespace tars;

bool ApiHelper::packRequest(const T2App::T2TarsRequest &req, IBizMessage *sendMsg)
{
    if (sendMsg == nullptr)
    {
        return false;
    }

    const map<string, string> &data = req.data;
    if (data.size() == 0)
    {
        return false;
    }

    IF2Packer *packer = NewPacker(2);
    if (packer == nullptr)
    {
        return false;
    }

    packer->AddRef();
    packer->BeginPack();

    for (auto iter = data.begin(); iter != data.end(); ++iter)
    {
        packer->AddField(iter->first.c_str(), 'S', iter->second.length());
    }

    for (auto iter = data.begin(); iter != data.end(); ++iter)
    {
        packer->AddStr(iter->second.c_str());
    }

    packer->EndPack();

    sendMsg->SetContent(packer->GetPackBuf(), packer->GetPackLen());

    packer->FreeMem(packer->GetPackBuf());
    packer->Release();
    packer = nullptr;
    
    return true;
}

bool ApiHelper::packResponse(const ApiResponsePtr& apiRspPtr, T2App::T2TarsResponse& rsp)
{
    if ((apiRspPtr.get() == nullptr) || (apiRspPtr->code != 0))
    {
        return false;
    }

    const void *buffer = apiRspPtr->data.c_str();
    int len = apiRspPtr->data.size();

    IF2UnPacker *unpacker = NewUnPacker((void *)buffer, len);
    if (unpacker == nullptr)
    {
        return false;
    }

    bool result = true;
    
    try
    {
        unpacker->AddRef();

        unpacker->SetCurrentDatasetByIndex(0);
        unpacker->First();

        int errNo = unpacker->GetInt("error_no");
        const char *errMsg = unpacker->GetStr("error_info");
        
        rsp.code = errNo;
        rsp.msg = (errMsg == nullptr ? "" : TC_Encoder::gbk2utf8(errMsg));

        unpacker->SetCurrentDatasetByIndex(1);

        int rowCount = unpacker->GetRowCount();
        int colCount = unpacker->GetColCount();

        FDLOG("rsp") << "helper|" << apiRspPtr->reqId << "|" << rowCount << "|" << colCount << endl;

        for (auto i = 0; i < rowCount; i++)
        {
            map<string, string> record;

            for (auto j = 0; j < colCount; j++)
            {
                string colName = unpacker->GetColName(j);
                char colType = unpacker->GetColType(j);
                string colValue = "";

                switch (colType)
                {
                case 'I':
                    getInt(unpacker, j, colValue);
                    break;
                case 'F':
                    getDouble(unpacker, j, colValue);
                    break;
                case 'C':
                    getChar(unpacker, j, colValue);
                    break;
                case 'S':
                    getString(unpacker, j, colValue);
                    break;
                case 'R':
                    getRaw(unpacker, j, colValue);
                    break;
                default:
                    break;
                }

                colValue = TC_Encoder::gbk2utf8(colValue);

                record.insert(make_pair(colName, colValue));
            }

            rsp.data.push_back(record);

            FDLOG("rsp") << "helper|" << apiRspPtr->reqId << "|" << i << "|" << TC_Common::tostr(record) << endl;

            unpacker->Next();
        }

        unpacker->Release();
    }
    catch(exception& e)
    {
        result = false;
        FDLOG("rsp") << "helper|" << apiRspPtr->reqId << "|exception: " << e.what() << endl;
    }
    catch (...)
    {
        result = false;
        FDLOG("rsp") << "helper|" << apiRspPtr->reqId << "|exception: " << "unknown" << endl;
    }

    return result;
}

time_t ApiHelper::getCost(const ApiRequestPtr &apiReqPtr)
{
    if (apiReqPtr.get() == nullptr)
    {
        return 0;
    }

    return (TC_TimeProvider::getInstance()->getNowMs() - apiReqPtr->createTime);
}

bool ApiHelper::getInt(IF2UnPacker *unpacker, int index, string &value)
{
    value = "";

    if (unpacker == nullptr)
    {
        return false;
    }

    value = std::to_string(unpacker->GetIntByIndex(index));
    
    return true;
}

bool ApiHelper::getDouble(IF2UnPacker *unpacker, int index, string &value)
{
    value = "";

    if (unpacker == nullptr)
    {
        return false;
    }

    value = std::to_string(unpacker->GetDoubleByIndex(index));

    return true;
}

bool ApiHelper::getChar(IF2UnPacker *unpacker, int index, string &value)
{
    value = "";

    if (unpacker == nullptr)
    {
        return false;
    }

    char data = unpacker->GetCharByIndex(index);

    value = string(1, data);
    
    return true;
}

bool ApiHelper::getString(IF2UnPacker *unpacker, int index, string &value)
{
    value = "";

    if (unpacker == nullptr)
    {
        return false;
    }

    const char *data = unpacker->GetStrByIndex(index);
    if (data == nullptr)
    {
        return false;
    }

    value = data;

    return true;
}

bool ApiHelper::getRaw(IF2UnPacker *unpacker, int index, string &value)
{
    value = "";

    if (unpacker == nullptr)
    {
        return false;
    }

    int len = 0;
    void *data = unpacker->GetRawByIndex(index, &len);

    if (data == nullptr || len <= 0)
    {
        return false;
    }

    value = string((const char *)data, len);

    return true;
}
