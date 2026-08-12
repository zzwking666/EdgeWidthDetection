#pragma once
#include <QString>

#include "imgPro_ImageProcessUtilty.hpp"

inline struct GlobalPath
{
public:
	QString projectHome = R"(D:\zfkjData\EdgeWidthDetection\)";
public:
	QString configRootPath = projectHome + R"(config\)";
	QString modelRootPath = projectHome + R"(model\)";
	QString EdgeWidthDetectionConfigPath = configRootPath + R"(EdgeWidthDetection.xml)";
	QString setConfigPath = configRootPath + R"(SetConfig.xml)";
	QString modelPath = modelRootPath + R"(EdgeWidthDetection.engine)";
public:
	QString imageSaveRootPath = projectHome + R"(savedImages\)";
	/// UPS 断电事件凭证文件保存目录（每次断电/关机保存配置时写一条记录）
	QString upsRecordRootPath = projectHome + R"(upsRecord\)";
public:
	QString testImgDirPath = R"(D:\ImgStorage\EdgeWidthDetection)";
}globalPath;

struct ClassId
{
public:
	static constexpr int minNum = 0;

	static constexpr int edge = 0;

	static constexpr int maxNum = 0;
	static constexpr int indexCount = 1;
	static std::unordered_map<rw::imgPro::ClassId, rw::imgPro::ClassIdName> classIdNameMap;
	static std::vector<rw::imgPro::ClassId> classids;
};

struct Utility
{
	static QString cameraIp1;
	static QString cameraIp2;
};

struct ModBusAddress
{
	static int shiceyahenkuanduAddress;
	static int shedingyahenbiaozhunkuanduzhiAddress;
	static int paizhaojiangejuliAddress;
	static int daizichangdubiaozhunAddress;
	static int jimiqiyiquanmaichongshuAddress;
	static int jimiqiyiquanzhouchangAddress;
	static int bujinyiquanmaichongshuAddress;
	static int bujinluojuAddress;
	static int daoyidongbuchangAddress;
	static int zidongdaoyidongzuidajuliAddress;
	static int daojiakeyidongdezuidajuliAddress;
	static int daoyidongsudumaichongpinlvAddress;
	static int shibiezhongxindianyutuxiangzhongxindianchazhiAddress;

	static int readPLCbaojingxinxiAddress;
	static int readPLCkeyipaizhaoxinhaoAddress;
	static int readPLCtingzhipaizhaoxinhaoAddress;
	static int readPLCdaizishicechangduAddress;
};

struct PlcReadItem
{
	uint16_t value;
	bool ok;
	int index;
};

struct PlcReadIndex
{
	static constexpr int shiceyahenkuandu = 1;				 // 实测压痕宽度
	static constexpr int shedingyahenbiaozhunzhi = 2;		 // 设定压痕标准值
	static constexpr int paizhaojiangejuli = 3;				 // 拍照间隔距离
	static constexpr int daizichangdubiaozhun = 4;			 // 袋子长度标准
	static constexpr int jimiqiyiquanmaichongshu = 5;		 // 计米器一圈脉冲数
	static constexpr int jimiqiyiquanzhouchang = 6;			 // 计米器一圈周长
	static constexpr int bujinyiquanmaichongshu = 7;		 // 步进一圈脉冲数
	static constexpr int bujinluoju = 8;					 // 步进螺距
	static constexpr int daoyidongbuchang = 9;				 // 刀移动补偿长度
	static constexpr int zidongdaoyidongzuidajuli = 10;		 // 自动刀移动最大距离
	static constexpr int daojiakeyidongdezuidajuli = 11;	 // 刀架可移动的最大距离
	static constexpr int daoyidongsudumaichongpinlv = 12;	 // 刀移动速度脉冲频率
	static constexpr int shibiezhongxindianyutuxiangzhongxindianchazhi = 17; // 识别中心点与图像中心点差值

	static constexpr int readPLCbaojingxinxi = 13;			 // PLC报警信息
	static constexpr int readPLCkeyipaizhaoxinhao = 14;      // PLC开机拍照信号
	static constexpr int readPLCtingzhipaizhaoxinhao = 15;   // PLC停止拍照信号
	static constexpr int readPLCdaizishicechangdu = 16;      // PLC袋子实测长度
};

// 运行状态（定义在此供 MatInfo 等基础数据结构使用，RuntimeInfoModule.hpp 直接引用）
enum class RunningState
{
	Debug,
	OpenRemoveFunc,
	Stop
};

// 图片信息
struct MatInfo {
	cv::Mat image;	// 图片内容
	float location;	// 记录拍照瞬间的时间点
	size_t index;	// 拍照的相机的下标
	// 帧到达程序瞬间（onFrameCaptured）的运行状态，处理线程按此状态分发，
	// 避免调试模式切到剔废模式后，切换窗口内到达的调试帧被当作剔废帧计数/写PLC
	RunningState captureState = RunningState::Stop;
};

