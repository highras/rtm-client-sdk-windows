#RTM Client Unity SDK
一、环境配置  
    a、从https://github.com/highras/fpnn-sdk-windows-cpp.git 下载fpnn代码  
       (rtm-client-sdk-windows-rtc中的RTCSDK需要依赖fpnn生成的库)  
    b、vs打开fpnn项目，修改项目属性-->c/c++-->代码生成中的运行库的MDd到MTd，MD到MT  
       (因为fpnn默认是MDd，rtm-client-sdk-windows-rtc默认是MTd)  
    注意:fpnn代码目录需要与rtm-client-sdk-windows-rtc代码目录平级  
      
二、结构目录    
    a、RTCSDk目录包含了rtc、rtm接口，以静态库形式导出  
    b、RTMDemo是Qt写的demo，目的在于测试rtm接口  
    c、RTCDemo是Qt写的demo，目的在于测试rtc接口  
    d、doc文档介绍了rtc、rtm接口  

仅支持windows  


