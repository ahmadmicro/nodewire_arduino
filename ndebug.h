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

  void setOutput(Stream* serial)
  {
      stream = serial;
  }

  void log(const char* msg)
  {
    if(stream!=NULL) stream->println(msg);
  }

  void log2(const char* msg)
  {
    if(stream!=NULL && (level==FRAMEWORK||level==LOW_LEVEL)) stream->println(msg);
  }


  void log3(const char* msg)
  {
    if(stream!=NULL && level==LOW_LEVEL) stream->println(msg);
  }

};

Debug debug;

#endif
