# RTM-RTC Windows C++ SDK

[TOC]

## 依赖

* [fpnn-sdk-windows](https://github.com/highras/fpnn-sdk-windows-cpp.git)

## 编译rtm-rtc-client-sdk-windows

* 下载fpnn-sdk-windows代码与rtm-rtc-client-sdk-windows代码到同级目录
* vs打开fpnn项目，右键项目，属性-->c/c++-->代码生成-->修改运行库的MDd到MTd，MD到MT(因为fpnn默认是MDd，rtm-rtc-client-sdk-windows默认是MTd)  
* 编译rtm-rtc-client-sdk-windows，默认生成RTCSDK.lib静态库

## 引入rtm-rtc-client-sdk-windows头文件、库(详情可见[RTMDemo](https://github.com/highras/rtm-rtc-client-sdk-windows/tree/master/RTMDemo))
* 右键项目，属性-->c/c++-->常规-->附加包含目录，添加以下头文件路径
    * ..\\..\fpnn-sdk-windows-cpp\fpnn-sdk\core
    * ..\\..\fpnn-sdk-windows-cpp\fpnn-sdk\base
    * ..\\..\fpnn-sdk-windows-cpp\fpnn-sdk\proto
    * ..\\..\fpnn-sdk-windows-cpp\fpnn-sdk\proto\rapidjson
    * ..\\..\fpnn-sdk-windows-cpp\fpnn-sdk\proto\msgpack
    * ..\RTCSDK\RTCSDK
* 右键项目，属性-->链接器-->输入，添加以下库路径(此处以debug为例)
    * ..\RTCSDK\x64\Debug\RTCSDK.lib

## 初始化rtm-rtc-client-sdk-windows实例
* 初始化rtm事件接口，详情参见[RTMEventHandler](https://github.com/highras/rtm-rtc-client-sdk-windows/blob/master/RTMDemo/mainwindow.cpp)中的eventhandler
* 初始化rtm实例，详情参见[RTMEventHandler](https://github.com/highras/rtm-rtc-client-sdk-windows/blob/master/RTMDemo/mainwindow.cpp)中的rtm
* 初始化client实例，详情参见[RTMEventHandler](https://github.com/highras/rtm-rtc-client-sdk-windows/blob/master/RTMDemo/mainwindow.cpp)中的tcpclient

## 登录以及其他接口的调用



      
二、结构目录    
    a、RTCSDk目录包含了rtc、rtm接口，以静态库形式导出  
    b、RTMDemo是Qt写的demo，目的在于测试rtm接口  
    c、RTCDemo是Qt写的demo，目的在于测试rtc接口  
    d、doc文档介绍了rtc、rtm接口  

仅支持windows  


