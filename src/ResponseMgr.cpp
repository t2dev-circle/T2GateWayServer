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
#include "ResponseMgr.h"
#include "RequestMgr.h"
#include "ApiHelper.h"
#include "T2TarsObj.h"

ResponseMgr::ResponseMgr()
{
    m_exit = false;
}

ResponseMgr::~ResponseMgr()
{
}

bool ResponseMgr::init()
{
    m_exit = false;
    m_thread = std::thread(std::bind(&ResponseMgr::threadFunc, this));

    return true;
}

void ResponseMgr::release()
{
    m_exit = true;

    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cond.notify_all();
    }

    if (m_thread.joinable())
    {
        m_thread.join();
    }
}

bool ResponseMgr::add(ApiResponsePtr &apiRspPtr)
{
    FDLOG("rsp") << "add|" << apiRspPtr->reqId << "|" << apiRspPtr->code << "|" << apiRspPtr->msg << endl;

    // 收到应答则删除请求
    ApiRequestPtr apiReqPtr = RequestMgr::getInstance()->remove(apiRspPtr->reqId);
    if (apiReqPtr.get() == nullptr)
    {
        FDLOG("rsp") << "add|" << apiRspPtr->reqId << "|" << apiRspPtr->code << "|" << apiRspPtr->msg << "|failed" << endl;

        return false;
    }

    apiRspPtr->reqPtr = apiReqPtr;

    // 放入应答队列
    bool result = m_rspQueue.enqueue(apiRspPtr);

    // 通知线程处理数据
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cond.notify_one();
    }

    FDLOG("rsp") << "add|" << apiRspPtr->reqId << "|" << apiRspPtr->code << "|" << apiRspPtr->msg << "|" 
                 << apiReqPtr->req.seqNo << "|" << apiReqPtr->req.funcNo << "|" << (result ? "OK" : "failed") << endl;

    return true;
}

bool ResponseMgr::process(ApiResponsePtr &apiRspPtr)
{
    FDLOG("rsp") << "process|" << apiRspPtr->reqId << "|" << apiRspPtr->code << "|" << apiRspPtr->msg << endl;

    T2App::T2TarsResponse rsp;

    // 将T2SDK格式应答转换Tars格式应答
    if (!ApiHelper::packResponse(apiRspPtr, rsp))
    {
        rsp.code = -1;
        rsp.msg = "Response packing failed";

        T2App::T2TarsObj::async_response_invoke(apiRspPtr->reqPtr->current, rsp.code, rsp);

        FDLOG("rsp") << "process|" << apiRspPtr->reqId << "|" << apiRspPtr->code << "|" << apiRspPtr->msg << "|failed" << endl;

        return false;
    }

    // 发送应答数据
    T2App::T2TarsObj::async_response_invoke(apiRspPtr->reqPtr->current, rsp.code, rsp);

    FDLOG("rsp") << "process|" << apiRspPtr->reqId << "|" << apiRspPtr->code << "|" << apiRspPtr->msg << "|" 
                 << rsp.seqNo << "|" << rsp.code << "|" << rsp.msg << "|OK|" 
                 << ApiHelper::getCost(apiRspPtr->reqPtr) << "ms" << endl;

    return true;
}

bool ResponseMgr::threadFunc()
{
    LOG_DEBUG << "ResponseMgr thread start" << endl;

    while (!m_exit)
    {
        // 等待队列数据
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_cond.wait(lock, [this] { return (m_exit || m_rspQueue.size_approx() > 0); });
        }

        if (m_exit)
        {
            break;
        }

        ApiResponsePtr apiRspPtr;
        // 取应答数据
        if (!m_rspQueue.try_dequeue(apiRspPtr))
        {
            continue;
        }

        // 处理应答数据
        process(apiRspPtr);
    }

    LOG_DEBUG << "ResponseMgr thread end" << endl;

    return true;
}
