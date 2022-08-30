#pragma once

#include "Serializable.h"
#include <tuple>

/// @brief returns true if type is a std::vector
template <typename T>
class is_vector : std::false_type
{
};

/// @brief returns true if type is a std::vector
template <typename T>
class is_vector<std::vector<T>> : std::true_type
{
};

/// @brief returns true if type is a std::vector
template <typename T>
constexpr auto is_vector_v = is_vector<T>{};

template <typename T>
class AutoSerializable : public Serializable
{
public:
   using Self = T;

   /// @brief serializes this to a json object
   void serializeTo(JsonObject& document) const override;

   /// @brief deserializes this from a json object
   void deserialize(const JsonObject& document) override;

   /// @brief static version of the deserialize method
   static T Deserialize(const JsonObject& document);

protected:

   /// @brief sets up all the members of the class
   template <typename FM, typename ... RM>
   void setup(FM&& fm, RM&& ... rm);

private:
   std::vector<std::function<void(JsonObject&, const T*)>> m_serializers;
   std::vector<std::function<void(const JsonObject&, T*)>> m_deserializers;
};

template <typename T>
template <typename FM, typename ... RM>
void AutoSerializable<T>::setup(FM&& fm, RM&& ... rm)
{
   m_serializers.push_back(
      [fm](JsonObject& doc, const T* _this)
      {
         auto func = std::get<0>(fm);
         auto name = std::get<0>(fm);
         
         using type_t = std::decay_t<decltype(std::invoke(func, _this))>;

         if (std::is_base_of_v<type_t, Serializable>)
         {
            JsonObject tmp;
            std::invoke(func, _this).serializeTo(tmp);
            doc[name] = tmp;
         }
         else if (is_vector_v<type_t>)
         {
            std::vector<JsonObject> tmp;
            for (auto& item: std::invoke(func, _this))
            {
               if (std::is_base_of_v<Serializable, type_t::value_type>)
               {
                  JsonObject tmp2;
                  item.serializeTo(tmp2);
                  tmp.push_back(tmp2);
               }
               else
               {
                  tmp.push_back(item);
               }
               doc[name] = tmp;
            }
         }
         else
         {
            doc[name] = std::invoke(func, _this);
         }
      }
   );

   m_deserializers.push_back(
      [fm](const JsonObject& doc, T* _this)
      {
         auto func = std::get<1>(fm);
         auto name = std::get<1>(fm);
         
         using type_t = std::remove_const_t<std::remove_reference_t<decltype(std::invoke(func, _this))>>;
         if (std::is_base_of_v<Serializable, type_t>)
         {
            std::invoke(func, _this).deserialize(doc[name]);
         }
         else if (is_vector_v<type_t>)
         {
            std::invoke(func, _this).clear();

            for (auto& item: doc[name].getArray())
            {
               if constexpr (std::is_base_of_v<Serializable, typename type_t::value_type>)
               {
                  std::invoke(func, _this).push_back(typename type_t::value_type());
                  std::invoke(func, _this).back().deserialize(item);
               }
               else
               {
                  std::invoke(func, _this).push_back(item.template get<typename type_t::value_type>());
               }
            }
         }
         else
         {
            std::invoke(func, _this) = doc[name].template get<type_t>();
         }
      }
   );

   if (sizeof...(RM) > 0)
   {
      setup(rm);
   }
}

template <typename T>
void AutoSerializable<T>::serializeTo(JsonObject& document) const
{
   for (auto& serializer: m_serializers)
   {
      serializer(document, static_cast<T*>(this));
   }
}

template <typename T>
void AutoSerializable<T>::deserialize(const JsonObject& document)
{
   for (auto& deserializer: m_deserializers)
   {
      deserializer(document, static_cast<T*>(this));
   }
}

template <typename T>
T AutoSerializable<T>::Deserialize(const JsonObject& document)
{
   T tmp;
   tmp.deserialize(document);
   return tmp;
}

#define MEMBER(NAME) std::make_tuple(std::string(NAME), Self::NAME)
