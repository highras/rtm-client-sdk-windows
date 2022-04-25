#include "RTCProxy.h"
#include "RTMClient.h"
#include "RTCClient.h"

#include <AudioPlayer.h>
#include <AudioRecorder.h>
#include <D3D12Renderer.h>

#include "OpenH264Decoder.h"

RTCProxy::RTCProxy(string rtmhost, unsigned short rtmport, int64_t pid, int64_t uid, shared_ptr<RTMEventHandler> rtmhandler, string rtchost, unsigned short rtcport, shared_ptr<RTCEventHandler> rtchandler):
    RTMProxy(rtmhost,rtmport, pid, uid, rtmhandler),
	rtc(new RTCClient(rtchost,rtcport)),
	player(new AudioPlayer()),
	recorder(new AudioRecorder())
{
	rtm->SetRTCEventHandler(make_shared<InternalEventHandler>(rtchandler, this));
    rtc->SetVideoCallback([this](int64_t rid, int64_t uid, int64_t seq, int64_t flags, int64_t timestamp, int64_t rotation, int64_t version, int32_t facing, int32_t captureLevel, vector<unsigned char> data, vector<unsigned char> sps, vector<unsigned char> pps) {
        auto mapiter = userMaps.find(uid);
        if (mapiter != userMaps.end())
        {
            if (mapiter->second->captureLevel != captureLevel)
            {
                mapiter->second->captureLevel = captureLevel;
                if (mapiter->second->hTimer != NULL)
                {
                    HANDLE hComplete = CreateEvent(NULL, true, false, NULL);
                    DeleteTimerQueueTimer(NULL, mapiter->second->hTimer, hComplete);
                    WaitForSingleObject(hComplete, INFINITE);
                    CloseHandle(hComplete);
                }
                int frameRate = 0;
                if (captureLevel == 1)
                {
                    frameRate = 15;
                }
                else
                {
                    frameRate = 30;
                }
                CreateTimerQueueTimer(&(mapiter->second->hTimer), NULL, TimerProc, mapiter->second, 0, 1000 / frameRate, WT_EXECUTEDEFAULT);
            }

            if (mapiter->second->decoder->IsInited())
            {
                unique_lock<mutex> lck(mapiter->second->dataMutex);

                auto iter = upper_bound(mapiter->second->frameQueue.begin(), mapiter->second->frameQueue.end(), seq,
                    [](const long long a, const pair<long long, vector<BYTE>>& b) {return a < b.first; });

                mapiter->second->frameQueue.emplace(iter, make_pair(seq, data));

                if (mapiter->second->frameQueue.size() > 3)
                {
                    mapiter->second->isReady = true;
                }

            }
            else
            {
                if (sps.size() > 0 && pps.size() > 0)
                {
                    mapiter->second->decoder->Decode(sps.data(), sps.size());
                    mapiter->second->decoder->Decode(pps.data(), pps.size());
                    mapiter->second->decoder->SetInited(true);
                }
            }
        }
        });

	rtc->SetP2PVideoCallback([this](int64_t uid, int64_t seq, int64_t flags, int64_t timestamp, int64_t rotation, int64_t version, int32_t facing, int32_t captureLevel, vector<unsigned char> data, vector<unsigned char> sps, vector<unsigned char> pps) {
		if (p2pUserData != nullptr)
		{
			if (p2pUserData->captureLevel != captureLevel)
			{
				p2pUserData->captureLevel = captureLevel;
				if (p2pUserData->hTimer != NULL)
				{
					HANDLE hComplete = CreateEvent(NULL, true, false, NULL);
					DeleteTimerQueueTimer(NULL, p2pUserData->hTimer, hComplete);
					WaitForSingleObject(hComplete, INFINITE);
					CloseHandle(hComplete);
				}
				int frameRate = 0;
				if (captureLevel == 1)
				{
					frameRate = 15;
				}
				else
				{
					frameRate = 30;
				}
				CreateTimerQueueTimer(&(p2pUserData->hTimer), NULL, TimerProc, p2pUserData, 0, 1000 / frameRate, WT_EXECUTEDEFAULT);
			}

			if (p2pUserData->decoder->IsInited())
			{
				unique_lock<mutex> lck(p2pUserData->dataMutex);

				auto iter = upper_bound(p2pUserData->frameQueue.begin(), p2pUserData->frameQueue.end(), seq,
					[](const long long a, const pair<long long, vector<BYTE>>& b) {return a < b.first; });

				p2pUserData->frameQueue.emplace(iter, make_pair(seq, data));

				if (p2pUserData->frameQueue.size() > 3)
				{
					p2pUserData->isReady = true;
				}

			}
			else
			{
				if (sps.size() > 0 && pps.size() > 0)
				{
					p2pUserData->decoder->Decode(sps.data(), sps.size());
					p2pUserData->decoder->Decode(pps.data(), pps.size());
					p2pUserData->decoder->SetInited(true);
				}
			}
		}
		});

    rtc->SetAudioCallback([this](int64_t uid, int64_t rid, int64_t seq, int64_t timestamp, vector<unsigned char>data) {
        player->PutAudioData(uid, timestamp, (char*)data.data(), data.size());
        });
	rtc->SetP2PAudioCallback([this](int64_t uid, int64_t seq, int64_t timestamp, vector<unsigned char>data) {
		player->PutAudioData(uid, timestamp, (char*)data.data(), data.size());
		});
    recorder->SetAudioCallback([this](shared_ptr<vector<BYTE>> data) {
        if (p2pStatus == 2)
        {
			if (!muted)
				rtc->SendP2PAudioData(audioSeq++, chrono::steady_clock::now().time_since_epoch().count() / 1000000, *data);
        }
        else
        {
            if (!muted)
                rtc->SendAudioData(currentRid, audioSeq++, chrono::steady_clock::now().time_since_epoch().count() / 1000000, *data);
        }
        });
}

RTCProxy::~RTCProxy()
{
    recorder->Stop();
    player->Stop();
}

void RTCProxy::StartAudio()
{
	player->Start();
	recorder->Start();
}

void RTCProxy::StopAudio()
{
	recorder->Stop();
	player->Stop();
}

void RTCProxy::CreateRTCRoom(int32_t type, int64_t rid, int32_t enableRecord, function<void(int errorCode, bool microphone)> callback)
{
    if (!busy)
    {
        rtm->CreateRTCRoom(type, rid, enableRecord, [this, rid, callback](int errorCode, string token) {
            if (errorCode == 0)
            {
                currentRid = rid;
				rtc->EnterRTCRoom(currentPid, rid, currentUid, token, [this,callback](int errorCode, bool microphone) {
					busy = true;
					callback(errorCode, microphone); 
					});
            }
            else
            {
                callback(errorCode, false);
            }
            });
    }
}

void RTCProxy::EnterRTCRoom(int64_t rid, function<void(int errorCode, bool microphone)> callback)
{
    if (!busy)
    {
        rtm->EnterRTCRoom(rid, [this, rid, callback](int errorCode, string token) {
            if (errorCode == 0)
            {
                rtc->EnterRTCRoom(currentPid, rid, currentUid, token, [rid, this, callback](int errorCode, bool microphone) {
                    if (errorCode == 0)
                    {
                        currentRid = rid;
						busy = true;
                    }
                    callback(errorCode, microphone);
                    });
            }
            else
            {
                callback(errorCode, false);
            }
            });
    }
}

void RTCProxy::ExitRTCRoom(function<void(int errorCode)> callback)
{
	rtm->ExitRTCRoom(currentPid, [callback,this](int errorCode) {
		{
			unique_lock<mutex> lck(drmutex);
			for (auto item : userMaps)
			{
				if (item.second->hTimer != NULL)
				{
					HANDLE hComplete = CreateEvent(NULL, true, false, NULL);
					DeleteTimerQueueTimer(NULL, item.second->hTimer, hComplete);
					WaitForSingleObject(hComplete, INFINITE);
					CloseHandle(hComplete);
				}
				delete item.second->decoder;
				delete item.second->renderer;
				delete item.second;
			}
			userMaps.clear();
		}
		busy = false;
		callback(errorCode);
		});
}

void RTCProxy::SubscribeVideo(int64_t rid, int64_t uid, HWND__* hwnd, uint32_t width, uint32_t height, function<void(int errorCode)> callback)
{
	unordered_set<int64_t> uids;
	uids.insert(uid);
	unique_lock<mutex> lck(drmutex);
	UserData* userData = new UserData();
	userData->decoder = new OpenH264Decoder(640, 480);
	userData->renderer = new D3D12Renderer(width, height, hwnd);
	userData->isReady = false;
	userData->hTimer = NULL;
	auto p = make_pair(uid, userData);
	userMaps.emplace(p);
	rtm->SubscribeVideo(rid, uids, [this, callback, uid](int errorCode) {
		if (errorCode != 0)
		{
			unique_lock<mutex> lck(drmutex);
			auto iter = userMaps.find(uid);
			delete iter->second->decoder;
			delete iter->second->renderer;
			delete iter->second;
			userMaps.erase(uid);
		}
		else
		{
			auto iter = userMaps.find(uid);
			iter->second->renderer->OnInit();
			iter->second->renderer->ChangeTextureSize(640, 480);
		}
		callback(errorCode);
		});
}

void RTCProxy::TimerProc(void* lpParameter, unsigned char TimerOrWaitFired)
{
	UserData* userData = (UserData*)lpParameter;
	unique_lock<mutex> lck(userData->dataMutex);
	if (userData->isReady)
	{
		vector<BYTE> data = userData->frameQueue.front().second;
		userData->frameQueue.pop_front();
		vector<BYTE> result = userData->decoder->Decode(data.data(), data.size());

		if (result.size() > 0)
		{
			userData->renderer->DrawFrame(result);
		}

		if (userData->frameQueue.size() == 0)
		{
			userData->isReady = false;
		}
	}
}

void RTCProxy::UnsubscribeVideo(int64_t rid, int64_t uid, function<void(int errorCode)> callback)
{
    unordered_set<int64_t> uids;
    uids.insert(uid);
    rtm->UnsubscribeVideo(rid, uids, [this,callback,uid](int errorCode) {
        if (errorCode == 0)
        {
            unique_lock<mutex> lck(drmutex);
            auto iter = userMaps.find(uid);
            if (iter->second->hTimer != NULL)
            {
                HANDLE hComplete = CreateEvent(NULL, true, false, NULL);
                DeleteTimerQueueTimer(NULL, iter->second->hTimer, hComplete);
                WaitForSingleObject(hComplete, INFINITE);
                CloseHandle(hComplete);
            }
            delete iter->second->decoder;
            delete iter->second->renderer;
            delete iter->second;
            userMaps.erase(uid);
        }
        callback(errorCode);
        });
}

void RTCProxy::Mute()
{
    muted = true;
}

void RTCProxy::Unmute()
{
    muted = false;
}

void RTCProxy::InviteUserIntoRTCRoom(int64_t rid, unordered_set<int64_t> uids, function<void(int errorCode)> callback)
{
	rtm->InviteUserIntoRTCRoom(rid, uids, [callback](int errorCode) {callback(errorCode); });
}

void RTCProxy::GetRTCRoomMembers(int64_t rid, function<void(int errorCode, RTCRoomMembers roomMembers)> callback)
{
	rtm->GetRTCRoomMembers(rid, [callback](int errorCode, RTMClient::RTCRoomMembers roomMembers) {callback(errorCode, *(RTCRoomMembers*)&roomMembers); });
}

void RTCProxy::GetRTCRoomMemberCount(int64_t rid, function<void(int errorCode, int64_t count)> callback)
{
	rtm->GetRTCRoomMemberCount(rid, [callback](int errorCode, int32_t count) {callback(errorCode, count); });
}

void RTCProxy::BlockUserVoiceInRTCRoom(int64_t rid, unordered_set<int64_t> uids, function<void(int errorCode)> callback)
{
	rtm->BlockUserVoiceInRTCRoom(rid, uids, [callback](int errorCode) {callback(errorCode); });
}

void RTCProxy::UnblockUserVoiceInRTCRoom(int64_t rid, unordered_set<int64_t> uids, function<void(int errorCode)> callback)
{
	rtm->UnblockUserVoiceInRTCRoom(rid, uids, [callback](int errorCode) {callback(errorCode); });
}

void RTCProxy::AdminCommand(int64_t rid, unordered_set<int64_t> uids, RTCEventHandler::AdminCommand command, function<void(int errorCode)> callback)
{
	rtm->AdminCommand(rid, uids, command, [callback](int errorCode) {callback(errorCode); });
}

void RTCProxy::RoomEvent(int64_t rid, vector<unsigned char> revent, function<void(int errorCode)> callback)
{
	rtm->RoomEvent(rid, revent, [callback](int errorCode) {callback(errorCode); });
}

void RTCProxy::RequestP2PRTC(int32_t type, int64_t peerUid, HWND__* hwnd, uint32_t width, uint32_t height, function<void(int errorCode)> callback)
{
    if (p2pStatus == 0 && (!busy))
    {
        p2pStatus = 1;
		busy = true;
		rtm->RequestP2PRTC(type, peerUid, [type, this, callback, hwnd, width, height](int errorCode, int64_t callId) {
			if (errorCode != 0)
			{
				p2pStatus = 0;
				busy = false;
				callback(errorCode);
			}
			else
			{
				if (type == 2)
				{
					p2pCallId = callId;
					UserData* userData = new UserData();
					userData->decoder = new OpenH264Decoder(640, 480);
					userData->renderer = new D3D12Renderer(width, height, hwnd);
					userData->isReady = false;
					userData->hTimer = NULL;
					userData->renderer->OnInit();
					userData->renderer->ChangeTextureSize(640, 480);
					p2pUserData = userData;
				}
				callback(errorCode);
			}
			});
    }
}

void RTCProxy::CancelP2PRTC(int64_t callId, function<void(int errorCode)> callback)
{
    if (p2pStatus == 1)
    {
		rtm->CancelP2PRTC(callId, [this, callback](int errorCode) {
			if (errorCode != 0)
			{
				callback(errorCode);
			}
			else
			{
				p2pStatus = 0;
				p2pCallId = 0;
				if (p2pUserData != nullptr)
				{
					if (p2pUserData->hTimer != NULL)
					{
						HANDLE hComplete = CreateEvent(NULL, true, false, NULL);
						DeleteTimerQueueTimer(NULL, p2pUserData->hTimer, hComplete);
						WaitForSingleObject(hComplete, INFINITE);
						CloseHandle(hComplete);
					}
					delete p2pUserData->decoder;
					delete p2pUserData->renderer;
					delete p2pUserData;
					p2pUserData = nullptr;
				}
				busy = false;
				callback(errorCode);
			}
			});
    }
}

void RTCProxy::CloseP2PRTC(int64_t callId, function<void(int errorCode)> callback)
{
	if (p2pStatus == 2)
	{
		rtm->CloseP2PRTC(callId, [this, callback](int errorCode) {
			if (errorCode != 0)
			{
				callback(errorCode);
			}
			else
			{
				p2pCallId = 0;
				p2pStatus = 0;
				if (p2pUserData != nullptr)
				{
					if (p2pUserData->hTimer != NULL)
					{
						HANDLE hComplete = CreateEvent(NULL, true, false, NULL);
						DeleteTimerQueueTimer(NULL, p2pUserData->hTimer, hComplete);
						WaitForSingleObject(hComplete, INFINITE);
						CloseHandle(hComplete);
					}
					delete p2pUserData->decoder;
					delete p2pUserData->renderer;
					delete p2pUserData;
					p2pUserData = nullptr;
				}
				busy = false;
				callback(errorCode);
			}
			});
	}
}

void RTCProxy::AcceptP2PRTC(int64_t callId, HWND__* hwnd, uint32_t width, uint32_t height, function<void(int errorCode)> callback)
{
	if (p2pStatus == 3)
	{
		rtm->AcceptP2PRTC(callId, [this, callback, hwnd, width, height](int errorCode) {
			if (errorCode != 0)
			{
				callback(errorCode);
			}
			else
			{
				UserData* userData = new UserData();
				userData->decoder = new OpenH264Decoder(640, 480);
				userData->renderer = new D3D12Renderer(width, height, hwnd);
				userData->isReady = false;
				userData->hTimer = NULL;
				userData->renderer->OnInit();
				userData->renderer->ChangeTextureSize(640, 480);
				p2pUserData = userData;

				p2pStatus = 2;
				callback(errorCode);
			}
			});
	}
}

void RTCProxy::RefuseP2PRTC(int64_t callId, function<void(int errorCode)> callback)
{
	if (p2pStatus == 3)
	{
		rtm->RefuseP2PRTC(callId, [this, callback](int errorCode) {
			if (errorCode != 0)
			{
				callback(errorCode);
			}
			else
			{
				p2pStatus = 0;
				callback(errorCode);
			}
			});
	}
}

RTCProxy::InternalEventHandler::InternalEventHandler(shared_ptr<RTCEventHandler> handler, RTCProxy* proxy):
	userEventHandler(handler),
	rtcProxy(proxy)
{

}

RTCProxy::InternalEventHandler::~InternalEventHandler() = default;

void RTCProxy::InternalEventHandler::OnUserEnterRTCRoom(int64_t uid, int64_t rid, int64_t mtime)
{
	userEventHandler->OnUserEnterRTCRoom(uid, rid, mtime);
}

void RTCProxy::InternalEventHandler::OnUserExitRTCRoom(int64_t uid, int64_t rid, int64_t mtime)
{
	userEventHandler->OnUserExitRTCRoom(uid, rid, mtime);
}

void RTCProxy::InternalEventHandler::OnRTCRoomClosed(int64_t rid)
{
	userEventHandler->OnRTCRoomClosed(rid);
}

void RTCProxy::InternalEventHandler::OnInviteIntoRTCRoom(int64_t fromUid, int64_t rid)
{
	userEventHandler->OnInviteIntoRTCRoom(fromUid, rid);
}

void RTCProxy::InternalEventHandler::OnKickOutFromRTCRoom(int64_t fromUid, int64_t rid)
{
	userEventHandler->OnKickOutFromRTCRoom(fromUid, rid);
}

void RTCProxy::InternalEventHandler::OnPullIntoRTCRoom(int64_t rid, string token)
{
	userEventHandler->OnPullIntoRTCRoom(rid, token);
}

void RTCProxy::InternalEventHandler::OnAdminCommand(AdminCommand command, vector<int64_t> uids)
{
	userEventHandler->OnAdminCommand(command, uids);
}

void RTCProxy::InternalEventHandler::OnRoomEvent(int64_t rid, RoomEvent roomEvent, vector<unsigned char> eventData)
{
	userEventHandler->OnRoomEvent(rid, roomEvent, eventData);
}

void RTCProxy::InternalEventHandler::OnPushP2PRTCRequest(int64_t callId, int64_t peerUid, int32_t type)
{
	rtcProxy->p2pStatus = 3;
	rtcProxy->busy = true;
	userEventHandler->OnPushP2PRTCRequest(callId, peerUid, type);
}

void RTCProxy::InternalEventHandler::OnPushP2PRTCEvent(int64_t callId, int64_t peerUid, int32_t type, int32_t p2pEvent)
{
	if (p2pEvent == 1 && rtcProxy->p2pStatus == 3)
	{
		rtcProxy->p2pStatus = 0;
	}
	else if (p2pEvent == 2 && rtcProxy->p2pStatus == 2)
	{
		rtcProxy->p2pStatus = 0;
	}
	else if (p2pEvent == 3 && rtcProxy->p2pStatus == 1)
	{
	}
	else if (p2pEvent == 4 && rtcProxy->p2pStatus == 1)
	{
		rtcProxy->p2pStatus = 0;
	}
	else if (p2pEvent == 5 && rtcProxy->p2pStatus == 1)
	{
		rtcProxy->p2pStatus = 0;
	}
	userEventHandler->OnPushP2PRTCEvent(callId, peerUid, type, p2pEvent);
}
