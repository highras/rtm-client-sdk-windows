#pragma once
#include <mutex>
#include <unordered_map>
#include "fpnn.h"

class RTMMsgFilter
{
enum class MsgCategories
{
    P2PMessageType,
    GroupMessageType,
    RoomMessageType,
    BroadcastMessageType,
};

struct MsgIdUnit
{
    MsgCategories messageType;
    long bizId;
    long uid;
    long mid;

    bool operator==(const MsgIdUnit& p) const
    {
        return messageType == p.messageType && bizId == p.bizId && uid == p.uid && mid == p.mid;
    }
};

class MsgHashFunction {
public:
    size_t operator()(const MsgIdUnit& p) const
    {
        return (hash<long>()(p.bizId)) + (hash<long>()(p.uid)) + (hash<long>()(p.mid)) + (hash<MsgCategories>()(p.messageType));
    }
};

const int expireSeconds = 20 * 60;
std::unordered_map<MsgIdUnit, long, MsgHashFunction> midCache;
std::mutex mux;

public:
    bool isContainMsg(long uid, long mid)
    {
        MsgIdUnit unit;
        unit.messageType = MsgCategories::P2PMessageType;
        unit.bizId = 0;
        unit.uid = uid;
        unit.mid = mid;

        return isContainMsgMsgIdUnit(unit);
    }

    bool isContainMsgGroupMsg(long groupId, long uid, long mid)
    {
        MsgIdUnit unit;
        unit.messageType = MsgCategories::GroupMessageType;
        unit.bizId = groupId;
        unit.uid = uid;
        unit.mid = mid;

        return isContainMsgMsgIdUnit(unit);
    }

    bool isContainMsgRoomMsg(long roomId, long uid, long mid)
    {
        MsgIdUnit unit;
        unit.messageType = MsgCategories::RoomMessageType;
        unit.bizId = roomId;
        unit.uid = uid;
        unit.mid = mid;

        return isContainMsgMsgIdUnit(unit);
    }

    bool isContainMsgBroadcastMsg(long uid, long mid)
    {
        MsgIdUnit unit;
        unit.messageType = MsgCategories::BroadcastMessageType;
        unit.bizId = 0;
        unit.uid = uid;
        unit.mid = mid;

        return isContainMsgMsgIdUnit(unit);
    }

    bool isContainMsgMsgIdUnit(MsgIdUnit unit)
    {
        long now = slack_real_sec();

        std::lock_guard<std::mutex> lock(mux);
        {
            if (midCache.find(unit) != midCache.end())
            {
                midCache[unit] = now;
                return true;
            }
            else
            {
                midCache.emplace(unit, now);
                clearExpired(now);
                return false;
            }
        }
    }

    void clearExpired(long now)
    {
        now -= expireSeconds;
        std::list<MsgIdUnit> expired;

        for (auto kvp : midCache)
        {
            if (kvp.second <= now)
                expired.push_back(kvp.first);
        }

        for (auto e : expired)
            midCache.erase(e);
    }
};
