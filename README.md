# RTM/RTC Windows C++ SDK

[TOC]

## 依赖

* [fpnn-sdk-windows](https://github.com/highras/fpnn-sdk-windows-cpp.git)

## rtm-client-sdk-windows项目配置

* 下载fpnn-sdk-windows代码与rtm-client-sdk-windows代码到同级目录
* vs打开fpnn项目，修改项目属性-->c/c++-->代码生成中的运行库的MDd到MTd，MD到MT(因为fpnn默认是MDd，rtm-client-sdk-windows默认是MTd)  
* 编译rtm-client-sdk-windows，默认生成静态库

## rtm-client-sdk-windows项目配置

## 注意事项

      
二、结构目录    
    a、RTCSDk目录包含了rtc、rtm接口，以静态库形式导出  
    b、RTMDemo是Qt写的demo，目的在于测试rtm接口  
    c、RTCDemo是Qt写的demo，目的在于测试rtc接口  
    d、doc文档介绍了rtc、rtm接口  

仅支持windows  


