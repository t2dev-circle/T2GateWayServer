# T2GateWayServer

#### 介绍
此服务是基于Tars框架的恒生T2SDK交易网关

#### 软件架构
基于TarsCpp开发的服务程序，主要使用线程+队列。

#### 版权说明
1.  T2SDK是恒生电子提供的接入SDK，私有版权，需要官方授权才能使用，包括账号；
2.  Tars框架是腾讯开源的微服务平台框架，基于BSD 3-Clause License；
3.  concurrentqueue是cameron314开源的无锁队列，基于BSD license；
4.  T2GateWayServer是t2dev-circle（同名公众号【恒生T2开发研习社】）开源的基于MIT license。

#### 安装教程
以下以Linux为例：

1.  安装Tars开发环境，可以参考官网（https://doc.tarsyun.com），如果只是本地开发和部署，不依赖于Tars框架，可以只编译安装TarsCpp。
2.  下载源代码，然后在src同级目录新建build目录，然后执行cmake编译：
cd build
cmake ..
make
3.  在build下的bin目录即可看到可执行程序，然后拷贝config下的文件到bin目录，修改配置文件，如IP和端口等。其中T2GateWayServer.config.conf是通讯相关配置，T2GateWayServer.conf是业务配置。
4.  在bin目录新建t2sdk目录，增加授权文件，文件名需要与配置文件T2GateWayServer.conf中的保持一致。
5.  本地启动执行如下命令：
./T2GateWayServer --config=T2GateWayServer.config.conf

#### 使用说明

1.  T2SDK需要找官方授权（https://ufx.hs.net/#/index），包括账号申请；
2.  可以不依赖Tars微服务平台，单机本地启动服务（当然也支持在平台发布）
3.  鉴于测试账号，测试环境，开发周期等条件有限，并未完整验证业务，上线前请务必严格测试验证

#### 致谢

特别感谢以下开源项目：
1.  TarsCpp：https://github.com/TarsCloud/TarsCpp
2.  concurrentqueue：https://github.com/cameron314/concurrentqueue

