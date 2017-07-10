#ifndef MLOG_INIT_H
#define MLOG_INIT_H

#include "memorylog.h"
#include <stdarg.h>



class mlog_init
{
public:
    mlog_init();
};



/*============================================
* FuncName    : mlog_msgcommon
* Description : mlog message common process
* @--         :
* Author      :
* Time        : 2017-06-11
============================================*/
#define  mlog_msgcommon(name, msg, msglen, fmt, args...)\
do{\
    pushmsgbyname(name, msg, msglen, fmt, ##args);\
}while(0)


/*============================================
* FuncName    : mlog_nomsgcommon
* Description : mlog no message common process
* @--         :
* Author      :
* Time        : 2017-06-11
============================================*/
#define  mlog_nomsgcommon(name,  fmt, args...)\
do{\
    pushmsgbyname(name, NULL, 0, fmt, ##args);\
}while(0)






/*============================================
* FuncName    : mlog_msgbyfunc
* Description : mlog message of function
* @--         :
* Author      :
* Time        : 2017-06-11
============================================*/
#define  mlog_msgbyfunc(msg, msglen, fmt, args...)\
do{\
    char buf[64];\
    timeval now;\
    gettimeofday(&now, NULL);\
    snprintf(buf, sizeof(buf), "func_%s", __FUNCTION__);\
    pushmsgbyname(buf, msg, msglen ,fmt"T:s-us:%-06u-%-06u, F:%-10s,L:%-8u", ##args, now.tv_sec, now.tv_usec,\
        __FUNCTION__, __LINE__);\
}while(0)



/*============================================
* FuncName    : mlog_msgbyfuncdefault
* Description : mlog message of function
* @--         :
* Author      :
* Time        : 2017-06-11
============================================*/
#define  mlog_msgbyfuncdefault  mlog_msgbyfunc(NULL, 0, "")



/*============================================
* FuncName    : mlogfunction
* Description : mlog message of function
* @--         :
* Author      :
* Time        : 2017-06-11
============================================*/
#define  mlogfunction  mlog_msgbyfuncdefault





















#endif // MLOG_INIT_H
