#include "memorylog.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>

#define MUTEX_P(mutex) pthread_mutex_lock   (&mutex)
#define MUTEX_V(mutex) pthread_mutex_unlock (&mutex)







MemoryLog *MemoryLog::pInstance = NULL;

/*============================================
* FuncName    : MemoryLog::MemoryLog
* Description :
* @           :
* Author      :
* Time        : 2017-06-05
============================================*/
MemoryLog::MemoryLog()
{
    mlog.clear();
    pthread_mutex_init (&mutex,NULL);
}

/*============================================
* FuncName    : MemoryLog::GetInstance
* Description :
* @           :
* Author      :
* Time        : 2017-06-05
============================================*/
MemoryLog * MemoryLog::GetInstance()
{
    if(pInstance == NULL)
    {
        pInstance = new MemoryLog();
    }
    return pInstance;
}

/*============================================
* FuncName    : MemoryLog::Version
* Description :
* @           :
* Author      :
* Time        : 2017-06-10
============================================*/
const char* MemoryLog::Version()
{
    return MLOG_VERSION;
}



/*============================================
* FuncName    : timeval_diff
* Description : caculate time diff
* @tvbegin    : begin time
* @tvend      : end time
* Author      :
* Time        : 2017-06-07
============================================*/
unsigned int timeval_diff(struct timeval *tvbegin, struct timeval *tvend)
{
    unsigned int diff = 0;
    if(tvbegin->tv_sec > tvend->tv_sec)
    {
        diff = (tvbegin->tv_sec - tvend->tv_sec) * 1000 + (tvbegin->tv_usec - tvend->tv_usec)/1000;
    }
    else if(tvbegin->tv_sec ==  tvend->tv_sec)
    {
        diff = (tvbegin->tv_usec > tvend->tv_usec) ? (tvbegin->tv_usec - tvend->tv_usec)/1000 :
                                                     (tvend->tv_usec - tvbegin->tv_usec)/1000;
    }
    else
    {
        diff = (tvend->tv_sec - tvbegin->tv_sec) * 1000 + (tvend->tv_usec - tvbegin->tv_usec)/1000;
    }

    return diff;
}


/*============================================
* FuncName    : MemoryLog::CheckPushLog
* Description : 
* @key        : 
* Author      : 
* Time        : 2017-06-07
============================================*/
bool MemoryLog::CheckPushLog(const char *key)
{
    MUTEX_P(mutex);
    MLOG_MAP_IT it = mlog.find(string(key));
    if(it != mlog.end())
    {
        MLOG_VEC &vec = it->second;

        if(get_mlogformat() && vec.size() >= get_mlogmaxsize())
        {
            T_MLOG tlog = *(vec.begin());
            free(tlog.msgaddr);
            vec.erase(vec.begin() + 0, vec.begin() + 1);//erase the first data
            MUTEX_V(mutex);
            return true;
        }

        if(vec.size() >= get_mlogmaxsize())
        {
            MUTEX_V(mutex);
            return false;
        }
        else
        {
            MUTEX_V(mutex);
            return true;
        }
    }
     MUTEX_V(mutex);

    return true;
}



/*============================================
* FuncName    : MemoryLog::PushLog
* Description : 推入数据流到内存
* @key        : 索引字
* @fmt        : 字符串格式
* @--         : 变长
* Author      :
* Time        : 2017-06-05
============================================*/
void MemoryLog::PushLog(const char *key, T_MLOG &tlog)
{
    MUTEX_P (mutex);

    MLOG_MAP_IT it = mlog.find(string(key));
    if(it != mlog.end())
    {
        MLOG_VEC &vec = it->second;
        if(vec.size() >= get_mlogmaxsize())
        {
            MUTEX_V(mutex);
            return;
        }
        vec.push_back(tlog);
    }
    else
    {
        MLOG_VEC vec;
        vec.push_back(tlog);
        mlog.insert(MLOG_MAP_PAIR(string(key), vec));
    }
    MUTEX_V(mutex);
}


/*============================================
* FuncName    : MemoryLog::ParseMsgBody
* Description : parse msg body , the format of msg show;print or save msg body
* @tlog       :
* @fp         :stdout or files pointer
* Author      :
* Time        : 2017-06-10
============================================*/
void MemoryLog::ParseMsgBody(T_MLOG &tlog, FILE *fp)
{
    if(0 == tlog.msglen && NULL == tlog.msgaddr)
        fprintf(fp, "==>%s\n", tlog.tipsinfo);
    else
    {
        char *pmsg = NULL;
        unsigned int msglen = tlog.msglen;
        fprintf(fp, "==>%s\n", tlog.tipsinfo);
        pmsg = (char *)malloc(msglen * 3 + (msglen < 16 ? 1 : msglen /16) *4);
        if(!pmsg)
        {
            return;
        }
        memset(pmsg, 0, msglen);
        unsigned int i = 0;
        unsigned int j = 0;
        unsigned int tnum = 0;
        unsigned int cpos = 0;
        char *rmsg = (char *)tlog.msgaddr;

        tnum = sprintf(pmsg + cpos, "\n");
        for(i = 0; i < msglen; i++)
        {
            j = i + 1;
            if(0 == i)
            {
                tnum = sprintf(pmsg + cpos, "\n");
                cpos += tnum;
            }
            if(0 == j % 8 && 0 != j % 16)
                tnum = sprintf(pmsg + cpos, "%02X  ", (unsigned char)rmsg[i]);
            else if( 0 == j % 16)
                tnum = sprintf(pmsg + cpos, "%02X", (unsigned char)rmsg[i]);
            else
                tnum = sprintf(pmsg + cpos, "%02X ", (unsigned char)rmsg[i]);

            cpos += tnum;
            if( 0 == (j) % 16)
            {
                tnum = sprintf(pmsg + cpos, "\n");
                cpos += tnum;
            }
        }
        fprintf(fp, "==>%s\n", pmsg);
        free(pmsg);
    }
}


/*============================================
* FuncName    : MemoryLog::ShowLogByName
* Description : show key's memory log
* @key        :
* @index      : show tips or not
* Author      :
* Time        : 2017-06-05
============================================*/
void MemoryLog::ShowLogByName(const char *key, bool index = true)
{
    MUTEX_P (mutex);
    if(index)
        fprintf(stdout, "show mlog key[%-10s], ", key);
    MLOG_MAP_IT it = mlog.find(string(key));
    if(it != mlog.end())
    {
        MLOG_VEC &vec = it->second;
        fprintf(stdout, "[%s] size:%u\n", key, vec.size());
        for(MLOG_VEC_IT vit = vec.begin(); vit != vec.end(); ++vit)
        {
            ParseMsgBody(*vit,stdout);
        }
        if(index)
            fprintf(stdout, "show mlog key[%s] size:%u done!!\n\n", key, vec.size());
    }
    else
    {
        if(index)
            fprintf(stdout, "no data!!\n\n");
    }
    MUTEX_V (mutex);
}

/*============================================
* FuncName    : MemoryLog::ShowLogAll
* Description : show all key's memory log
* @           :
* Author      :
* Time        : 2017-06-05
============================================*/
void MemoryLog::ShowLogAll()
{
    if(!mlog.size() )
    {
        fprintf(stdout, "show mlog, no data!!\n");
        return;
    }
    fprintf(stdout, "show all mlog data, size:%u\n", mlog.size());
    for(MLOG_MAP_IT it  = mlog.begin(); it != mlog.end(); ++it)
    {
        ShowLogByName(it->first.c_str(), false);
    }

    fprintf(stdout, "show all mlog data, size:%u, done!!\n", mlog.size());
}

/*============================================
* FuncName    : MemoryLog::ShowLogKeys
* Description : show memory log's keys
* @           :
* Author      :
* Time        : 2017-06-05
============================================*/
void MemoryLog::ShowLogKeys(FILE *fp)
{
    if(!mlog.size() )
    {
        fprintf(fp, "show mlog keys, no data!!\n");
        return;
    }
    fprintf(fp, "show mlog keys, size:%u\n", mlog.size());
    if(mlog.size())
    {
        fprintf(fp,"[key            ] [count]   [msglen]\n");
    }

    for(MLOG_MAP_IT it  = mlog.begin(); it != mlog.end(); ++it)
    {
        MLOG_VEC &vec = it->second;
        unsigned int msglen = 0;
        for(MLOG_VEC_IT vit = vec.begin(); vit != vec.end(); ++vit)
        {
            msglen += (*vit).msglen;
        }
        fprintf(fp,"%-020s %-07u %-08u\n" ,
                it->first.c_str(),
                it->second.size(),
                msglen);
    }

    fprintf(fp, "show mlog keys, size:%u, done!!\n", mlog.size());
}

/*============================================
* FuncName    : MemoryLog::FindLogKeys
* Description : find memory log keys by name
* @fp         :
* @name       : the key words to find
* Author      :
* Time        : 2017-06-15
============================================*/
void MemoryLog::FindLogKeys(FILE *fp, const char *name)
{
    if(!mlog.size() )
    {
        fprintf(fp, "find mlog keys by \"%s\", no data!!\n", name);
        return;
    }
    fprintf(fp, "find mlog keys by \"%s\", mlog size:%u\n", name, mlog.size());
    if(mlog.size())
    {
        fprintf(fp,"[key            ] [count]   [msglen]\n");
    }

    unsigned int findsize = 0;
    for(MLOG_MAP_IT it  = mlog.begin(); it != mlog.end(); ++it)
    {
        string findstr = it->first;
        if(!strstr(findstr.c_str(), name))
        {
            continue;
        }
        findsize++;
        MLOG_VEC &vec = it->second;
        unsigned int msglen = 0;
        for(MLOG_VEC_IT vit = vec.begin(); vit != vec.end(); ++vit)
        {
            msglen += (*vit).msglen;
        }
        fprintf(fp,"%-020s %-07u %-08u\n" ,
                it->first.c_str(),
                it->second.size(),
                msglen);
    }

    fprintf(fp, "find mlog keys by \"%s\", mlog size:%u, find size:%u done!!\n",
            name ,
            mlog.size(), findsize);
}



/*============================================
* FuncName    : MemoryLog::ClearLogByName
* Description :
* @key        :
* Author      :
* Time        : 2017-06-05
============================================*/
void MemoryLog::ClearLogByName(const char *key, bool tips = true, bool lock = true)
{
    if(lock)
        MUTEX_P (mutex);
    if(tips)
        fprintf(stdout, "clear mlog key[%s], ", key);
    MLOG_MAP_IT it = mlog.find(string(key));
    if(it != mlog.end())
    {
        MLOG_VEC &vec = it->second;
        for(MLOG_VEC_IT vit = vec.begin(); vit != vec.end(); ++vit)
        {
            T_MLOG tlog = (*vit);
            free(tlog.msgaddr);
        }
        vec.clear();
        mlog.erase(it);
        if(tips)
            fprintf(stdout, "\nclear mlog key[%s] done!!\n", key);
    }
    else
    {
        if(tips)
            fprintf(stdout, "no data!!\n");
    }
    if(lock)
        MUTEX_V (mutex);
}

/*============================================
* FuncName    : MemoryLog::ClearLogAll
* Description :
* @           :
* Author      :
* Time        : 2017-06-05
============================================*/
void MemoryLog::ClearLogAll()
{
    MUTEX_P (mutex);
    if(!mlog.size() )
    {
        fprintf(stdout, "no data!!\n");
        MUTEX_V (mutex);
        return;
    }
    fprintf(stdout, "clear all mlog, size:%u\n", mlog.size());
    vector<string> keys;
    vector<string>::iterator it;
    keys.clear();
    for(MLOG_MAP_IT it  = mlog.begin(); it != mlog.end(); ++it)
    {
        keys.push_back(it->first);
    }

    for(it = keys.begin(); it != keys.end(); it++)
        ClearLogByName((*it).c_str(),false, false);
    mlog.clear();
    keys.clear();

    fprintf(stdout, "clear all mlog, size:%u, done!!\n", mlog.size());
    MUTEX_V (mutex);
}

/*============================================
* FuncName    : MemoryLog::SaveLog2FileByName
* Description :
* @key        :
* @tips       :
* Author      :
* Time        : 2017-06-05
============================================*/
void MemoryLog::SaveLog2FileByName(const char *key, const char *filewithpath = "/tmp/aaa.mlog",  bool tips = true, FILE *fother = NULL)
{
    string name(filewithpath);
    FILE *fp = NULL;
    if(NULL == fother)
    {
        fp = fopen(name.c_str(), "w");
        if(!fp)
        {
            fprintf(stderr, "open file err![%-10s] %s\n", name.c_str(), strerror(errno));
            return;
        }
    }
    else
    {
        fp = fother;
    }

    timeval now;
    gettimeofday(&now, NULL);

    MUTEX_P (mutex);
    if(tips && name != ""){
//        fprintf(fp, "time:s-us: %u-%u\n", now.tv_sec, now.tv_usec);
        fprintf(stdout, "save mlog to file[%-10s], ", name.c_str());
        fprintf(fp, "save mlog to file[%-10s], ", name.c_str());
    }
    else if(tips)
    {
//        fprintf(fp, "time:s-us: %u-%u\n", now.tv_sec, now.tv_usec);
        fprintf(stdout, "save mlog key[%-10s], ", key);
        fprintf(fp, "save mlog key[%-10s], ", key);
    }
    MLOG_MAP_IT it = mlog.find(string(key));
    if(it != mlog.end())
    {
        MLOG_VEC &vec = it->second;
        fprintf(fp, "[%s] size:%u\n", key, vec.size());
        for(MLOG_VEC_IT vit = vec.begin(); vit != vec.end(); ++vit)
        {
            ParseMsgBody(*vit,fp);
        }
        if(tips){
            fprintf(stdout, "save mlog key[%-10s], done!!\n\n", key);
            fprintf(fp, "save mlog key[%-10s], done!!\n\n", key);
        }
    }
    else
    {
        if(tips)
            fprintf(fp, "no data!!\n\n");
    }
    fflush(fp);
    if(NULL == fother)
    {
        fclose(fp);
    }
    MUTEX_V (mutex);
}

/*============================================
* FuncName    : MemoryLog::SaveLog2FileAll
* Description :
* @           :
* Author      :
* Time        : 2017-06-05
============================================*/
void MemoryLog::SaveLog2FileAll(const char *filewithpath = NULL)
{

    if(!mlog.size() )
    {
        fprintf(stdout, "save mlog to file, no data!!\n");
        return;
    }

    const char *name = "./savemlogs.mlog";
    if(NULL != filewithpath)
    {
        name = filewithpath;
    }
    FILE *fp = fopen(name, "w");
    if(!fp)
    {
        fprintf(stderr, "open file err![%-10s] %s\n", name, strerror(errno));
        return;
    }

    SaveLog2FileKeys(fp);

    fprintf(stdout, "save mlog to file[%-10s], map size:%u\n",name,  mlog.size());
    fprintf(fp, "save mlog to file, size:%u\n", mlog.size());
    for(MLOG_MAP_IT it  = mlog.begin(); it != mlog.end(); ++it)
    {
        SaveLog2FileByName(it->first.c_str(), "",  true, fp);
    }

    fprintf(fp, "save mlog to file[%-10s], size:%u, done!!\n",name, mlog.size());
    fprintf(stdout, "save mlog to file[%-10s], size:%u, done!!\n",name, mlog.size());

    fclose(fp);
}

void MemoryLog::SaveLog2FileKeys(FILE *felse = NULL)
{
    FILE *fp = NULL;
    if(NULL == felse)
    {
        const char *name = "./mlog_keys.txt";
        fp = fopen(name, "w");
        if(!fp)
        {
            fprintf(stderr, "open file err![%-10s] %s\n", name, strerror(errno));
            return;
        }
    }
    else
    {
        fp = felse;
    }
    if(!mlog.size() )
    {
        fprintf(stdout, "save mlog keys, no data!!\n");
        fprintf(fp, "save mlog keys, no data!!\n");
        return;
    }

    fprintf(stdout, "save mlog keys, size:%u, done!!\n", mlog.size());
    fprintf(fp, "save mlog keys, size:%u, done!!\n", mlog.size());
    if(mlog.size())
    {
        fprintf(fp,"[key            ] [count]   [msglen]\n");
    }

    for(MLOG_MAP_IT it  = mlog.begin(); it != mlog.end(); ++it)
    {
        MLOG_VEC &vec = it->second;
        unsigned long long msglen = 0;
        for(MLOG_VEC_IT vit = vec.begin(); vit != vec.end(); ++vit)
        {
            msglen += (*vit).msglen;
        }
        fprintf(fp,"%-020s %-07u %-08llu\n" ,
                it->first.c_str(),
                it->second.size(),
                msglen);
    }

    fprintf(fp, "show mlog keys, size:%u, done!!\n", mlog.size());

    if(NULL == felse)
    {
        fclose(fp);
    }
}




#ifdef __cplusplus
extern "C" {
#endif

/* func_declare_begin */
void showmlogbyname( const char *key);
void showmlogall();
void clearmlogbyname( const char *key);
void clearmlogall();
/* func_declare_end */


/* func_implement */

/*============================================
* FuncName    : pushlogbyname
* Description :
* @key        :
* @fmt        :
* @--         :
* Author      :
* Time        : 2017-06-06
============================================*/
void pushlogbyname(const char *key, char *fmt, ...)
{
    MemoryLog *pInstance = MemoryLog::GetInstance();
    if(!pInstance->CheckPushLog(key))
        return;

    va_list ap;
    char buf[1024];
    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);

    T_MLOG tLog = {0};
    snprintf(tLog.tipsinfo, sizeof(tLog.tipsinfo), "%s", buf);

    pInstance->PushLog(key, tLog);
}

/*============================================
* FuncName    : pushmsgbyname
* Description :
* @key        :
* @fmt        :
* @--         :
* Author      :
* Time        : 2017-06-06
============================================*/
void pushmsgbyname(const char *key, void *msg, unsigned int msglen, char *fmt, ...)
{
    MemoryLog *pInstance = MemoryLog::GetInstance();
    if(!pInstance->CheckPushLog(key))
        return;
    T_MLOG tLog = {0};
    char buf[1024];

    va_list ap;
    char *pmsg = NULL;
    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);
    memset(&tLog, 0 ,sizeof(tLog));
    snprintf(tLog.tipsinfo, sizeof(tLog.tipsinfo), "%s", buf);
    pInstance->PushLog(key, tLog);


    if(NULL != msg || 0!= msglen) // record msg
    {
        if(!pInstance->CheckPushLog(key))
            return;
        pmsg = (char *)malloc(msglen);
        if(!pmsg)
            return;
        memset(pmsg, 0, msglen);
        memcpy(pmsg, msg, msglen);
        timeval now;
        timeval end;
        gettimeofday(&now, NULL);
        sprintf(buf, "msg:%p, len:%u;bf time: s-us: %u-%u",
                msg, msglen,now.tv_sec, now.tv_usec);
        memset(&tLog, 0 ,sizeof(tLog));
        snprintf(tLog.tipsinfo, sizeof(tLog.tipsinfo), "%s", buf);
        tLog.msgaddr = pmsg;
        tLog.msglen = msglen;
        pInstance->PushLog(key, tLog);


        if(!pInstance->CheckPushLog(key))
            return;
        gettimeofday(&end, NULL);
        sprintf(buf, "msg:%p, len:%u;af time: s-us: %u-%u ;elpse time::%-3u ms",
                msg, msglen,end.tv_sec, end.tv_usec,timeval_diff(&end, &now));

        memset(&tLog, 0 ,sizeof(tLog));
        snprintf(tLog.tipsinfo, sizeof(tLog.tipsinfo), "%s", buf);
        pInstance->PushLog(key, tLog);
    }


}




/*============================================
* FuncName    : showmlogbyname
* Description :
* @key        :
* Author      :
* Time        : 2017-06-05
============================================*/
void showmlogbyname( const char *key)
{
    MemoryLog *pInstance = MemoryLog::GetInstance();
    pInstance->ShowLogByName(key);

}

/*============================================
* FuncName    : showmlogall
* Description :
* @           :
* Author      :
* Time        : 2017-06-05
============================================*/
void showmlogall()
{
    MemoryLog *pInstance = MemoryLog::GetInstance();
    pInstance->ShowLogAll();
}

/*============================================
* FuncName    : clearmlogbyname
* Description :
* @key        :
* Author      :
* Time        : 2017-06-05
============================================*/
void clearmlogbyname( const char *key)
{
    MemoryLog *pInstance = MemoryLog::GetInstance();
    pInstance->ClearLogByName(key);
}

/*============================================
* FuncName    : clearmlogall
* Description :
* @           :
* Author      :
* Time        : 2017-06-05
============================================*/
void clearmlogall()
{
    MemoryLog *pInstance = MemoryLog::GetInstance();
    pInstance->ClearLogAll();
}
/*============================================
* FuncName    : showmlogkeys
* Description :
* @           :
* Author      :
* Time        : 2017-06-05
============================================*/
void showmlogkeys()
{
    MemoryLog *pInstance = MemoryLog::GetInstance();
    pInstance->ShowLogKeys(stdout);
}

/*============================================
* FuncName    : findmlogkeys
* Description :
* @           :
* Author      :
* Time        : 2017-06-05
============================================*/
void findmlogkeys(const char *name)
{
    MemoryLog *pInstance = MemoryLog::GetInstance();
    pInstance->FindLogKeys(stdout, name);
}


/*============================================
* FuncName    : savemlog2filebyname
* Description :
* @key        :
* Author      :
* Time        : 2017-06-05
============================================*/
void savemlog2filebyname(const char *key, const char *filewithpath)
{
    MemoryLog *pInstance = MemoryLog::GetInstance();
    pInstance->SaveLog2FileByName(key, filewithpath, true, NULL);
}

/*============================================
* FuncName    : savemlog2fileall
* Description :
* @           :
* Author      :
* Time        : 2017-06-05
============================================*/
void savemlog2fileall(const char *filewithpath)
{
    MemoryLog *pInstance = MemoryLog::GetInstance();
    pInstance->SaveLog2FileAll(filewithpath);
}

/*============================================
* FuncName    : savemlog2filekeys
* Description :
* @           :
* Author      :
* Time        : 2017-06-05
============================================*/
void savemlog2filekeys()
{
    MemoryLog *pInstance = MemoryLog::GetInstance();
    pInstance->SaveLog2FileKeys();
}




/* variable declare begin */
unsigned int mlogmaxsize = 100000;
/* variable declare end */
/*
Set and Get for mlogmaxsize
*/
/*============================================
* FuncName    : set_mlogmaxsize
* Description :
* @_mlogmaxsize  :
* Author      :
* Time        : 2017-06-05
============================================*/
void set_mlogmaxsize(unsigned int _mlogmaxsize)
{
    mlogmaxsize = _mlogmaxsize;
}

/*============================================
* FuncName    : get_mlogmaxsize
* Description :
* @--         :
* Author      :
* Time        : 2017-06-05
============================================*/
unsigned int get_mlogmaxsize(void)
{
    return mlogmaxsize;
}


/* variable declare begin */
static int mlogformat = 1;
/* variable declare end */
/*
Set and Get for mlogformat
*/
/*============================================
* FuncName    : set_mlogformat
* Description :
* @_mlogformat  : the data format : erase the first data when full or keep the full
*                0 - keep the full
*                1 - erase the first data (default)
* Author      :
* Time        : 2017-06-10
============================================*/
void set_mlogformat(int _mlogformat)
{
    mlogformat = _mlogformat;
}

/*============================================
* FuncName    : get_mlogformat
* Description :
* @--         :
* Author      :
* Time        : 2017-06-10
============================================*/
int get_mlogformat(void)
{
    return mlogformat;
}



/*============================================
* FuncName    : mloghelp
* Description :
* @           :
* Author      :
* Time        : 2017-06-10
============================================*/
void mloghelp()
{
    const char *usage = "memory log help:\n"
    "push log by name     --   pushlogbyname(const char *key, char *fmt, ...)\n"
    "push msg by name     --   pushmsgbyname(const char *key, *msg, unsigned int msglen, char *fmt, ...)\n"
    "show log by name     --   showmlogbyname( const char *key)\n"
    "show log of all      --   showmlogall()\n"
    "clear log by name    --   clearmlogbyname( const char *key)\n"
    "clear log of all     --   clearmlogall()\n"
    "show mlog keys       --   showmlogkeys()\n"
    "find mlog keys by name      --   findmlogkeys(const char *name)\n"
    "save log 2file by name      --   savemlog2filebyname(const char *key)\n"
    "save log 2file of all       --   savemlog2fileall(const char *filewithpath)\n"
    "save log 2file of keys      --   savemlog2filekeys()\n"
    "set mlog store max size     --   set_mlogmaxsize(unsigned int _mlogmaxsize)\n"
    "get mlog store max size     --   get_mlogmaxsize(void)\n"
    ;
    printf(usage);
    printf("mlog max size     :%u\n", get_mlogmaxsize());
    printf("mlog record format:%u\n", get_mlogformat());
    printf("    0 - no overwrite, 1 - overwrite\n");
    printf("mlog version      :%s\n", MemoryLog::GetInstance()->Version());
}




#ifdef __cplusplus
}
#endif

