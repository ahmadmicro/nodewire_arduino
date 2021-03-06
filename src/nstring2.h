#ifndef NSTRING_H
#define NSTRING_H

#include <ndebug.h>

#if defined (__STM32F1__)
#include <itoa.h>
#include <avr/dtostrf.h>
#endif

#if defined (STM32_HIGH_DENSITY)
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

/*
Todo:
1. operator == and operator != for Array and object
2. a predefined blank nString to be returned for null values, e.g. when index not found

*/
class nString{
  private:
  public:
      //common properties
      char* theBuf = NULL;
      int size = 0;
      int type = n_String;
      bool should_dispose = false;
      int precision = 2;
      int size_double = 15;


      // array properties
      int len = 0;
      int capacity = 0;
      nString** elements = NULL;

      // object properties
      nString* keys = NULL;

      nString(){
        should_dispose = true;
        theBuf = NULL;
        size = 0;
        capacity = 0;
        len = 0;
        elements = NULL;
        keys = NULL;
        type = n_String;
      }

      ~nString()
      {
        dispose();
      }

      void dispose()
      {
        if(should_dispose==true && theBuf!=NULL) {
          delete[] theBuf;
          theBuf = NULL;
        }
        if(elements!=NULL && len!=0)
        {
          for(int i=0;i<len;i++)
          {
            delete elements[i];
            elements[i] =  NULL;
          }
          delete[] elements;
          elements = NULL;
        }

        if(type == n_Object && keys!=NULL)
        {
            if(!keys->should_dispose)
                delete[] keys->theBuf;
            delete keys;
            keys = NULL;
        }
      }

      nString(const char* buff)
      {
        size = strlen(buff)+1;
        theBuf = new char[size];
        strncpy(theBuf, buff, size);
        should_dispose = true;

        capacity = 0;
        len = 0;
        elements = NULL;
        keys = NULL;
        type = n_String;
      }

      nString (const nString& op)
      {
          theBuf = new char[op.size];
          size = op.size;
          type = n_String;
          should_dispose = true;

          capacity = 0;
          len = 0;
          elements = NULL;
          keys = NULL;

          copy(op);
      }

      void copy(const nString& op)
      {
        type = op.type;
        if(op.type==n_String || op.type==n_Float || op.type==n_Int)
        {
            if(op.size<=size)
            {
              memcpy(theBuf,  op.theBuf, op.size);
            }
            else if(!should_dispose)
            {
              //Serial.print("truncated copy:");
              Serial.println(op.theBuf);
              memcpy(theBuf,  op.theBuf, size-1);  
            }
            else if(size!=0)
            {
                char* temp = new char[op.size];
                memcpy(temp, op.theBuf, op.size);
                delete[] theBuf;
                theBuf = temp;
                size = op.size;
            }
            else if(should_dispose)
            {
                theBuf =  new char[op.size];
                size = op.size;
                memcpy(theBuf,  op.theBuf, op.size);
            }
            else
                Serial.println("can't copy");
            theBuf[size-1]=0; //todo this is the problem?
        }
        else if(op.elements!=NULL)
        {
            delete_elements();
            if(op.size<=size)
            {
              memcpy(theBuf, op.theBuf, op.size);
              if(op.size!=size)
                 memset(theBuf+op.size, '\0' ,size-op.size);
            }
            else
            {
              memcpy(theBuf, op.theBuf, size);
              //Serial.println("copy object truncated");
            }
            elements = new nString*[op.capacity];
            len = op.len;
            capacity = op.capacity;
            for(int i=0;i<len;i++)
            {
              if(i==0)
                elements[i] = new nString(theBuf, op.elements[i]->size);
              else
                elements[i] = new nString(elements[i-1]->theBuf+op.elements[i-1]->size, op.elements[i]->size);
              elements[i]->type = op.elements[i]->type;
              *elements[i] = *op.elements[i];
            }
            if(op.keys!=NULL)
            {
              char* temp = new char[op.keys->size];
              keys = new nString(temp, op.keys->size);
              *keys = *op.keys;
              keys->type=n_Array;
              keys->len=op.keys->len;
              keys->capacity=op.keys->capacity;
            }

            type = op.type;
        }
        else
        {
          Serial.print("Failed to assign 2:");
          Serial.println(op.theBuf);
          type = 0;
        }
      }

      nString(char* buff,  int len)
      {
        theBuf = buff;
        size = len;
        should_dispose = false;

        capacity = 0;
        len = 0;
        elements = NULL;
        keys = NULL;
        type = n_String;
      }

      void setBuffer(char* buff,  int len)
      {
        if(theBuf==NULL) // Not tested
        {
          theBuf = buff;
          size = len;
          should_dispose = false;
        }
      }

      bool createBuffer(int l)
      {
        if(theBuf==NULL) 
        {
          theBuf = new char[l];
          size = l;
          type = n_String;
          should_dispose = true;
          clearBuffer();

          capacity = 0;
          len = 0;
          elements = NULL;
          keys = NULL;
          return true;
        }
        return false;
      }

      nString(double buff)
      {
        theBuf = new char[size_double];
        should_dispose = true;
        size =  size_double;
        dtostrf(buff, size-1, precision, theBuf);
        type = n_Float;

        capacity = 0;
        len = 0;
        elements = NULL;
        keys = NULL;
      }

      nString(int op)
      {
        theBuf = new char[10];
        size = 10;
        clearBuffer();
        should_dispose = true;
        *this = op;

        capacity = 0;
        len = 0;
        elements = NULL;
        keys = NULL;
        type = n_Int;
      }

      nString(long op)
      {
        theBuf = new char[15];
        size = 15;
        clearBuffer();
        should_dispose = true;
        *this = op;

        capacity = 0;
        len = 0;
        elements = NULL;
        keys = NULL;
        type = n_Int;
      }

      void clearBuffer()
      {
        memset(theBuf, '\0', size);
      }

      void fillBuffer(char c) {
        memset(theBuf, c, size);
      }

      nString& operator=(const nString& op)
      {
        if(this!=&op)
        {
          if(theBuf==NULL)
          {
            theBuf = new char[op.size];
            size= op.size;
            should_dispose = true;
          }
          copy(op);
        }
        return *this;
      }

      nString& operator=(const char* op)
      {
        if(theBuf==NULL)
        {
          theBuf = new char[strlen(op)+1];
          strcpy(theBuf,op);
          size = strlen(op)+1;
          should_dispose = true;
        }
        else
        {
          copy(op);
          //strcpy(theBuf, op);
        }

        type = n_String;
        return *this;
      }

      nString operator=(int op)
      {
        if(theBuf==NULL)
        {
          //Serial.println("EMPTY");
          theBuf = new char[10];
          should_dispose = true;
          size = 10;
          clearBuffer();
        }

        itoa(op, theBuf, 10);
        type = n_Int;
        return *this;
      }

      nString operator=(long op)
      {
        if(theBuf==NULL)
        {
          theBuf = new char[15];
          should_dispose = true;
          size = 15;
          clearBuffer();
        }

        ltoa(op, theBuf, 10);
        type = n_Int;
        return *this;
      }

      nString& operator=(double op)
      {
        if(theBuf==NULL)
        {
          theBuf = new char[size_double];
          should_dispose = true;
          size = size_double;
          clearBuffer();
        }
        dtostrf(op, size-1, precision, theBuf);
        trim();
        type = n_Float;
        return *this;
      }

      nString& operator+=(const nString& op)
      {
        if(size>(strlen(theBuf)+strlen(op.theBuf)))
        {
           strcat(theBuf, op.theBuf);
           type = op.type;
        }
        else if(should_dispose)
        {
          char* temp = new char[strlen(theBuf)+strlen(op.theBuf)+1];
          strcpy(temp, theBuf);
          strcat(temp, op.theBuf);
          delete[] theBuf;
          theBuf = temp;
          size = strlen(theBuf)+1;
          type = op.type;
        }
        return *this;
      }
      nString operator+=(const char* op)
      {
        if(size>strlen(theBuf)+strlen(op))
        {
          strcat(theBuf, op);
        }
        else if(should_dispose)
        {
          char* temp = new char[strlen(theBuf)+strlen(op)+1];
          strcpy(temp, theBuf);
          strcat(temp, op);
          delete[] theBuf;
          theBuf = temp;
          size = strlen(theBuf)+1;
        }
        type = n_String;
        return *this;
      }
      nString operator+=(const int op)
      {
        int lenght = 4;
        if (op>5000) lenght = 10;
        if(size<strlen(theBuf)+lenght)
        {
          if(should_dispose)
          {
            size = strlen(theBuf)+lenght;
            char* temp1 = new char[size];
            strcpy(temp1, theBuf);
            char temp[lenght]; itoa(op, temp, lenght);
            strcat(temp1, temp);
            delete[] theBuf;
            theBuf = temp1;
          }
          else
          {
            /* not added */
          }
          
        }
        else
        {
          char temp[10]; itoa(op, temp, 10);
          strcat(theBuf, temp);
          type = n_String;
        }
        
        return *this;
      }
      
      nString operator+=(const double op)
      {
        if(size<strlen(theBuf)+size_double)
        {
          if(should_dispose)
          {
            size = strlen(theBuf)+size_double;
            char* temp1 = new char[size];
            strcpy(temp1, theBuf);
            char temp[size_double]; dtostrf(op, size-1, precision, temp1);
            strcat(temp1, temp);
            delete[] theBuf;
            theBuf = temp1;
          }
          else
          {
            /* not added */
          }
        }
        else
        {
          char temp[size_double]; dtostrf(op, size-1, precision, temp);
          strcat(theBuf, temp);
        }

        return *this;
      }

      friend nString operator+(nString lhs, const nString& op)
      {
        lhs+=op;
        return lhs;
      }

      nString operator+(const char* op)
      {
        //strcat(theBuf, op);
        *this+=op;
        return *this;
      }

      nString operator+(const int op)
      {
        *this+=op;
        return *this;
      }

      nString operator+(const double op)
      {
        *this+=op;
        return *this;
      }

      bool operator==(nString op)
      {
        if(strcmp(theBuf, op.theBuf)==0) return true; else return false;
      }
      bool operator==(const char* op)
      {
        if(strcmp(theBuf, op)==0 && type!=n_Array && type!=n_Object) return true; else return false;
      }

      bool operator!=(nString op)
      {
        if(strcmp(theBuf, op.theBuf)==0) return false; else return true;
      }

      bool operator!=(const char* op)
      {
        if(type==n_Object || type==n_Array) return true;
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

      explicit operator long()
      {
        return atol(theBuf);
      }

      explicit operator double()
      {
        return atof(theBuf);
      }


      int index(nString sub)
      {
          int n = strlen(sub.theBuf);
          int i;
          size_t ss = strlen(theBuf);
          for(i=0;i<=ss;i++)
          {
            if(strncmp(sub.theBuf, theBuf+i, n)==0)
            {
              return i;
            }
          }

          return -1;
      }

      int last_index(nString sub)
      {
          int result=-1;
          int n = strlen(sub.theBuf);
          int i=-1;
          for(i=0;i<=size;i++)
          {
              if(strncmp(sub.theBuf, theBuf+i, n)==0) result=i;
          }
          return result;
      }

     void trim()
     {
        int pos = 0;
        while(isspace(theBuf[pos]) && pos<size) pos++;
        if(pos!=0)
            *this = (theBuf+pos);
        int end = strlen(theBuf)-1;
        while(isspace(theBuf[end]) && end!=pos) 
        {
          theBuf[end] = '\0';
          end--;
        }
     }

      nString& tail(int i)
      {
        if(type!=n_Array && type!=n_Object)
        {
          if(capacity==0)
          {
            elements = new nString*[2];
            capacity = 2; len = 2;
          }
          elements[0] = new nString(theBuf, i+1);
          elements[1] = new nString(theBuf+i+1, size-(i+1));
          theBuf[i] = '\0';
          type = n_Array;
          return *elements[1];
        }
        else
        {
            if(len!=0)
                return *elements[1];
            else
                Serial.println("error");
        }
        return *elements[1];
      }

      nString& head(int i)
      {
        if(type!=n_Array && type!=n_Object)
        {
          if(capacity==0)
          {
            elements = new nString*[2];
            capacity = 2; len = 2;
          }
          elements[0] = new nString(theBuf, i+1);
          elements[1] = new nString(theBuf+i+1, size-(i+1));
          theBuf[i] = '\0';
          type = n_Array;
          return *elements[0];
        }
        else
        {
            if(len!=0)
                return *elements[1];
            else
                Serial.println("error");
        }
        return *elements[0];
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

      void create_object(nString dKeys)
      {
        if(theBuf==NULL) createBuffer(30);
        delete_elements();
        char* temp = new char[dKeys.size];
        keys = new nString(temp, dKeys.size);
        *keys = dKeys;
        capacity = keys->split(' ');
        len = capacity;
        elements = new nString*[capacity];

        int ss = size/capacity;
        for(int i=0;i<capacity;i++) elements[i]=new nString(theBuf+(ss*i),ss);

        type = n_Object;
      }

      void convert_object(nString dKeys)
      {
        if(type==n_Array)
        {
          if(keys==NULL)
          {
            char* temp = new char[dKeys.size];
            keys = new nString(temp, dKeys.size);
          }
          //memset(keys->theBuf, '\0', keys->size);
          *keys = dKeys;
          //keys->theBuf[dKeys.size-1] = 0;
          len = keys->split(' ');
          if(len>capacity) {
              debug.log2("object can't fit into existing array!");
            //Serial.println("E DON HAPPEN!");
            //resize(len);
          }
          //len = capacity;

          type = n_Object;
        }
        else
        {
            debug.log2("only arrays can be converted to object!");
        }
      }

      nString& operator[](nString k)
      {
        int i = keys->find(k);
        if(i!=-1 && i<len)
          return *elements[i];
        return *this;
      }

      void append(nString item)
      {
        if(len<capacity)
        {
          if(len==0)
             elements[len] = new nString(theBuf, item.size);
          else if(len+1==capacity)
          {
              elements[len] = new nString(elements[len-1]->theBuf+elements[len-1]->size, size-(elements[len-1]->theBuf-theBuf+elements[len-1]->size));
          }
          else
             elements[len] = new nString(elements[len-1]->theBuf+elements[len-1]->size, item.size);
          *elements[len] = item;
          len++;
        }
      }

      void pop(int location)
      {
          if(len!=0 && location>=0 && location<len)
          {
              if(len!=1)
              {
                  char* target = elements[location]->theBuf;
                  char* source = elements[location+1]->theBuf;

                  for(long i = 0; i<size-(target-theBuf);i++)
                      target[i] = source[i];
                  int cum = 0;
                  int ss = elements[location+1]->size;
                  for(int i=location+1;i<len-1;i++)
                  {
                    cum = cum + elements[i]->size - elements[i-1]->size;
                    elements[i]->theBuf += cum;
                  }

                  cum = 0;
                  for(int i=location+1;i<len-1;i++)  
                  {
                    cum = cum + elements[i+1]->size - elements[i]->size;
                    elements[i]->size = elements[i+1]->size;
                    elements[i]->type = elements[i+1]->type;
                    if(elements[i+1]->type == n_Array || elements[i+1]->type == n_Object)
                    {
                      elements[i]->len = elements[i+1]->len;
                      elements[i]->capacity = elements[i+1]->capacity;
                      elements[i]->elements = new nString*[elements[i]->capacity];
                      for(int j=0;j<elements[i+1]->len;j++)
                      {
                        elements[i]->elements[j] = elements[i+1]->elements[j];
                        elements[i]->elements[j]->theBuf = elements[i]->theBuf+(elements[i+1]->elements[j]->theBuf-elements[i+1]->theBuf)+elements[i+1]->size;
                        elements[i]->elements[j]->size = elements[i+1]->elements[j]->size;
                      }
                      elements[i+1]->elements = NULL;
                      if(elements[i+1]->type == n_Object)
                      {
                        elements[i]->keys = elements[i+1]->keys;
                        elements[i+1]->keys = NULL;
                      }
                    }
                  }
                  elements[location]->size = ss;
              }

              delete elements[len-1];
              elements[len-1] = NULL;
              len--;
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
      /*
 4   4   6     5     5   4
one two three four five six
one three four five six
*/
      int find(nString query)
      {
        nString item = query;
        int q = item.split('=');
        for(int i=0;i<len;i++)
        {
          if(q==1)
          {
            if((*this)[i]==item) return i;
          }
          else
          {
            if((*this)[i][item[0]]==item[1]) return i;
          }
        }
        return -1;
      }

      void delete_elements()
      {
        if(len!=0 && elements!=NULL)
        {
          for(int i=0;i<len;i++)
          {
            if(elements[i]!=NULL)
            {
              delete elements[i];
              elements[i] =  NULL;
            }
          }

          delete[] elements;
          elements = NULL;
          capacity = 0;
          len = 0;
        }
        if(type == n_Object && keys!=NULL)
        {
            if(!keys->should_dispose)
                delete[] keys->theBuf;
            if(keys!=NULL)
            {
                delete keys;
                keys = NULL;
            }
        }
      }

      int split(char c)
      {
        //delete_elements();
        if(size>0)
        {
          int ll=1;
          int l = strlen(theBuf);
          //Serial.print("len:");Serial.println(strlen(theBuf));
          for(int i=0; i<l; i++)  if(theBuf[i]==c) ll++;
          if(capacity<ll)
          {
            delete_elements();
            elements = new nString*[ll];
            capacity = ll;
            len = ll;
            for(int i=0; i<capacity; i++) elements[i] = NULL;
          }
          len = ll;

          int j = 0;
          int next_element = 0;

          for(int i=0; i<l; i++)
          {
            if(theBuf[i]==c)
            {
              theBuf[i] = 0;
              if(elements[j] == NULL)
                elements[j] = new nString(theBuf + next_element, i - next_element+1);
              else
              {
                elements[j]->theBuf = theBuf + next_element;
                elements[j]->size = i+1 - next_element;
              }
              next_element = i+1;
              j++;
            }
          }
          if(elements[j] == NULL)
            elements[j] = new nString(theBuf + next_element, size - next_element);
          else
          {
            elements[j]->theBuf = theBuf + next_element;
            elements[j]->size = size - next_element;
          }
          type = n_Array;
          return len;
        }
        return 0;
      }

      void join(char separator)
      {
        if(type==n_Array)
        {
          for(int i=0;i<len;i++)
          {
            int l = strlen(elements[i]->theBuf);
            if(i!=len-1)
              elements[i]->theBuf[strlen(elements[i]->theBuf)] = separator;

            for(int j=l+1;j<elements[i]->size;j++) elements[i]->theBuf[j] = 0;

          }
          delete_elements();
          int end = 0;
          for(int i = 0;i<size-1;i++) if(theBuf[i]=='\0') end = i;
          for(int i = 0;i<end-1;i++)
          {
            while(theBuf[i]=='\0')
            {
              for(int loc=i;loc<size-1;loc++)
              {
                  theBuf[loc] = theBuf[loc+1];
              }
              end--;
              if(i==end) break;
            }
          }
          type = n_String;
        }
      }

      void collapse()
      {
        delete_elements();
        type = n_String;
      }

      int splitCSV()
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
              elements[j] = new nString(theBuf + next_element, i - next_element + 1);
              elements[j]->trim();
              if(elements[j]->theBuf[0]=='\"')
                elements[j]->removeends();
              next_element = i+1;
              j++;
            }
        }
        elements[j] = new nString(theBuf + next_element, size - next_element);
        elements[j]->trim();
        if(elements[j]->theBuf[0]=='\"')
          elements[j]->removeends();
        type = n_Array;
        return len;
      }

      int splitPT(char sep=' ')
      {
        char separator='\0';
        //char opposite;
        bool looking = true;

        delete_elements();
        len = 1;
        int stop = 0;

        for(int i = 0;i<size;i++)
        {
          if(looking || (separator==sep && theBuf[i]!=sep))
          {
            if(theBuf[i]==sep) {
              theBuf[i] = '\0';
              len++;
              stop = i;
              //looking = false;
              separator = sep;
            }
            else if(theBuf[i]=='[')
            {
              separator = '['; looking = false;
            }
            else if(theBuf[i]=='{')
            {
              separator = '{'; looking = false;
            }
            else if(theBuf[i]=='\"')
            {
              separator = '\"'; looking = false;
            }
            else if(theBuf[i]=='\'')
            {
              separator = '\''; looking = false;
            }
          }
          else
          {
            if(separator==sep && theBuf[i]!=sep) looking = true;
            if(separator=='[' && theBuf[i]==']') looking = true;
            if(separator=='{' && theBuf[i]=='}') looking = true;
            if(separator=='\"' && theBuf[i]=='\"') looking = true;
            if(separator=='\'' && theBuf[i]=='\'') looking = true;
          }
        }

        elements = new nString*[len];
        capacity = len;
        int el = 1;
       int prev=0;
       for(int i=0;i<=stop;i++)
       {
           if(theBuf[i]=='\0')
           {
               elements[el-1] = new nString(theBuf + prev, i + 1 - prev);
               prev = i+1;
               i++;
               el++;
           }
       }
        elements[len-1] = new nString(theBuf + prev, size - prev);
        type = n_Array;
        return len;
      }

      void to_csv()
      {
          for(int i = 0; i<len-1;i++)
          {
              int start = strlen(elements[i]->theBuf);
              for(int j=start;j<elements[i]->size;j++)
                elements[i]->theBuf[j] = ' ';
              elements[i]->theBuf[elements[i]->size-1] = ',';
          }
          delete_elements();
          type = n_String;
      }

      void toString(int ss=100)
      {
        if(should_dispose)
        {
          char* buff = new char[ss];
          dump_json(buff);
          dispose();
          theBuf = buff;
          size = ss;
          capacity = 0;
          len = 0;
          type = n_String;
        }
      }

      int dump_json(char* buff) //converts n_Object to string
      {
        if(type==n_String)
        {
          int end = strlen(theBuf);
          buff[0] = '\"';
          buff[end+1] = '\"';
          for(int i=0;i<end;i++) buff[i+1] = theBuf[i];
          buff[end+2] = '\0';
          return end + 2;
        }
        else if(type==n_Array)
        {
          int end = 1;
          buff[0]  = '[';
          for(int i=0;i<len;i++)
          {
            end += elements[i]->dump_json(buff+end);
            buff[end] = ',';
            end++;
          }

          buff[end-1] = ']';
          buff[end] =  0;
          return end;
        }
        else if(type == n_Object)
        {
          int end = 1;
          buff[0]  = '{';
          for(int i=0;i<len;i++)
          {
            end+=(*keys)[i].dump_json(buff+end);
            buff[end]=':';end++;
            end+=elements[i]->dump_json(buff+end);
            buff[end]=',';end++;
          }
          buff[end-1] = '}';
          buff[end] =  0;
          return end;
        }
        else
        {
          int end = strlen(theBuf);
          for(int i=0;i<=end;i++) buff[i] = theBuf[i];
          return end;
        }
      }

      void removeends()
      {
        int last  = strlen(theBuf);
        int i = 0;
        theBuf[last-1] = '\0';
        while(theBuf[i]!='\0'){
           theBuf[i]  = theBuf[i+1];
           i++;
         }
        //theBuf[i]  = theBuf[i+1];

      }

      void parse_as_json() // converts n_string to n_Object
      {
        trim();
        if(theBuf[0]=='\"' || theBuf[0]=='\'')
        {
          //string
          type = n_String;
          removeends();
        }
        else if(theBuf[0]=='[')
        {
          //array or object
          removeends();
          splitPT(',');
          for(int i = 0;i<len;i++)
          {
            elements[i]->parse_as_json();
          }

          type = n_Array;
        }
        else if(theBuf[0]=='{')
        {
          //array or object
          removeends();
          splitPT(',');
          if(keys==NULL)
            keys = new nString();
          else
            keys->collapse();
          for(int i = 0;i<len;i++)
          {
            elements[i]->trim();
            elements[i]->splitPT(':');
            elements[i][0].parse_as_json();
            if(keys->theBuf==NULL)
              *keys = elements[i][0].theBuf;
            else {
                *keys+=" ";
                *keys+=elements[i][0].theBuf;
            }
          }
          keys->split(' ');
          for(int i = 0;i<len;i++)
          {
            //elements[i]->trim();
            *elements[i] = (*elements[i])[1].theBuf;
            elements[i]->collapse();
            elements[i]->parse_as_json();
          }
          type  = n_Object;
        }
        else
        {
          type = n_Int;
          for(int i = 0; i<strlen(theBuf);i++)
          {
            if(theBuf[i]=='.')
            {
              type = n_Float; break;
            }
          }
        }
      }

      void println(Stream* serial)
      {
        print();
        serial->println();
      }

      void tabs(int n)
      {
        for(int i=0;i<n;i++) Serial.print("  ");
      }

      void dump()
      {
        Serial.print("theBuf");
        Serial.print(":");
        Serial.println(theBuf);
        Serial.print("size");
        Serial.print(":");
        Serial.println(size);
        Serial.print("type");
        Serial.print(":");
        Serial.println(type);
        Serial.print("should_dispose");
        Serial.print(":");
        Serial.println(should_dispose);
        Serial.print("len");
        Serial.print(":");
        Serial.println(len);
        Serial.print("capacity");
        Serial.print(":");
        Serial.println(capacity);
        Serial.print("elements");
        Serial.print(":");
        Serial.println((long)elements);
        Serial.print("keys");
        Serial.print(":");
        Serial.println((long)keys);
      }

      void print(int n=1)
      {
        if(type==n_Array)
        {
           Serial.println("[");
           for(int i=0;i<len;i++)
           {
             tabs(n);
             elements[i]->print(n+1);
             Serial.println(",");
           }
           tabs(n-1);
           Serial.print("]");
        }
        else if(type==n_Object)
        {
          Serial.println("{");
          for(int i=0;i<len;i++)
          {
            tabs(n);
            (*keys)[i].print();
            Serial.print(":");
            elements[i]->print(n+1);
            Serial.println(",");
          }
          tabs(n-1);
          Serial.print("}");
        }
        else if(type==n_String)
        {
          Serial.print("\"");
          Serial.print(theBuf);
          Serial.print("\"");
        }
        else
        {
          Serial.print(theBuf);
        }
      }
};

#endif
