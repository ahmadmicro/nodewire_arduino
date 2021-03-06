#include <nstring2.h>
#include <EEPROM.h>
#ifndef EEPROM_File_H
#define EEPROM_File_H

#ifdef ESP8266
   #define EEPROMFILESIZE 2048
#elif defined ESP32
   #define EEPROMFILESIZE 4096
#else
   #define EEPROMFILESIZE 1024
#endif

class EEPROM_File
{
private:
  void readEEPROM(char* buffer, int address, int size)
  {
   int i  = 0;
   while(i<size)
   {
      buffer[i] = EEPROM.read(address+i);
      i++;
   }
   if(buffer[0]==255) buffer[0]=0;
  }

  void  writeEEPROM(const char* content, int address, int size)
  {
     int i  = address;
     while(i<address+size)
     {
       EEPROM.write(i, content[i-address]);
       i++;
     }
     #if defined ESP8266 || ESP32
      EEPROM.commit();
     #endif
  }

public:
  EEPROM_File()
  {
      #if defined ESP32 || ESP8266
        EEPROM.begin(EEPROMFILESIZE);
      #endif
  }

  void format()
  {
      int i  = 0;
      while(i<EEPROMFILESIZE)
      {
        EEPROM.write(i, 0);
        i++;
      }
      #if defined ESP8266 || ESP32
        EEPROM.commit();
      #endif
  }

  void create_FS(int no_files)
  {
    /*
       * marker * 4 chars : NWFS
       * no files * 4
       -------------------
       filename  * 8 chars + 1
       len       * 4 chars + 1
       start loc * 4 chars + 1
       cap       * 4 chars + 1
       -----> 24
    */
    nString nofiles = no_files;
    nofiles.theBuf[4] = 0;
    writeEEPROM("NWFS",0,4);
    writeEEPROM(nofiles.theBuf,4,4);
    for(int i=0;i<no_files;i++)
    {
      writeEEPROM("EMPTY 0 0 0",8+i*24,12);
    }
  }

  int no_files()
  {
    char marker[10];
    readEEPROM(marker,0,4);
    if(strncmp(marker,"NWFS",4)==0)
    {
      nString no(marker,5);
      readEEPROM(marker,4,4);
      return (int)no;
    }
    return -1;
  }

  int filelenght(nString filename)
  {
    char directory[24];
    nString entry(directory, 24);
    int no = no_files();
    if(no==-1) return -1;
    for(int fileno=0;fileno<no;fileno++)
    {
      entry.collapse();
      readEEPROM(directory,8+fileno*24, 24);
      entry.split(' ');
      if(entry[0]==filename)
      {
        return (int)entry[1];
      }
      entry.clearBuffer();
      entry.type=n_String;
    }
    return -1;
  }

  nString getDirectory()
  {
    char directory[24];
    nString entry(directory, 24);
    int no = no_files();
    if(no==-1) return "";
    nString Directory;//(new char[no*24]);
    Directory.createBuffer(no*24);
    for(int fileno=0;fileno<no;fileno++)
    {
      entry.collapse();
      entry.clearBuffer();
      readEEPROM(directory,8+fileno*24, 24);
      //entry.split(' ');
      Directory+=entry;
      Directory+= "\n";
    }
    return Directory;
  }

  bool open(nString filename, nString& content)
  {
    char directory[24];
    nString entry(directory, 24);
    int no = no_files();
    if(no==-1) return false;
    for(int fileno=0;fileno<no;fileno++)
    {
      entry.collapse();
      readEEPROM(directory,8+fileno*24, 24);
      entry.split(' ');
      if(entry[0]==filename)
      {
        int lenght = (int)entry[1];
        int pos = (int)entry[2];
        int cap = (int)entry[3];
        readEEPROM(content.theBuf, pos, lenght);
        return true;
      }
    }
    return false;
  }

  bool create_file(nString filename, int capacity)
  {
    char directory[24];
    nString entry(directory, 24);
    int no = no_files();
    if(no==-1) return false;
    int pos = 8+24*no;
    for(int fileno=0;fileno<no;fileno++)
    {
      entry.collapse();
      entry.clearBuffer();
      readEEPROM(directory,8+fileno*24, 24);
      entry.split(' ');
      if(entry[0]==filename || entry[0]=="EMPTY")
      {
        if(entry[0]=="EMPTY")
        {
          entry.collapse();
          entry.clearBuffer();
          entry = filename;
          entry+=  " ";
          entry += 0;
          entry += " ";
          entry += pos;
          entry += " ";
          entry+= capacity;

          writeEEPROM(directory, 8+fileno*24, 24);
          return true;
        }
        else
        {
          return false;
        }
        return false;
      }
      else
      {
        pos = (int)entry[2]+(int)entry[3];
      }
    }
    return false;
  }

  bool save(nString filename, nString content)
  {
    char directory[24];
    nString entry(directory, 24);
    int no = no_files();
    if(no==-1) return false;
    int pos = 8+24*no;
    for(int fileno=0;fileno<no;fileno++)
    {
      //entry.type = n_String;
      entry.collapse();
      entry.clearBuffer();
      readEEPROM(directory,8+fileno*24, 24);
      entry.split(' ');
      if(entry[0]==filename || entry[0]=="EMPTY")
      {
        if(entry[0]=="EMPTY")
        {
          entry.collapse();
          entry.clearBuffer();
          entry = filename;
          entry+=  " ";
          entry += content.size;
          entry += " ";
          entry += pos;
          entry += " ";
          entry+= content.size;

          writeEEPROM(directory, 8+fileno*24, 24);
          writeEEPROM(content.theBuf, pos, content.size);

          return true;
        }
        else if(strlen(content.theBuf)<=(int)entry[3])
        {
            int pos = (int) entry[2];
            int cap = (int) entry[3];

            entry.collapse();
            entry.clearBuffer();
            entry = filename;
            entry+=  " ";
            entry += content.size;
            entry += " ";
            entry += pos;
            entry += " ";
            entry+= cap;

            writeEEPROM(directory, 8+fileno*24, 24);
            writeEEPROM(content.theBuf, pos, content.size);
            return true;
        }
        debug.log2("failed to save");
        return false;
      }
      else
      {
        pos = (int)entry[2]+(int)entry[3];
      }
    }
    return false;
  }
};


#endif
