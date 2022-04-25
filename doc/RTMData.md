/**************************************RTM数据同步接口**************************************/	
/**
* @desc		设置数据
* @param	key				键
* @param	val				值
* @return 	错误码
*/
int32_t DataGet(string key, string& val);

/**
* @desc		获取数据
* @param	key				键
* @param	val				值
* @return 	错误码
*/
int32_t DataSet(string key, string val);

/**
* @desc		删除数据
* @param	key				键
* @return 	错误码
*/
int32_t DataDel(string key);


/**************************************RTM数据异步接口**************************************/	
/**
* @desc		设置数据
* @param	key				键
* @param	val				值
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void DataGet(string key, function<void(int errorCode, string val)> callback);

/**
* @desc		获取数据
* @param	key				键
* @param	val				值
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void DataSet(string key, string val, function<void(int errorCode)> callback);

/**
* @desc		删除数据
* @param	key				键
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void DataDel(string key, function<void(int errorCode)> callback);