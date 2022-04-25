/**************************************RTM事件通知接口**************************************/	
/**
* @desc		被踢出事件
* @param	
* @return 	
*/
void OnKickout();

/**
* @desc		被踢出房间事件
* @param	rid				房间id
* @return 	
*/
void OnKickoutRoom(int64_t rid);

/**
* @desc		接收到chat消息事件
* @param	from			消息发送者
* @param	to				消息接收者
* @param	mid				消息id
* @param	msg				消息内容
* @param	attrs			消息属性
* @param	mtime			消息发送时间
* @return 	
*/
void OnPushChat(int64_t from, int64_t to, int64_t mid, string msg, string attrs, int64_t mtime);

/**
* @desc		接收到cmd消息事件
* @param	from			消息发送者
* @param	to				消息接收者
* @param	mid				消息id
* @param	msg				消息内容
* @param	attrs			消息属性
* @param	mtime			消息发送时间
* @return 	
*/
void OnPushCmd(int64_t from, int64_t to, int64_t mid, string msg, string attrs, int64_t mtime);

/**
* @desc		接收到msg消息事件
* @param	from			消息发送者
* @param	to				消息接收者
* @param	mtype			消息类型
* @param	mid				消息id
* @param	msg				消息内容
* @param	attrs			消息属性
* @param	mtime			消息发送时间
* @return 	
*/
void OnPushMsg(int64_t from, int64_t to, int8_t mtype, int64_t mid, string msg, string attrs, int64_t mtime);

/**
* @desc		接收到文件消息事件
* @param	from			消息发送者
* @param	to				消息接收者
* @param	mtype			消息类型
* @param	mid				消息id
* @param	msg				消息内容
* @param	attrs			消息属性
* @param	mtime			消息发送时间
* @return 	
*/
void OnPushFile(int64_t from, int64_t to, int8_t mtype, int64_t mid, string msg, string attrs, int64_t mtime);

/**
* @desc		接收到群组chat消息事件
* @param	from			消息发送者
* @param	gid				群组id
* @param	mid				消息id
* @param	msg				消息内容
* @param	attrs			消息属性
* @param	mtime			消息发送时间
* @return 	
*/
void OnPushGroupChat(int64_t from, int64_t gid, int64_t mid, string msg, string attrs, int64_t mtime);

/**
* @desc		接收到群组消息cmd事件
* @param	from			消息发送者
* @param	gid				群组id
* @param	mid				消息id
* @param	msg				消息内容
* @param	attrs			消息属性
* @param	mtime			消息发送时间
* @return 	
*/
void OnPushGroupCmd(int64_t from, int64_t gid, int64_t mid, string msg, string attrs, int64_t mtime);

/**
* @desc		接收到群组file消息事件
* @param	from			消息发送者
* @param	gid				群组id
* @param	mtype			消息类型
* @param	mid				消息id
* @param	msg				消息内容
* @param	attrs			消息属性
* @param	mtime			消息发送时间
* @return 	
*/
void OnPushGroupFile(int64_t from, int64_t gid, int8_t mtype, int64_t mid, string msg, string attrs, int64_t mtime);

/**
* @desc		接收到群组msg消息事件
* @param	from			消息发送者
* @param	gid				群组id
* @param	mtype			消息类型
* @param	mid				消息id
* @param	msg				消息内容
* @param	attrs			消息属性
* @param	mtime			消息发送时间
* @return 	
*/
void OnPushGroupMsg(int64_t from, int64_t gid, int8_t mtype, int64_t mid, string msg, string attrs, int64_t mtime);

/**
* @desc		接收到房间chat消息事件
* @param	from			消息发送者
* @param	gid				群组id
* @param	mid				消息id
* @param	msg				消息内容
* @param	attrs			消息属性
* @param	mtime			消息发送时间
* @return 	
*/
void OnPushRoomChat(int64_t from, int64_t rid, int64_t mid, string msg, string attrs, int64_t mtime);

/**
* @desc		接收到房间cmd消息事件
* @param	from			消息发送者
* @param	gid				群组id
* @param	mid				消息id
* @param	msg				消息内容
* @param	attrs			消息属性
* @param	mtime			消息发送时间
* @return 	
*/
void OnPushRoomCmd(int64_t from, int64_t rid, int64_t mid, string msg, string attrs, int64_t mtime);

/**
* @desc		接收到房间file消息事件
* @param	from			消息发送者
* @param	gid				群组id
* @param	mtype			消息类型
* @param	mid				消息id
* @param	msg				消息内容
* @param	attrs			消息属性
* @param	mtime			消息发送时间
* @return 	
*/
void OnPushRoomFile(int64_t from, int64_t rid, int8_t mtype, int64_t mid, string msg, string attrs, int64_t mtime);

/**
* @desc		接收到房间msg消息事件
* @param	from			消息发送者
* @param	gid				群组id
* @param	mtype			消息类型
* @param	mid				消息id
* @param	msg				消息内容
* @param	attrs			消息属性
* @param	mtime			消息发送时间
* @return 	
*/
void OnPushRoomMsg(int64_t from, int64_t rid, int8_t mtype, int64_t mid, string msg, string attrs, int64_t mtime);

/**
* @desc		接收到广播chat消息事件
* @param	from			消息发送者
* @param	gid				群组id
* @param	mid				消息id
* @param	msg				消息内容
* @param	attrs			消息属性
* @param	mtime			消息发送时间
* @return 	
*/

void OnPushBroadcastChat(int64_t from, int64_t mid, string msg, string attrs, int64_t mtime);
/**
* @desc		接收到广播cmd消息事件
* @param	from			消息发送者
* @param	gid				群组id、
* @param	mid				消息id
* @param	msg				消息内容
* @param	attrs			消息属性
* @param	mtime			消息发送时间
* @return 	
*/
void OnPushBroadcastCmd(int64_t from, int64_t mid, string msg, string attrs, int64_t mtime);

/**
* @desc		接收到广播file消息事件
* @param	from			消息发送者
* @param	gid				群组id
* @param	mtype			消息类型
* @param	mid				消息id
* @param	msg				消息内容
* @param	attrs			消息属性
* @param	mtime			消息发送时间
* @return 	
*/
void OnPushBroadcastFile(int64_t from, int8_t mtype, int64_t mid, string msg, string attrs, int64_t mtime);

/**
* @desc		接收到广播msg消息事件
* @param	from			消息发送者
* @param	gid				群组id
* @param	mtype			消息类型
* @param	mid				消息id
* @param	msg				消息内容
* @param	attrs			消息属性
* @param	mtime			消息发送时间
* @return 	
*/
void OnPushBroadcastMsg(int64_t from, int8_t mtype, int64_t mid, string msg, string attrs, int64_t mtime);

/**
* @desc		连接关闭事件
* @param	closeError		关闭错误码		
* @return 	
*/
void connnectionClosed(int32_t closeError);

/**
* @desc		重连结束事件
* @param	successful		重连结果
* @param	retryAgain		是否属于二次重连
* @param	errorCode		重连错误码
* @param	retriedCount	重连次数
* @return 	
*/
void reloginCompleted(bool successful, bool retryAgain, int errorCode, int retriedCount);

/**
* @desc		重连开始事件
* @param	lastErrorCode	上次重连错误码
* @param	retriedCount	已经重连次数
* @return 	
*/
bool reloginWillStart(int lastErrorCode, int retriedCount);