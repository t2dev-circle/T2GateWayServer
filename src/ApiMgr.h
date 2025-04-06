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
#ifndef ApiMgrH
#define ApiMgrH

#include <atomic>
#include <vector>
#include <string>
#include <mutex>
#include "util/tc_singleton.h"
#include "Api.h"
#include "Public.h"

class Api;

class ApiMgr : public TC_Singleton<ApiMgr>
{
public:
    ApiMgr();
    ~ApiMgr();
public:
    bool init();
    void release();
public:
    ApiPtr get();
    int genReqId();
private:
    bool initMain(const std::vector<std::string> &mainServers, const ConfigData &configData);
    bool initBackup(const std::vector<std::string> &mainServers, const ConfigData &configData);

    ApiPtr getMain();
    ApiPtr getBackup();
private:
    std::vector<ApiPtr> m_mainApis;
    std::vector<ApiPtr> m_backupApis;
    std::atomic<int> m_mainIndex;
    std::atomic<int> m_backupIndex;
    std::atomic<int> m_reqId;
    std::mutex m_mutex;
};

#endif // ApiMgrH
