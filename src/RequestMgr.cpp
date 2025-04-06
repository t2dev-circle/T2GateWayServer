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
#include "RequestMgr.h"
#include "ConfigMgr.h"
#include "T2TarsObj.h"
#include "ApiHelper.h"

RequestMgr::RequestMgr()
{
    m_exit = false;
}

RequestMgr::~RequestMgr()
{
}

bool RequestMgr::init()
{
    const ConfigData &configData = ConfigMgr::getInstance()->getConfig();

    m_queuePtr = new ApiRequestQueue(configData.m_reqTimeout);

    m_exit = false;
    m_thread = std::thread(std::bind(&RequestMgr::threadFunc, this));

    return true;
}

void RequestMgr::release()
{
    m_exit = true;
    
    if (m_thread.joinable())
    {
        m_thread.join();
    }
}

bool RequestMgr::add(const ApiRequestPtr &apiReqPtr)
{
    if (m_queuePtr.get() == nullptr)
    {
        FDLOG("api") << "push|" << apiReqPtr->reqId << "|" 
                 << apiReqPtr->req.seqNo << "|" << apiReqPtr->req.funcNo << "|" << "invalid queue" << endl;

        return false;
    }

    bool result = m_queuePtr->push(apiReqPtr, apiReqPtr->reqId);

    FDLOG("api") << "push|" << apiReqPtr->reqId << "|" 
                 << apiReqPtr->req.seqNo << "|" << apiReqPtr->req.funcNo << "|" << (result ? "OK" : "failed") << endl;

    return result;
}

ApiRequestPtr RequestMgr::remove(int reqId)
{
    ApiRequestPtr apiReqPtr = m_queuePtr->erase(reqId);

    if (apiReqPtr.get() == nullptr)
    {
        FDLOG("api") << "remove|" << reqId << "|failed" << endl;
    }
    else
    {
        FDLOG("api") << "remove|" << apiReqPtr->reqId << "|" 
                 << apiReqPtr->req.seqNo << "|" << apiReqPtr->req.funcNo << "|OK" << endl;
    }

    return apiReqPtr;
}

void RequestMgr::processTimeout(ApiRequestPtr &apiReqPtr)
{
    if (apiReqPtr.get() == nullptr)
    {
        FDLOG("api") << "timeout|" << "request invalid" << endl;

        return ;
    }

    T2App::T2TarsResponse rsp;

    rsp.code = -1;
    rsp.msg = "Request processing timeout";
    rsp.seqNo = apiReqPtr->reqId;

    // 发送应答数据
    T2App::T2TarsObj::async_response_invoke(apiReqPtr->current, rsp.code, rsp);

    FDLOG("rsp") << "timeout|" << apiReqPtr->reqId << "|" 
                 << rsp.seqNo << "|" << rsp.code << "|" << rsp.msg << "|"
                 << ApiHelper::getCost(apiReqPtr) << "|" << endl;
}

bool RequestMgr::threadFunc()
{
    ApiRequestQueue::data_functor func(&RequestMgr::processTimeout);

    LOG_DEBUG << "RequestMgr thread start: timeout=" << m_queuePtr->getTimeout() << endl;

    while (!m_exit)
    {
        if (m_queuePtr->empty())
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }

        // 处理超时请求
        m_queuePtr->timeout(func);

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    LOG_DEBUG << "RequestMgr thread exit" << endl;

    return true;
}
