/**************************************RTC事件接口**************************************/	
/**
* @desc		进入房间事件
* @param	uid				用户id
* @param	rid				房间id
* @param	mtime			时间
* @return 	
*/
void OnUserEnterRTCRoom(int64_t uid, int64_t rid, int64_t mtime);

/**
* @desc		退出房间事件
* @param	uid				用户id
* @param	rid				房间id
* @param	mtime			时间
* @return 	
*/
void OnUserExitRTCRoom(int64_t uid, int64_t rid, int64_t mtime);

/**
* @desc		关闭房间事件
* @param	uid				用户id
* @param	rid				房间id
* @param	mtime			时间
* @return 	
*/
void OnRTCRoomClosed(int64_t rid);

/**
* @desc		邀请事件
* @param	fromUid			邀请用户id
* @param	rid				房间id
* @return 	
*/
void OnInviteIntoRTCRoom(int64_t fromUid, int64_t rid);

/**
* @desc		踢出事件
* @param	fromUid			踢出用户id
* @param	rid				房间id
* @return 	
*/
void OnKickOutFromRTCRoom(int64_t fromUid, int64_t rid);

/**
* @desc		音视频权限事件
* @param	command			权限标志
* @param	uids			被设置权限的用户列表
* @return 	
*/
void OnAdminCommand(AdminCommand command, vector<int64_t> uids);

/**
* @desc		房间事件
* @param	rid				房间id
* @param	roomEvent		用户事件标志
* @param	eventData		事件列表
* @return 	
*/
void OnRoomEvent(int64_t rid, RoomEvent roomEvent, vector<unsigned char>eventData);

/**
* @desc		房间事件
* @param	rid				房间id
* @param	roomEvent		用户事件标志
* @param	eventData		事件列表
* @return 	
*/
void OnPushP2PRTCRequest(int64_t callId, int64_t peerUid, int32_t type);

/**
* @desc		p2p事件
* @param	callId			呼叫方id
* @param	peerUid			被呼叫方id
* @param	type			事件类型
* @param	p2pEvent		p2p事件
* @return 	
*/
void OnPushP2PRTCEvent(int64_t callId, int64_t peerUid, int32_t type, int32_t p2pEvent);