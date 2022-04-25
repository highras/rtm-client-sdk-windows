#pragma once
#include <atomic>

struct RTMAuthStatusInfo
{
    std::vector<std::function<void(long, long, bool, int)>> authCallbacks;
    std::unordered_map<string, string> attr;
    string token;
    string lang;

    int remainedTimeout = 0;
    long lastActionMsecTimeStamp = 0;
    bool isAuthStatusInfoValid = false;

    void updateAuthInfo(std::function<void(long, long, bool, int)> cb,
                        std::unordered_map<string, string> attrt,
                        string tokent,
                        string langt,
                        int remainedTimeoutt,
                        long lastActionMsecTimeStampt,
                        bool isAuthStatusInfoValidt)
    {
        authCallbacks.push_back(cb);
        attr = attrt;
        token = tokent;
        lang = langt;
        remainedTimeout = remainedTimeoutt;
        lastActionMsecTimeStamp = lastActionMsecTimeStampt;
        isAuthStatusInfoValid = isAuthStatusInfoValidt;
    }
};

struct RTMAutoReloginInfo
{
    bool disabled = true;
    bool canRelogin = false;
    int reloginCount = 0;
    int lastErrorCode = 0;
    long lastReloginMS = 0;

    string token;
    string lang;
    std::unordered_map<string, string> attr;
    bool isAutoReloginValid = false;

    void login()
    {
        if (disabled)
        {
            disabled = false;
            reloginCount = 0;
            lastErrorCode = 0;
        }
        else if (canRelogin)
        {
            reloginCount += 1;
        }
    }

    void loginSuccessful()
    {
        canRelogin = true;
        reloginCount = 0;
        lastErrorCode = 0;
    }

    void disableLogin()
    {
        disabled = true;
        canRelogin = false;
    }

    void updateLoginInfo(bool authStatus, RTMAuthStatusInfo authStatusInfo)
    {
        if (isAutoReloginValid)
        {
            if (authStatusInfo.isAuthStatusInfoValid)
            {
                token = authStatusInfo.token;
                attr = authStatusInfo.attr;
                lang = authStatusInfo.lang;
            }

            if (authStatus && !canRelogin)
                loginSuccessful();
        }
    }
};



