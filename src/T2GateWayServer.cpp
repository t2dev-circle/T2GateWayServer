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
#include "T2GateWayServer.h"
#include "T2TarsObjImp.h"
#include "ConfigMgr.h"
#include "ApiMgr.h"
#include "RequestMgr.h"
#include "ResponseMgr.h"

using namespace std;

// 本地启动：./T2GateWayServer --config=T2GateWayServer.config.conf

T2GateWayServer g_app;

/////////////////////////////////////////////////////////////////
void
T2GateWayServer::initialize()
{
    //initialize application here:

    addServant<T2TarsObjImp>(ServerConfig::Application + "." + ServerConfig::ServerName + ".T2TarsObj");

    LOG_DEBUG << "initialize start" << endl;

    // 配置初始化
    if (!ConfigMgr::getInstance()->init())
    {
        LOG_DEBUG << "ConfigMgr init failed" << endl;
        
        exit(0);
    }

    // 请求管理初始化
    if (!RequestMgr::getInstance()->init())
    {
        LOG_DEBUG << "RequestMgr init failed" << endl;
        
        exit(0);
    }

    // 应答管理初始化
    if (!ResponseMgr::getInstance()->init())
    {
        LOG_DEBUG << "ResponseMgr init failed" << endl;
        
        exit(0);
    }

    // API管理初始化
    if (!ApiMgr::getInstance()->init())
    {
        LOG_DEBUG << "ResponseMgr init failed" << endl;
        
        exit(0);
    }

    LOG_DEBUG << "initialize end" << endl;
}
/////////////////////////////////////////////////////////////////
void
T2GateWayServer::destroyApp()
{
    //destroy application here:

    LOG_DEBUG << "destroyApp start" << endl;

    // 请求管理资源释放
    LOG_DEBUG << "RequestMgr release" << endl;
    RequestMgr::getInstance()->release();

    // 应答管理资源释放
    LOG_DEBUG << "ResponseMgr release" << endl;
    ResponseMgr::getInstance()->release();

    // API管理资源释放
    LOG_DEBUG << "ApiMgr release" << endl;
    ApiMgr::getInstance()->release();

    LOG_DEBUG << "destroyApp end" << endl;
}
/////////////////////////////////////////////////////////////////
int
main(int argc, char* argv[])
{
    try
    {
        g_app.main(argc, argv);
        g_app.waitForShutdown();
    }
    catch (std::exception& e)
    {
        cerr << "std::exception:" << e.what() << std::endl;
    }
    catch (...)
    {
        cerr << "unknown exception." << std::endl;
    }
    return -1;
}
/////////////////////////////////////////////////////////////////
