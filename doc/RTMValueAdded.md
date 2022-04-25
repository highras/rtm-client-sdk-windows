/**************************************RTM增值服务同步接口**************************************/	
/**
* @desc		设置语言
* @param	lang			指定语言
* @return 	错误码
*/
int32_t SetLang(string lang);

/**
* @desc		翻译
* @param	text			原文本
* @param	src				文本原语言
* @param	dst				目标语言
* @param	type			chat或mail
* @param	profanity		翻译结果敏感与过滤(off/censor，默认off)
* @param	source			翻译结果原文本
* @param	target			翻译结果目标文本
* @param	sourceText		翻译结果文本原语言
* @param	targetText		翻译结果目标语言
* @return 	错误码
*/
int32_t Translate(string text, string src, string dst, string type, string profanity, string& source, string& target, string& sourceText, string& targetText);

/**
* @desc		过滤敏感词
* @param	text			原文本
* @param	classify		文本分类检测
* @param	otext			过滤后的文本
* @param	classification	过滤后的分类文本
* @return 	错误码
*/
int32_t Profanity(string text, bool classify, string& otext, vector<string>& classification);

/**
* @desc		语音转文本
* @param	audio			音频url或二进制(看type字段)
* @param	type			指定audio字段类型(1-url，2-二进制)
* @param	lang			音频语言
* @param	codec			音频编码(默认AMR_WB)
* @param	srate			音频采样率(默认16k)
* @param	text			转义后的文本
* @param	olang			转义后的语言
* @return 	错误码
*/
int32_t Speech2Text(string audio, int32_t type, string lang, string codec, int32_t srate, string& text, string& olang);

/**
* @desc		文本审核
* @param	text			文本
* @param	result			审核结果
* @param	otext			审核后的文本
* @param	tags			触发的分类
* @param	wlist			敏感词列表
* @return 	错误码
*/
int32_t TCheck(string text, int32_t& result, string& otext, vector<string>& tags, vector<string>& wlist);

/**
* @desc		图片审核
* @param	image			图片url或二进制(看type字段)
* @param	type			指定image字段类型(1-url，2-二进制)
* @param	result			审核结果
* @param	tags			触发的分类
* @return 	错误码
*/
int32_t ICheck(string image, int32_t type, int32_t& result, vector<string>& tags);

/**
* @desc		音频审核
* @param	audio			音频url或二进制(看type字段)
* @param	type			指定audio字段类型(1-url，2-二进制)
* @param	lang			音频语言
* @param	codec			音频编码(默认AMR_WB)
* @param	srate			音频采样率(默认16k)
* @param	result			审核结果
* @param	tags			触发的分类
* @return 	错误码
*/
int32_t ACheck(string audio, int32_t type, string lang, string codec, int32_t srate, int32_t& result, vector<string>& tags);

/**
* @desc		视频审核
* @param	video			视频url或二进制(看type字段)
* @param	type			指定video字段类型(1-url，2-二进制)
* @param	videoName		视频名字(type=2，必选)
* @param	result			审核结果
* @param	tags			触发的分类
* @return 	错误码
*/
int32_t VCheck(string video, int32_t type, string videoName, int32_t& result, vector<string>& tags);


/**************************************RTM增值服务异步接口**************************************/	
/**
* @desc		设置语言
* @param	lang			指定语言
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void SetLang(string lang, function<void(int errorCode)> callback);

/**
* @desc		翻译
* @param	text			原文本
* @param	src				文本原语言
* @param	dst				目标语言
* @param	type			chat或mail
* @param	profanity		翻译结果敏感与过滤(off/censor，默认off)
* @param	source			翻译结果原文本
* @param	target			翻译结果目标文本
* @param	sourceText		翻译结果文本原语言
* @param	targetText		翻译结果目标语言
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void Translate(string text, string src, string dst, string type, string profanity, function<void(int errorCode, string source, string target, string sourceText, string targetText)> callback);

/**
* @desc		过滤敏感词
* @param	text			原文本
* @param	classify		文本分类检测
* @param	otext			过滤后的文本
* @param	classification	过滤后的分类文本
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void Profanity(string text, bool classify, function<void(int errorCode, string text, vector<string> classification)> callback);

/**
* @desc		语音转文本
* @param	audio			音频url或二进制(看type字段)
* @param	type			指定audio字段类型(1-url，2-二进制)
* @param	lang			音频语言
* @param	codec			音频编码(默认AMR_WB)
* @param	srate			音频采样率(默认16k)
* @param	text			转义后的文本
* @param	olang			转义后的语言
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void Speech2Text(string audio, int32_t type, string lang, string codec, int32_t srate, function<void(int errorCode, string text, string lang)> callback);

/**
* @desc		文本审核
* @param	text			文本
* @param	result			审核结果
* @param	otext			审核后的文本
* @param	tags			触发的分类
* @param	wlist			敏感词列表
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void TCheck(string text, function<void(int errorCode, int32_t result, string text, vector<string> tags, vector<string> wlist)> callback);

/**
* @desc		图片审核
* @param	image			图片url或二进制(看type字段)
* @param	type			指定image字段类型(1-url，2-二进制)
* @param	result			审核结果
* @param	tags			触发的分类
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void ICheck(string image, int32_t type, function<void(int errorCode, int32_t result, vector<string> tags)> callback);

/**
* @desc		音频审核
* @param	audio			音频url或二进制(看type字段)
* @param	type			指定audio字段类型(1-url，2-二进制)
* @param	lang			音频语言
* @param	codec			音频编码(默认AMR_WB)
* @param	srate			音频采样率(默认16k)
* @param	result			审核结果
* @param	tags			触发的分类
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void ACheck(string audio, int32_t type, string lang, string codec, int32_t srate, function<void(int errorCode, int32_t result, vector<string> tags)> callback);

/**
* @desc		视频审核
* @param	video			视频url或二进制(看type字段)
* @param	type			指定video字段类型(1-url，2-二进制)
* @param	videoName		视频名字(type=2，必选)
* @param	result			审核结果
* @param	tags			触发的分类
* @param	errorCode		错误码
* @param	callback		异步回调
* @return 	
*/
void VCheck(string video, int32_t type, string videoName, function<void(int errorCode, int32_t result, vector<string> tags)> callback);
