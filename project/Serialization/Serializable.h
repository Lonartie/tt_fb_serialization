#pragma once

#include "JsonObject.h"
#include <ostream>

/// @brief Interface for serializable classes
class Serializable
{
public:

   /// @brief serialize method to serializeTo data to a document
   virtual void serializeTo(JsonObject& document) const = 0;
   JsonObject serialize() const;

   /// @brief deserialize method to deserialize data from a document
   virtual void deserialize(const JsonObject& document) = 0;

   bool operator==(const Serializable& other);

   bool operator!=(const Serializable& other);
};

namespace std
{
   inline std::ostream& operator<<(std::ostream& stream, const Serializable& serializable)
   {
      JsonObject document;
      serializable.serializeTo(document);
      stream << document.toString();
      return stream;
   }

   inline std::istream& operator>>(std::istream& stream, Serializable& serializable)
   {
      std::string doc;
      stream >> doc;
      JsonObject document = JsonObject::fromString(doc);
      serializable.deserialize(document);
      return stream;
   }
}
