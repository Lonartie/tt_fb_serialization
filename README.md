# Trainee task - find bug - serialization
This is a hard one!</br>
You need to find the bugs which are well hidden in plain sight.</br>
As always, make as few changes as possible.</br>
Keep an eye on you style and memory issues.</br>

### Background:
This repository contains some code for automatic serialization of objects.</br>
It uses a custom (incomplete) ```json``` serializer.</br>
The ```AutoSerializable.h``` file is made out of mostly ```templated``` methods.</br>

### Requirements: <br/>
* You are allowed to modify all files except for the main.cpp file

### Expected output: <br/>
```json
{"m_innerClass":{"m_bool":true,"m_stringList":["Hello","1"]},"m_innerClassList":[{"m_bool":false,"m_stringList":["World","2"]},{"m_bool":true,"m_stringList":["I <3 templates","3"]}]}
{"m_innerClass":{"m_bool":true,"m_stringList":["Hello","1"]},"m_innerClassList":[{"m_bool":false,"m_stringList":["World","2"]},{"m_bool":true,"m_stringList":["I <3 templates","3"]}]}
```
(Needless to say that it should not crash)</br>
#### Happy coding!