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
#ifndef ApiCallbackH
#define ApiCallbackH

#include "t2sdk_interface.h"

class ApiCallback : public CCallbackInterface
{
public:
    ApiCallback();
    ~ApiCallback();
public:
    virtual unsigned long  FUNCTION_CALL_MODE QueryInterface(const char *iid, IKnown **ppv);

    virtual unsigned long  FUNCTION_CALL_MODE AddRef();

    virtual unsigned long  FUNCTION_CALL_MODE Release();

    /**
    * 套接字连接成功
    * @param lpConnection 发生该事件的连接对象
    */
    virtual void FUNCTION_CALL_MODE OnConnect(CConnectionInterface *lpConnection);

    /**
    * 完成安全连接
    * @param lpConnection 发生该事件的连接对象
    */
    virtual void FUNCTION_CALL_MODE OnSafeConnect(CConnectionInterface *lpConnection);

    /**
    * 完成注册
    * @param lpConnection 发生该事件的连接对象
    */
    virtual void FUNCTION_CALL_MODE OnRegister(CConnectionInterface *lpConnection);

    /**
    * 连接被断开
    * @param lpConnection 发生该事件的连接对象
    */
    virtual void FUNCTION_CALL_MODE OnClose(CConnectionInterface *lpConnection);

    /**
    * 发送完成
    * @param lpConnection 发生该事件的连接对象
    * @param hSend        发送句柄
    * @param reserved1    保留字段
    * @param reserved2    保留字段
    * @param nQueuingData 发送队列中剩余个数，使用者可以用此数值控制发送的速度，即小于某值时进行发送
    * @see Send()
    */
    virtual void FUNCTION_CALL_MODE OnSent(CConnectionInterface *lpConnection, int hSend, void *reserved1, void *reserved2, int nQueuingData);

    virtual void FUNCTION_CALL_MODE Reserved1(void *a, void *b, void *c, void *d);

    virtual void FUNCTION_CALL_MODE Reserved2(void *a, void *b, void *c, void *d);

    virtual int  FUNCTION_CALL_MODE Reserved3();

    virtual void FUNCTION_CALL_MODE Reserved4();

    virtual void FUNCTION_CALL_MODE Reserved5();

    virtual void FUNCTION_CALL_MODE Reserved6();

    virtual void FUNCTION_CALL_MODE Reserved7();

    /**
    * 收到SendBiz异步发送的请求的应答
    * @param lpConnection    发生该事件的连接对象
    * @param hSend           发送句柄
    * @param lpUnPackerOrStr 指向解包器指针或者错误信息
    * @param nResult         收包结果
    * 如果nResult等于0，表示业务数据接收成功，并且业务操作成功，lpUnPackerOrStr指向一个解包器，此时应首先将该指针转换为IF2UnPacker *。
    * 如果nResult等于1，表示业务数据接收成功，但业务操作失败了，lpUnPackerOrStr指向一个解包器，此时应首先将该指针转换为IF2UnPacker *。
    * 如果nResult等于2，表示收到非业务错误信息，lpUnPackerOrStr指向一个可读的字符串错误信息。
    * 如果nResult等于3，表示业务包解包失败。lpUnPackerOrStr指向NULL。
    * 如果nResult等于4，表示业务包为空。lpUnpackerOrStr指向NULL。lpRetData这部分结果还会存在
    */
    virtual void FUNCTION_CALL_MODE OnReceivedBiz(CConnectionInterface *lpConnection, int hSend, const void *lpUnPackerOrStr, int nResult);

		/**
    * 收到SendBiz异步发送的请求的应答
    * @param lpConnection    发生该事件的连接对象
    * @param hSend           发送句柄
    * @param lpRetData 其他需要返回的应答内容，根据需要获取
    * @param lpUnPackerOrStr 指向解包器指针或者错误信息
    * @param nResult         收包结果
    * 如果nResult等于0，表示业务数据接收成功，并且业务操作成功，lpUnpackerOrStr指向一个解包器，此时应首先将该指针转换为IF2UnPacker *。
    * 如果nResult等于1，表示业务数据接收成功，但业务操作失败了，lpUnpackerOrStr指向一个解包器，此时应首先将该指针转换为IF2UnPacker *。
    * 如果nResult等于2，表示收到非业务错误信息，lpUnpackerOrStr指向一个可读的字符串错误信息。
    * 如果nResult等于3，表示业务包解包失败。lpUnpackerOrStr指向NULL。
    * 如果nResult等于4，表示业务包为空。lpUnpackerOrStr指向NULL。lpRetData这部分结果还会存在
    */
    virtual void FUNCTION_CALL_MODE OnReceivedBizEx(CConnectionInterface *lpConnection, int hSend, LPRET_DATA lpRetData, const void *lpUnpackerOrStr, int nResult);
	//20130624 xuxp 回调增加BizMessage接口
		/**
		* 收到发送时指定了ReplyCallback选项的请求的应答或者是没有对应请求的数据
		* @param lpConnection 发生该事件的连接对象
		* @param hSend        发送句柄
		* @param lpMsg        业务消息指针
		*/
	virtual void FUNCTION_CALL_MODE OnReceivedBizMsg(CConnectionInterface *lpConnection, int hSend, IBizMessage *lpMsg);
};

#endif // ApiCallbackH
