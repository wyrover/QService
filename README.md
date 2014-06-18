QService
========

this is a C/S service framework.should compile on Linux, *BSD, Solaris, Windows, and more. 

on windows compile it by vs2010, on linux run ./mk.

you can config service by file config.xml, in folder Bin/config.

it was easy to use, you can only compile this interface(in classs CEventInterface):

virtual void onSerciveStartUp(void){};

virtual void onSerciveShutDown(void){};

virtual void onSocketClose(void){};

virtual void onTimerEvent(void){};

virtual void onSocketRead(const char *, const size_t){};

virtual void onLinkedServer(class CSession *pSession){};

this project was used in mobile game server and other application.
