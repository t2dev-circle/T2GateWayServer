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
#ifndef ApiHelperH
#define ApiHelperH

#include "t2sdk_interface.h"
#include "Public.h"
#include "T2TarsObj.h"

class ApiHelper
{
public:
    static bool packRequest(const T2App::T2TarsRequest &req, IBizMessage *sendMsg);
    static bool packResponse(const ApiResponsePtr &apiRspPtr, T2App::T2TarsResponse &rsp);

    static time_t getCost(const ApiRequestPtr &apiReqPtr);
private:
    static bool getInt(IF2UnPacker *unpacker, int index, string &value);
    static bool getDouble(IF2UnPacker *unpacker, int index, string &value);
    static bool getChar(IF2UnPacker *unpacker, int index, string &value);
    static bool getString(IF2UnPacker *unpacker, int index, string &value);
    static bool getRaw(IF2UnPacker *unpacker, int index, string &value);
};

#endif // ApiHelperH
