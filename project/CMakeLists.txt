project(project)
add_executable(project main.cpp Serialization/Serializable.h Serialization/JsonObject.cpp Serialization/JsonObject.h Serialization/AutoSerializable.h Serialization/Serializable.cpp)