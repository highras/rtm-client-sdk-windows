/**************************************RTM登录同步接口**************************************/	
/**
* @desc		登录
* @param	token			令牌
* @param	pid				项目id
* @param	uid				用户id
* @param	successed		登录结果
* @param	attr			用户属性
* @param	lang			用户语言
* @param	timeout			登录超时
* @return 	错误码
*/
int32_t Login(string token, int64_t& pid, int64_t& uid, bool& successed, std::unordered_map<string, string> attr = std::unordered_map<string, string>(), string lang = "", int timeout = 0);

/**
* @desc		登出
* @param	
* @return 	错误码
*/
int32_t Bye();


/**************************************RTM登录异步接口**************************************/
/**
* @desc		登录
* @param	token			令牌
* @param	pid				项目id
* @param	uid				用户id
* @param	successed		登录结果
* @param	attr			用户属性
* @param	lang			用户语言
* @param	timeout			登录超时
* @param	callback		异步回调
* @return 	
*/
void Login(function<void(int64_t pid, int64_t uid, int errorCode, bool successed)> callback, string token, std::unordered_map<string, string> attr = std::unordered_map<string, string>(), string lang = "", int timeout = 0);

/**
* @desc		登出
* @param	callback		异步回调
* @return 	错误码
*/
void Bye(function<void(int errorCode)> callback);