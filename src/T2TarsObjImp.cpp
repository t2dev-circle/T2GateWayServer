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
#include "T2TarsObjImp.h"
#include "servant/Application.h"
#include "ApiMgr.h"
#include "ConfigMgr.h"

//////////////////////////////////////////////////////
void T2TarsObjImp::initialize()
{
    //initialize servant here:
    //...
}

//////////////////////////////////////////////////////
void T2TarsObjImp::destroy()
{
    //destroy servant here:
    //...
}

tars::Int32 T2TarsObjImp::invoke(const T2App::T2TarsRequest &req, T2App::T2TarsResponse &rsp, tars::TarsCurrentPtr current)
{
    FDLOG("req") << req.seqNo << "|" << req.funcNo << "|" << TC_Common::tostr(req.data) << endl;

    // 测试则直接返回
    if (ConfigMgr::getInstance()->isTest())
    {
        FDLOG("rsp") << req.seqNo << "|" << req.funcNo << "|" << rsp.code << "|" << rsp.msg << endl;

        return 0;
    }

    // 获取API
    ApiPtr api = ApiMgr::getInstance()->get();
    if (api.get() == nullptr)
    {
        rsp.msg = "No avaiable connection.";
        rsp.code = -1;

        FDLOG("rsp") << req.seqNo << "|" << req.funcNo << "|" << rsp.code << "|" << rsp.msg << endl;

        return -1;
    }

    // 处理请求
    bool result = api->process(req, rsp, current);
    if (!result)
    {
        FDLOG("rsp") << req.seqNo << "|" << req.funcNo << "|" << rsp.code << "|" << rsp.msg << endl;
    }

    return (result ? 0 : -1);
}
