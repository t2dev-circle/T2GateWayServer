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
#ifndef ResponseMgrH
#define ResponseMgrH

#include <queue>
#include <mutex>
#include <condition_variable>
#include "util/tc_singleton.h"
#include "util/tc_thread_pool.h"
#include "Public.h"
#include "concurrentqueue.h"

using namespace moodycamel;

class ResponseMgr : public TC_Singleton<ResponseMgr>
{
public:
    ResponseMgr();
    ~ResponseMgr();
public:
    bool init();
    void release();
public:
    bool add(ApiResponsePtr &apiRspPtr);
    bool process(ApiResponsePtr &apiRspPtr);
private:
    bool threadFunc();
private:
    ConcurrentQueue<ApiResponsePtr> m_rspQueue;
    std::atomic_bool m_exit;
    std::thread m_thread;
    std::mutex m_mutex;
    std::condition_variable m_cond;
};

#endif // ResponseMgrH
