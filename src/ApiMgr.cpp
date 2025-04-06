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
#include "ApiMgr.h"
#include "servant/Application.h"
#include "Api.h"
#include "ConfigMgr.h"

ApiMgr::ApiMgr()
{
    m_mainIndex = 0;
    m_backupIndex = 0;
    m_reqId = 0;
}

ApiMgr::~ApiMgr()
{
    release();
}

bool ApiMgr::init()
{
    const ConfigData &configData = ConfigMgr::getInstance()->getConfig();
    const std::vector<std::string> &mainServers = ConfigMgr::getInstance()->getMainServers();
    const std::vector<std::string> &backupServers = ConfigMgr::getInstance()->getBackupServers();
    
    // 分别创建主备连接
    for (int i = 0; i < configData.m_connNum; i++)
    {
        bool result = initMain(mainServers, configData);
        LOG_DEBUG << "ApiMgr initMain: " << i << "|" << TC_Common::tostr(mainServers) << "|" << (result ? "OK" : "failed") << endl;

        result = initBackup(backupServers, configData);
        LOG_DEBUG << "ApiMgr initBackup: " << i << "|" << TC_Common::tostr(backupServers) << "|" << (result ? "OK" : "failed") << endl;
    }

    LOG_DEBUG << "ApiMgr init: " << "main size=" << m_mainApis.size() << "|backup size=" << m_backupApis.size() << endl;

    return (m_mainApis.size() > 0 || m_backupApis.size() > 0);
}

void ApiMgr::release()
{
    std::unique_lock<std::mutex> lock(m_mutex);

    for (size_t i = 0; i < m_mainApis.size(); i++)
    {
        ApiPtr &ptr = m_mainApis[i];

        ptr->release();
    }

    std::vector<ApiPtr> mainApis;
    m_mainApis.swap(mainApis);

    for (size_t i = 0; i < m_backupApis.size(); i++)
    {
        ApiPtr &ptr = m_backupApis[i];

        ptr->release();
    }

    std::vector<ApiPtr> backupApis;
    m_backupApis.swap(backupApis);
}

bool ApiMgr::initMain(const std::vector<std::string> &mainServers, const ConfigData &configData)
{
    const size_t mainSize = mainServers.size();

    for (size_t i = 0; i < mainSize; i++)
    {
        const string &server = mainServers[i];

        ApiPtr ptr = new Api();
        if (!ptr->init(server, configData))
        {
            FDLOG("api") << "init main failed|" << ptr->getErrMsg() << endl;

            continue;
        }

        if (!ptr->connect(configData.m_connTimeout))
        {
            FDLOG("api") << "connect main failed|" << ptr->getErrMsg() << endl;

            continue;
        }

        m_mainApis.push_back(ptr);
    }

    return (m_mainApis.size() > 0);
}

bool ApiMgr::initBackup(const std::vector<std::string> &backupServers, const ConfigData &configData)
{
    const size_t backupSize = backupServers.size();

    for (size_t i = 0; i < backupSize; i++)
    {
        const string &server = backupServers[i];

        ApiPtr ptr = new Api();
        if (!ptr->init(server, configData))
        {
            FDLOG("api") << "init backup failed|" << ptr->getErrMsg() << endl;

            continue;
        }

        if (!ptr->connect(configData.m_connTimeout))
        {
            FDLOG("api") << "connect backup failed|" << ptr->getErrMsg() << endl;

            continue;
        }

        m_backupApis.push_back(ptr);
    }

    return (m_backupApis.size() > 0);
}

ApiPtr ApiMgr::get()
{
    // 先取主连接，主不可用则再取备连接
    ApiPtr apiPtr = getMain();

    FDLOG("api") << "get api main|" << apiPtr.get() << endl;

    if (apiPtr.get() == nullptr)
    {
        apiPtr = getBackup();

        FDLOG("api") << "get api backup|" << apiPtr.get() << endl;
    }

    return apiPtr;
}

ApiPtr ApiMgr::getMain()
{
    if (m_mainApis.size() == 0)
    {
        return nullptr;
    }

    ApiPtr apiPtr = nullptr;
    const int apiSize = (int)m_mainApis.size();

    for (;;)
    {
        m_mainIndex = (m_mainIndex + 1) % apiSize; 

        if (m_mainApis[m_mainIndex]->isValid())
        {
            apiPtr = m_mainApis[m_mainIndex];
            break;
        }

        if (m_mainIndex >= apiSize)
        {
            break;
        }
    }

    return apiPtr;
}

ApiPtr ApiMgr::getBackup()
{
    if (m_backupApis.size() == 0)
    {
        return nullptr;
    }

    ApiPtr apiPtr = nullptr;
    const int apiSize = (int)m_backupApis.size();

    for (;;)
    {
        m_backupIndex = (m_backupIndex + 1) % apiSize; 

        if (m_backupApis[m_backupIndex]->isValid())
        {
            apiPtr = m_backupApis[m_backupIndex];
            break;
        }

        if (m_backupIndex >= apiSize)
        {
            break;
        }
    }

    return apiPtr;
}

int ApiMgr::genReqId()
{
    ++m_reqId;

    if (m_reqId > MAX_REQ_ID)
    {
        m_reqId = 1;
    }

    return m_reqId;
}
