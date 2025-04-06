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
#ifndef PUBLIC_H
#define PUBLIC_H

#include "servant/Application.h"
#include "util/tc_timeout_queue.h"
#include "T2TarsObj.h"
#include <vector>
#include <map>
#include <string>

#define DEFAULT_TIMEOUT 1000
#define MAX_REQ_ID 1000000000

struct ConfigData
{
    bool m_test;
    int m_connNum;
    int m_connTimeout;
    int m_reqTimeout;
    int m_systemNo;
    std::map<std::string, std::string> m_t2sdk;
    std::map<std::string, std::string> m_safe;
};

struct ApiRequest : public TC_HandleBase
{
    int reqId;
    T2App::T2TarsRequest req;
    tars::TarsCurrentPtr current;
    time_t createTime;
};

typedef TC_AutoPtr<ApiRequest> ApiRequestPtr; 

struct ApiResponse : public TC_HandleBase
{
    int code;
    string msg;
    int reqId;
    string data;
    ApiRequestPtr reqPtr;
};

typedef TC_AutoPtr<ApiResponse> ApiResponsePtr; 

typedef TC_TimeoutQueue<ApiRequestPtr> ApiRequestQueue;
typedef TC_AutoPtr<ApiRequestQueue> ApiRequestQueuePtr; 

#endif // PUBLIC_H
