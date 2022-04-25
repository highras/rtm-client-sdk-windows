/**************************************RTM语音接口**************************************/
/**
* @desc		开始录音
* @param	
* @return 	true			成功
*			false			失败 
*/
bool StartRecordAudio();

/**
* @desc  	结束录音
* @param	recordata		音频数据
* @return 
*/
void StopRecordAudio(RTMAudio& recordata);	

/**
* @desc		开始播放音频
* @param   	recordata 		音频数据
* @return 	true			成功
*			false			失败 
*/
bool StartPlayAudio(RTMAudio recordata); 
	
/**
* @desc	  	结束播放
* @param    
* @return 	true			成功
*			false			失败 
*/
bool StopPlayAudio(); 

/**
* @desc	  	音频数据
* @param	audioCodec		音频编码格式
* @param	audioLang		音频语言
* @param	audioData		音频数据
* @param	audioDur		音频时长
* @param	audioSampleRate	音频采样率
* @return
*/
struct RTMAudio
{
    std::string audioCodec = "amr-wb";
    std::string audioLang = "zh-CN";
    std::string audioData; 
    int64_t audioDur = 0; 
    int32_t audioSampleRate = 16000;
};


