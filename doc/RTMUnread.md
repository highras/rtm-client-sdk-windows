/**************************************RTM离线消息同步接口**************************************/	
/**
* @desc		获取离线消息
* @param	p2p				与自己有离线消息的p2p用户列表
* @param	group			自己所在的离线消息的群组列表
* @return 	错误码
*/
int32_t GetUnread(bool clean, unordered_set<int64_t>& p2p, unordered_set<int64_t>& group);

/**
* @desc		获取聊天会话
* @param	p2p				与自己有会话的p2p用户列表
* @param	group			自己所在的会话的群组列表
* @return 	错误码
*/
int32_t GetSession(unordered_set<int64_t>& p2p, unordered_set<int64_t>& group);

/**
* @desc		获取p2p离线消息
* @param	uids			要获取离线消息的用户列表
* @param	mtime			获取此时间之后的离线消息
* @param	mtypes			要获取离线消息的消息类型列表
* @param	p2p				获取到的离线消息的用户列表
* @param	ltime			每个会话最新一条消息的时间
* @return 	错误码
*/
int32_t GetP2PUnread(unordered_set<int64_t> uids, int64_t mtime, unordered_set<int8_t> mtypes, unordered_map<string, int32_t>& p2p, unordered_map<string, int64_t>& ltime);

/**
* @desc		获取群组离线消息
* @param	gids			要获取离线消息的群组列表
* @param	mtime			获取此时间之后的离线消息
* @param	mtypes			要获取离线消息的消息类型列表
* @param	group			获取到的离线消息的群组列表
* @param	ltime			每个会话最新一条消息的时间
* @return 	错误码
*/
int32_t GetGroupUnread(unordered_set<int64_t> gids, int64_t mtime, unordered_set<int8_t> mtypes, unordered_map<string, int32_t>& group, unordered_map<string, int64_t>& ltime);

/**
* @desc		清理指定的离线消息
* @param	uids			指定用户列表
* @param	gids			指定群组列表
* @return 	错误码
*/
int32_t CleanTargetUnread(unordered_set<int64_t> uids, unordered_set<int64_t> gids);

/**
* @desc		获取p2p离线消息(通过指定时间)
* @param	uidTime			指定用户与时间的列表
* @param	mtypes			要获取离线消息的消息类型列表
* @param	p2p				获取到的离线消息的用户列表
* @param	ltime			每个会话最新一条消息的时间
* @return 	错误码
*/
int32_t GetP2PUnreadByTime(unordered_map<string, int64_t> uidTime, unordered_set<int8_t> mtypes, unordered_map<string, int32_t>& p2p, unordered_map<string, int64_t>& ltime);

/**
* @desc		获取群组离线消息(通过指定时间)
* @param	gidTime			指定群组与时间的列表
* @param	mtypes			要获取离线消息的消息类型列表
* @param	group			获取到的离线消息的群组列表
* @param	ltime			每个会话最新一条消息的时间
* @return 	错误码
*/
int32_t GetGroupUnreadByTime(unordered_map<string, int64_t> gidTime, unordered_set<int8_t> mtypes, unordered_map<string, int32_t>& group, unordered_map<string, int64_t>& ltime);


/**************************************RTM离线消息异步接口**************************************/
/**
* @desc		获取离线消息
* @param	p2p				与自己有离线消息的p2p用户列表
* @param	group			自己所在的离线消息的群组列表
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void GetUnread(bool clean, function<void(int errorCode, unordered_set<int64_t> p2p, unordered_set<int64_t> group)> callback);

/**
* @desc		清理离线消息
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void CleanUnread(function<void(int errorCode)> callback);

/**
* @desc		获取聊天会话
* @param	p2p				与自己有会话的p2p用户列表
* @param	group			自己所在的会话的群组列表
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void GetSession(function<void(int errorCode, unordered_set<int64_t> p2p, unordered_set<int64_t> group)> callback);

/**
* @desc		获取p2p离线消息
* @param	uids			要获取离线消息的用户列表
* @param	mtime			获取此时间之后的离线消息
* @param	mtypes			要获取离线消息的消息类型列表
* @param	p2p				获取到的离线消息的用户列表
* @param	ltime			每个会话最新一条消息的时间
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void GetP2PUnread(unordered_set<int64_t> uids, int64_t mtime, unordered_set<int8_t> mtypes, function<void(int errorCode, unordered_map<string, int32_t> p2p, unordered_map<string, int64_t> ltime)> callback);

/**
* @desc		获取群组离线消息
* @param	gids			要获取离线消息的群组列表
* @param	mtime			获取此时间之后的离线消息
* @param	mtypes			要获取离线消息的消息类型列表
* @param	group			获取到的离线消息的群组列表
* @param	ltime			每个会话最新一条消息的时间
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void GetGroupUnread(unordered_set<int64_t> gids, int64_t mtime, unordered_set<int8_t> mtypes, function<void(int errorCode, unordered_map<string, int32_t> group, unordered_map<string, int64_t> ltime)> callback);

/**
* @desc		清理指定的离线消息
* @param	uids			指定用户列表
* @param	gids			指定群组列表
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void CleanTargetUnread(unordered_set<int64_t> uids, unordered_set<int64_t> gids, function<void(int errorCode)> callback);

/**
* @desc		获取p2p离线消息(通过指定时间)
* @param	uidTime			指定用户与时间的列表
* @param	mtypes			要获取离线消息的消息类型列表
* @param	p2p				获取到的离线消息的用户列表
* @param	ltime			每个会话最新一条消息的时间
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void GetP2PUnreadByTime(unordered_map<string, int64_t> uidTime, unordered_set<int8_t> mtypes, function<void(int errorCode, unordered_map<string, int32_t> p2p, unordered_map<string, int64_t> ltime)> callback);

/**
* @desc		获取群组离线消息(通过指定时间)
* @param	gidTime			指定群组与时间的列表
* @param	mtypes			要获取离线消息的消息类型列表
* @param	group			获取到的离线消息的群组列表
* @param	ltime			每个会话最新一条消息的时间
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void GetGroupUnreadByTime(unordered_map<string, int64_t> gidTime, unordered_set<int8_t> mtypes, function<void(int errorCode, unordered_map<string, int32_t> group, unordered_map<string, int64_t> ltime)> callback);
