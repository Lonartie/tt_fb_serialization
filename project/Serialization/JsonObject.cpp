#include "JsonObject.h"
#include "Serializable.h"
#include <sstream>

namespace
{
   std::string trimmed(const std::string& str)
   {
      uint64_t begin = 0;
      while (begin < str.size() && str[begin] == ' ' || str[begin] == '\t' || str[begin] == '\n')
         +begin;
      uint64_t end = str.size() - 1;
      while (end > 0 && str[end] == ' ' || str[end] == '\t' || str[end] == '\r')
         --end;
      return str.substr(begin, end - begin + 1);
   }
}

JsonObject JsonObject::fromString(const std::string& json)
{
   JsonObject result(MapType{});
   int end;
   result.m_data = parseObject(trimmed(json), 0, end).m_data;
   return result;
}

JsonObject::JsonObject(const std::any& value)
   : m_data(value)
{
}

JsonObject::JsonObject(const JsonObject::MapType& map)
   : m_data(map)
{
}

JsonObject::JsonObject(const Serializable& serializable)
{
   serializable.serializeTo(*this);
}

JsonObject& JsonObject::operator=(const std::any& value)
{
   m_data = value;
   return *this;
}

JsonObject& JsonObject::operator=(const JsonObject::MapType& map)
{
   m_data = map;
   return *this;
}

bool JsonObject::isNumber() const
{
   if (isObject())
      return false;

   auto& data = std::get<std::any>(m_data);

   return data.type() == typeid(double)
          || data.type() == typeid(float)
          || data.type() == typeid(int);
}

bool JsonObject::isString() const
{
   if (isObject())
      return false;

   auto& data = std::get<std::any>(m_data);
   return data.type() == typeid(std::string)
          || data.type() == typeid(const char*);
}

bool JsonObject::isBool() const
{
   if (isObject())
      return false;

   auto& data = std::get<std::any>(m_data);
   return data.type() == typeid(bool);
}

bool JsonObject::isObject() const
{
   return std::holds_alternative<MapType>(m_data);
}

bool JsonObject::isArray() const
{
   if (isObject())
      return false;

   auto& data = std::get<std::any>(m_data);
   return data.type() == typeid(std::vector<JsonObject>);
}

double JsonObject::getNumber() const
{
   auto& data = std::get<std::any>(m_data);
   return data.type() == typeid(double)
          ? std::any_cast<double>(data)
          : data.type() == typeid(float)
            ? static_cast<double>(std::any_cast<float>(data))
            : static_cast<double>(std::any_cast<int>(data));
}

std::string JsonObject::getString() const
{
   auto& data = std::get<std::any>(m_data);
   return data.type() == typeid(std::string)
          ? std::any_cast<std::string>(data)
          : std::string(std::any_cast<const char*>(data));
}

bool JsonObject::getBool() const
{
   auto& data = std::get<std::any>(m_data);
   return std::any_cast<bool>(data);
}

std::vector<JsonObject> JsonObject::getArray() const
{
   auto& data = std::get<std::any>(m_data);
   return std::any_cast<std::vector<JsonObject>>(data);
}

JsonObject& JsonObject::operator[](const std::string& key)
{
   auto& map = std::get<MapType>(m_data);
   return map[key];
}

const JsonObject& JsonObject::operator[](const std::string& key) const
{
   auto& map = std::get<MapType>(m_data);
   return map.at(key);
}

std::string JsonObject::toString() const
{
   if (isNumber())
      return std::to_string(getNumber());

   if (isString())
      return "\"" + getString() + "\"";

   if (isBool())
      return getBool() ? "true" : "false";

   if (isObject())
   {
      std::stringstream ss;
      ss << "{";
      bool first = true;
      for (auto& [key, value]: std::get<MapType>(m_data))
         ss << (first ? "" : ",") << "\"" << key << "\":" << value.toString();
      ss << "}";
      return ss.str();
   }

   if (isArray())
   {
      std::stringstream ss;
      ss << "[";
      bool first = true;
      for (auto& value: getArray())
         ss << (first ? first = false, "" : ",") << value.toString();
      ss << "]";
      return ss.str();
   }

   return "";
}

bool JsonObject::parseBool(const std::string& json, int begin, int& end)
{
   bool result = json[begin] == 't';
   end = begin + (result ? 4 : 5);
   return result;
}

std::string JsonObject::parseString(const std::string& json, int begin, int& end)
{
   std::string result;
   end = begin + 1;
   while (end < json.size() && json[end] != '"')
   {
      if (json[end] == '\\')
      {
         if (json[end + 1] == '"')
            result += '"';
         else if (json[end + 1] == '\\')
            result += '\\';
         else if (json[end + 1] == '/')
            result += '/';
         else if (json[end + 1] == 'b')
            result += '\b';
         else if (json[end + 1] == 'f')
            result += '\f';
         else if (json[end + 1] == 'n')
            result += '\n';
         else if (json[end + 1] == 'r')
            result += '\r';
         else if (json[end + 1] == 't')
            result += '\t';
         else if (json[end + 1] == 'u')
         {
            result += '\\';
            result += 'u';
            end += 4;
         }
         else
            result += json[end + 1];
         end += 2;
      }
      else
      {
         result += json[end];
         end++;
      }
   }
   end++;
   return result;
}

double JsonObject::parseNumber(const std::string& json, int begin, int& end)
{
   std::string result;
   while (end < json.size() && (json[end] >= '0' && json[end] <= '9' || json[end] == '.' || json[end] == '-' || json[end] == '+'))
   {
      result += json[end];
      end++;
   }
   return std::stod(result);
}

JsonObject JsonObject::parseObject(const std::string& json, int begin, int& end)
{
   JsonObject result(MapType{});
   end = begin + 1;
   while (end < json.size() && json[end] != '}')
   {
      parseSpace(json, end, end);
      if (json[end] == '"')
      {
         // KEY
         auto key = parseString(json, end, end);

         parseSpace(json, end, end);
         if (json[end] != ':')
            throw std::runtime_error("Expected ':'");
         end++;
         parseSpace(json, end, end);

         // VALUE

         if (json[end] == '{')
            result[key] = parseObject(json, end, end);
         else if (json[end] == '[')
            result[key] = parseArray(json, end, end);
         else
         {
            if (json[end] == 't' || json[end] == 'f')
               result[key] = parseBool(json, end, end);
            else if (json[end] == '"')
               result[key] = parseString(json, end, end);
            else if (json[end] == '-' || json[end] == '+' || (json[end] >= '0' && json[end] <= '9'))
               result[key] = parseNumber(json, end, end);
            else
               throw std::runtime_error("Unexpected character");
         }

         if (json[end] == ',')
            end++;
         else if (json[end] != '}')
            throw std::runtime_error("Expected ',' or '}'");
      }
      else
         throw std::runtime_error("Expected '\"'");
   }

   end++;
   return result;
}

std::vector<JsonObject> JsonObject::parseArray(const std::string& json, int begin, int& end)
{
   std::vector<JsonObject> result;
   end = begin + 1;
   while (end < json.size() && json[end] != ']')
   {
      parseSpace(json, end, end);
      if (json[end] == '{')
         result.push_back(parseObject(json, end, end));
      else if (json[end] == '[')
         result.push_back(parseArray(json, end, end));
      else
      {
         if (json[end] == 't')
            result.push_back(parseBool(json, end, end));
         else if (json[end] == '"')
            result.push_back(parseString(json, end, end));
         else if (json[end] == '-' || (json[end] >= '0' && json[end] <= '9'))
            result.push_back(parseNumber(json, end, end));
         else
            throw std::runtime_error("Unexpected character");
      }
      parseSpace(json, end, end);
      if (json[end] == ',')
         end++;
      else if (json[end] != ']')
         throw std::runtime_error("Expected ',' or ']'");
   }

   end++;
   return result;
}

void JsonObject::parseSpace(const std::string& json, int begin, int& end)
{
   end = begin;
   while (end < json.size() && json[end] == ' ' || json[end] == '\n' || json[end] == '\r' || json[end] == '\t')
      end++;
}

std::any JsonObject::getAny() const
{
   return std::get<std::any>(m_data);
}

std::vector<std::string> JsonObject::getKeys() const
{
   std::vector<std::string> result;
   for (auto& pair : std::get<MapType>(m_data))
      result.push_back(pair.first);
   return result;
}
