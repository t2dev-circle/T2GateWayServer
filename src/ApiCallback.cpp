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
#include "ApiCallback.h"
#include "Public.h"
#include "ResponseMgr.h"
#include "util/tc_encoder.h"
#include "servant/Application.h"

ApiCallback::ApiCallback()
{
}

ApiCallback::~ApiCallback()
{
}

unsigned long ApiCallback::QueryInterface(const char *iid, IKnown **ppv)
{
    return 0;
}

unsigned long ApiCallback::AddRef()
{
    return 0;
}

unsigned long ApiCallback::Release()
{
    return 0;
}

void ApiCallback::OnConnect(CConnectionInterface *lpConnection)
{
    FDLOG("callback") << "OnConnect|" << lpConnection << endl;
}

void ApiCallback::OnSafeConnect(CConnectionInterface *lpConnection)
{
    FDLOG("callback") << "OnSafeConnect|" << lpConnection << endl;
}

void ApiCallback::OnRegister(CConnectionInterface *lpConnection)
{
    FDLOG("callback") << "OnRegister|" << lpConnection << endl;
}

void ApiCallback::OnClose(CConnectionInterface *lpConnection)
{
    FDLOG("callback") << "OnClose|" << lpConnection << endl;
}

void ApiCallback::OnSent(CConnectionInterface *lpConnection, int hSend, void *reserved1, void *reserved2, int nQueuingData)
{
    FDLOG("callback") << "OnSent|" << hSend << "|" << lpConnection << endl;
}

void ApiCallback::Reserved1(void *a, void *b, void *c, void *d)
{

}

void ApiCallback::Reserved2(void *a, void *b, void *c, void *d)
{
    
}

int ApiCallback::Reserved3()
{
    return 0;
}

void ApiCallback::Reserved4()
{

}

void ApiCallback::Reserved5()
{
    
}

void ApiCallback::Reserved6()
{
    
}

void ApiCallback::Reserved7()
{
    
}

void ApiCallback::OnReceivedBiz(CConnectionInterface *lpConnection, int hSend, const void *lpUnPackerOrStr, int nResult)
{

}

void ApiCallback::OnReceivedBizEx(CConnectionInterface *lpConnection, int hSend, LPRET_DATA lpRetData, const void *lpUnpackerOrStr, int nResult)
{

}

void ApiCallback::OnReceivedBizMsg(CConnectionInterface *lpConnection, int hSend, IBizMessage *lpMsg)
{
    FDLOG("callback") << "OnReceivedBizMsg|" << hSend << "|" << lpConnection << "|" << lpMsg << endl;

    if (lpMsg == nullptr)
    {
        return ;
    }

    int len = 0;
    ApiResponsePtr apiRspPtr = new ApiResponse();

    apiRspPtr->reqId = lpMsg->GetPacketId();
    apiRspPtr->code  = lpMsg->GetErrorNo();

    if (apiRspPtr->code != 0)
    {
        apiRspPtr->msg = TC_Encoder::gbk2utf8(lpMsg->GetErrorInfo());
    }
    else
    {        
        const void *buffer = lpMsg->GetContent(len);

        apiRspPtr->data = string((const char *)buffer, len);
        apiRspPtr->msg = "";
    }

    FDLOG("callback") << "OnReceivedBizMsg|" << hSend << "|" << lpConnection << "|" << lpMsg << "|" 
                      << apiRspPtr->reqId << "|" << apiRspPtr->code << "|" << apiRspPtr->msg << "|"
                      << len << "|" << apiRspPtr->data.size() << endl;

    // 加入应答队列
    ResponseMgr::getInstance()->add(apiRspPtr);
}
