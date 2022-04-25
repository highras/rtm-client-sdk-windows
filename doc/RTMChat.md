/**************************************RTM聊天同步接口**************************************/	
/**
* @desc		发送聊天消息
* @param	to				消息接收者
* @param	mid				消息id
* @param	msg				消息内容
* @param	attrs			消息属性
* @param	mtime			消息发送时间
* @return 	错误码
*/
int32_t SendChat(int64_t to, int64_t mid, string msg, string attrs, int64_t& mtime);

/**
* @desc		发送命令消息
* @param	to				消息接收者
* @param	mid				消息id
* @param	msg				消息内容
* @param	attrs			消息属性
* @param	mtime			消息发送时间
* @return 	错误码
*/
int32_t SendCmd(int64_t to, int64_t mid, string msg, string attrs, int64_t& mtime);

/**
* @desc		发送群组聊天消息
* @param	gid				消息发送的群组id
* @param	mid				消息id
* @param	msg				消息内容
* @param	attrs			消息属性
* @param	mtime			消息发送时间
* @return 	错误码
*/
int32_t SendGroupChat(int64_t gid, int64_t mid, string msg, string attrs, int64_t& mtime);

/**
* @desc		发送群组命令消息
* @param	gid				消息发送的群组id
* @param	mid				消息id
* @param	msg				消息内容
* @param	attrs			消息属性
* @param	mtime			消息发送时间
* @return 	错误码
*/
int32_t SendGroupCmd(int64_t gid, int64_t mid, string msg, string attrs, int64_t& mtime);

/**
* @desc		发送房间聊天消息
* @param	rid				消息发送的房间id
* @param	mid				消息id
* @param	msg				消息内容
* @param	attrs			消息属性
* @param	mtime			消息发送时间
* @return 	错误码
*/
int32_t SendRoomChat(int64_t rid, int64_t mid, string msg, string attrs, int64_t& mtime);

/**
* @desc		发送房间命令消息
* @param	rid				消息发送的房间id
* @param	mid				消息id
* @param	msg				消息内容
* @param	attrs			消息属性
* @param	mtime			消息发送时间
* @return 	错误码
*/
int32_t SendRoomCmd(int64_t rid, int64_t mid, string msg, string attrs, int64_t& mtime);

/**
* @desc		获取群组聊天消息
* @param	gid				群组id
* @param	desc			true-从end时间倒序获取，false-从begin时间顺序获取
* @param	num				获取消息数量
* @param	begin			获取消息开始时间(下一轮查询使用返回的obegin)
* @param	end				获取消息结束时间(下一轮查询使用返回的oend)
* @param	lastid			获取消息列表的最后一条消息id(第一次填默认0下,一轮查询使用返回的olastid)
* @param	onum			获取到的消息实际数量
* @param	olastid			获取到的消息列表的最后一条消息id
* @param	obegin			获取到的消息开始时间
* @param	oend			获取到的消息结束时间
* @param	msgs			获取到的消息
* @return 	错误码
*/
int32_t GetGroupChat(int64_t gid, bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, int16_t& onum, int64_t& olastid, int64_t& obegin, int64_t& oend, vector<CommonMsg>& msgs);

/**
* @desc		获取房间聊天消息
* @param	rid				房间id
* @param	desc			true-从end时间倒序获取，false-从begin时间顺序获取
* @param	num				获取消息数量
* @param	begin			获取消息开始时间(下一轮查询使用返回的obegin)
* @param	end				获取消息结束时间(下一轮查询使用返回的oend)
* @param	lastid			获取消息列表的最后一条消息id(第一次填默认0下,一轮查询使用返回的olastid)
* @param	onum			获取到的消息实际数量
* @param	olastid			获取到的消息列表的最后一条消息id
* @param	obegin			获取到的消息开始时间
* @param	oend			获取到的消息结束时间
* @param	msgs			获取到的消息
* @return 	错误码
*/
int32_t GetRoomChat(int64_t rid, bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, int16_t& onum, int64_t& olastid, int64_t& obegin, int64_t& oend, vector<CommonMsg>& msgs);

/**
* @desc		获取广播聊天消息
* @param	desc			true-从end时间倒序获取，false-从begin时间顺序获取
* @param	num				获取消息数量
* @param	begin			获取消息开始时间(下一轮查询使用返回的obegin)
* @param	end				获取消息结束时间(下一轮查询使用返回的oend)
* @param	lastid			获取消息列表的最后一条消息id(第一次填默认0下,一轮查询使用返回的olastid)
* @param	onum			获取到的消息实际数量
* @param	olastid			获取到的消息列表的最后一条消息id
* @param	obegin			获取到的消息开始时间
* @param	oend			获取到的消息结束时间
* @param	msgs			获取到的消息
* @return 	错误码
*/
int32_t GetBroadcastChat(bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid,int16_t& onum, int64_t& olastid, int64_t& obegin, int64_t& oend, vector<CommonMsg>& msgs);

/**
* @desc		获取p2p聊天消息
* @param	ouid			用户id
* @param	desc			true-从end时间倒序获取，false-从begin时间顺序获取
* @param	num				获取消息数量
* @param	begin			获取消息开始时间(下一轮查询使用返回的obegin)
* @param	end				获取消息结束时间(下一轮查询使用返回的oend)
* @param	lastid			获取消息列表的最后一条消息id(第一次填默认0下,一轮查询使用返回的olastid)
* @param	onum			获取到的消息实际数量
* @param	olastid			获取到的消息列表的最后一条消息id
* @param	obegin			获取到的消息开始时间
* @param	oend			获取到的消息结束时间
* @param	msgs			获取到的消息
* @return 	错误码
*/
int32_t GetP2PChat(int64_t ouid, bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, int16_t& onum, int64_t& olastid, int64_t& obegin, int64_t& oend, vector<CommonMsg>& msgs);

/**
* @desc		删除聊天消息
* @param	from			消息发送者
* @param	mid				消息id
* @param	xid				用户id/房间id/群组id
* @param	type			消息类型(1-p2p,2-group,3-room)
* @return 	错误码
*/
int32_t DelChat(int64_t from, int64_t mid, int64_t xid, int8_t type);

/**
* @desc		获取聊天消息
* @param	from			消息发送者
* @param	mid				消息id
* @param	xid				用户id/房间id/群组id
* @param	type			消息类型(1-p2p,2-group,3-room，4-broadcast)
* @param	id				获取到的消息id
* @param	mtype			获取到的消息类型
* @param	msg				获取到的消息内容
* @param	attrs			获取到的消息属性
* @param	mtime			获取到的消息时间
* @return 	错误码
*/
int32_t GetChat(int64_t from, int64_t mid, int64_t xid, int8_t type, int64_t& id, int8_t& mtype, string& msg, string& attrs, int64_t& mtime);


/**************************************RTM聊天异步接口**************************************/
/**
* @desc		发送聊天消息
* @param	to				消息接收者
* @param	mid				消息id
* @param	msg				消息内容
* @param	attrs			消息属性
* @param	mtime			消息发送时间
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void SendChat(int64_t to, int64_t mid, string msg, string attrs, function<void(int errorCode, int64_t mtime)> callback);

/**
* @desc		发送命令消息
* @param	to				消息接收者
* @param	mid				消息id
* @param	msg				消息内容
* @param	attrs			消息属性
* @param	mtime			消息发送时间
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void SendCmd(int64_t to, int64_t mid, string msg, string attrs, function<void(int errorCode, int64_t mtime)> callback);

/**
* @desc		发送群组聊天消息
* @param	gid				消息发送的群组id
* @param	mid				消息id
* @param	msg				消息内容
* @param	attrs			消息属性
* @param	mtime			消息发送时间
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void SendGroupChat(int64_t gid, int64_t mid, string msg, string attrs, function<void(int errorCode, int64_t mtime)> callback);

/**
* @desc		发送群组命令消息
* @param	gid				消息发送的群组id
* @param	mid				消息id
* @param	msg				消息内容
* @param	attrs			消息属性
* @param	mtime			消息发送时间
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void SendGroupCmd(int64_t gid, int64_t mid, string msg, string attrs, function<void(int errorCode, int64_t mtime)> callback);

/**
* @desc		发送房间聊天消息
* @param	rid				消息发送的房间id
* @param	mid				消息id
* @param	msg				消息内容
* @param	attrs			消息属性
* @param	mtime			消息发送时间
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void SendRoomChat(int64_t rid, int64_t mid, string msg, string attrs, function<void(int errorCode, int64_t mtime)> callback);

/**
* @desc		发送房间命令消息
* @param	rid				消息发送的房间id
* @param	mid				消息id
* @param	msg				消息内容
* @param	attrs			消息属性
* @param	mtime			消息发送时间
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void SendRoomCmd(int64_t rid, int64_t mid, string msg, string attrs, function<void(int errorCode, int64_t mtime)> callback);

/**
* @desc		获取群组聊天消息
* @param	gid				群组id
* @param	desc			true-从end时间倒序获取，false-从begin时间顺序获取
* @param	num				获取消息数量
* @param	begin			获取消息开始时间(下一轮查询使用返回的obegin)
* @param	end				获取消息结束时间(下一轮查询使用返回的oend)
* @param	lastid			获取消息列表的最后一条消息id(第一次填默认0下,一轮查询使用返回的olastid)
* @param	onum			获取到的消息实际数量
* @param	olastid			获取到的消息列表的最后一条消息id
* @param	obegin			获取到的消息开始时间
* @param	oend			获取到的消息结束时间
* @param	msgs			获取到的消息
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void GetGroupChat(int64_t gid, bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, function<void(int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<CommonMsg> msgs)> callback);

/**
* @desc		获取房间聊天消息
* @param	rid				房间id
* @param	desc			true-从end时间倒序获取，false-从begin时间顺序获取
* @param	num				获取消息数量
* @param	begin			获取消息开始时间(下一轮查询使用返回的obegin)
* @param	end				获取消息结束时间(下一轮查询使用返回的oend)
* @param	lastid			获取消息列表的最后一条消息id(第一次填默认0下,一轮查询使用返回的olastid)
* @param	onum			获取到的消息实际数量
* @param	olastid			获取到的消息列表的最后一条消息id
* @param	obegin			获取到的消息开始时间
* @param	oend			获取到的消息结束时间
* @param	msgs			获取到的消息
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void GetRoomChat(int64_t rid, bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, function<void(int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<CommonMsg> msgs)> callback);

/**
* @desc		获取广播聊天消息
* @param	desc			true-从end时间倒序获取，false-从begin时间顺序获取
* @param	num				获取消息数量
* @param	begin			获取消息开始时间(下一轮查询使用返回的obegin)
* @param	end				获取消息结束时间(下一轮查询使用返回的oend)
* @param	lastid			获取消息列表的最后一条消息id(第一次填默认0下,一轮查询使用返回的olastid)
* @param	onum			获取到的消息实际数量
* @param	olastid			获取到的消息列表的最后一条消息id
* @param	obegin			获取到的消息开始时间
* @param	oend			获取到的消息结束时间
* @param	msgs			获取到的消息
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void GetBroadcastChat(bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, function<void(int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<CommonMsg> msgs)> callback);

/**
* @desc		获取p2p聊天消息
* @param	ouid			用户id
* @param	desc			true-从end时间倒序获取，false-从begin时间顺序获取
* @param	num				获取消息数量
* @param	begin			获取消息开始时间(下一轮查询使用返回的obegin)
* @param	end				获取消息结束时间(下一轮查询使用返回的oend)
* @param	lastid			获取消息列表的最后一条消息id(第一次填默认0下,一轮查询使用返回的olastid)
* @param	onum			获取到的消息实际数量
* @param	olastid			获取到的消息列表的最后一条消息id
* @param	obegin			获取到的消息开始时间
* @param	oend			获取到的消息结束时间
* @param	msgs			获取到的消息
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void GetP2PChat(int64_t ouid, bool desc, int16_t num, int64_t begin, int64_t end, int64_t lastid, function<void(int errorCode, int16_t num, int64_t lastid, int64_t begin, int64_t end, vector<CommonMsg> msgs)> callback);

/**
* @desc		删除聊天消息
* @param	from			消息发送者
* @param	mid				消息id
* @param	xid				用户id/房间id/群组id
* @param	type			消息类型(1-p2p,2-group,3-room)
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void DelChat(int64_t from, int64_t mid, int64_t xid, int8_t type, function<void(int errorCode)> callback);

/**
* @desc		获取聊天消息
* @param	from			消息发送者
* @param	mid				消息id
* @param	xid				用户id/房间id/群组id
* @param	type			消息类型(1-p2p,2-group,3-room，4-broadcast)
* @param	id				获取到的消息id
* @param	mtype			获取到的消息类型
* @param	msg				获取到的消息内容
* @param	attrs			获取到的消息属性
* @param	mtime			获取到的消息时间
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void GetChat(int64_t from, int64_t mid, int64_t xid, int8_t type, function<void(int errorCode, int64_t id, int8_t mtype, string msg, string attrs, int64_t mtime)> callback);
