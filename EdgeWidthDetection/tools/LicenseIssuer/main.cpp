#include <actCry_ActivationInfo.hpp>
#include <QCryptographicHash>
#include <QString>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>

#include "hoei_CPUInfo.hpp"
#include "hoei_MotherBoardInfo.hpp"

namespace
{
	constexpr char kProductName[] = "EdgeWidthDetection";
	constexpr char kActivationKey[] = "EdgeWidthDetectionActivationKey";

	std::string toUpper(const std::string& str)
	{
		std::string result;
		result.reserve(str.size());
		for (unsigned char ch : str)
		{
			result.push_back(static_cast<char>(std::toupper(ch)));
		}
		return result;
	}

	bool isValidMotherboardUuid(const std::string& uuid)
	{
		if (uuid.empty())
			return false;

		const std::string upper = toUpper(uuid);
		if (upper == "UNKNOWN UUID")
			return false;
		if (upper == "FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF")
			return false;
		if (upper.find("O.E.M") != std::string::npos)
			return false;
		if (upper.find("TO BE FILLED") != std::string::npos)
			return false;

		return true;
	}

	std::string getSystemDiskSerialNumber()
	{
		try
		{
			char buffer[256];
			FILE* pipe = _popen("wmic path Win32_DiskDrive where Index=0 get SerialNumber", "r");
			if (!pipe)
				return {};

			std::string result;
			while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
			{
				result += buffer;
			}
			_pclose(pipe);

			std::istringstream stream(result);
			std::string line;
			std::getline(stream, line);
			std::getline(stream, line);

			line.erase(std::remove_if(line.begin(), line.end(), [](unsigned char ch) {
				return std::isspace(ch) || ch == '.';
			}), line.end());

			return line;
		}
		catch (...)
		{
			return {};
		}
	}

	std::string generateMachineCode()
	{
		std::string mbUuid = rw::hoei::MotherBoardInfoFactory::GetMotherboardUniqueID();
		std::string cpuModel = rw::hoei::CPUInfoFactory::GetCPUModel();
		std::string diskSerial = getSystemDiskSerialNumber();

		if (!isValidMotherboardUuid(mbUuid))
			mbUuid = "UNKNOWN_MB";

		std::string combined = std::string(kProductName) + "|" + mbUuid + "|" + cpuModel + "|" + diskSerial;
		QByteArray data(combined.data(), static_cast<int>(combined.size()));
		return QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex().toStdString();
	}

	std::chrono::system_clock::time_point parseDateTime(const std::string& str)
	{
		std::tm tmBuf{};
		std::istringstream ss(str);
		ss >> std::get_time(&tmBuf, "%Y-%m-%d");
		if (ss.fail())
		{
			throw std::runtime_error("日期格式错误，应为 YYYY-MM-DD");
		}
		tmBuf.tm_hour = 0;
		tmBuf.tm_min = 0;
		tmBuf.tm_sec = 0;
		auto timeT = std::mktime(&tmBuf);
		return std::chrono::system_clock::from_time_t(timeT);
	}
}

int main(int argc, char* argv[])
{
	if (argc == 2 && std::strcmp(argv[1], "--machine-code") == 0)
	{
		std::cout << generateMachineCode() << std::endl;
		return 0;
	}

	if (argc != 2)
	{
		std::cerr << "用法:" << std::endl;
		std::cerr << "  LicenseIssuer --machine-code" << std::endl;
		std::cerr << "  LicenseIssuer <机器码>" << std::endl;
		std::cerr << "示例: LicenseIssuer ABCD1234..." << std::endl;
		return 1;
	}

	try
	{
		rw::actCry::ActivationInfo info;
		info.hwid = argv[1];
		info.startTime = std::chrono::system_clock::time_point{};
		info.endTime = parseDateTime("2099-12-31");

		auto code = rw::actCry::ActivationInfo::generateActivationCode(info, kActivationKey);
		std::cout << code << std::endl;
		return 0;
	}
	catch (const std::exception& e)
	{
		std::cerr << "生成激活码失败: " << e.what() << std::endl;
		return 1;
	}
}
