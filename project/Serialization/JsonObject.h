#pragma once

#include <string>
#include <map>
#include <vector>
#include <variant>
#include <any>

class Serializable;

/// @brief
class JsonObject
{
public:
   using MapType = std::map<std::string, JsonObject>;
   using ObjectVariant = std::variant<std::any, MapType>;

   JsonObject() = default;

   /// @brief creates a JsonObject from a string
   static JsonObject fromString(const std::string& json);

   /// @brief creates a JsonObject from a value
   JsonObject(const std::any& value);

   /// @brief creates a JsonObject from a map
   JsonObject(const MapType& map);

   /// @brief creates a JsonObject from a serializable
   JsonObject(const Serializable& serializable);

   /// @brief creates a JsonObject from a generic
   template <typename T>
   JsonObject(const T& value)
      : m_data(std::any(value))
   {}

   /// @brief assigns a value to the JsonObject
   JsonObject& operator=(const std::any& value);

   /// @brief assigns a map to the JsonObject
   JsonObject& operator=(const MapType& map);

   /// @brief assigns a generic to the JsonObject
   template <typename T>
   JsonObject& operator=(const T& value)
   {
      m_data = std::any(value);
      return *this;
   }

   /// @returns whether the key is a number
   bool isNumber() const;

   /// @returns whether the key is a string
   bool isString() const;

   /// @returns whether the key is a boolean
   bool isBool() const;

   /// @returns whether the key is an object
   bool isObject() const;

   /// @returns whether the key is an array
   bool isArray() const;

   /// @returns the value as any
   std::any getAny() const;

   /// @returns the value of the key as a number
   double getNumber() const;

   /// @returns the value of the key as a string
   std::string getString() const;

   /// @returns the value of the key as a boolean
   bool getBool() const;

   /// @returns the value of the key as an array
   std::vector<JsonObject> getArray() const;

   /// @returns the value of the key
   JsonObject& operator[](const std::string& key);

   /// @returns the value of the key
   const JsonObject& operator[](const std::string& key) const;

   /// @returns the JsonObject as a string
   std::string toString() const;

   /// @returns the keys of the JsonObject
   std::vector<std::string> getKeys() const;

   /// @returns the value as a T
   template <typename T>
   T get() const
   {
      return std::any_cast<T>(std::get<std::any>(m_data));
   }

   inline auto begin() { return std::get<MapType>(m_data).begin(); }
   inline auto end() { return std::get<MapType>(m_data).end(); }

   inline auto begin() const { return std::get<MapType>(m_data).begin(); }
   inline auto end() const { return std::get<MapType>(m_data).end(); }

private:
   static std::string parseString(const std::string& json, int begin, int& end);
   static double parseNumber(const std::string& json, int begin, int& end);
   static bool parseBool(const std::string& json, int begin, int& end);
   static std::vector<JsonObject> parseArray(const std::string& json, int begin, int& end);
   static JsonObject parseObject(const std::string& json, int begin, int& end);
   static void parseSpace(const std::string& json, int begin, int& end);

private:
   ObjectVariant m_data = MapType {}; ///< value of the JsonObject
};
