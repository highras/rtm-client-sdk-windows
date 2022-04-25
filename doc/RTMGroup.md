/**************************************RTM群组同步接口**************************************/	
/**
* @desc		添加群组成员
* @param	gid				群组id
* @param	uids			成员列表
* @return 	错误码
*/
int32_t AddGroupMembers(int64_t gid, unordered_set<int64_t> uids);

/**
* @desc		删除群组成员
* @param	gid				群组id
* @param	uids			成员列表
* @return 	错误码
*/
int32_t DelGroupMembers(int64_t gid, unordered_set<int64_t> uids);

/**
* @desc		获取群组成员列表
* @param	gid				群组id
* @param	online			成员是否在线
* @param	uids			成员列表
* @param	onlines			在线的成员列表
* @return 	错误码
*/
int32_t GetGroupMembers(int64_t gid, bool online, unordered_set<int64_t>& uids, unordered_set<int64_t>& onlines);

/**
* @desc		获取群组成员总数
* @param	gid				群组id
* @param	online			成员是否在线
* @param	cn				成员总数
* @param	onlines			在线成员总数
* @return 	错误码
*/
int32_t GetGroupCount(int64_t gid, bool online, int32_t& cn, int32_t& onlines);

/**
* @desc		获取群组总数
* @param	gids			群组总数
* @return 	错误码
*/
int32_t GetUserGroups(unordered_set<int64_t>& gids);

/**
* @desc		设置群组信息
* @param	gid				群组id
* @param	oinfo			群组信息
* @param	pinfo			群组信息
* @return 	错误码
*/
int32_t SetGroupInfo(int64_t gid, string oinfo, string pinfo);

/**
* @desc		获取群组公开信息
* @param	gid				群组id
* @param	oinfo			群组信息
* @return 	错误码
*/
int32_t GetGroupOpenInfo(int64_t gid, string& oinfo);

/**
* @desc		获取多个群组信息
* @param	gids			群组ids
* @param	info			群组信息表
* @return 	错误码
*/
int32_t GetGroupsOpenInfo(unordered_set<int64_t> gids, unordered_map<string, string>& info);


/**************************************RTM群组异步接口**************************************/	
/**
* @desc		添加群组成员
* @param	gid				群组id
* @param	uids			成员列表
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void AddGroupMembers(int64_t gid, unordered_set<int64_t> uids, function<void(int errorCode)> callback);

/**
* @desc		删除群组成员
* @param	gid				群组id
* @param	uids			成员列表
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
void DelGroupMembers(int64_t gid, unordered_set<int64_t> uids, function<void(int errorCode)> callback);

/**
* @desc		获取群组成员列表
* @param	gid				群组id
* @param	online			成员是否在线
* @param	uids			成员列表
* @param	onlines			在线的成员列表
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
void GetGroupMembers(int64_t gid, bool online, function<void(int errorCode, unordered_set<int64_t> uids, unordered_set<int64_t> onlines)> callback);

/**
* @desc		获取群组成员总数
* @param	gid				群组id
* @param	online			成员是否在线
* @param	cn				成员总数
* @param	onlines			在线成员总数
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
void GetGroupCount(int64_t gid, bool online, function<void(int errorCode, int32_t cn, int32_t onlines)> callback);

/**
* @desc		获取群组总数
* @param	gids			群组总数
* @param	errorCode		错误码
* @param	callback		异步回调
* @return
void GetUserGroups(function<void(int errorCode, unordered_set<int64_t> gids)> callback);

/**
* @desc		设置群组信息
* @param	gid				群组id
* @param	oinfo			群组信息
* @param	pinfo			群组信息
* @param	errorCode		错误码
* @param	callback		异步回调
* @return
void SetGroupInfo(int64_t gid, string oinfo, string pinfo, function<void(int errorCode)> callback);

/**
* @desc		获取群组公开信息
* @param	gid				群组id
* @param	oinfo			群组信息
* @param	errorCode		错误码
* @param	callback		异步回调
* @return
void GetGroupOpenInfo(int64_t gid, function<void(int errorCode, string oinfo)> callback);

/**
* @desc		获取多个群组信息
* @param	gids			群组ids
* @param	info			群组信息表
* @param	errorCode		错误码
* @param	callback		异步回调
* @return
void GetGroupsOpenInfo(unordered_set<int64_t> gids, function<void(int errorCode, unordered_map<string, string> info)> callback);