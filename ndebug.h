#ifndef DEBUG_H
#define DEBUG_H

enum Debug_Levels {
  APPLICATION,
  FRAMEWORK,
  LOW_LEVEL
};

class Debug
{
  Stream* stream = NULL;

public:
  Debug_Levels level = APPLICATION;

  void setDebug(Stream* serial)
  {
      stream = serial;
  }

  void log(char* msg)
  {
    if(stream!=NULL) stream->println(msg);
  }

  void log2(char* msg)
  {
    if(stream!=NULL && level<=FRAMEWORK) stream->println(msg);
  }

  void log3(char* msg)
  {
    if(stream!=NULL && level<=LOW_LEVEL) stream->println(msg);
  }

};

Debug debug;

#endif
