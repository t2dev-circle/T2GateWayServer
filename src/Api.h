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
#ifndef ApiH
#define ApiH

#include <atomic>
#include "t2sdk_interface.h"
#include "util/tc_autoptr.h"
#include "T2TarsObj.h"
#include "ApiCallback.h"
#include "Public.h"

using namespace tars;

class Api: public TC_HandleBase
{
public:
    Api();
    ~Api();
public:
    bool init(const std::string &server, const ConfigData &configData);
    void release();

    bool connect(int timeout);
    bool send(const T2App::T2TarsRequest &req, T2App::T2TarsResponse &rsp, tars::TarsCurrentPtr current);
    bool isValid();
public:
    bool process(const T2App::T2TarsRequest &req, T2App::T2TarsResponse &rsp, tars::TarsCurrentPtr current);

    string getErrMsg() const;
private:
    CConfigInterface *m_config;
    CConnectionInterface *m_conn;
    ApiCallback m_callback;
    std::string m_errMsg;
    int m_systemNo;
};

typedef TC_AutoPtr<Api> ApiPtr;

#endif // ApiH
