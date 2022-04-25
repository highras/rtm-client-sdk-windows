#include "RTMProxy.h"
#include "RTMClient.h"
#include "./RTMUnitis/RTMClientManager.h"

RTMProxy::RTMProxy(string rtmhost, unsigned short rtmport, int64_t pid, int64_t uid,  shared_ptr<RTMEventHandler> rtmhandler)
{
    currentUid = uid;
    currentPid = pid;

    rtm = RTMClientManager::instance()->getgetClientByPidAndUid(pid, uid);
    if (!rtm)
    {
        rtm = std::make_shared<RTMClient>(rtmhost, rtmport, pid, uid, rtmhandler);
        RTMClientManager::instance()->pushClient(pid, uid, rtm);
    }
}

RTMProxy::~RTMProxy()
{
}


int32_t RTMProxy::Login(string token, int64_t& opid, int64_t& ouid, bool& osuccessed, std::unordered_map<string, string> attr, string lang, int timeout)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (token.empty())
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->Login([this, &prom, &errcode, &opid , &ouid , &osuccessed](int64_t pid, int64_t uid, int32_t errorCode, bool successed) {
        errcode = errorCode;

        if (errorCode == 0)
        {
            opid = pid;
            ouid = uid;
            osuccessed = successed;
        }
        prom.set_value(true);
    }, token, attr, lang, timeout);

    f.get();
    return errcode;
}

int32_t RTMProxy::Bye()
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->Bye([&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::AddAttrs(unordered_map<string, string> attrs)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (attrs.size() == 0)
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->AddAttrs(attrs, [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::GetAttrs(unordered_map<string, string>& oattrs)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->GetAttrs([&prom, &errcode, &oattrs](int errorCode, unordered_map<string, string> attrs) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            oattrs = attrs;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::AddDebugLog(string msg, string attrs)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (msg.empty() || attrs.empty())
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->AddDebugLog(msg, attrs, [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::GetServerTime(int64_t& omts)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->GetServerTime([&prom, &errcode, &omts](int errorCode, int64_t mts) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            omts = mts;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::SendChat(int64_t to, int64_t mid, string msg, string attrs, int64_t& omtime)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (msg.empty() || attrs.empty())
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->SendChat(to, mid, msg, attrs, [&prom, &errcode, &omtime](int errorCode, int64_t mtime) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            omtime = mtime;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::SendCmd(int64_t to, int64_t mid, string msg, string attrs, int64_t& omtime)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (msg.empty() || attrs.empty())
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->SendCmd(to, mid, msg, attrs, [&prom, &errcode, &omtime](int errorCode, int64_t mtime) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            omtime = mtime;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;

}

int32_t RTMProxy::SendGroupChat(int64_t gid, int64_t mid, string msg, string attrs, int64_t& omtime)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (msg.empty() || attrs.empty())
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->SendGroupChat(gid, mid, msg, attrs, [&prom, &errcode, &omtime](int errorCode, int64_t mtime) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            omtime = mtime;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::SendGroupCmd(int64_t gid, int64_t mid, string msg, string attrs, int64_t& omtime)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (msg.empty() || attrs.empty())
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->SendGroupCmd(gid, mid, msg, attrs, [&prom, &errcode, &omtime](int errorCode, int64_t mtime) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            omtime = mtime;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::SendRoomChat(int64_t rid, int64_t mid, string msg, string attrs, int64_t& omtime)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (msg.empty() || attrs.empty())
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->SendRoomChat(rid, mid, msg, attrs, [&prom, &errcode, &omtime](int errorCode, int64_t mtime) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            omtime = mtime;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::SendRoomCmd(int64_t rid, int64_t mid, string msg, string attrs, int64_t& omtime)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (msg.empty() || attrs.empty())
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->SendRoomCmd(rid, mid, msg, attrs, [&prom, &errcode, &omtime](int errorCode, int64_t mtime) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            omtime = mtime;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::GetGroupChat(int64_t gid, bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, int16_t& onum, int64_t& olastid, int64_t& obegin, int64_t& oend, vector<CommonMsg>& omsgs)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (gid<0 || begin > end)
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->GetGroupChat(gid, desc, num, begin, end, lastid,
        [&prom, &errcode, &onum, &olastid, &obegin, &oend, &omsgs](int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<RTMClient::CommonMsg> msgs) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            onum = num;
            olastid = lastid;
            obegin = begin;
            oend = end;
            for (int i = 0; i < msgs.size(); ++i)
            {
                CommonMsg tmsg;
                tmsg.id = msgs[i].id;
                tmsg.from = msgs[i].from;
                tmsg.mtype = msgs[i].mtype;
                tmsg.mid = msgs[i].mid;
                tmsg.deleted = msgs[i].deleted;
                tmsg.msg = msgs[i].msg;
                tmsg.attrs = msgs[i].attrs;
                tmsg.mtime = msgs[i].mtime;
                omsgs.push_back(tmsg);
            }
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::GetRoomChat(int64_t rid, bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, int16_t& onum, int64_t& olastid, int64_t& obegin, int64_t& oend, vector<CommonMsg>& omsgs)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (rid<0 || begin > end)
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->GetRoomChat(rid, desc, num, begin, end, lastid,
        [&prom, &errcode, &onum, &olastid, &obegin, &oend, &omsgs](int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<RTMClient::CommonMsg> msgs) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            onum = num;
            olastid = lastid;
            obegin = begin;
            oend = end;
            for (int i = 0; i < msgs.size(); ++i)
            {
                CommonMsg tmsg;
                tmsg.id = msgs[i].id;
                tmsg.from = msgs[i].from;
                tmsg.mtype = msgs[i].mtype;
                tmsg.mid = msgs[i].mid;
                tmsg.deleted = msgs[i].deleted;
                tmsg.msg = msgs[i].msg;
                tmsg.attrs = msgs[i].attrs;
                tmsg.mtime = msgs[i].mtime;
                omsgs.push_back(tmsg);
            }
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::GetBroadcastChat(bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, int16_t& onum, int64_t& olastid, int64_t& obegin, int64_t& oend, vector<CommonMsg>& omsgs)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (begin > end)
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->GetBroadcastChat(desc, num, begin, end, lastid,
        [&prom, &errcode, &onum, &olastid, &obegin, &oend, &omsgs](int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<RTMClient::CommonMsg> msgs) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            onum = num;
            olastid = lastid;
            obegin = begin;
            oend = end;
            for (int i = 0; i < msgs.size(); ++i)
            {
                CommonMsg tmsg;
                tmsg.id = msgs[i].id;
                tmsg.from = msgs[i].from;
                tmsg.mtype = msgs[i].mtype;
                tmsg.mid = msgs[i].mid;
                tmsg.deleted = msgs[i].deleted;
                tmsg.msg = msgs[i].msg;
                tmsg.attrs = msgs[i].attrs;
                tmsg.mtime = msgs[i].mtime;
                omsgs.push_back(tmsg);
            }
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::GetP2PChat(int64_t uid, bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, int16_t& onum, int64_t& olastid, int64_t& obegin, int64_t& oend, vector<CommonMsg>& omsgs)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (uid<0 || begin > end)
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->GetP2PChat(uid, desc, num, begin, end, lastid,
        [&prom, &errcode, &onum, &olastid, &obegin, &oend, &omsgs](int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<RTMClient::CommonMsg> msgs) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            onum = num;
            olastid = lastid;
            obegin = begin;
            oend = end;
            for (int i = 0; i < msgs.size(); ++i)
            {
                CommonMsg tmsg;
                tmsg.id = msgs[i].id;
                tmsg.from = msgs[i].from;
                tmsg.mtype = msgs[i].mtype;
                tmsg.mid = msgs[i].mid;
                tmsg.deleted = msgs[i].deleted;
                tmsg.msg = msgs[i].msg;
                tmsg.attrs = msgs[i].attrs;
                tmsg.mtime = msgs[i].mtime;
                omsgs.push_back(tmsg);
            }
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::DelChat(int64_t from, int64_t mid, int64_t xid, int8_t type)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->DelChat(from, mid, xid, type, [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::GetChat(int64_t from, int64_t mid, int64_t xid, int8_t type, int64_t& oid, int8_t& omtype, string& omsg, string& oattrs, int64_t& omtime)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->GetChat(from, mid, xid, type,
        [&prom, &errcode, &oid, &omtype, &omsg, &oattrs, &omtime](int errorCode, int64_t id, int8_t mtype, string msg, string attrs, int64_t mtime) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            oid = id;
            omtype = mtype;
            omsg = msg;
            oattrs = attrs;
            omtime = mtime;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}



int32_t RTMProxy::SendMsg(int64_t to, int64_t mid, int8_t mtype, string msg, string attrs, int64_t& omtime)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (msg.empty() || attrs.empty())
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->SendMsg(to, mid, mtype, msg, attrs, [&prom, &errcode, &omtime](int errorCode, int64_t mtime) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            omtime = mtime;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::SendGroupMsg(int64_t gid, int64_t mid, int8_t mtype, string msg, string attrs, int64_t& omtime)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (msg.empty() || attrs.empty())
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->SendGroupMsg(gid, mid, mtype, msg, attrs, [&prom, &errcode, &omtime](int errorCode, int64_t mtime) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            omtime = mtime;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::SendRoomMsg(int64_t rid, int64_t mid, int8_t mtype, string msg, string attrs, int64_t& omtime)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (msg.empty() || attrs.empty())
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->SendGroupMsg(rid, mid, mtype, msg, attrs, [&prom, &errcode, &omtime](int errorCode, int64_t mtime) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            omtime = mtime;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}


int32_t RTMProxy::GetGroupMsg(int64_t gid, bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, 
    unordered_set<int8_t> mtypes, int16_t& onum, int64_t& olastid, int64_t& obegin, int64_t& oend, vector<CommonMsg>& omsgs)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (gid<0 || begin > end || mtypes.size()==0)
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->GetGroupMsg(gid, desc, num, begin, end, lastid, mtypes,
        [&prom, &errcode, &onum, &olastid, &obegin, &oend, &omsgs](int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<RTMClient::CommonMsg> msgs) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            onum = num;
            olastid = lastid;
            obegin = begin;
            oend = end;
            for (int i = 0; i < msgs.size(); ++i)
            {
                CommonMsg tmsg;
                tmsg.id = msgs[i].id;
                tmsg.from = msgs[i].from;
                tmsg.mtype = msgs[i].mtype;
                tmsg.mid = msgs[i].mid;
                tmsg.deleted = msgs[i].deleted;
                tmsg.msg = msgs[i].msg;
                tmsg.attrs = msgs[i].attrs;
                tmsg.mtime = msgs[i].mtime;
                omsgs.push_back(tmsg);
            }
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::GetRoomMsg(int64_t rid, bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid,
    unordered_set<int8_t> mtypes, int16_t& onum, int64_t& olastid, int64_t& obegin, int64_t& oend, vector<CommonMsg>& omsgs)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (rid<0 || begin > end || mtypes.size() == 0)
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->GetRoomMsg(rid, desc, num, begin, end, lastid, mtypes,
        [&prom, &errcode, &onum, &olastid, &obegin, &oend, &omsgs](int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<RTMClient::CommonMsg> msgs) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            onum = num;
            olastid = lastid;
            obegin = begin;
            oend = end;
            for (int i = 0; i < msgs.size(); ++i)
            {
                CommonMsg tmsg;
                tmsg.id = msgs[i].id;
                tmsg.from = msgs[i].from;
                tmsg.mtype = msgs[i].mtype;
                tmsg.mid = msgs[i].mid;
                tmsg.deleted = msgs[i].deleted;
                tmsg.msg = msgs[i].msg;
                tmsg.attrs = msgs[i].attrs;
                tmsg.mtime = msgs[i].mtime;
                omsgs.push_back(tmsg);
            }
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::GetBroadcastMsg(bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, 
    unordered_set<int8_t> mtypes, int16_t& onum, int64_t& olastid, int64_t& obegin, int64_t& oend, vector<CommonMsg>& omsgs)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (begin > end || mtypes.size() == 0)
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->GetBroadcastMsg(desc, num, begin, end, lastid, mtypes,
        [&prom, &errcode, &onum, &olastid, &obegin, &oend, &omsgs](int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<RTMClient::CommonMsg> msgs) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            onum = num;
            olastid = lastid;
            obegin = begin;
            oend = end;
            for (int i = 0; i < msgs.size(); ++i)
            {
                CommonMsg tmsg;
                tmsg.id = msgs[i].id;
                tmsg.from = msgs[i].from;
                tmsg.mtype = msgs[i].mtype;
                tmsg.mid = msgs[i].mid;
                tmsg.deleted = msgs[i].deleted;
                tmsg.msg = msgs[i].msg;
                tmsg.attrs = msgs[i].attrs;
                tmsg.mtime = msgs[i].mtime;
                omsgs.push_back(tmsg);
            }
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::GetP2PMsg(int64_t uid, bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, 
    unordered_set<int8_t> mtypes, int16_t& onum, int64_t& olastid, int64_t& obegin, int64_t& oend, vector<CommonMsg>& omsgs)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (uid<0 || begin > end || mtypes.size() == 0)
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->GetP2PMsg(uid, desc, num, begin, end, lastid, mtypes,
        [&prom, &errcode, &onum, &olastid, &obegin, &oend, &omsgs](int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<RTMClient::CommonMsg> msgs) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            onum = num;
            olastid = lastid;
            obegin = begin;
            oend = end;
            for (int i = 0; i < msgs.size(); ++i)
            {
                CommonMsg tmsg;
                tmsg.id = msgs[i].id;
                tmsg.from = msgs[i].from;
                tmsg.mtype = msgs[i].mtype;
                tmsg.mid = msgs[i].mid;
                tmsg.deleted = msgs[i].deleted;
                tmsg.msg = msgs[i].msg;
                tmsg.attrs = msgs[i].attrs;
                tmsg.mtime = msgs[i].mtime;
                omsgs.push_back(tmsg);
            }
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::DelMsg(int64_t from, int64_t mid, int64_t xid, int8_t type)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->DelMsg(from, mid, xid, type, [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::GetMsg(int64_t from, int64_t mid, int64_t xid, int8_t type, int64_t& oid, int8_t& omtype, string& omsg, string& oattrs, int64_t& omtime)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->GetMsg(from, mid, xid, type,
        [&prom, &errcode, &oid, &omtype, &omsg, &oattrs, &omtime](int errorCode, int64_t id, int8_t mtype, string msg, string attrs, int64_t mtime) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            oid = id;
            omtype = mtype;
            omsg = msg;
            oattrs = attrs;
            omtime = mtime;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::GetUnread(bool clean, unordered_set<int64_t>& op2p, unordered_set<int64_t>& ogroup)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->GetUnread(clean,
        [&prom, &errcode, &op2p, &ogroup](int errorCode, unordered_set<int64_t> p2p, unordered_set<int64_t> group) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            op2p = p2p;
            ogroup = group;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::GetSession(unordered_set<int64_t>& op2p, unordered_set<int64_t>& ogroup)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->GetSession([&prom, &errcode, &op2p, &ogroup](int errorCode, unordered_set<int64_t> p2p, unordered_set<int64_t> group) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            op2p = p2p;
            ogroup = group;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::GetP2PUnread(unordered_set<int64_t> uids, int64_t mtime, unordered_set<int8_t> mtypes, unordered_map<string, int32_t>& op2p, unordered_map<string, int64_t>& oltime)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (uids.size()==0 || mtypes.size() == 0)
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->GetP2PUnread(uids, mtime, mtypes,
        [&prom, &errcode, &op2p, &oltime](int errorCode, unordered_map<string, int32_t> p2p, unordered_map<string, int64_t> ltime) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            op2p = p2p;
            oltime = ltime;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::GetGroupUnread(unordered_set<int64_t> gids, int64_t mtime, unordered_set<int8_t> mtypes, unordered_map<string, int32_t>& ogroup, unordered_map<string, int64_t>& oltime)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (gids.size() == 0 || mtypes.size() == 0)
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->GetGroupUnread(gids, mtime, mtypes,
        [&prom, &errcode, &ogroup, &oltime](int errorCode, unordered_map<string, int32_t> group, unordered_map<string, int64_t> ltime) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            ogroup = group;
            oltime = ltime;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

// imlib
int32_t RTMProxy::CleanTargetUnread(unordered_set<int64_t> uids, unordered_set<int64_t> gids)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (uids.size() == 0 || gids.size() == 0)
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->CleanTargetUnread(uids, gids,
        [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

// imlib
int32_t RTMProxy::GetP2PUnreadByTime(unordered_map<string, int64_t> uidTime, unordered_set<int8_t> mtypes, unordered_map<string, int32_t>& op2p, unordered_map<string, int64_t>& oltime)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (uidTime.size() == 0 || mtypes.size() == 0)
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->GetP2PUnreadByTime(uidTime, mtypes,
        [&prom, &errcode, &op2p, &oltime](int errorCode, unordered_map<string, int32_t> p2p, unordered_map<string, int64_t> ltime) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            op2p = p2p;
            oltime = ltime;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

// imlib
int32_t RTMProxy::GetGroupUnreadByTime(unordered_map<string, int64_t> gidTime, unordered_set<int8_t> mtypes, unordered_map<string, int32_t>& ogroup, unordered_map<string, int64_t>& oltime)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (gidTime.size() == 0 || mtypes.size() == 0)
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->GetGroupUnreadByTime(gidTime, mtypes,
        [&prom, &errcode, &ogroup, &oltime](int errorCode, unordered_map<string, int32_t> group, unordered_map<string, int64_t> ltime) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            ogroup = group;
            oltime = ltime;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}


int32_t RTMProxy::SetLang(string lang)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->SetLang(lang, [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::Translate(string text, string src, string dst, string type, string profanity, string& osource, string& otarget, string& osourceText, string& otargetText)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (text.empty() || src.empty() || dst.empty() || type.empty() || profanity.empty())
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->Translate(text, src, dst, type, profanity, 
        [&prom, &errcode, &osource, &otarget, &osourceText, &otargetText](int errorCode, string source, string target, string sourceText, string targetText) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            osource = source;
            otarget = target;
            osourceText = sourceText;
            otargetText = targetText;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::Profanity(string text, bool classify, string& otext, vector<string>& oclassification)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (text.empty())
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->Profanity(text, classify,
        [&prom, &errcode, &otext, &oclassification](int errorCode, string text, vector<string> classification) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            otext = text;
            oclassification = oclassification;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::Speech2Text(string audio, int32_t type, string lang, string codec, int32_t srate, string& otext, string& olang)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (audio.empty() || lang.empty() || codec.empty())
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->Speech2Text(audio, type, lang, codec, srate,
        [&prom, &errcode, &otext, &olang](int errorCode, string text, string lang) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            otext = text;
            olang = lang;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::TCheck(string text, int32_t& oresult, string& otext, vector<string>& otags, vector<string>& owlist)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (text.empty())
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->TCheck(text,
        [&prom, &errcode, &oresult, &otext, &otags, &owlist](int errorCode, int32_t result, string text, vector<string> tags, vector<string> wlist) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            oresult = result;
            otext = text;
            otags = tags;
            owlist = wlist;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::ICheck(string image, int32_t type, int32_t& oresult, vector<string>& otags)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (image.empty())
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->ICheck(image, type,
        [&prom, &errcode, &oresult, &otags](int errorCode, int32_t result, vector<string> tags) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            oresult = result;
            otags = tags;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::ACheck(string audio, int32_t type, string lang, string codec, int32_t srate, int32_t& oresult, vector<string>& otags)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (audio.empty())
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->ACheck(audio, type, lang, codec, srate,
        [&prom, &errcode, &oresult, &otags](int errorCode, int32_t result, vector<string> tags) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            oresult = result;
            otags = tags;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::VCheck(string video, int32_t type, string videoName, int32_t& oresult, vector<string>& otags)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (video.empty() || videoName.empty())
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->VCheck(video, type, videoName,
        [&prom, &errcode, &oresult, &otags](int errorCode, int32_t result, vector<string> tags) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            oresult = result;
            otags = tags;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}


int32_t RTMProxy::FileToken(int64_t xid, int16_t sendType, string& otoken, string& oendpoint)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->FileToken(xid, sendType,
        [&prom, &errcode, &otoken, &oendpoint](int errorCode, string token, string endpoint) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            otoken = token;
            oendpoint = endpoint;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::SendFile(int32_t pid, int8_t mtype, std::string fileText, std::string fileName, std::string fileExt, std::string userAttrs, int64_t& mtime, int timeout)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->SendFile([&prom, &errcode, &mtime](int32_t errorCode, int64_t time) {
            errcode = errorCode;
            if (errorCode == 0)
            {
                mtime = time;
            }
            prom.set_value(true);
        }, pid, mtype, fileText, fileName, fileExt, userAttrs, timeout);

    f.get();
    return errcode;
}

int32_t RTMProxy::SendGroupFile(int32_t gid, int8_t mtype, std::string fileText, std::string fileName, std::string fileExt, std::string userAttrs, int64_t& mtime, int timeout)
{

    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->SendGroupFile([&prom, &errcode, &mtime](int32_t errorCode, int64_t time) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            mtime = time;
        }
        prom.set_value(true);
        }, gid, mtype, fileText, fileName, fileExt, userAttrs, timeout);

    f.get();
    return errcode;
}

int32_t RTMProxy::SendRoomFile(int32_t rid, int8_t mtype, std::string fileText, std::string fileName, std::string fileExt, std::string userAttrs, int64_t& mtime, int timeout)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->SendRoomFile([&prom, &errcode, &mtime](int32_t errorCode, int64_t time) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            mtime = time;
        }
        prom.set_value(true);
        }, rid, mtype, fileText, fileName, fileExt, userAttrs, timeout);

    f.get();
    return errcode;
}

int32_t RTMProxy::SendFile(int32_t pid, RTMAudio rtmAudio, int64_t& mtime, int timeout)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->SendFile([&prom, &errcode, &mtime](int32_t errorCode, int64_t time) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            mtime = time;
        }
        prom.set_value(true);
        }, pid, rtmAudio, timeout);

    f.get();
    return errcode;
}

int32_t RTMProxy::SendGroupFile(int32_t gid, RTMAudio rtmAudio, int64_t& mtime, int timeout)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->SendGroupFile([&prom, &errcode, &mtime](int32_t errorCode, int64_t time) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            mtime = time;
        }
        prom.set_value(true);
        }, gid, rtmAudio, timeout);

    f.get();
    return errcode;
}

int32_t RTMProxy::SendRoomFile(int32_t rid, RTMAudio rtmAudio, int64_t& mtime, int timeout)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->SendRoomFile([&prom, &errcode, &mtime](int32_t errorCode, int64_t time) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            mtime = time;
        }
        prom.set_value(true);
        }, rid, rtmAudio, timeout);

    f.get();
    return errcode;
}


int32_t RTMProxy::GetOnlineUsers(unordered_set<int64_t> uids, unordered_set<int64_t>& ouids)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (uids.size() == 0)
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->GetOnlineUsers(uids,
        [&prom, &errcode, &ouids](int errorCode, unordered_set<int64_t> uids) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            ouids = uids;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::SetUserInfo(string oinfo, string pinfo)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (oinfo.empty() || pinfo.empty())
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->SetUserInfo(oinfo, pinfo,
        [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::GetUserInfo(string& oinfo, string& opinfo)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->GetUserInfo([&prom, &errcode, &oinfo, &opinfo](int errorCode, string info, string pinfo) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            oinfo = info;
            opinfo = pinfo;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::GetUserOpenInfo(unordered_set<int64_t> uids, unordered_map<string, string>& oinfo)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (uids.size() == 0)
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->GetUserOpenInfo(uids, [&prom, &errcode, &oinfo](int errorCode, unordered_map<string, string> info) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            oinfo = info;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::AddFriends(unordered_set<int64_t> friends)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (friends.size() == 0)
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->AddFriends(friends, [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::DelFriends(unordered_set<int64_t> friends)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (friends.size() == 0)
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->DelFriends(friends, [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::GetFriends(unordered_set<int64_t>& ouids)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->GetFriends([&prom, &errcode, &ouids](int errorCode, unordered_set<int64_t> uids) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            ouids = uids;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::AddBlacks(unordered_set<int64_t> blacks)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (blacks.size() == 0)
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->AddBlacks(blacks, [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::DelBlacks(unordered_set<int64_t> blacks)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (blacks.size() == 0)
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->DelBlacks(blacks, [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::GetBlacks(unordered_set<int64_t>& ouids)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->GetBlacks([&prom, &errcode, &ouids](int errorCode, unordered_set<int64_t> uids) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            ouids = uids;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}


int32_t RTMProxy::AddGroupMembers(int64_t gid, unordered_set<int64_t> uids)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (uids.size() == 0)
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->AddGroupMembers(gid, uids, [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::DelGroupMembers(int64_t gid, unordered_set<int64_t> uids)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (uids.size() == 0)
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->DelGroupMembers(gid, uids, [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::GetGroupMembers(int64_t gid, bool online, unordered_set<int64_t>& ouids, unordered_set<int64_t>& oonlines)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->GetGroupMembers(gid, online, [&prom, &errcode, &ouids, &oonlines](int errorCode, unordered_set<int64_t> uids, unordered_set<int64_t> onlines) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            ouids = uids;
            oonlines = onlines;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::GetGroupCount(int64_t gid, bool online, int32_t& oocn, int32_t& oonlines)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->GetGroupCount(gid, online, [&prom, &errcode, &oocn, &oonlines](int errorCode, int32_t ocn, int32_t onlines) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            oocn = ocn;
            oonlines = onlines;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::GetUserGroups(unordered_set<int64_t>& ogids)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->GetUserGroups([&prom, &errcode, &ogids](int errorCode, unordered_set<int64_t> gids) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            ogids = gids;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::SetGroupInfo(int64_t gid, string oinfo, string pinfo)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (oinfo.empty() || pinfo.empty())
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->SetGroupInfo(gid, oinfo, pinfo, [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::GetGroupOpenInfo(int64_t gid, string& oinfo)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->GetGroupOpenInfo(gid, [&prom, &errcode, &oinfo](int errorCode, string info) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            oinfo = info;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::GetGroupsOpenInfo(unordered_set<int64_t> gids, unordered_map<string, string>& oinfo)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (gids.size() == 0)
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->GetGroupsOpenInfo(gids, [&prom, &errcode, &oinfo](int errorCode, unordered_map<string, string> info) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            oinfo = info;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::EnterRoom(int64_t rid)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->EnterRoom(rid, [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::LeaveRoom(int64_t rid)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->LeaveRoom(rid, [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::GetUserRooms(unordered_set<int64_t>& orooms)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->GetUserRooms([&prom, &errcode, &orooms](int errorCode, unordered_set<int64_t> rooms) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            orooms = rooms;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::SetRoomInfo(int64_t rid, string oinfo, string pinfo)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (oinfo.empty() || pinfo.empty())
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->SetRoomInfo(rid, oinfo, pinfo, [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::GetRoomInfo(int64_t rid, string& oinfo, string& opinfo)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->GetRoomInfo(rid, [&prom, &errcode, &oinfo, &opinfo](int errorCode, string info, string pinfo) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            oinfo = info;
            opinfo = pinfo;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::GetRoomOpenInfo(int64_t rid, string& ooinfo)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->GetRoomOpenInfo(rid, [&prom, &errcode, &ooinfo](int errorCode, string oinfo) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            ooinfo = oinfo;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::GetRoomsOpenInfo(unordered_set<int64_t> rids, unordered_map<string, string>& oinfo)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (rids.size() == 0)
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->GetRoomsOpenInfo(rids, [&prom, &errcode, &oinfo](int errorCode, unordered_map<string, string> info) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            oinfo = info;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::GetRoomMemers(int64_t rid, unordered_set<int64_t>& ouids)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->GetRoomMemers(rid, [&prom, &errcode, &ouids](int errorCode, unordered_set<int64_t> uids) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            ouids = uids;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::GetRoomCount(unordered_set<int64_t> rids, unordered_map<string, int32_t>& oinfo)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (rids.size() == 0)
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->GetRoomCount(rids, [&prom, &errcode, &oinfo](int errorCode, unordered_map<string, int32_t> info) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            oinfo = info;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::DataGet(string key, string& oval)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (key.empty())
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->DataGet(key, [&prom, &errcode, &oval](int errorCode, string val) {
        errcode = errorCode;
        if (errorCode == 0)
        {
            oval = val;
        }
        prom.set_value(true);
    });

    f.get();
    return errcode;
}

int32_t RTMProxy::DataSet(string key, string val)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (key.empty())
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->DataSet(key, val, [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();
    return errcode;

}

int32_t RTMProxy::DataDel(string key)
{
    if (!rtm)
        return FPNN_EC_CORE_INVALID_CONNECTION;

    if (key.empty())
        return RTM_EC_EMPTY_PARAMETER;

    std::promise<bool> prom;
    std::future<bool> f = prom.get_future();
    int32_t errcode = 0;

    rtm->DataDel(key, [&prom, &errcode](int errorCode) {
        errcode = errorCode;
        prom.set_value(true);
    });

    f.get();
    return errcode;
}



/***********************************************Asynchronous interface*****************************************************/
void RTMProxy::Login(function<void(int64_t pid, int64_t uid, int errorCode, bool successed)> callback, string token, std::unordered_map<string, string> attr, string lang, int timeout)
{
    if (!rtm)
    {
        callback(currentPid, currentUid, FPNN_EC_CORE_INVALID_CONNECTION, 0);
        return;
    }

    if (token.empty())
    {
        callback(currentPid, currentUid, RTM_EC_EMPTY_PARAMETER, 0);
        return;
    }

    rtm->Login([this, callback](int64_t pid, int64_t uid, int errorCode, bool successed) {
        if (errorCode == 0)
        {

        }
        callback(pid, uid, errorCode, successed);
        }, token, attr, lang, timeout);
}

void RTMProxy::Bye(function<void(int errorCode)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION);
        return;
    }

    rtm->Bye([callback](int errorCode) {
        callback(errorCode);
    });
}

void RTMProxy::AddAttrs(unordered_map<string, string> attrs, function<void(int errorCode)> callback)
{
    if (!rtm) 
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION);
        return;
    }

    if (attrs.size() == 0)
    {
        callback(RTM_EC_EMPTY_PARAMETER);
        return;
    }

    rtm->AddAttrs(attrs, [callback](int errorCode) {
        callback(errorCode);
    });
}

void RTMProxy::GetAttrs(function<void(int errorCode, unordered_map<string, string> attrs)> callback)
{
    if (!rtm) {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, unordered_map<string, string>());
        return;
    }

    rtm->GetAttrs([callback](int errorCode, unordered_map<string, string> attrs) {
        callback(errorCode, attrs);
    });
}

void RTMProxy::AddDebugLog(string msg, string attrs, function<void(int errorCode)> callback)
{
    if (!rtm)
    {  
        callback(FPNN_EC_CORE_INVALID_CONNECTION);
        return;
    }

    if (msg.empty() || attrs.empty())
    {
        callback(RTM_EC_EMPTY_PARAMETER);
        return;
    }

    rtm->AddDebugLog(msg, attrs, [callback](int errorCode) {
        callback(errorCode);
    });
}

void RTMProxy::GetServerTime(function<void(int errorCode, int64_t mts)> callback)
{
    if (!rtm)
    {  
        callback(FPNN_EC_CORE_INVALID_CONNECTION, 0);
        return;
    }

    rtm->GetServerTime([callback](int errorCode, int64_t mts) {
        callback(errorCode, mts);
    });
}

void RTMProxy::SendChat(int64_t to, int64_t mid, string msg, string attrs, function<void(int errorCode, int64_t mtime)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, 0);
        return;
    }

    if (msg.empty() || attrs.empty())
    {
        callback(RTM_EC_EMPTY_PARAMETER, 0);
        return;
    }

    rtm->SendChat(to, mid, msg, attrs, [callback](int errorCode, int64_t mtime) {
        callback(errorCode, mtime);
    });
}

void RTMProxy::SendCmd(int64_t to, int64_t mid, string msg, string attrs, function<void(int errorCode, int64_t mtime)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, 0);
        return;
    }

    if (msg.empty() || attrs.empty())
    {
        callback(RTM_EC_EMPTY_PARAMETER, 0);
        return;
    }

    rtm->SendCmd(to, mid, msg, attrs, [callback](int errorCode, int64_t mtime) {
        callback(errorCode, mtime);
    });
}

void RTMProxy::SendGroupChat(int64_t gid, int64_t mid, string msg, string attrs, function<void(int errorCode, int64_t mtime)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, 0);
        return;
    }

    if (msg.empty() || attrs.empty())
    {
        callback(RTM_EC_EMPTY_PARAMETER, 0);
        return;
    }

    rtm->SendGroupChat(gid, mid, msg, attrs, [callback](int errorCode, int64_t mtime) {
        callback(errorCode, mtime);
    });
}

void RTMProxy::SendGroupCmd(int64_t gid, int64_t mid, string msg, string attrs, function<void(int errorCode, int64_t mtime)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, 0);
        return;
    }

    if (msg.empty() || attrs.empty())
    {
        callback(RTM_EC_EMPTY_PARAMETER, 0);
        return;
    }

    rtm->SendGroupChat(gid, mid, msg, attrs, [callback](int errorCode, int64_t mtime) {
        callback(errorCode, mtime);
    });
}

void RTMProxy::SendRoomChat(int64_t rid, int64_t mid, string msg, string attrs, function<void(int errorCode, int64_t mtime)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, 0);
        return;
    }

    if (msg.empty() || attrs.empty())
    {
        callback(RTM_EC_EMPTY_PARAMETER, 0);
        return;
    }

    rtm->SendRoomChat(rid, mid, msg, attrs, [callback](int errorCode, int64_t mtime) {
        callback(errorCode, mtime);
    });
}

void RTMProxy::SendRoomCmd(int64_t rid, int64_t mid, string msg, string attrs, function<void(int errorCode, int64_t mtime)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, 0);
        return;
    }

    if (msg.empty() || attrs.empty())
    {
        callback(RTM_EC_EMPTY_PARAMETER, 0);
        return;
    }

    rtm->SendRoomChat(rid, mid, msg, attrs, [callback](int errorCode, int64_t mtime) {
        callback(errorCode, mtime);
    });
}

void RTMProxy::GetGroupChat(int64_t gid, bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, function<void(int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<CommonMsg> msgs)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, 0, 0, 0, 0, vector<CommonMsg>());
        return;
    }

    if (gid<0 || begin>end)
    {
        callback(RTM_EC_EMPTY_PARAMETER, 0, 0, 0, 0, vector<CommonMsg>());
        return;
    }

    rtm->GetGroupChat(gid, desc, num, begin, end, lastid,
        [callback](int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<RTMClient::CommonMsg> msgs) {

        vector<CommonMsg> tmsgs;
        for (int i = 0; i < msgs.size(); ++i)
        {
            CommonMsg tmsg;
            tmsg.id = msgs[i].id;
            tmsg.from = msgs[i].from;
            tmsg.mtype = msgs[i].mtype;
            tmsg.mid = msgs[i].mid;
            tmsg.deleted = msgs[i].deleted;
            tmsg.msg = msgs[i].msg;
            tmsg.attrs = msgs[i].attrs;
            tmsg.mtime = msgs[i].mtime;
            tmsgs.push_back(tmsg);
        }
        callback(errorCode, num, lastid, begin, end, tmsgs);
    });
}

void RTMProxy::GetRoomChat(int64_t rid, bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, function<void(int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<CommonMsg> msgs)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, 0, 0, 0, 0, vector<CommonMsg>());
        return;
    }

    if (rid<0 || begin>end)
    {
        callback(RTM_EC_EMPTY_PARAMETER, 0, 0, 0, 0, vector<CommonMsg>());
        return;
    }

    rtm->GetRoomChat(rid, desc, num, begin, end, lastid,
        [callback](int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<RTMClient::CommonMsg> msgs) {

        vector<CommonMsg> tmsgs;
        for (int i = 0; i < msgs.size(); ++i)
        {
            CommonMsg tmsg;
            tmsg.id = msgs[i].id;
            tmsg.from = msgs[i].from;
            tmsg.mtype = msgs[i].mtype;
            tmsg.mid = msgs[i].mid;
            tmsg.deleted = msgs[i].deleted;
            tmsg.msg = msgs[i].msg;
            tmsg.attrs = msgs[i].attrs;
            tmsg.mtime = msgs[i].mtime;
            tmsgs.push_back(tmsg);
        }
        callback(errorCode, num, lastid, begin, end, tmsgs);
    });
}

void RTMProxy::GetBroadcastChat(bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, function<void(int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<CommonMsg> msgs)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, 0, 0, 0, 0, vector<CommonMsg>());
        return;
    }

    if (begin>end)
    {
        callback(RTM_EC_EMPTY_PARAMETER, 0, 0, 0, 0, vector<CommonMsg>());
        return;
    }

    rtm->GetBroadcastChat(desc, num, begin, end, lastid,
        [callback](int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<RTMClient::CommonMsg> msgs) {

        vector<CommonMsg> tmsgs;
        for (int i = 0; i < msgs.size(); ++i)
        {
            CommonMsg tmsg;
            tmsg.id = msgs[i].id;
            tmsg.from = msgs[i].from;
            tmsg.mtype = msgs[i].mtype;
            tmsg.mid = msgs[i].mid;
            tmsg.deleted = msgs[i].deleted;
            tmsg.msg = msgs[i].msg;
            tmsg.attrs = msgs[i].attrs;
            tmsg.mtime = msgs[i].mtime;
            tmsgs.push_back(tmsg);
        }
        callback(errorCode, num, lastid, begin, end, tmsgs);
    });
}

void RTMProxy::GetP2PChat(int64_t ouid, bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, function<void(int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<CommonMsg> msgs)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, 0, 0, 0, 0, vector<CommonMsg>());
        return;
    }

    if (begin>end)
    {
        callback(RTM_EC_EMPTY_PARAMETER, 0, 0, 0, 0, vector<CommonMsg>());
        return;
    }

    rtm->GetP2PChat(ouid, desc, num, begin, end, lastid,
        [callback](int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<RTMClient::CommonMsg> msgs) {
        vector<CommonMsg> tmsgs;
        for (int i = 0; i < msgs.size(); ++i)
        {
            CommonMsg tmsg;
            tmsg.id = msgs[i].id;
            tmsg.from = msgs[i].from;
            tmsg.mtype = msgs[i].mtype;
            tmsg.mid = msgs[i].mid;
            tmsg.deleted = msgs[i].deleted;
            tmsg.msg = msgs[i].msg;
            tmsg.attrs = msgs[i].attrs;
            tmsg.mtime = msgs[i].mtime;
            tmsgs.push_back(tmsg);
        }
        callback(errorCode, num, lastid, begin, end, tmsgs);
    });
}

void RTMProxy::DelChat(int64_t from, int64_t mid, int64_t xid, int8_t type, function<void(int errorCode)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION);
        return;
    }

    rtm->DelChat(from, mid, xid, type, [callback](int errorCode) {
        callback(errorCode);
    });
}

void RTMProxy::GetChat(int64_t from, int64_t mid, int64_t xid, int8_t type, function<void(int errorCode, int64_t id, int8_t mtype, string msg, string attrs, int64_t mtime)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, 0, 0, "", "", 0);
        return;
    }

    rtm->GetMsg(from, mid, xid, type, [callback](int errorCode, int64_t id, int8_t mtype, string msg, string attrs, int64_t mtime) {
        callback(errorCode, id, mtype, msg, attrs, mtime);
    });
}


void RTMProxy::SendMsg(int64_t to, int64_t mid, int8_t mtype, string msg, string attrs, function<void(int errorCode, int64_t mtime)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, 0);
        return;
    }

    if (msg.empty() || attrs.empty())
    {
        callback(RTM_EC_EMPTY_PARAMETER, 0);
        return;
    }

    rtm->SendMsg(to, mid, mtype, msg, attrs, [callback](int errorCode, int64_t mtime) {
        callback(errorCode, mtime);
    });
}

void RTMProxy::SendGroupMsg(int64_t gid, int64_t mid, int8_t mtype, string msg, string attrs, function<void(int errorCode, int64_t mtime)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, 0);
        return;
    }

    if (gid<0 || attrs.empty())
    {
        callback(RTM_EC_EMPTY_PARAMETER, 0);
        return;
    }

    rtm->SendGroupMsg(gid, mid, mtype, msg, attrs, [callback](int errorCode, int64_t mtime) {
        callback(errorCode, mtime);
    });
}

void RTMProxy::SendRoomMsg(int64_t rid, int64_t mid, int8_t mtype, string msg, string attrs, function<void(int errorCode, int64_t mtime)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, 0);
        return;
    }

    if (rid < 0 || attrs.empty())
    {
        callback(RTM_EC_EMPTY_PARAMETER, 0);
        return;
    }

    rtm->SendRoomMsg(rid, mid, mtype, msg, attrs, [callback](int errorCode, int64_t mtime) {
        callback(errorCode, mtime);
    });
}

     
void RTMProxy::GetGroupMsg(int64_t gid, bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, unordered_set<int8_t> mtypes, 
    function<void(int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<CommonMsg> msgs)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, 0, 0, 0, 0, vector<CommonMsg>());
        return;
    }

    if (gid < 0 || mtypes.size()==0)
    {
        callback(RTM_EC_EMPTY_PARAMETER, 0, 0, 0, 0, vector<CommonMsg>());
        return;
    }

    rtm->GetGroupMsg(gid, desc, num, begin, end, lastid, mtypes, 
        [callback](int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<RTMClient::CommonMsg> msgs) {

        vector<CommonMsg> tmsgs;
        for (int i=0; i<msgs.size(); ++i)
        {
            CommonMsg tmsg;
            tmsg.id = msgs[i].id;
            tmsg.from = msgs[i].from;
            tmsg.mtype = msgs[i].mtype;
            tmsg.mid = msgs[i].mid;
            tmsg.deleted = msgs[i].deleted;
            tmsg.msg = msgs[i].msg;
            tmsg.attrs = msgs[i].attrs;
            tmsg.mtime = msgs[i].mtime;
            tmsgs.push_back(tmsg);
        }
        callback(errorCode, num, lastid, begin, end, tmsgs);
    });
}

void RTMProxy::GetRoomMsg(int64_t rid, bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, unordered_set<int8_t> mtypes, 
    function<void(int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<CommonMsg> msgs)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, 0, 0, 0, 0, vector<CommonMsg>());
        return;
    }

    if (rid < 0 || mtypes.size() == 0)
    {
        callback(RTM_EC_EMPTY_PARAMETER, 0, 0, 0, 0, vector<CommonMsg>());
        return;
    }

    rtm->GetRoomMsg(rid, desc, num, begin, end, lastid, mtypes, 
        [callback](int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<RTMClient::CommonMsg> msgs) {

        vector<CommonMsg> tmsgs;
        for (int i = 0; i < msgs.size(); ++i)
        {
            CommonMsg tmsg;
            tmsg.id = msgs[i].id;
            tmsg.from = msgs[i].from;
            tmsg.mtype = msgs[i].mtype;
            tmsg.mid = msgs[i].mid;
            tmsg.deleted = msgs[i].deleted;
            tmsg.msg = msgs[i].msg;
            tmsg.attrs = msgs[i].attrs;
            tmsg.mtime = msgs[i].mtime;
            tmsgs.push_back(tmsg);
        }
        callback(errorCode, num, lastid, begin, end, tmsgs);
    }); 
}

void RTMProxy::GetBroadcastMsg(bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, unordered_set<int8_t> mtypes, 
    function<void(int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<CommonMsg> msgs)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, 0, 0, 0, 0, vector<CommonMsg>());
        return;
    }

    if (mtypes.size() == 0)
    {
        callback(RTM_EC_EMPTY_PARAMETER, 0, 0, 0, 0, vector<CommonMsg>());
        return;
    }

    rtm->GetBroadcastMsg(desc, num, begin, end, lastid, mtypes, 
        [callback](int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<RTMClient::CommonMsg> msgs) {
        
        vector<CommonMsg> tmsgs;
        for (int i = 0; i < msgs.size(); ++i)
        {
            CommonMsg tmsg;
            tmsg.id = msgs[i].id;
            tmsg.from = msgs[i].from;
            tmsg.mtype = msgs[i].mtype;
            tmsg.mid = msgs[i].mid;
            tmsg.deleted = msgs[i].deleted;
            tmsg.msg = msgs[i].msg;
            tmsg.attrs = msgs[i].attrs;
            tmsg.mtime = msgs[i].mtime;
            tmsgs.push_back(tmsg);
        }
        callback(errorCode, num, lastid, begin, end, tmsgs);
    });
}

void RTMProxy::GetP2PMsg(int64_t ouid, bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, unordered_set<int8_t> mtypes, 
    function<void(int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<CommonMsg> msgs)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, 0, 0, 0, 0, vector<CommonMsg>());
        return;
    }

    if (mtypes.size() == 0)
    {
        callback(RTM_EC_EMPTY_PARAMETER, 0, 0, 0, 0, vector<CommonMsg>());
        return;
    }

    rtm->GetP2PMsg(ouid, desc, num, begin, end, lastid, mtypes, 
        [callback](int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<RTMClient::CommonMsg> msgs) {
        vector<CommonMsg> tmsgs;
        for (int i = 0; i < msgs.size(); ++i)
        {
            CommonMsg tmsg;
            tmsg.id = msgs[i].id;
            tmsg.from = msgs[i].from;
            tmsg.mtype = msgs[i].mtype;
            tmsg.mid = msgs[i].mid;
            tmsg.deleted = msgs[i].deleted;
            tmsg.msg = msgs[i].msg;
            tmsg.attrs = msgs[i].attrs;
            tmsg.mtime = msgs[i].mtime;
            tmsgs.push_back(tmsg);
        }
        callback(errorCode, num, lastid, begin, end, tmsgs);
    });
}

void RTMProxy::DelMsg(int64_t from, int64_t mid, int64_t xid, int8_t type, function<void(int errorCode)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION);
        return;
    }

    rtm->DelMsg(from, mid, xid, type, [callback](int errorCode) {
        callback(errorCode);
    });
}

void RTMProxy::GetMsg(int64_t from, int64_t mid, int64_t xid, int8_t type, function<void(int errorCode, int64_t id, int8_t mtype, string msg, string attrs, int64_t mtime)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, 0, 0, "", "", 0);
        return;
    }

    rtm->GetMsg(from, mid, xid, type, [callback](int errorCode, int64_t id, int8_t mtype, string msg, string attrs, int64_t mtime) {
        callback(errorCode, id, mtype, msg, attrs, mtime);
    });
}

void RTMProxy::GetUnread(bool clean, function<void(int errorCode, unordered_set<int64_t> p2p, unordered_set<int64_t> group)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, unordered_set<int64_t>(), unordered_set<int64_t>());
        return;
    }

    rtm->GetUnread(clean, [callback](int errorCode, unordered_set<int64_t> p2p, unordered_set<int64_t> group) {
        callback(errorCode, p2p, group);
    });
}

void RTMProxy::CleanUnread(function<void(int errorCode)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION);
        return;
    }

    rtm->CleanUnread([callback](int errorCode) {
        callback(errorCode);
    });
}

void RTMProxy::GetSession(function<void(int errorCode, unordered_set<int64_t> p2p, unordered_set<int64_t> group)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, unordered_set<int64_t>(), unordered_set<int64_t>());
        return;
    }

    rtm->GetSession([callback](int errorCode, unordered_set<int64_t> p2p, unordered_set<int64_t> group) {
        callback(errorCode, p2p, group);
    });
}

void RTMProxy::GetP2PUnread(unordered_set<int64_t> uids, int64_t mtime, unordered_set<int8_t> mtypes, function<void(int errorCode, unordered_map<string, int32_t> p2p, unordered_map<string, int64_t> ltime)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, unordered_map<string, int32_t>(), unordered_map<string, int64_t>());
        return;
    }

    if (uids.size()==0 || mtypes.size() == 0)
    {
        callback(RTM_EC_EMPTY_PARAMETER, unordered_map<string, int32_t>(), unordered_map<string, int64_t>());
        return;
    }

    rtm->GetP2PUnread(uids, mtime, mtypes, [callback](int errorCode, unordered_map<string, int32_t> p2p, unordered_map<string, int64_t> ltime) {
        callback(errorCode, p2p, ltime);
    });
}

void RTMProxy::GetGroupUnread(unordered_set<int64_t> gids, int64_t mtime, unordered_set<int8_t> mtypes, function<void(int errorCode, unordered_map<string, int32_t> group, unordered_map<string, int64_t> ltime)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, unordered_map<string, int32_t>(), unordered_map<string, int64_t>());
        return;
    }

    if (gids.size() == 0 || mtypes.size() == 0)
    {
        callback(RTM_EC_EMPTY_PARAMETER, unordered_map<string, int32_t>(), unordered_map<string, int64_t>());
        return;
    }

    rtm->GetGroupUnread(gids, mtime, mtypes, [callback](int errorCode, unordered_map<string, int32_t> group, unordered_map<string, int64_t> ltime) {
        callback(errorCode, group, ltime);
    });
}

// imlib
void RTMProxy::CleanTargetUnread(unordered_set<int64_t> uids, unordered_set<int64_t> gids, function<void(int errorCode)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION);
        return;
    }

    if (gids.size() == 0 || uids.size() == 0)
    {
        callback(RTM_EC_EMPTY_PARAMETER);
        return;
    }

    rtm->CleanTargetUnread(uids, gids, [callback](int errorCode) {
        callback(errorCode);
    });
}

// imlib
void RTMProxy::GetP2PUnreadByTime(unordered_map<string, int64_t> uidTime, unordered_set<int8_t> mtypes, function<void(int errorCode, unordered_map<string, int32_t> p2p, unordered_map<string, int64_t> ltime)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, unordered_map<string, int32_t>(), unordered_map<string, int64_t>());
        return;
    }

    if (uidTime.size() == 0 || mtypes.size() == 0)
    {
        callback(RTM_EC_EMPTY_PARAMETER, unordered_map<string, int32_t>(), unordered_map<string, int64_t>());
        return;
    }

    rtm->GetP2PUnreadByTime(uidTime, mtypes, [callback](int errorCode, unordered_map<string, int32_t> p2p, unordered_map<string, int64_t> ltime) {
        callback(errorCode, p2p, ltime);
    });
}

// imlib
void RTMProxy::GetGroupUnreadByTime(unordered_map<string, int64_t> gidTime, unordered_set<int8_t> mtypes, function<void(int errorCode, unordered_map<string, int32_t> group, unordered_map<string, int64_t> ltime)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, unordered_map<string, int32_t>(), unordered_map<string, int64_t>());
        return;
    }

    if (gidTime.size() == 0 || mtypes.size() == 0)
    {
        callback(RTM_EC_EMPTY_PARAMETER, unordered_map<string, int32_t>(), unordered_map<string, int64_t>());
        return;
    }

    rtm->GetGroupUnreadByTime(gidTime, mtypes, [callback](int errorCode, unordered_map<string, int32_t> group, unordered_map<string, int64_t> ltime) {
        callback(errorCode, group, ltime);
    });
}


void RTMProxy::SetLang(string lang, function<void(int errorCode)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION);
        return;
    }

    if (lang.empty())
    {
        callback(RTM_EC_EMPTY_PARAMETER);
        return;
    }

    rtm->SetLang(lang, [callback](int errorCode) {
        callback(errorCode);
    });
}

void RTMProxy::Translate(string text, string src, string dst, string type, string profanity, function<void(int errorCode, string source, string target, string sourceText, string targetText)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, "", "", "", "");
        return;
    }

    if (text.empty() || src.empty() || dst.empty() || type.empty() || profanity.empty())
    {
        callback(RTM_EC_EMPTY_PARAMETER, "", "", "", "");
        return;
    }

    rtm->Translate(text, src, dst, type, profanity, [callback](int errorCode, string source, string target, string sourceText, string targetText) {
        callback(errorCode, source, target, sourceText, targetText);
    });
}

void RTMProxy::Profanity(string text, bool classify, function<void(int errorCode, string text, vector<string> classification)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, "", vector<string>());
        return;
    }

    if (text.empty())
    {
        callback(RTM_EC_EMPTY_PARAMETER, "", vector<string>());
        return;
    }

    rtm->Profanity(text, classify, [callback](int errorCode, string text, vector<string> classification) {
        callback(errorCode, text, classification);
    });
}

void RTMProxy::Speech2Text(string audio, int32_t type, string lang, string codec, int32_t srate, function<void(int errorCode, string text, string lang)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, "", "");
        return;
    }

    if (audio.empty() || lang.empty() || codec.empty())
    {
        callback(RTM_EC_EMPTY_PARAMETER, "", "");
        return;
    }

    rtm->Speech2Text(audio, type, lang, codec, srate, [callback](int errorCode, string text, string lang) {
        callback(errorCode, text, lang);
    });
}

void RTMProxy::TCheck(string text, function<void(int errorCode, int32_t result, string text, vector<string> tags, vector<string> wlist)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, 0, "", vector<string>(), vector<string>());
        return;
    }

    if (text.empty())
    {
        callback(RTM_EC_EMPTY_PARAMETER, 0, "", vector<string>(), vector<string>());
        return;
    }

    rtm->TCheck(text, [callback](int errorCode, int32_t result, string text, vector<string> tags, vector<string> wlist) {
        callback(errorCode, result, text, tags, wlist);
    });
}

void RTMProxy::ICheck(string image, int32_t type, function<void(int errorCode, int32_t result, vector<string> tags)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, 0, vector<string>());
        return;
    }

    if (image.empty())
    {
        callback(RTM_EC_EMPTY_PARAMETER, 0, vector<string>());
        return;
    }

    rtm->ICheck(image, type, [callback](int errorCode, int32_t result, vector<string> tags) {
        callback(errorCode, result, tags);
    });
}

void RTMProxy::ACheck(string audio, int32_t type, string lang, string codec, int32_t srate, function<void(int errorCode, int32_t result, vector<string> tags)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, 0, vector<string>());
        return;
    }

    if (audio.empty() || lang.empty() || codec.empty())
    {
        callback(RTM_EC_EMPTY_PARAMETER, 0, vector<string>());
        return;
    }

    rtm->ACheck(audio, type, lang, codec, srate, [callback](int errorCode, int32_t result, vector<string> tags) {
        callback(errorCode, result, tags);
    });
}

void RTMProxy::VCheck(string video, int32_t type, string videoName, function<void(int errorCode, int32_t result, vector<string> tags)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, 0, vector<string>());
        return;
    }

    if (video.empty() || videoName.empty())
    {
        callback(RTM_EC_EMPTY_PARAMETER, 0, vector<string>());
        return;
    }

    rtm->VCheck(video, type, videoName, [callback](int errorCode, int32_t result, vector<string> tags) {
        callback(errorCode, result, tags);
    });
}


void RTMProxy::FileToken(int64_t xid, int16_t sendType, function<void(int errorCode, string token, string endpoint)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, "", "");
        return;
    }

    rtm->FileToken(xid, sendType, [callback](int errorCode, string token, string endpoint) {
        callback(errorCode, token, endpoint);
    });
}

void RTMProxy::SendFile(function<void(int errorCode, int64_t mtime)> callback, int32_t pid, int8_t mtype, std::string fileText, std::string fileName, std::string fileExt, std::string userAttrs, int timeout)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, 0);
        return;
    }

    rtm->SendFile([callback](int errorCode, int64_t mtime) {
        callback(errorCode, mtime);
        }, pid, mtype, fileText, fileName, fileExt, userAttrs, timeout);
}

void RTMProxy::SendGroupFile(function<void(int errorCode, int64_t mtime)> callback, int32_t gid, int8_t mtype, std::string fileText, std::string fileName, std::string fileExt, std::string userAttrs, int timeout)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, 0);
        return;
    }

    rtm->SendGroupFile([callback](int errorCode, int64_t mtime) {
        callback(errorCode, mtime);
        }, gid, mtype, fileText, fileName, fileExt, userAttrs, timeout);
}

void RTMProxy::SendRoomFile(function<void(int errorCode, int64_t mtime)> callback, int32_t rid, int8_t mtype, std::string fileText, std::string fileName, std::string fileExt, std::string userAttrs, int timeout)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, 0);
        return;
    }

    rtm->SendRoomFile([callback](int errorCode, int64_t mtime) {
        callback(errorCode, mtime);
        }, rid, mtype, fileText, fileName, fileExt, userAttrs, timeout);
}

void RTMProxy::SendFile(function<void(int errorCode, int64_t mtime)> callback, int32_t pid, RTMAudio rtmAudio, int timeout)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, 0);
        return;
    }

    rtm->SendFile([callback](int errorCode, int64_t mtime) {
        callback(errorCode, mtime);
        }, pid, rtmAudio, timeout);
}

void RTMProxy::SendGroupFile(function<void(int errorCode, int64_t mtime)> callback, int32_t gid, RTMAudio rtmAudio, int timeout)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, 0);
        return;
    }

    rtm->SendGroupFile([callback](int errorCode, int64_t mtime) {
        callback(errorCode, mtime);
        }, gid, rtmAudio, timeout);
}

void RTMProxy::SendRoomFile(function<void(int errorCode, int64_t mtime)> callback, int32_t rid, RTMAudio rtmAudio, int timeout)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, 0);
        return;
    }

    rtm->SendRoomFile([callback](int errorCode, int64_t mtime) {
        callback(errorCode, mtime);
        }, rid, rtmAudio, timeout);
}


void RTMProxy::GetOnlineUsers(unordered_set<int64_t> uids, function<void(int errorCode, unordered_set<int64_t> uids)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, unordered_set<int64_t>());
        return;
    }

    if (uids.size()==0)
    {
        callback(RTM_EC_EMPTY_PARAMETER, unordered_set<int64_t>());
        return;
    }

    rtm->GetOnlineUsers(uids, [callback](int errorCode, unordered_set<int64_t> uids) {
        callback(errorCode, uids);
    });
}

void RTMProxy::SetUserInfo(string oinfo, string pinfo, function<void(int errorCode)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION);
        return;
    }

    if (oinfo.empty() || pinfo.empty())
    {
        callback(RTM_EC_EMPTY_PARAMETER);
        return;
    }

    rtm->SetUserInfo(oinfo, pinfo, [callback](int errorCode) {
        callback(errorCode);
    });
}

void RTMProxy::GetUserInfo(function<void(int errorCode, string oinfo, string pinfo)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, "", "");
        return;
    }

    rtm->GetUserInfo([callback](int errorCode, string oinfo, string pinfo) {
        callback(errorCode, oinfo, pinfo);
    });
}

void RTMProxy::GetUserOpenInfo(unordered_set<int64_t> uids, function<void(int errorCode, unordered_map<string, string> info)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, unordered_map<string, string>());
        return;
    }

    if (uids.size() == 0)
    {
        callback(RTM_EC_EMPTY_PARAMETER, unordered_map<string, string>());
        return;
    }

    rtm->GetUserOpenInfo(uids, [callback](int errorCode, unordered_map<string, string> info) {
        callback(errorCode, info);
    });
}


void RTMProxy::AddFriends(unordered_set<int64_t> friends, function<void(int errorCode)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION);
        return;
    }

    if (friends.size() == 0)
    {
        callback(RTM_EC_EMPTY_PARAMETER);
        return;
    }

    rtm->AddFriends(friends, [callback](int errorCode) {
        callback(errorCode);
    });
}

void RTMProxy::DelFriends(unordered_set<int64_t> friends, function<void(int errorCode)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION);
        return;
    }

    if (friends.size() == 0)
    {
        callback(RTM_EC_EMPTY_PARAMETER);
        return;
    }

    rtm->DelFriends(friends, [callback](int errorCode) {
        callback(errorCode);
    });
}

void RTMProxy::GetFriends(function<void(int errorCode, unordered_set<int64_t> uids)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, unordered_set<int64_t>());
        return;
    }

    rtm->GetFriends([callback](int errorCode, unordered_set<int64_t> uids) {
        callback(errorCode, uids);
    });
}

void RTMProxy::AddBlacks(unordered_set<int64_t> blacks, function<void(int errorCode)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION);
        return;
    }

    if (blacks.size() == 0)
    {
        callback(RTM_EC_EMPTY_PARAMETER);
        return;
    }

    rtm->AddBlacks(blacks, [callback](int errorCode) {
        callback(errorCode);
    });
}

void RTMProxy::DelBlacks(unordered_set<int64_t> blacks, function<void(int errorCode)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION);
        return;
    }

    if (blacks.size() == 0)
    {
        callback(RTM_EC_EMPTY_PARAMETER);
        return;
    }

    rtm->DelBlacks(blacks, [callback](int errorCode) {
        callback(errorCode);
    });
}

void RTMProxy::GetBlacks(function<void(int errorCode, unordered_set<int64_t> uids)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, unordered_set<int64_t>());
        return;
    }

    rtm->GetBlacks([callback](int errorCode, unordered_set<int64_t> uids) {
        callback(errorCode, uids);
    });
}


void RTMProxy::AddGroupMembers(int64_t gid, unordered_set<int64_t> uids, function<void(int errorCode)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION);
        return;
    }

    if (uids.size() == 0)
    {
        callback(RTM_EC_EMPTY_PARAMETER);
        return;
    }

    rtm->AddGroupMembers(gid, uids, [callback](int errorCode) {
        callback(errorCode);
    });
}

void RTMProxy::DelGroupMembers(int64_t gid, unordered_set<int64_t> uids, function<void(int errorCode)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION);
        return;
    }

    if (uids.size() == 0)
    {
        callback(RTM_EC_EMPTY_PARAMETER);
        return;
    }

    rtm->DelGroupMembers(gid, uids, [callback](int errorCode) {
        callback(errorCode);
    });
}

void RTMProxy::GetGroupMembers(int64_t gid, bool online, function<void(int errorCode, unordered_set<int64_t> uids, unordered_set<int64_t> onlines)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, unordered_set<int64_t>(), unordered_set<int64_t>());
        return;
    }

    rtm->GetGroupMembers(gid, online, [callback](int errorCode, unordered_set<int64_t> uids, unordered_set<int64_t> onlines) {
        callback(errorCode, uids, onlines);
    });
}

void RTMProxy::GetGroupCount(int64_t gid, bool online, function<void(int errorCode, int32_t cn, int32_t onlines)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, 0, 0);
        return;
    }

    rtm->GetGroupCount(gid, online, [callback](int errorCode, int32_t cn, int32_t onlines) {
        callback(errorCode, cn, onlines);
    });
}

void RTMProxy::GetUserGroups(function<void(int errorCode, unordered_set<int64_t> gids)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, unordered_set<int64_t>());
        return;
    }

    rtm->GetUserGroups([callback](int errorCode, unordered_set<int64_t> gids) {
        callback(errorCode, gids);
    });
}

void RTMProxy::SetGroupInfo(int64_t gid, string oinfo, string pinfo, function<void(int errorCode)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION);
        return;
    }

    rtm->SetGroupInfo(gid, oinfo, pinfo, [callback](int errorCode) {
        callback(errorCode);
    });
}

void RTMProxy::GetGroupOpenInfo(int64_t gid, function<void(int errorCode, string oinfo)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, "");
        return;
    }

    rtm->GetGroupOpenInfo(gid, [callback](int errorCode, string oinfo) {
        callback(errorCode, oinfo);
    });
}

void RTMProxy::GetGroupsOpenInfo(unordered_set<int64_t> gids, function<void(int errorCode, unordered_map<string, string> info)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, unordered_map<string, string>());
        return;
    }

    if (gids.size() == 0)
    {
        callback(RTM_EC_EMPTY_PARAMETER, unordered_map<string, string>());
        return;
    }

    rtm->GetGroupsOpenInfo(gids, [callback](int errorCode, unordered_map<string, string> info) {
        callback(errorCode, info);
    });
}


void RTMProxy::EnterRoom(int64_t rid, function<void(int errorCode)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION);
        return;
    }

    rtm->EnterRoom(rid, [callback](int errorCode) {
        callback(errorCode);
    });
}

void RTMProxy::LeaveRoom(int64_t rid, function<void(int errorCode)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION);
        return;
    }

    rtm->LeaveRoom(rid, [callback](int errorCode) {
        callback(errorCode);
    });
}

void RTMProxy::GetUserRooms(function<void(int errorCode, unordered_set<int64_t> rooms)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, unordered_set<int64_t>());
        return;
    }

    rtm->GetUserRooms([callback](int errorCode, unordered_set<int64_t> rooms) {
        callback(errorCode, rooms);
    });
}

void RTMProxy::SetRoomInfo(int64_t rid, string oinfo, string pinfo, function<void(int errorCode)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION);
        return;
    }

    if (oinfo.empty() || pinfo.empty())
        callback(RTM_EC_EMPTY_PARAMETER);

    rtm->SetRoomInfo(rid, oinfo, pinfo, [callback](int errorCode) {
        callback(errorCode);
    });
}

void RTMProxy::GetRoomInfo(int64_t rid, function<void(int errorCode, string oinfo, string pinfo)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, "", "");
        return;
    }

    rtm->GetRoomInfo(rid, [callback](int errorCode, string oinfo, string pinfo) {
        callback(errorCode, oinfo, pinfo);
    });
}

void RTMProxy::GetRoomOpenInfo(int64_t rid, function<void(int errorCode, string oinfo)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, "");
        return;
    }

    rtm->GetRoomOpenInfo(rid, [callback](int errorCode, string oinfo) {
        callback(errorCode, oinfo);
    });
}

void RTMProxy::GetRoomsOpenInfo(unordered_set<int64_t> rids, function<void(int errorCode, unordered_map<string, string> info)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, unordered_map<string, string>());
        return;
    }

    if (rids.size() == 0)
    {
        callback(RTM_EC_EMPTY_PARAMETER, unordered_map<string, string>());
        return;
    }

    rtm->GetRoomsOpenInfo(rids, [callback](int errorCode, unordered_map<string, string> info) {
        callback(errorCode, info);
    });
}

void RTMProxy::GetRoomMemers(int64_t rid, function<void(int errorCode, unordered_set<int64_t> uids)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, unordered_set<int64_t>());
        return;
    }

    rtm->GetRoomMemers(rid, [callback](int errorCode, unordered_set<int64_t> uids) {
        callback(errorCode, uids);
    });
}

void RTMProxy::GetRoomCount(unordered_set<int64_t> rids, function<void(int errorCode, unordered_map<string, int32_t> info)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, unordered_map<string, int32_t>());
        return;
    }

    if (rids.size() == 0)
    {
        callback(RTM_EC_EMPTY_PARAMETER, unordered_map<string, int32_t>());
        return;
    }

    rtm->GetRoomCount(rids, [callback](int errorCode, unordered_map<string, int32_t> info) {
        callback(errorCode, info);
    });
}


void RTMProxy::DataGet(string key, function<void(int errorCode, string val)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION, "");
        return;
    }

    if (key.empty())
    {
        callback(RTM_EC_EMPTY_PARAMETER, "");
        return;
    }

    rtm->DataGet(key, [callback](int errorCode, string val) {
        callback(errorCode, val);
    });
}

void RTMProxy::DataSet(string key, string val, function<void(int errorCode)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION);
        return;
    }

    if (key.empty() || val.empty())
    {
        callback(RTM_EC_EMPTY_PARAMETER);
        return;
    }

    rtm->DataSet(key, val, [callback](int errorCode) {
        callback(errorCode);
    });
}

void RTMProxy::DataDel(string key, function<void(int errorCode)> callback)
{
    if (!rtm)
    {
        callback(FPNN_EC_CORE_INVALID_CONNECTION);
        return;
    }

    if (key.empty())
    {
        callback(RTM_EC_EMPTY_PARAMETER);
        return;
    }

    rtm->DataDel(key, [callback](int errorCode) {
        callback(errorCode);
    });
}

bool RTMProxy::StartRecordAudio()
{
    if (!rtm)
        return false;

    return rtm->StartRecordAudio();
}

void RTMProxy::StopRecordAudio(RTMAudio& recordata)
{
    if (!rtm)
        return;

    rtm->StopRecordAudio(recordata);
}


bool RTMProxy::StartPlayAudio(RTMAudio recordata)
{
    if (!rtm)
        return false;

    return rtm->StartPlayAudio(recordata);
}

bool RTMProxy::StopPlayAudio()
{
    if (!rtm)
        return false;

    return rtm->StopPlayAudio();
}