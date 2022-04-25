/**************************************RTC接口**************************************/	
/**
* @desc		创建房间
* @param	type			房间类型(默认2)
* @param	rid				房间id
* @param	enableRecord	录制标志
* @param	microphone		麦克风标志
* @param	errorCode		错误码
* @param	callback		错误标志
* @return 	
*/
void CreateRTCRoom(int32_t type, int64_t rid, int32_t enableRecord, function<void(int errorCode, bool microphone)> callback);

/**
* @desc		进入房间
* @param	rid				房间id
* @param	errorCode		错误码
* @param	callback		错误标志
* @return 	
*/
void EnterRTCRoom(int64_t rid, function<void(int errorCode, bool microphone)> callback);

/**
* @desc		离开房间
* @param	errorCode		错误码
* @param	callback		错误标志
* @return 	
*/
void ExitRTCRoom(function<void(int errorCode)> callback);

/**
* @desc		订阅
* @param	rid				房间id
* @param	uid				用户id
* @param	hwnd			视频窗口句柄
* @param	width			视频窗口宽
* @param	height			视频窗口高
* @param	errorCode		错误码
* @param	callback		错误标志
* @return 	
*/
void SubscribeVideo(int64_t rid, int64_t uid, HWND__* hwnd, uint32_t width, uint32_t height, function<void(int errorCode)> callback);

/**
* @desc		取消订阅
* @param	rid				房间id
* @param	uid				用户id
* @param	errorCode		错误码
* @param	callback		错误标志
* @return 	
*/
void UnsubscribeVideo(int64_t rid, int64_t uid, function<void(int errorCode)> callback);

/**
* @desc		邀请用户
* @param	rid				房间id
* @param	uids			被邀请的用户ids
* @param	errorCode		错误码
* @param	callback		错误标志
* @return 	
*/
void InviteUserIntoRTCRoom(int64_t rid, unordered_set<int64_t> uids, function<void(int errorCode)> callback);

/**
* @desc		获取房间用户
* @param	rid				房间id
* @param	roomMembers		房间用户
* @param	errorCode		错误码
* @param	callback		错误标志
* @return 	
*/
void GetRTCRoomMembers(int64_t rid, function<void(int errorCode, RTCRoomMembers roomMembers)> callback);

/**
* @desc		获取房间用户数量
* @param	rid				房间id
* @param	count			房间用户数量
* @param	errorCode		错误码
* @param	callback		错误标志
* @return 	
*/
void GetRTCRoomMemberCount(int64_t rid, function<void(int errorCode, int64_t count)> callback);

/**
* @desc		静音其他用户
* @param	rid				房间id
* @param	uids			静音用户列表
* @param	errorCode		错误码
* @param	callback		错误标志
* @return 	
*/
void BlockUserVoiceInRTCRoom(int64_t rid, unordered_set<int64_t> uids, function<void(int errorCode)> callback);

/**
* @desc		取消静音其他用户
* @param	rid				房间id
* @param	uids			取消静音用户列表
* @param	errorCode		错误码
* @param	callback		错误标志
* @return 	
*/
void UnblockUserVoiceInRTCRoom(int64_t rid, unordered_set<int64_t> uids, function<void(int errorCode)> callback);

/**
* @desc		设置用户音视频权限
* @param	rid				房间id
* @param	uids			设置权限的用户列表
* @param	command			音视频权限标志
* @param	errorCode		错误码
* @param	callback		错误标志
* @return 	
*/
void AdminCommand(int64_t rid, unordered_set<int64_t> uids, RTCEventHandler::AdminCommand command, function<void(int errorCode)> callback);


/**
* @desc		请求视频(1v1)
* @param	type			房间类型(默认2)
* @param	peerUid			被呼叫方id
* @param	hwnd			视频窗口句柄
* @param	width			视频窗口宽
* @param	height			视频窗口高
* @param	errorCode		错误码
* @param	callback		错误标志
* @return 	
*/
void RequestP2PRTC(int32_t type, int64_t peerUid, HWND__* hwnd, uint32_t width, uint32_t height, function<void(int errorCode)> callback);

/**
* @desc		挂断视频(1v1)
* @param	callId			呼叫方id
* @param	errorCode		错误码
* @param	callback		错误标志
* @return 	
*/
void CancelP2PRTC(int64_t callId, function<void(int errorCode)> callback);

/**
* @desc		关闭视频(1v1)
* @param	callId			呼叫方id
* @param	errorCode		错误码
* @param	callback		错误标志
* @return 	
*/
void CloseP2PRTC(int64_t callId, function<void(int errorCode)> callback);

/**
* @desc		接通视频(1v1)
* @param	callId			呼叫方id
* @param	hwnd			视频窗口句柄
* @param	width			视频窗口宽
* @param	height			视频窗口高
* @param	errorCode		错误码
* @param	callback		错误标志
* @return 	
*/
void AcceptP2PRTC(int64_t callId, HWND__* hwnd, uint32_t width, uint32_t height, function<void(int errorCode)> callback);

/**
* @desc		拒接视频(1v1)
* @param	callId			呼叫方id
* @param	errorCode		错误码
* @param	callback		错误标志
* @return 	
*/
void RefuseP2PRTC(int64_t callId, function<void(int errorCode)> callback);

/**
* @desc		静音
* @return 	
*/
void Mute();

/**
* @desc		取消静音
* @return 	
*/
void Unmute();

/**
* @desc		开始音频录制
* @return 	
*/
void StartAudio();

/**
* @desc		取消音频录制
* @return 	
*/
void StopAudio();