#include <MyEventHandler.h>
#include <qDebug.h>
void MyEventHandler::OnKickout() {}
void MyEventHandler::OnKickoutRoom(int64_t rid) {}
void MyEventHandler::OnPushMsg(int64_t from, int64_t to, int8_t mtype, int64_t mid, string msg, string attrs, int64_t mtime) 
{
	qDebug() << "from:" << from << ", to:" << to << ", mtype:" << mtype << ", mid:" << mid << ", msg:" 
		<< QString::fromStdString(msg) << ", attrs:" << QString::fromStdString(attrs) << ", mtime:" << mtime;
}

void MyEventHandler::OnPushGroupMsg(int64_t from, int64_t gid, int8_t mtype, int64_t mid, string msg, string attrs, int64_t mtime)
{
	qDebug() << "from:" << from << ", gid:" << gid << ", mtype:" << mtype << ", mid:" << mid << ", msg:"
		<< QString::fromStdString(msg) << ", attrs:" << QString::fromStdString(attrs) << ", mtime:" << mtime;
}
void MyEventHandler::OnPushRoomMsg(int64_t from, int64_t rid, int8_t mtype, int64_t mid, string msg, string attrs, int64_t mtime)
{
	qDebug() << "from:" << from << ", rid:" << rid << ", mtype:" << mtype << ", mid:" << mid << ", msg:"
		<< QString::fromStdString(msg) << ", attrs:" << QString::fromStdString(attrs) << ", mtime:" << mtime;
}

void MyEventHandler::OnPushBroadcastMsg(int64_t from, int8_t mtype, int64_t mid, string msg, string attrs, int64_t mtime)
{
	qDebug() << "from:" << from <<", mtype:" << mtype << ", mid:" << mid << ", msg:"
		<< QString::fromStdString(msg) << ", attrs:" << QString::fromStdString(attrs) << ", mtime:" << mtime;
}

void MyEventHandler::OnPushFile(int64_t from, int64_t to, int8_t mtype, int64_t mid, string msg, string attrs, int64_t mtime) 
{
	qDebug() << "from:" << from << ", to:" << to << ", mtype:" << mtype << ", mid:" << mid << ", msg:"
		<< QString::fromStdString(msg) << ", attrs:" << QString::fromStdString(attrs) << ", mtime:" << mtime;
}

void MyEventHandler::OnPushGroupFile(int64_t from, int64_t gid, int8_t mtype, int64_t mid, string msg, string attrs, int64_t mtime)
{
	qDebug() << "from:" << from << ", gid:" << gid << ", mtype:" << mtype << ", mid:" << mid << ", msg:"
		<< QString::fromStdString(msg) << ", attrs:" << QString::fromStdString(attrs) << ", mtime:" << mtime;
}

void MyEventHandler::OnPushRoomFile(int64_t from, int64_t rid, int8_t mtype, int64_t mid, string msg, string attrs, int64_t mtime)
{
	qDebug() << "from:" << from << ", rid:" << rid << ", mtype:" << mtype << ", mid:" << mid << ", msg:"
		<< QString::fromStdString(msg) << ", attrs:" << QString::fromStdString(attrs) << ", mtime:" << mtime;
}

void MyEventHandler::OnPushBroadcastFile(int64_t from, int8_t mtype, int64_t mid, string msg, string attrs, int64_t mtime)
{
	qDebug() << "from:" << from << ", mtype:" << mtype << ", mid:" << mid << ", msg:"
		<< QString::fromStdString(msg) << ", attrs:" << QString::fromStdString(attrs) << ", mtime:" << mtime;
}

void MyEventHandler::OnPushChat(int64_t from, int64_t to, int64_t mid, string msg, string attrs, int64_t mtime)
{
	qDebug() << "from:" << from << ", to:" << to <<", mid:" << mid << ", msg:"
		<< QString::fromStdString(msg) << ", attrs:" << QString::fromStdString(attrs) << ", mtime:" << mtime;
}

void MyEventHandler::OnPushCmd(int64_t from, int64_t to, int64_t mid, string msg, string attrs, int64_t mtime) 
{
	qDebug() << "from:" << from << ", to:" << to << ", mid:" << mid << ", msg:"
		<< QString::fromStdString(msg) << ", attrs:" << QString::fromStdString(attrs) << ", mtime:" << mtime;
}

void MyEventHandler::OnPushGroupChat(int64_t from, int64_t gid, int64_t mid, string msg, string attrs, int64_t mtime)
{
	qDebug() << "from:" << from << ", gid:" << gid << ", mid:" << mid << ", msg:"
		<< QString::fromStdString(msg) << ", attrs:" << QString::fromStdString(attrs) << ", mtime:" << mtime;
}
void MyEventHandler::OnPushGroupCmd(int64_t from, int64_t gid, int64_t mid, string msg, string attrs, int64_t mtime)
{
	qDebug() << "from:" << from << ", gid:" << gid << ", mid:" << mid << ", msg:"
		<< QString::fromStdString(msg) << ", attrs:" << QString::fromStdString(attrs) << ", mtime:" << mtime;
}

void MyEventHandler::OnPushRoomChat(int64_t from, int64_t rid, int64_t mid, string msg, string attrs, int64_t mtime)
{
	qDebug() << "from:" << from << ", rid:" << rid << ", mid:" << mid << ", msg:"
		<< QString::fromStdString(msg) << ", attrs:" << QString::fromStdString(attrs) << ", mtime:" << mtime;
}

void MyEventHandler::OnPushRoomCmd(int64_t from, int64_t rid, int64_t mid, string msg, string attrs, int64_t mtime) 
{
	qDebug() << "from:" << from << ", rid:" << rid << ", mid:" << mid << ", msg:"
		<< QString::fromStdString(msg) << ", attrs:" << QString::fromStdString(attrs) << ", mtime:" << mtime;
}

void MyEventHandler::OnPushBroadcastChat(int64_t from, int64_t mid, string msg, string attrs, int64_t mtime)
{
	qDebug() << "from:" << from <<", mid:" << mid << ", msg:"
		<< QString::fromStdString(msg) << ", attrs:" << QString::fromStdString(attrs) << ", mtime:" << mtime;
}

void MyEventHandler::OnPushBroadcastCmd(int64_t from, int64_t mid, string msg, string attrs, int64_t mtime)
{
	qDebug() << "from:" << from << ", mid:" << mid << ", msg:"
		<< QString::fromStdString(msg) << ", attrs:" << QString::fromStdString(attrs) << ", mtime:" << mtime;
}

void MyEventHandler::connnectionClosed(int32_t closeError)
{
	qDebug() << "connnectionClosed ret:" << closeError;
}

void MyEventHandler::reloginCompleted(bool successful, bool retryAgain, int errorCode, int retriedCount)
{
	qDebug() << "connnectionClosed successful:" << successful << ", retryAgain:" << retryAgain << ", errorCode:" << errorCode << ", retriedCount:" << retriedCount;
}

bool MyEventHandler::reloginWillStart(int lastErrorCode, int retriedCount)
{
	qDebug() << "reloginWillStart lastErrorCode:" << lastErrorCode << ", retriedCount:" << retriedCount;

	return true;
}
