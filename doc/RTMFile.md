/**************************************RTM文件同步接口**************************************/	
/**
* @desc		发送文件
* @param	pid				项目id
* @param	mtype			消息类型
* @param	fileText		文件二进制
* @param	fileName		文件名字
* @param	fileExt			文件扩展名
* @param	userAttrs		用户自定义属性(json串)
* @param	mtime			发送时间
* @param	timeout			超时
* @return 	错误码
*/
int32_t SendFile(int32_t pid, int8_t mtype, std::string fileText, std::string fileName, std::string fileExt, std::string userAttrs, int64_t& mtime, int timeout = 120);

/**
* @desc		发送群组文件
* @param	gid				群组id
* @param	mtype			消息类型
* @param	fileText		文件二进制
* @param	fileName		文件名字
* @param	fileExt			文件扩展名
* @param	userAttrs		用户自定义属性(json串)
* @param	mtime			发送时间
* @param	timeout			超时
* @return 	错误码
*/
int32_t SendGroupFile(int32_t gid, int8_t mtype, std::string fileText, std::string fileName, std::string fileExt, std::string userAttrs, int64_t& mtime, int timeout = 120);

/**
* @desc		发送房间文件
* @param	rid				房间id
* @param	mtype			消息类型
* @param	fileText		文件二进制
* @param	fileName		文件名字
* @param	fileExt			文件扩展名
* @param	userAttrs		用户自定义属性(json串)
* @param	mtime			发送时间
* @param	timeout			超时
* @return 	错误码
*/
int32_t SendRoomFile(int32_t rid, int8_t mtype, std::string fileText, std::string fileName, std::string fileExt, std::string userAttrs, int64_t& mtime, int timeout = 120);

/**
* @desc		发送离线语音文件
* @param	pid				项目id
* @param	rtmAudio		离线语音数据(看RTMAudio.md介绍)
* @param	mtime			发送时间
* @param	timeout			超时
* @return 	错误码
*/
int32_t SendFile(int32_t pid, RTMAudio rtmAudio, int64_t& mtime, int timeout = 120);

/**
* @desc		发送离线语音群组文件
* @param	gid				群组id
* @param	rtmAudio		离线语音数据(看RTMAudio.md介绍)
* @param	mtime			发送时间
* @param	timeout			超时
* @return 	错误码
*/
int32_t SendGroupFile(int32_t gid, RTMAudio rtmAudio, int64_t& mtime, int timeout = 120);

/**
* @desc		发送离线语音房间文件
* @param	rid				房间id
* @param	rtmAudio		离线语音数据(看RTMAudio.md介绍)
* @param	mtime			发送时间
* @param	timeout			超时
* @return 	错误码
*/
int32_t SendRoomFile(int32_t rid, RTMAudio rtmAudio, int64_t& mtime, int timeout = 120);

/**************************************RTM文件异步接口**************************************/	
/**
* @desc		发送文件
* @param	pid				项目id
* @param	mtype			消息类型
* @param	fileText		文件二进制
* @param	fileName		文件名字
* @param	fileExt			文件扩展名
* @param	userAttrs		用户自定义属性(json串)
* @param	mtime			发送时间
* @param	timeout			超时
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void SendFile(function<void(int errorCode, int64_t mtime)> callback, int32_t pid, int8_t mtype, std::string fileText, std::string fileName, std::string fileExt, std::string userAttrs, int timeout = 120);

/**
* @desc		发送群组文件
* @param	gid				群组id
* @param	mtype			消息类型
* @param	fileText		文件二进制
* @param	fileName		文件名字
* @param	fileExt			文件扩展名
* @param	userAttrs		用户自定义属性(json串)
* @param	mtime			发送时间
* @param	timeout			超时
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void SendGroupFile(function<void(int errorCode, int64_t mtime)> callback, int32_t gid, int8_t mtype, std::string fileText, std::string fileName, std::string fileExt, std::string userAttrs, int timeout = 120);

/**
* @desc		发送房间文件
* @param	rid				房间id
* @param	mtype			消息类型
* @param	fileText		文件二进制
* @param	fileName		文件名字
* @param	fileExt			文件扩展名
* @param	userAttrs		用户自定义属性(json串)
* @param	mtime			发送时间
* @param	timeout			超时
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void SendRoomFile(function<void(int errorCode, int64_t mtime)> callback, int32_t rid, int8_t mtype, std::string fileText, std::string fileName, std::string fileExt, std::string userAttrs, int timeout = 120);

/**
* @desc		发送离线语音文件
* @param	pid				项目id
* @param	rtmAudio		离线语音数据(看RTMAudio.md介绍)
* @param	mtime			发送时间
* @param	timeout			超时
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void SendFile(function<void(int errorCode, int64_t mtime)> callback, int32_t pid, RTMAudio rtmAudio, int timeout = 120);

/**
* @desc		发送离线语音群组文件
* @param	gid				群组id
* @param	rtmAudio		离线语音数据(看RTMAudio.md介绍)
* @param	mtime			发送时间
* @param	timeout			超时
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void SendGroupFile(function<void(int errorCode, int64_t mtime)> callback, int32_t gid, RTMAudio rtmAudio, int timeout = 120);

/**
* @desc		发送离线语音房间文件
* @param	rid				房间id
* @param	rtmAudio		离线语音数据(看RTMAudio.md介绍)
* @param	mtime			发送时间
* @param	timeout			超时
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void SendRoomFile(function<void(int errorCode, int64_t mtime)> callback, int32_t rid, RTMAudio rtmAudio, int timeout = 120);