/**************************************RTM用户同步接口**************************************/	
/**
* @desc		获取在线用户列表
* @param	uids			指定用户列表
* @param	ouids			在线用户列表
* @return 	错误码
*/
int32_t GetOnlineUsers(unordered_set<int64_t> uids, unordered_set<int64_t>& ouids);

/**
* @desc		设置当前用户信息
* @param	oinfo			指定信息
* @param	pinfo			指定信息
* @return 	错误码
*/
int32_t SetUserInfo(string oinfo, string pinfo);

/**
* @desc		获取当前用户信息
* @param	oinfo			获取信息
* @param	pinfo			获取信息
* @return 	错误码
*/
int32_t GetUserInfo(string& oinfo, string& pinfo);

/**
* @desc		获取用户列表公开信息
* @param	uids			指定用户列表
* @param	info			公开的用户信息列表
* @return 	错误码
*/
int32_t GetUserOpenInfo(unordered_set<int64_t> uids, unordered_map<string, string>& info);


/**************************************RTM用户异步接口**************************************/	
/**
* @desc		获取在线用户列表
* @param	uids			指定用户列表
* @param	ouids			在线用户列表
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void GetOnlineUsers(unordered_set<int64_t> uids, function<void(int errorCode, unordered_set<int64_t> ouids)> callback);

/**
* @desc		设置当前用户信息
* @param	oinfo			指定信息
* @param	pinfo			指定信息
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void SetUserInfo(string oinfo, string pinfo, function<void(int errorCode)> callback);

/**
* @desc		获取当前用户信息
* @param	oinfo			获取信息
* @param	pinfo			获取信息
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void GetUserInfo(function<void(int errorCode, string oinfo, string pinfo)> callback);

/**
* @desc		获取用户列表公开信息
* @param	uids			指定用户列表
* @param	info			公开的用户信息列表
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void GetUserOpenInfo(unordered_set<int64_t> uids, function<void(int errorCode, unordered_map<string, string> info)> callback);