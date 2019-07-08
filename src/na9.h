#ifndef NA9_H
#define NA9_H

typedef void (*A9CallHandler)(const char*);
typedef void (*A9MsgHandler)(const char*, const char*);

class A9Modem
{
    private:
        Stream* _serial;
        char in_buff[200];
        A9MsgHandler sms = NULL;
        A9CallHandler incoming = NULL;
        int index = 0;
    public:
        void begin(Stream* serial)
        {
            _serial = serial;
        }
        void call(const char* number)
        {
            char buffer[30];
            sprintf(buffer, "a9 set call %s ", number);
            _serial->println(buffer);
        }
        void send(const char* number, const char* message)
        {
            char buffer[180];
            sprintf(buffer, "a9 set sms \"%s:%s\" ", number, message);
            _serial->println(buffer);
        }
        void answer()
        {
            _serial->println("a9 set answer 1 ");
        }

        void on_incoming(A9CallHandler handler)
        {
            incoming = handler;
        }

        void on_message(A9MsgHandler handler)
        {
            sms = handler;
        }

        void run()
        {
            while (_serial->available()) {
                char inChar = (char)_serial->read();

                if (inChar == '\n' || index >= (sizeof(in_buff)-2)) {
                    in_buff[index]=0;
                    index = 0;
                    //Serial.print("received:");
                    //Serial.println(in_buff);
                    if(strncmp(in_buff, "any portvalue incoming", 22) ==0 && incoming!=NULL)
                    {
                        char* start = in_buff+23;
                        char* end = strchr(start, ' ');
                        *end = 0;
                        incoming(start);
                    }

                    if(strncmp(in_buff, "any portvalue sms", 17) ==0 && sms!=NULL)
                    {
                        char* number = in_buff+28;
                        char* end = strchr(number, '\"');
                        char* message = strstr(in_buff, "\"msg\":")+7;
                        *end = 0;
                        end = strstr(message, "\"} a9");
                        *end = 0;
                        sms(number, message);
                    }
                     
                    return;
                }
                else
                {
                    in_buff[index] = inChar;
                    index++;
                }
            }

        }
};

#endif