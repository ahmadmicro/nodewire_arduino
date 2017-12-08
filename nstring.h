#ifndef NSTRING_H
#define NSTRING_H

#if defined (__STM32F1__)
#include <itoa.h>
#endif

#if  defined (STM32_HIGH_DENSITY)
#include <itoa.h>
#include <avr/dtostrf.h>
#endif

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#define n_Object 1
#define n_Array 2
#define n_Int 3
#define n_Float 4
#define n_String 5

class nString{
  private:
  public:
      //common properties
      char* theBuf = NULL;
      int size = 0;
      int type = n_String;

      // array properties
      int len = 0;
      int capacity = 0;

      nString(){}
      nString(char* buff)
      {
        theBuf = buff;
        size = strlen(buff)+1;
      }

      nString(char* buff,  int len)
      {
        theBuf = buff;
        size = len;
      }

      nString(double buff)
      {
        theBuf = "char[10]  ";
        dtostrf(buff, 6, 2, theBuf);
        type = n_Float;
        size =  strlen(theBuf)+1;
      }

      void clearBuffer()
      {
        memset(theBuf, '\0', size);
      }

      nString operator=(const nString& op)
      {
        if(theBuf==NULL)
        {
          theBuf = op.theBuf;
          size =  strlen(theBuf)+1;
        }
        else if(strlen(op.theBuf)<size)
          strcpy(theBuf, op.theBuf);

        type = n_String;
      }
      nString operator=(char* op)
      {
        if(theBuf==NULL)
        {
          theBuf = op;
        }
        else
        {
          strcpy(theBuf, op);
        }

        type = n_String;
      }
      nString operator=(int op)
      {
        if(theBuf==NULL)
          theBuf = "0123456789";
        itoa(op, theBuf, 10);
        type = n_Int;
      }
      nString operator=(double op)
      {
        if(theBuf==NULL)
          theBuf = "0123456789";
        dtostrf(op, 6, 2, theBuf);
        type = n_Float;
      }

      nString operator+=(const nString& op)
      {
        strcat(theBuf, op.theBuf);
        type = n_String;
      }
      nString operator+=(const char* op)
      {
        strcat(theBuf, op);
        type = n_String;
      }
      nString operator+=(const int op)
      {
        char temp[10]; itoa(op, temp, 10);
        strcat(theBuf, temp);
        type = n_Int;
      }
      nString operator+=(const double op)
      {
        char temp[10]; dtostrf(op, 6, 2, temp);
        strcat(theBuf, temp);
        type = n_Float;
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

      explicit operator char*()
      {
        return theBuf;
      }

      explicit operator int()
      {
        return atoi(theBuf);
      }

      explicit operator double()
      {
        return atof(theBuf);
      }


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

      nString operator[](int j)
      {
        int i=0; int k=1;
        if(j==0) return nString(theBuf);
        while(true)
        {
          if(theBuf[i]==0)
          {
             if(k==j) return nString(theBuf+i+1);
             k++;
          }
          i++;
        }
        return nString(theBuf+i);
      }

      int find(nString item, int count)
      {
        for(int i=0;i<count;i++)
        {
          if((*this)[i]==item) return i;
        }
        return -1;
      }

      int split(char c)
      {
        int j=1;
        int len = strlen(theBuf);
        for(int i=0; i<len; i++)
        {
          if(theBuf[i]==c)
          {
            theBuf[i] = 0; j++;
          }
        }
        type = n_Array;
        len = j;
        return j;
      }

      int parseCSV()
      {
        // "Ahmad","Sadiq","Senior Manager"
        int j=1;
        int len = strlen(theBuf);
        bool suspend = false;

        for(int i=0; i<len; i++)
        {
            if(theBuf[i]=='\"') suspend = !suspend;
            if(!suspend && theBuf[i]==',')
            {
              theBuf[i] = 0; j++;
            }
        }
        type = n_Object;
        return j;
      }

      int parsePT()
      {
        return 0;
      }

};


#endif
