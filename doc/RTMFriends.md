/**************************************RTM好友同步接口**************************************/	
/**
* @desc		添加好友
* @param	friends			好友列表
* @return 	错误码
*/
int32_t AddFriends(unordered_set<int64_t> friends);

/**
* @desc		删除好友
* @param	friends			好友列表
* @return 	错误码
*/
int32_t DelFriends(unordered_set<int64_t> friends);

/**
* @desc		获取好友
* @param	uids			好友列表
* @return 	错误码
*/
int32_t GetFriends(unordered_set<int64_t>& uids);

/**
* @desc		添加黑名单
* @param	blacks			黑名单
* @return 	错误码
*/
int32_t AddBlacks(unordered_set<int64_t> blacks);

/**
* @desc		移除黑名单
* @param	blacks			黑名单
* @return 	错误码
*/
int32_t DelBlacks(unordered_set<int64_t> blacks);

/**
* @desc		获取黑名单
* @param	uids			黑名单
* @return 	错误码
*/
int32_t GetBlacks(unordered_set<int64_t>& uids);


/**************************************RTM好友异步接口**************************************/
/**
* @desc		添加好友
* @param	friends			好友列表
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void AddFriends(unordered_set<int64_t> friends, function<void(int errorCode)> callback);

/**
* @desc		删除好友
* @param	friends			好友列表
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void DelFriends(unordered_set<int64_t> friends, function<void(int errorCode)> callback);

/**
* @desc		获取好友
* @param	uids			好友列表
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void GetFriends(function<void(int errorCode, unordered_set<int64_t> uids)> callback);

/**
* @desc		添加黑名单
* @param	blacks			黑名单
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void AddBlacks(unordered_set<int64_t> blacks, function<void(int errorCode)> callback);

/**
* @desc		移除黑名单
* @param	blacks			黑名单
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void DelBlacks(unordered_set<int64_t> blacks, function<void(int errorCode)> callback);

/**
* @desc		获取黑名单
* @param	uids			黑名单
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void GetBlacks(function<void(int errorCode, unordered_set<int64_t> uids)> callback);