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
#include "ConfigMgr.h"
#include "servant/Application.h"

bool ConfigMgr::init()
{
    m_confFile.parseFile(ServerConfig::BasePath + ServerConfig::ServerName + ".conf");

    const string baseKey = "/conf";
    const string connKey = baseKey + "/conn";
    const string t2sdkKey = baseKey + "/t2sdk";
    const string safeKey = baseKey + "/safe";
    const string bizKey = baseKey + "/biz";
    string curKey = "";

    // 是否测试（直接返回应答）
    curKey = baseKey + "/" + "<test>";
    m_confData.m_test = TC_Common::strto<bool>(m_confFile.get(curKey, "0"));

    // 单个服务地址的连接数
    curKey = connKey + "/" + "<conn_num>";
    m_confData.m_connNum = TC_Common::strto<int>(m_confFile.get(curKey, "1"));

    // 连接超时（单位毫秒）
    curKey = connKey + "/" + "<conn_timeout>";
    m_confData.m_connTimeout = TC_Common::strto<int>(m_confFile.get(curKey, "5000"));

    // 多个服务地址，按逗号分隔
    // 示例：121.41.126.194:9359;121.41.126.194:9359
    curKey = connKey + "/" + "<main_servers>";
    string mainServers = m_confFile.get(curKey, "121.41.126.194:9359");
    m_mainServers = TC_Common::sepstr<string>(mainServers, ";");

    curKey = connKey + "/" + "<backup_servers>";
    string backupServers = m_confFile.get(curKey, "121.41.126.194:9359");
    m_backupServers = TC_Common::sepstr<string>(backupServers, ";");

    // 系统请求编号(一般：2-普通 3-信用 5-个股期权，具体以柜台为准)
    curKey = connKey + "/" + "<system_no>";
    m_confData.m_systemNo = TC_Common::strto<int>(m_confFile.get(curKey, "2"));

    // 对应SDK配置文件中t2sdk配置
    m_confData.m_t2sdk = m_confFile.getDomainMap(t2sdkKey);
    
    // 对应SDK配置文件中safe配置
    m_confData.m_safe = m_confFile.getDomainMap(safeKey);

    // 业务请求超时（毫秒）
    curKey = bizKey + "/" + "<req_timeout>";
    m_confData.m_reqTimeout = TC_Common::strto<int>(m_confFile.get(curKey, "10000"));

    LOG_DEBUG << "config: " << "test=" << m_confData.m_test << endl;
    LOG_DEBUG << "config: " << "connNum=" << m_confData.m_connNum << "|connTimeout=" << m_confData.m_connTimeout << endl;
    LOG_DEBUG << "config: " << "mainServers=" << TC_Common::tostr(m_mainServers) << endl;
    LOG_DEBUG << "config: " << "backupServers=" << TC_Common::tostr(m_backupServers) << endl;
    LOG_DEBUG << "config: " << "t2sdk=" << TC_Common::tostr(m_confData.m_t2sdk) << endl;
    LOG_DEBUG << "config: " << "safe=" << TC_Common::tostr(m_confData.m_safe) << endl;
    LOG_DEBUG << "config: " << "reqTimeout=" << m_confData.m_reqTimeout << endl;

    return true;
}

const ConfigData & ConfigMgr::getConfig() const
{
    return m_confData;
}

const std::vector<std::string> & ConfigMgr::getMainServers() const
{
    return m_mainServers;
}

const std::vector<std::string> & ConfigMgr::getBackupServers() const
{
    return m_backupServers;
}

const bool ConfigMgr::isTest() const
{
    return m_confData.m_test;
}
