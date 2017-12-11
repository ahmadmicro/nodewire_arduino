#include <nstring.h>
#include <FS.h>

class nData
{
  size_t rec_len;
  char* rec_content;
  nString row;

  int loc = 0;

  nString fields;
  char f_buff[50];
  int no_fields;

  nString fname;
  char fname_buff[30];

public:
  nData(nString filename, nString dfields, int rec_lenght) // e.g. ("record", "staffid name", 100)
  {
    rec_len = rec_lenght;
    rec_content = new char[rec_lenght];
    row.setBuffer(rec_content, rec_lenght);

    fname.setBuffer(fname_buff, sizeof(fname_buff));
    fname = filename;

    fields.setBuffer(f_buff, sizeof(f_buff));
    fields = dfields;
    no_fields = fields.split(' ');

    if(SPIFFS.begin())
      Serial.println("File system initialized");
  }

  void truncate()
  {
    SPIFFS.remove((char*)fname);
  }

  void add(nString record)
  {
      File f = SPIFFS.open((char*)fname, "a+"); // for reeading and appending
      row = record;
      if(f)
      {
        f.write((const uint8_t*)rec_content, rec_len);
        f.close();
      }
      else
      {
        Serial.println("file failed to open");
      }
  }

  int read(int location)
  {
      File f = SPIFFS.open((char*)fname, "a+"); // for reeading and appending
      loc = location;
      if(f)
      {
          f.seek(loc*rec_len, SeekSet);
          int n = f.readBytes(rec_content, rec_len);
          f.close();
          row.parseCSV();
          return n;
      }
      else
      {
        Serial.println("file failed to open");
      }

      return 0;
  }

  bool find(nString query)
  {
    query.split('=');
    nString dfield = query[0];
    nString dval = query[1];
    int dloc = 0;

    while(read(dloc++))
    {
      if((*this)[dfield]==dval)
      {
         loc = dloc-1;
         return true;
      }
    }

    return false;
  }

  bool findnext(nString query)
  {
    query.split('=');
    nString dfield = query[0];
    nString dval = query[1];
    int dloc = loc;

    while(read(dloc++))
    {
      if((*this)[dfield]==dval)
      {
         loc = dloc-1;
         return true;
      }
    }

    return false;
  }

  nString operator[](nString field)
  {
    int ind = fields.find(field);
    if(ind!=-1)
    {
      return row[ind];
    }
    else
    {
      Serial.println("Field not found");
    }
  }

  void update(nString record)
  {
    File f = SPIFFS.open((char*)fname, "a+"); // for reeading and appending
    File g = SPIFFS.open("/tmp", "a+");

    int ll = 0;
    int size = f.size()/rec_len;
    if(f && g)
    {
       while(ll<size)
       {
          f.seek(ll*rec_len, SeekSet);
          g.seek(ll*rec_len, SeekSet);
          if(ll==loc)
            row = record;
          else
            f.readBytes(rec_content, rec_len);
          g.write((const uint8_t*)rec_content, rec_len);
          ll++;
       }
       f.close();
       g.close();

       SPIFFS.remove((char*)fname);
       SPIFFS.rename("/tmp", (char*)fname);
    }
    else
    {
      Serial.println("file failed to open");
    }
  }

};
