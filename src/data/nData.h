#include <nstring2.h>
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
      debug.log2("File system initialized");
  }

  int no_records()
  {
      File f = SPIFFS.open((char*)fname, "a+");
      int len = f.size()/rec_len;;
      f.close();
      return len;
  }

  void truncate()
  {
    SPIFFS.remove((char*)fname);
  }

  void add(nString record)
  {
      File f = SPIFFS.open((char*)fname, "a+"); // for reeading and appending
      row.collapse();
      row = record;
      if(f)
      {
        f.write((const uint8_t*)rec_content, rec_len);
        f.close();
      }
      else
      {
        debug.log2("file failed to open");
      }
  }

  int read(int location)
  {
      File f = SPIFFS.open((char*)fname, "a+"); // for reeading and appending
      loc = location;
      if(f)
      {
          if(!f.seek(loc*rec_len, SeekSet)) return 0;
          int n = f.readBytes(rec_content, rec_len);
          f.close();
          row.collapse();
          row.splitCSV();
          return n;
      }
      else
      {
        debug.log2("file failed to open");
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
      nString vv = (*this)[dfield];
      vv.trim();
      if(vv==dval)
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

   bool findlast(nString query)
   {
       query.split('=');
       nString dfield = query[0];
       nString dval = query[1];
       int dloc = no_records()-1;

       while(read(dloc--))
       {
         nString vv = (*this)[dfield];
         vv.trim();
         if(vv==dval)
         {
            loc = dloc+1;
            return true;
         }
       }

       return false;
   }

  nString& operator[](nString field)
  {
    int ind = fields.find(field);
    if(ind!=-1)
    {
      return row[ind];
    }
    else
    {
      debug.log2(field.theBuf);
      debug.log2("Field not found");
    }
  }

  nString& to_object()
  {
      if(row.type==n_Array)
      {
         fields.join(' ');
         row.convert_object(fields);
         fields.split(' ');
      }

      return row;
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

       //row.splitCSV();

       SPIFFS.remove((char*)fname);
       SPIFFS.rename("/tmp", (char*)fname);
    }
    else
    {
      debug.log2("file failed to open");
    }
  }

  void delete_row(nString record)
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
          if(ll!=loc)
          {
            f.readBytes(rec_content, rec_len);
            g.write((const uint8_t*)rec_content, rec_len);
          }
          ll++;
       }
       f.close();
       g.close();

       SPIFFS.remove((char*)fname);
       SPIFFS.rename("/tmp", (char*)fname);
    }
    else
    {
      debug.log2("file failed to open");
    }
  }

};
