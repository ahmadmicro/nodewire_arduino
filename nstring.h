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
      bool should_dispose = false;

      // array properties
      int len = 0;
      int capacity = 0;
      nString** elements = NULL;

      // object properties
      nString* keys = NULL;

      nString(){
        should_dispose = false;
        theBuf = NULL;
        size = 0;
        type = n_String;
      }

      ~nString()
      {
        //Serial.print("dispose:");
        //Serial.print(theBuf);
        if(should_dispose==true && theBuf!=NULL) {
          free(theBuf);
          //Serial.println("->freed");
        }
        //Serial.println();

        if(elements!=NULL)
        {
          for(int i=0;i<len;i++) delete elements[i];
        }
      }

      nString(const char* buff)
      {
        theBuf = strdup(buff);
        size = strlen(buff)+1;
        should_dispose = true;
      }

      nString (const nString& op)
      {
          theBuf = op.theBuf;
          size =  strlen(theBuf)+1;
          should_dispose = false;
          type = n_String;

          // todo: handle reference counting and non n_String types
      }

      nString(char* buff,  int len)
      {
        theBuf = buff;
        size = len;
        should_dispose = false;
      }

      void setBuffer(char* buff,  int len)
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
        if(strlen(op.theBuf)<size)
        {
          strcpy(theBuf, op.theBuf);
          type = n_String;
        }
        return *this;
      }

      nString operator=(const char* op)
      {
        if(theBuf==NULL)
        {
          theBuf = strdup(op);
          size = strlen(op)+1;
          should_dispose = true;
        }
        else
        {
          strcpy(theBuf, op);
        }

        type = n_String;
        return *this;
      }

      nString operator=(int op)
      {
        if(theBuf==NULL)
          theBuf = "0123456789";
        itoa(op, theBuf, 10);
        type = n_Int;
        return *this;
      }
      nString operator=(double op)
      {
        if(theBuf==NULL)
          theBuf = "0123456789";
        dtostrf(op, 6, 2, theBuf);
        type = n_Float;
        return *this;
      }

      nString operator+=(const nString& op)
      {
        strcat(theBuf, op.theBuf);
        type = n_String;
        return *this;
      }
      nString operator+=(const char* op)
      {
        strcat(theBuf, op);
        type = n_String;
        return *this;
      }
      nString operator+=(const int op)
      {
        char temp[10]; itoa(op, temp, 10);
        strcat(theBuf, temp);
        type = n_String;
        return *this;
      }
      nString operator+=(const double op)
      {
        char temp[10]; dtostrf(op, 6, 2, temp);
        strcat(theBuf, temp);
        return *this;
      }

      nString operator+(const nString& op)
      {
        strcat(theBuf, op.theBuf);
        return *this;
      }
      nString operator+(const char* op)
      {
        strcat(theBuf, op);
        return *this;
      }
      nString operator+(const int op)
      {
        char temp[10]; itoa(op, temp, 10);
        strcat(theBuf, temp);
        return *this;
      }
      nString operator+(const double op)
      {
        char temp[10]; dtostrf(op, 9, 2, temp);
        strcat(theBuf, temp);
        return *this;
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


      nString& substring(nString sub)
      {
          int n = strlen(sub.theBuf);
          int i;
          for(i=0;i<=size;i++)
          {
            if(strncmp(sub.theBuf, theBuf+i, n)==0)
            {
              return this->tail(i);
            }
          }

          return this->tail(strlen(theBuf));
      }

      nString& last_substring(nString sub)
      {
        int result;
        int n = strlen(sub.theBuf);
        int i;
        for(i=0;i<=size;i++)
        {
          if(strncmp(sub.theBuf, theBuf+i, n)==0) result=i;
        }
        return this->tail(result);
      }

      nString& tail(int i)
      {
        if(type!=n_Array && type!=n_Object)
        {
          if(capacity==0)
          {
            elements = new nString*[1];
            capacity = 1; len = 1;
          }
          elements[0] = new nString(theBuf+i, size-i);
          return *elements[0];
        }
        else
           return *this;
      }

      nString& head(int i)
      {
        if(type!=n_Array && type!=n_Object)
        {
          if(capacity==0)
          {
            elements = new nString*[1];
            capacity = 1; len = 1;
          }
          elements[0] = new nString(theBuf, i);
          return *elements[0];
        }
        else
           return *this;
      }

      nString& operator[](int j)
      {
        if(j>=0 && j<len)
          return *elements[j];
        return *this;
      }

      void create_array(int Capacity)
      {
        delete_elements();
        capacity = Capacity;
        elements = new nString*[capacity];
        type = n_Array;
      }

      void create_object(nString Keys)
      {
        delete_elements();
        keys = new nString(new char[Keys.size], Keys.size);
        *keys = Keys;
        capacity = keys->split(' ');
        elements = new nString*[capacity];

        int ss = size/capacity;
        for(int i=0;i<capacity;i++) elements[i]=new nString(theBuf+(ss*i),ss);

        type = n_Object;
      }

      nString& operator[](nString k)
      {
        int i = keys->find(k);
        if(i!=-1)
          return *elements[i];
        return *this;
      }

      void append(nString item)
      {
        if(len<capacity)
        {
          if(len==0)
             elements[len] = new nString(theBuf, item.size);
          else
             elements[len] = new nString(elements[len-1]->theBuf+elements[len-1]->size, item.size);
          *elements[len] = item;
          len++;
        }
      }

      void pop()
      {
        if(len!=0)
        {
          delete elements[len-1];
          elements[len-1] = NULL;
          len--;
        }
      }

      int find(nString item)
      {
        for(int i=0;i<len;i++)
        {
          if((*this)[i]==item) return i;
        }
        return -1;
      }

      void delete_elements()
      {
        if(capacity!=0 && elements!=NULL)
        {
          //Serial.print("deleting elements:");
          //Serial.println(len);
          for(int i=0;i<len;i++)
          {
            delete elements[i];
            elements[i] = NULL;
          }

          capacity = 0;
          len = 0;
        }
      }

      int split(char c)
      {
        delete_elements();
        len=1;
        int l = strlen(theBuf);
        //Serial.print("len:");Serial.println(strlen(theBuf));
        for(int i=0; i<l; i++)  if(theBuf[i]==c) len++;

        if(capacity<len)
        {
          delete elements;
          elements = new nString*[len];
          capacity = len;
        }

        int j = 0;
        int next_element = 0;

        for(int i=0; i<l; i++)
        {
          if(theBuf[i]==c)
          {
            theBuf[i] = 0;
            elements[j] = new nString(theBuf + next_element, i - next_element);
            next_element = i+1;
            j++;
          }
        }
        elements[j] = new nString(theBuf + next_element, size - next_element);
        type = n_Array;
        return len;
      }

      void join(char separator)
      {
        if(type==n_Array)
        {
          int cc = 1;
          for(int i=0;i<size;i++)
          {
            if(theBuf[i]==0) { theBuf[i]= separator; cc++;}
            if(cc==len) break;
          }
          delete_elements();
          type = n_String;
        }
      }

      int parseCSV()
      {
        delete_elements();
        len=1;
        int l = strlen(theBuf);
        bool suspend = false;

        for(int i=0; i<l; i++)
        {
            if(theBuf[i]=='\"') suspend = !suspend;
            if(!suspend && theBuf[i]==',')
            {
                len++;
            }
        }

        suspend = false;

        if(capacity<len)
        {
          delete elements;
          elements = new nString*[len];
          capacity = len;
        }

        int j = 0;
        int next_element = 0;


        for(int i=0; i<l; i++)
        {
            if(theBuf[i]=='\"') suspend = !suspend;
            if(!suspend && theBuf[i]==',')
            {
              theBuf[i] = 0;
              elements[j] = new nString(theBuf + next_element, i - next_element);
              next_element = i+1;
              j++;
            }
        }
        elements[j] = new nString(theBuf + next_element, size - next_element);
        type = n_Array;
        return len;
      }

      int parsePT()
      {
        return 0;
      }
};

#endif
