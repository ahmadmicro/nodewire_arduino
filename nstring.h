#ifndef NSTRING_H
#define NSTRING_H

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

class nString{
  private:
  public:
      char* theBuf = NULL;
      char altBuf[10];

      nString(){}
      nString(char* buff)
      {
        theBuf = buff;
      }
      nString(double buff)
      {
        theBuf = altBuf;
        dtostrf(buff, 6, 2, theBuf);
      }

      void clearBuffer(int size)
      {
        memset(theBuf, '\0', size);
      }

      nString operator=(const nString& op)
      {
        if(theBuf==NULL)
          theBuf = altBuf;
        else
          strcpy(theBuf, op.theBuf);
      }
      nString operator=(char* op)
      {
        if(theBuf==NULL)
          theBuf = altBuf;
        else
          strcpy(theBuf, op);
      }
      nString operator=(int op)
      {
        if(theBuf==NULL)
          theBuf = altBuf;
        itoa(op, theBuf, 10);
      }
      nString operator=(double op)
      {
        if(theBuf==NULL)
          theBuf = altBuf;
        dtostrf(op, 6, 2, theBuf);
      }

      nString operator+=(const nString& op)
      {
        strcat(theBuf, op.theBuf);
      }
      nString operator+=(const char* op)
      {
        strcat(theBuf, op);
      }
      nString operator+=(const int op)
      {
        char temp[10]; itoa(op, temp, 10);
        strcat(theBuf, temp);
      }
      nString operator+=(const double op)
      {
        char temp[10]; dtostrf(op, 6, 2, temp);
        strcat(theBuf, temp);
      }

      nString operator+(const nString& op)
      {
        nString nw(theBuf);
        strcat(theBuf, op.theBuf);
        return nw;
      }
      nString operator+(const char* op)
      {
        nString nw(theBuf);
        strcat(theBuf, op);
        return nw;
      }
      nString operator+(const int op)
      {
        nString nw(theBuf);
        char temp[10]; itoa(op, temp, 10);
        strcat(theBuf, temp);
        return nw;
      }
      nString operator+(const double op)
      {
        nString nw(theBuf);
        char temp[10]; dtostrf(op, 9, 2, temp);
        strcat(theBuf, temp);
        return nw;
      }

      bool operator==(nString op)
      {
        if(strcmp(theBuf, op.theBuf)==0) return true; else return false;
      }
      bool operator==(char* op)
      {
        if(strcmp(theBuf, op)==0) return true; else return false;
      }

      bool operator!=(nString op)
      {
        if(strcmp(theBuf, op.theBuf)==0) return false; else return true;
      }
      bool operator!=(char* op)
      {
        if(strcmp(theBuf, op)==0) return false; else return true;
      }
      /*operator char*()
      {
        return theBuf;
      }*/


      nString find(nString sub)
      {
          for(int i=0;i<=strlen(theBuf);i++)
          {
            int n = strlen(sub.theBuf);
            if(strncmp(sub.theBuf, theBuf+i, n)==0) return nString(theBuf+i);
          }

          return nString(theBuf+strlen(theBuf));
      }

      nString findlast(nString sub)
      {
        int result;
        for(int i=0;i<=strlen(theBuf);i++)
        {
          int n = strlen(sub.theBuf);
          if(strncmp(sub.theBuf, theBuf+i, n)==0) result=i;
        }
        return nString(theBuf+result);
      }

      nString index(int i)
      {
        return nString(theBuf+i);
      }

};

#endif
