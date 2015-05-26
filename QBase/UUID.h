
#ifndef Q_UUID_H_
#define Q_UUID_H_

#include "Macros.h"

#ifdef Q_UUID

#ifndef Q_OS_WIN
typedef struct _GUID
{
    unsigned long Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char Data4[8];
} GUID, UUID;
#endif

/*UUID
yum install uuid uuid-devel
mkdir /usr/include/uuid
cp /usr/include/uuid.h  /usr/include/uuid/
yum install e2fsprogs-devel
yum install libuuid libuuid-devel
*/
class CUUID
{
public:
    CUUID(void);
    ~CUUID(void);

    /*ªÒ»°“ªUUID*/
    std::string getUUID(void);

private:
    int createGuid(GUID &stUUID);
};

#endif//Q_UUID

#endif//Q_UUID_H_
