#include "Serializable.h"

bool Serializable::operator==(const Serializable& other)
{
   JsonObject docA, docB;
   this->serializeTo(docA);
   other.serializeTo(docB);
   return docA.toString() == docB.toString();
}

bool Serializable::operator!=(const Serializable& other)
{
   return !(*this == other);
}

JsonObject Serializable::serialize() const
{
   JsonObject doc;
   serializeTo(doc);
   return doc;
}
