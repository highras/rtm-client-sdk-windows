#include "mainwindow.h"
#include <QApplication>
#include <atomic>
#include "./RTMUnitis/RTMMsgFilter.h"
#include "FPJson.h"
#include "document.h"
#include "writer.h"
#include "stringbuffer.h"

void RTMMsgFilterTestCase(RTMMsgFilter& filter, std::atomic<int16_t>& msgFilterCnt, std::atomic<int16_t>& msgNoFilterCnt)
{
    std::vector<std::thread> threads;
    for (int i = 0; i < 100; ++i)
    {
        threads.push_back(std::thread([&filter, &msgFilterCnt, &msgNoFilterCnt]()
            {
                for (int j = 200; j < 220; ++j)
                {
                    bool bfind = filter.isContainMsg(j, j);
                    if (bfind)
                        msgFilterCnt.fetch_add(1, std::memory_order_relaxed);
                    else
                        msgNoFilterCnt.fetch_add(1, std::memory_order_relaxed);
                }
            }));
    }

    for (int i = 0; i < 100; ++i)
        threads[i].join();
}

std::string RTMCustomJsonTest()
{
    rapidjson::StringBuffer rtmBuf;
    {
        rapidjson::Writer<rapidjson::StringBuffer> writer(rtmBuf);
        writer.StartObject();

        writer.Key("d");
        writer.String("0");
        writer.Key("e");
        writer.Int(0);
        writer.Key("f");
        writer.String("000000");

        writer.EndObject();

        std::cout << "subjson build1:" << rtmBuf.GetString() << std::endl;
    }
    std::string srtm = rtmBuf.GetString();

    rapidjson::Document fileDoc;
    fileDoc.SetObject();
    rapidjson::Document rtmDoc(&fileDoc.GetAllocator());
    rtmDoc.SetObject();
    if (!srtm.empty() && rtmDoc.Parse(srtm.c_str()).HasParseError())
       return "";
    rapidjson::StringBuffer srtmBuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(srtmBuf);
    rtmDoc.Accept(writer);
    std::cout << "srtmBuf build:" << srtmBuf.GetString() << std::endl;
    {
        std::string sa = "1";
        rapidjson::Value va(sa.c_str(), rtmDoc.GetAllocator());
        rtmDoc.AddMember("a", va, rtmDoc.GetAllocator());
        rtmDoc.AddMember("b", 2, rtmDoc.GetAllocator());
        rtmDoc.AddMember("c", 3, rtmDoc.GetAllocator());

        std::cout << "subjson build2:" << rtmBuf.GetString() << std::endl;
    }

    rapidjson::StringBuffer fileBuf;
    {
        fileDoc.AddMember("rtm", rtmDoc, fileDoc.GetAllocator());
        fileDoc.AddMember("z", 399, fileDoc.GetAllocator());

        rapidjson::Writer<rapidjson::StringBuffer> writer(fileBuf);
        fileDoc.Accept(writer);

        std::cout << "json build:" << fileBuf.GetString() << std::endl;
    }

    {
        rapidjson::Document parseDoc;
        if (parseDoc.Parse(fileBuf.GetString()).HasParseError())
            return "";
        if (parseDoc.IsObject() == false)
            return "";

        if (parseDoc.HasMember("rtm"))
        {
            rapidjson::Value& rtmAttrs = parseDoc["rtm"];
            if (rtmAttrs.HasMember("a") && rtmAttrs["a"].IsString())
                std::cout << "json parse:" << rtmAttrs["a"].GetString() << std::endl;
        }
        if (parseDoc.HasMember("z") && parseDoc["z"].IsInt())
            std::cout << "json parse:" << parseDoc["z"].GetInt() << std::endl;
    }

    return fileBuf.GetString();
}

int main(int argc, char *argv[])
{
    //filter test
    std::cout << "/*********************************************************************************" << std::endl;
    {
        std::atomic<int16_t> msgFilterCnt = { 0 };
        std::atomic<int16_t> msgNoFilterCnt = { 0 };
        RTMMsgFilter filter;
        RTMMsgFilterTestCase(filter, msgFilterCnt, msgNoFilterCnt);
        std::cout << "TestCase--->msgFilterCnt count:" << msgFilterCnt.load(std::memory_order_relaxed) 
            << ", msgNoFilterCnt count:" << msgNoFilterCnt.load(std::memory_order_relaxed) << std::endl;
    }
    std::cout << "*********************************************************************************/" << std::endl;

    //custom json
    std::cout << "/*********************************************************************************" << std::endl;
    {
        std::string ret = RTMCustomJsonTest();
        std::cout << "TestCase--->custom json:" << ret << std::endl;;
    }
    std::cout << "*********************************************************************************/" << std::endl;

    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        qFatal("WSAStartup failed: %d\n", iResult);
    }

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    auto result = a.exec();
    WSACleanup();
    return result;
}
