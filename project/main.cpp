#include <iostream>
#include <cassert>
#include <utility>
#include "Serialization/JsonObject.h"
#include "Serialization/AutoSerializable.h"

class InnerClass : public AutoSerializable<InnerClass>
{
public:
   InnerClass(std::vector<std::string> names = {}, bool value = {})
      : m_stringList(std::move(names))
        , m_bool(value)
   {
      setup(MEMBER(m_stringList),
            MEMBER(m_bool));
   }

   std::vector<std::string> m_stringList;
   bool m_bool;
};

class OuterClass : public AutoSerializable<OuterClass>
{
public:
   OuterClass(InnerClass test = {}, std::vector<InnerClass> tests = {})
      : m_innerClass(std::move(test))
        , m_innerClassList(std::move(tests))
   {
      setup(MEMBER(m_innerClass),
            MEMBER(m_innerClassList));
   }

   InnerClass m_innerClass;
   std::vector<InnerClass> m_innerClassList;
};

int main()
{
   OuterClass test;
   test.m_innerClass.m_stringList = {"Hello", "1"};
   test.m_innerClass.m_bool = true;
   test.m_innerClassList.push_back({{"World", "2"}, false});
   test.m_innerClassList.push_back({{"I <3 templates", "3"}, true});

   auto document = test.serialize();
   std::cout << document.toString() << std::endl;

   OuterClass test2 = OuterClass::Deserialize(JsonObject::fromString("\n\t" + document.toString() + "\n  \r\t"));
   auto document2 = test2.serialize();
   std::cout << document2.toString() << std::endl;

   assert(test == test2);
   assert(document.toString() == document2.toString());
   return EXIT_SUCCESS;
}
