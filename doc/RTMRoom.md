/**************************************RTM房间同步接口**************************************/	
/**
* @desc		进入房间
* @param	rid				房间id
* @return 	错误码
*/
int32_t EnterRoom(int64_t rid);

/**
* @desc		离开房间
* @param	rid				房间id
* @return 	错误码
*/
int32_t LeaveRoom(int64_t rid);

/**
* @desc		获取当前用户进入的所有房间id
* @param	rooms			房间ids
* @return 	错误码
*/
int32_t GetUserRooms(unordered_set<int64_t>& rooms);

/**
* @desc		设置房间信息
* @param	rid				房间id
* @param	oinfo			房间信息
* @param	pinfo			房间信息
* @return 	错误码
*/
int32_t SetRoomInfo(int64_t rid, string oinfo, string pinfo);

/**
* @desc		获取房间信息
* @param	rid				房间id
* @param	oinfo			房间信息
* @param	pinfo			房间信息
* @return 	错误码
*/
int32_t GetRoomInfo(int64_t rid, string& oinfo, string& pinfo);

/**
* @desc		获取房间公开信息
* @param	rid				房间id
* @param	oinfo			房间信息
* @return 	错误码
*/
int32_t GetRoomOpenInfo(int64_t rid, string& oinfo);

/**
* @desc		获取指定房间的公开信息
* @param	rids			房间ids
* @param	info			房间信息
* @return 	错误码
*/
int32_t GetRoomsOpenInfo(unordered_set<int64_t> rids, unordered_map<string, string>& info);

/**
* @desc		获取房间用户列表
* @param	rid				房间id
* @param	uids			用户列表
* @return 	错误码
*/
int32_t GetRoomMemers(int64_t rid, unordered_set<int64_t>& uids);

/**
* @desc		获取指定房间用户总数
* @param	rids			房间ids
* @param	info			用户总数
* @return 	错误码
*/
int32_t GetRoomCount(unordered_set<int64_t> rids, unordered_map<string, int32_t>& info);


/**************************************RTM房间异步接口**************************************/
/**
* @desc		进入房间
* @param	rid				房间id
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void EnterRoom(int64_t rid, function<void(int errorCode)> callback);

/**
* @desc		离开房间
* @param	rid				房间id
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void LeaveRoom(int64_t rid, function<void(int errorCode)> callback);

/**
* @desc		获取当前用户进入的所有房间id
* @param	rooms			房间ids
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void GetUserRooms(function<void(int errorCode, unordered_set<int64_t> rooms)> callback);

/**
* @desc		设置房间信息
* @param	rid				房间id
* @param	oinfo			房间信息
* @param	pinfo			房间信息
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void SetRoomInfo(int64_t rid, string oinfo, string pinfo, function<void(int errorCode)> callback);

/**
* @desc		获取房间信息
* @param	rid				房间id
* @param	oinfo			房间信息
* @param	pinfo			房间信息
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void GetRoomInfo(int64_t rid, function<void(int errorCode, string oinfo, string pinfo)> callback);

/**
* @desc		获取房间公开信息
* @param	rid				房间id
* @param	oinfo			房间信息
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void GetRoomOpenInfo(int64_t rid, function<void(int errorCode, string oinfo)> callback);

/**
* @desc		获取指定房间的公开信息
* @param	rids			房间ids
* @param	info			房间信息
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void GetRoomsOpenInfo(unordered_set<int64_t> rids, function<void(int errorCode, unordered_map<string, string> info)> callback);

/**
* @desc		获取房间用户列表
* @param	rid				房间id
* @param	uids			用户列表
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void GetRoomMemers(int64_t rid, function<void(int errorCode, unordered_set<int64_t> uids)> callback);

/**
* @desc		获取指定房间用户总数
* @param	rids			房间ids
* @param	info			用户总数
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void GetRoomCount(unordered_set<int64_t> rids, function<void(int errorCode, unordered_map<string, int32_t> info)> callback);