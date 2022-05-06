# RTM-RTC Windows C++ SDK

[TOC]

## 依赖

* [fpnn-sdk-windows](https://github.com/highras/fpnn-sdk-windows-cpp.git)

## 编译rtm-rtc-client-sdk-windows

* 下载fpnn-sdk-windows代码与rtm-rtc-client-sdk-windows代码到同级目录
* vs打开fpnn项目，项目属性-->c/c++-->代码生成-->修改运行库的MDd到MTd，MD到MT(因为fpnn默认是MDd，rtm-rtc-client-sdk-windows默认是MTd)  
* 编译rtm-rtc-client-sdk-windows，默认生成RTCSDK.lib静态库

## 引入rtm-rtc-client-sdk-windows头文件、库
* 


      
二、结构目录    
    a、RTCSDk目录包含了rtc、rtm接口，以静态库形式导出  
    b、RTMDemo是Qt写的demo，目的在于测试rtm接口  
    c、RTCDemo是Qt写的demo，目的在于测试rtc接口  
    d、doc文档介绍了rtc、rtm接口  

仅支持windows  


