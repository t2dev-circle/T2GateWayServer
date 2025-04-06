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
#include "Api.h"
#include "Public.h"
#include "RequestMgr.h"
#include "ApiHelper.h"
#include "ApiMgr.h"
#include "util/tc_timeprovider.h"

Api::Api()
{
    m_config = nullptr;
    m_conn = nullptr;
    m_errMsg = "";
    m_systemNo = 2; 
}

Api::~Api()
{
    release();
}

bool Api::init(const std::string &server, const ConfigData &configData)
{
    m_config = NewConfig();
    if (m_config == nullptr)
    {
        m_errMsg = "NewConfig failed";

        return false;
    }

    m_config->AddRef();

    // 保存系统编号（用于发送业务请求使用）
    m_systemNo = configData.m_systemNo;

    // 设置服务地址(格式：ip:port)
    m_config->SetString("t2sdk", "servers", server.c_str());
    // 设置自动重连
    m_config->SetString("t2sdk", "auto_reconnect", "1"); 

    LOG_DEBUG << "t2sdk: " << "servers|" << server << endl;

    // t2sdk配置项设置
    const string t2sdkSection = "t2sdk";
    const string safeSection = "safe";
    const string licenseFileKey = "license_file";
    const string certFileKey = "cert_file";
    const string t2sdkBasePath = ServerConfig::BasePath + t2sdkSection + "/";
    const string safeBasePath = ServerConfig::BasePath + safeSection + "/";
    string t2sdkFilePath = "";
    string certFilePath = "";

    for (auto t2sdkIter = configData.m_t2sdk.begin(); t2sdkIter != configData.m_t2sdk.end(); ++t2sdkIter)
    {
        if (t2sdkIter->first == licenseFileKey)
		{
			t2sdkFilePath = t2sdkBasePath + t2sdkIter->second;

			m_config->SetString(t2sdkSection.c_str(), t2sdkIter->first.c_str(), t2sdkFilePath.c_str());

			LOG_DEBUG << "t2sdk: " << t2sdkIter->first << "|" << t2sdkIter->second << "|" << t2sdkFilePath << endl;
		}
		else
		{
			m_config->SetString(t2sdkSection.c_str(), t2sdkIter->first.c_str(), t2sdkIter->second.c_str());

			LOG_DEBUG << "t2sdk: " << t2sdkIter->first << "|" << t2sdkIter->second << endl;
		}
    }

    // safe配置项设置
    for (auto safeIter = configData.m_safe.begin(); safeIter != configData.m_safe.end(); ++safeIter)
    {
        if (safeIter->first == certFileKey)
		{
			certFilePath = safeBasePath + safeIter->second;

			m_config->SetString(safeSection.c_str(), safeIter->first.c_str(), certFilePath.c_str());

			LOG_DEBUG << "safe: " << safeIter->first << "|" << safeIter->second << certFilePath << "|" << endl;
		}
		else
		{
			m_config->SetString(safeSection.c_str(), safeIter->first.c_str(), safeIter->second.c_str());

			LOG_DEBUG << "safe: " << safeIter->first << "|" << safeIter->second << endl;
		}
    }

    return true;
}

void Api::release()
{
    try
    {
        if (m_conn != nullptr)
        {
            m_conn->Release();
            m_conn = nullptr;
        }

        if (m_config != nullptr)
        {
            m_config->Release();
            m_config = nullptr;
        }
    }
    catch(exception& e)
    {
        LOG_DEBUG << "api release exception: " << e.what() << endl;
    }
    catch (...)
    {
        LOG_DEBUG << "api release unknown exception"<< endl;
    }
}

bool Api::connect(int timeout)
{
    if (timeout < DEFAULT_TIMEOUT)
    {
        timeout = DEFAULT_TIMEOUT;
    }

    m_errMsg = "";

    if (m_config == nullptr)
    {
        m_errMsg = "CConfigInterface invalid";

        return false;
    }

    if (m_conn != nullptr)
    {
        m_conn->Release();
        m_conn = nullptr;
    }

    m_conn = NewConnection(m_config);
    if (m_conn == nullptr)
    {
        m_errMsg = "NewConnection failed";

        return false;
    }

    m_conn->AddRef();

    int ret = m_conn->Create2BizMsg(&m_callback);
    if (ret != 0)
    {
        m_errMsg = "Create2BizMsg failed";

        return false;
    }

    // 超时单位毫秒
    ret = m_conn->Connect(timeout);
    if (ret != 0)
    {
        m_errMsg = m_conn->GetErrorMsg(ret);

        return false;
    }

    return true;
}

bool Api::isValid()
{
    if (m_conn == nullptr)
    {
        return false;
    }

    int connStatus = m_conn->GetStatus();

    return (connStatus & CConnectionInterface::Registered);
}

bool Api::send(const T2App::T2TarsRequest &req, T2App::T2TarsResponse &rsp, tars::TarsCurrentPtr current)
{
    m_errMsg = "";

    if (m_conn == nullptr)
    {
        m_errMsg = "CConnectionInterface invalid";

        rsp.code = -1;
        rsp.msg = m_errMsg;

        return false;
    }

    IBizMessage *sendMsg = NewBizMessage();
    if (sendMsg == nullptr)
    {
        m_errMsg = "NewBizMessage failed";

        rsp.code = -1;
        rsp.msg = m_errMsg;

        return false;
    }

    const int reqId = ApiMgr::getInstance()->genReqId();

    sendMsg->AddRef();

    sendMsg->SetFunction(req.funcNo);
	sendMsg->SetPacketType(REQUEST_PACKET);
    sendMsg->SetSystemNo(m_systemNo); // 2-普通 3-信用 5-个股期权（具体以柜台为准）
    sendMsg->SetPacketId(reqId);

    // 将Tars请求包转换为T2SDK请求包
    if (!ApiHelper::packRequest(req, sendMsg))
    {
        m_errMsg = "Request packing failed";

        rsp.code = -1;
        rsp.msg = m_errMsg;

        sendMsg->Release();
        sendMsg = nullptr;

        return false;
    }

    // 保存请求
    ApiRequestPtr apiReqPtr = new ApiRequest();
    apiReqPtr->req = req;
    apiReqPtr->current = current;
    apiReqPtr->reqId = reqId;
    apiReqPtr->createTime = TC_TimeProvider::getInstance()->getNowMs();

    // 加入请求队列
    if (!RequestMgr::getInstance()->add(apiReqPtr))
    {
        m_errMsg = "Request adding failed";

        rsp.code = -1;
        rsp.msg = m_errMsg;

        sendMsg->Release();
        sendMsg = nullptr;

        return false;
    }

    // 异步请求(在回调类中接收应答数据)
    const int asyncReq = 1;
    int ret = m_conn->SendBizMsg(sendMsg, asyncReq);
    if (ret < 0)
    {
        // 删除请求
        RequestMgr::getInstance()->remove(apiReqPtr->reqId);

        m_errMsg = m_conn->GetErrorMsg(ret);

        rsp.code = -1;
        rsp.msg = m_errMsg;

        sendMsg->Release();
        sendMsg = nullptr;

        return false;
    }

    sendMsg->Release();
    sendMsg = nullptr;

    return true;
}

bool Api::process(const T2App::T2TarsRequest &req, T2App::T2TarsResponse &rsp, tars::TarsCurrentPtr current)
{
    // 发送请求（如果发送失败者直接返回应答包，否则通过T2SDK回调类处理后返回应答包）
    if (!send(req, rsp, current))
    {
        return false;
    }

    // 设置异步发送应答数据（发送请求成功则默认是异步返回应答）
    current->setResponse(false);

    return true;
}

string Api::getErrMsg() const
{
    return m_errMsg;
}
