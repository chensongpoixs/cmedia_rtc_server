#ifndef MS_RTC_PARAMETERS_HPP
#define MS_RTC_PARAMETERS_HPP

//#include "common.hpp"
//#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
#include <vector>
#include "cnet_type.h"
//using json = nlohmann::json;
#include <json/json.h>
namespace RTC
{
	class Parameters
	{
	public:
		class Value
		{
		public:
			enum class Type
			{
				BOOLEAN = 1,
				INTEGER,
				DOUBLE,
				STRING,
				ARRAY_OF_INTEGERS
			};

		public:
			explicit Value(bool booleanValue) : type(Type::BOOLEAN), booleanValue(booleanValue)
			{
			}

			explicit Value(int32_t integerValue) : type(Type::INTEGER), integerValue(integerValue)
			{
			}

			explicit Value(double doubleValue) : type(Type::DOUBLE), doubleValue(doubleValue)
			{
			}

			explicit Value(std::string& stringValue) : type(Type::STRING), stringValue(stringValue)
			{
			}

			explicit Value(std::string&& stringValue) : type(Type::STRING), stringValue(stringValue)
			{
			}

			explicit Value(std::vector<int32_t>& arrayOfIntegers)
			  : type(Type::ARRAY_OF_INTEGERS), arrayOfIntegers(arrayOfIntegers)
			{
			}

		public:
			Type type;
			bool booleanValue{ false };
			int32_t integerValue{ 0 };
			double doubleValue{ 0.0 };
			const std::string stringValue;
			const std::vector<int32_t> arrayOfIntegers;
		};

	public:
		explicit Parameters();
		//void FillJson(json& jsonObject) const;
		void Set(Json::Value& data); 
		bool HasBoolean(const std::string& key) const;
		bool HasInteger(const std::string& key) const;
		bool HasPositiveInteger(const std::string& key) const;
		bool HasDouble(const std::string& key) const;
		bool HasString(const std::string& key) const;
		bool HasArrayOfIntegers(const std::string& key) const;
		bool IncludesInteger(const std::string& key, int32_t integer) const;
		bool GetBoolean(const std::string& key) const;
		int32_t GetInteger(const std::string& key) const;
		double GetDouble(const std::string& key) const;
		const std::string& GetString(const std::string& key) const;
		const std::vector<int32_t>& GetArrayOfIntegers(const std::string& key) const;
		
		void insert(const char * key, int32_t value);
		void insert(const char * key, const char *value);
		
	public:
		std::unordered_map<std::string, Value> mapKeyValues;
		std::map<std::string, std::string>    codecs_map;
	};
} // namespace RTC

#endif
