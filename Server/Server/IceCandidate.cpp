//#define MS_CLASS "RTC::IceCandidate"
// #define MS_LOG_DEV_LEVEL 3

#include "IceCandidate.hpp"
//#include "Logger.hpp"
#include <ostream>
#include <sstream>   
namespace RTC
{
	/* Instance methods. */

	//void IceCandidate::FillJson(json& jsonObject) const
	//{
	//	MS_TRACE();

	//	// Add foundation.
	//	jsonObject["foundation"] = this->foundation;

	//	// Add priority.
	//	jsonObject["priority"] = this->priority;

	//	// Add ip.
	//	jsonObject["ip"] = this->ip;

	//	// Add protocol.
	//	switch (this->protocol)
	//	{
	//		case Protocol::UDP:
	//			jsonObject["protocol"] = "udp";
	//			break;

	//		case Protocol::TCP:
	//			jsonObject["protocol"] = "tcp";
	//			break;
	//	}

	//	// Add port.
	//	jsonObject["port"] = this->port;

	//	// Add type.
	//	switch (this->type)
	//	{
	//		case CandidateType::HOST:
	//			jsonObject["type"] = "host";
	//			break;
	//	}

	std::string  IceCandidate::candidate()
	{
		std::ostringstream cmd;
		cmd << foundation << " 1  ";
		// Add foundation.
		//value["iceCandidates"]["foundation"] = this->foundation;

		//// Add priority.
		//value["iceCandidates"]["priority"] = this->priority;

		//// Add ip.
		//value["iceCandidates"]["ip"] = this->ip;

		//// Add protocol.
		switch (this->protocol)
		{
		case Protocol::UDP:
			//value["iceCandidates"]["protocol"] = "udp";
			cmd << "udp ";
			break;

		case Protocol::TCP:
			//value["iceCandidates"]["protocol"] = "tcp";
			cmd << "tcp ";
			break;
		}
		cmd << priority << " " << ip << " " << port  << " type ";
		//// Add port.
		//value["iceCandidates"]["port"] = this->port;

		// Add type.
		switch (this->type)
		{
			case CandidateType::HOST:
			{
				//value["iceCandidates"]["type"] = "host";
				cmd << "host";
				break;
			}
		}
		return cmd.str();

		//// Add tcpType.
		//if (this->protocol == Protocol::TCP)
		//{
		//	switch (this->tcpType)
		//	{
		//	case TcpCandidateType::PASSIVE:
		//		value["iceCandidates"]["tcpType"] = "passive";
		//		break;
		//	}
		//}
	}

	//	// Add tcpType.
	//	if (this->protocol == Protocol::TCP)
	//	{
	//		switch (this->tcpType)
	//		{
	//			case TcpCandidateType::PASSIVE:
	//				jsonObject["tcpType"] = "passive";
	//				break;
	//		}
	//	}
	//}
} // namespace RTC
