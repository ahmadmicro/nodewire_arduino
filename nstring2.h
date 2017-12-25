#ifndef NSTRING_H
#define NSTRING_H

#if defined (__STM32F1__)
#include <itoa.h>
#include <avr/dtostrf.h>
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
      static int precision;

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
        if(should_dispose==true && theBuf!=NULL) {
          delete[] theBuf;
        }
        if(elements!=NULL)
        {
          for(int i=0;i<capacity;i++)
          {
            delete elements[i];
            elements[i] =  NULL;
          }
          delete[] elements;
          elements = NULL;
        }
        if(keys!=NULL)
        {
          delete[] keys->theBuf;
          delete keys;
          keys = NULL;
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
         // buffer shared with op
          /*theBuf = op.theBuf;
          size =  op.size;
          should_dispose = false;
          type = op.type;
          if(op.elements!=NULL)
          {
            elements = op.elements;
            capacity = op.capacity;
            len = op.len;
            if(op.keys!=NULL)
            {
              keys = op.keys;
            }
          }*/

          theBuf = new char[op.size];
          size = op.size;
          type = n_String;
          should_dispose = true;
          copy(op);
      }

      void copy(const nString& op)
      {
        if(op.type==n_String || op.type==n_Float || op.type==n_Int)
        {
            //strcpy(theBuf, op.theBuf);
            if(op.size<=size)
              memcpy(theBuf,  op.theBuf, op.size);
            else if(!should_dispose)
                memcpy(theBuf,  op.theBuf, size-1);
            else
            {
                char* temp = new char[op.size];
                memcpy(temp, op.theBuf, op.size);
                delete[] theBuf;
                theBuf = temp;
                size = op.size;
            }
            theBuf[size-1]=0;
            type = op.type;
        }
        else if(op.elements!=NULL)
        {
           delete_elements();
            if(op.size<=size)
              memcpy(theBuf, op.theBuf, op.size);
            else
              memcpy(theBuf, op.theBuf, size);
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
          Serial.print("Failed to assign:");
          Serial.println(op.theBuf);
        }
      }

      nString(char* buff,  int len)
      {
        theBuf = buff;
        size = len;
        should_dispose = false;
      }

      void setBuffer(char* buff,  int len)
      {
        if(theBuf==NULL) // Not tested
        {
          theBuf = buff;
          size = len;
        }
      }

      nString(double buff)
      {
        theBuf = new char[10];
        should_dispose = true;
        size =  10;
        dtostrf(buff, size-1, precision, theBuf);
        type = n_Float;
      }

      nString(int op)
      {
        theBuf = new char[10];
        size = 10;
        clearBuffer();
        should_dispose = true;
        *this = op;
      }

      nString(long op)
      {
        theBuf = new char[15];
        size = 15;
        clearBuffer();
        should_dispose = true;
        *this = op;
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
          //Serial.println("EMPTY");
          theBuf = new char[15];
          should_dispose = true;
          size = 15;
          clearBuffer();
        }

        ltoa(op, theBuf, 15);
        type = n_Int;
        return *this;
      }

      nString& operator=(double op)
      {
        if(theBuf==NULL)
        {
          theBuf = new char[10];
          should_dispose = true;
          size = 10;
          clearBuffer();
        }
        dtostrf(op, size-1, precision, theBuf);
        type = n_Float;
        return *this;
      }

      nString& operator+=(const nString& op)
      {
        if(size>=(strlen(theBuf)+strlen(op.theBuf)))
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
        if(size>=strlen(theBuf)+strlen(op))
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
        char temp[10]; itoa(op, temp, 10);
        strcat(theBuf, temp);
        type = n_String;
        return *this;
      }
      nString operator+=(const double op)
      {
        char temp[10]; dtostrf(op, size-1, precision, temp);
        strcat(theBuf, temp);
        return *this;
      }

      friend nString& operator+(nString lhs, const nString& op)
      {
        //strcat(lhs.theBuf, op.theBuf);
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
        char temp[10]; itoa(op, temp, 10);
        strcat(theBuf, temp);
        return *this;
      }
      nString operator+(const double op)
      {
        char temp[10]; dtostrf(op, size-1, precision, temp);
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

      explicit operator long()
      {
        return atol(theBuf);
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

      void create_object(nString dKeys)
      {
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
            //Serial.println("E DON HAPPEN!");
            //resize(len);
          }
          //len = capacity;

          type = n_Object;
        }
      }

      nString& operator[](nString k)
      {
        int i = keys->find(k);
        if(i!=-1 && i<capacity)
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

      /*void resize(int new_capacity)
      {
        int new_size = size/new_capacity;
        for(int i=0;i<capacity;i++) elements[i]->size = new_size;
        if(capacity<new_capacity)
        {
          nString** temp = new nString*[new_capacity];
          for(int i=0;i<capacity;i++) temp[i]=elements[i];
          free(elements);
          elements = temp;
          int old_capacity = capacity;
          capacity = new_capacity;
          for(int i=old_capacity;i<new_capacity;i++) append("");
        }
      }*/

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
        if(capacity!=0 && elements!=NULL)
        {
          for(int i=0;i<capacity;i++)
          {
            delete elements[i];
            elements[i] =  NULL;
          }

          delete[] elements;
          elements = NULL;
          capacity = 0;
          len = 0;
        }
        if(keys!=NULL)
        {
          delete[] keys->theBuf;
          delete keys;
          keys = NULL;
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
                elements[j]->setBuffer(theBuf + next_element, i - next_element+1);
              next_element = i+1;
              j++;
            }
          }
          if(elements[j] == NULL)
            elements[j] = new nString(theBuf + next_element, size - next_element);
          else
            elements[j]->setBuffer(theBuf + next_element, size - next_element);
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
              elements[j] = new nString(theBuf + next_element, i - next_element);
              next_element = i+1;
              j++;
            }
        }
        elements[j] = new nString(theBuf + next_element, size - next_element);
        type = n_Array;
        return len;
      }

      int splitPT(char sep=' ')
      {
        char separator='\0';
        char opposite;
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
              looking = false;
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
          }
          else
          {
            if(separator==sep && theBuf[i]!=sep) looking = true;
            if(separator=='[' && theBuf[i]==']') looking = true;
            if(separator=='{' && theBuf[i]=='}') looking = true;
            if(separator=='\"' && theBuf[i]=='\"') looking = true;
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
            elements[el-1] = new nString(theBuf + prev, i - prev);
            prev = i+1;
            el++;
          }
        }
        elements[len-1] = new nString(theBuf + prev, size - prev);
        type = n_Array;
        return len;
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
        if(theBuf[0]=='\"')
        {
          //string
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

          keys = new nString();
          for(int i = 0;i<len;i++)
          {
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
            if(theBuf[i]=='.')
            {
              type = n_Float; break;
            }

        }

      }

      void println(Stream* serial)
      {
        print();
        serial->println();
      }

      void print()
      {
        if(type==n_Array)
        {
           Serial.println("[");
           for(int i=0;i<len;i++)
           {
             Serial.print("  ");
             elements[i]->print();
             Serial.println(",");
           }
           Serial.println("]");
        }
        else if(type==n_Object)
        {
          Serial.println("{");
          for(int i=0;i<len;i++)
          {
            Serial.print("  ");
            (*keys)[i].print();
            Serial.print(":");
            elements[i]->print();
            Serial.println(",");
          }
          Serial.println("}");
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
