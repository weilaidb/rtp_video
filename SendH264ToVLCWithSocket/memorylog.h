#ifndef MEMORYLOG_H
#define MEMORYLOG_H

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <stdarg.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

using namespace std;

#define MLOG_VERSION ("2.2")



/*
** the memory log store info
*/
typedef struct T_MLOG{
    void *msgaddr          ;    /* store the address of msgs */
    unsigned int msglen    ;    /* the message len           */
    char tipsinfo[128]     ;    /* tips info                 */
}T_MLOG, *P_MLOG;





class MemoryLog
{
public:
    MemoryLog();
    void PushLog(const char *key, T_MLOG &tlog);
    bool CheckPushLog(const char *key);
    void ShowLogByName(const char *key, bool index);
    void ParseMsgBody(T_MLOG &tlog, FILE *fp);
    void ShowLogAll();
    void ClearLogByName(const char *key, bool tips, bool lock);
    void ClearLogAll();
    void ShowLogKeys(FILE *fp);
    void FindLogKeys(FILE *fp, const char *name);
    void SaveLog2FileByName(const char *key, const char *filewithpath, bool tips, FILE *fother);
    void SaveLog2FileAll(const char *filewithpath);
    void SaveLog2FileKeys(FILE *felse);


public:
    typedef std::vector<T_MLOG> MLOG_VEC;
    typedef std::vector<T_MLOG>::iterator MLOG_VEC_IT;
    typedef std::map<std::string, std::vector<T_MLOG> > MLOG_MAP;
    typedef std::map<std::string, std::vector<T_MLOG> >::iterator MLOG_MAP_IT;
    typedef std::pair<std::string, std::vector<T_MLOG> > MLOG_MAP_PAIR;
    typedef std::pair<std::map<std::string, std::vector<T_MLOG> >, bool > MLOG_MAP_RET;

private:
    MLOG_MAP mlog;
    pthread_mutex_t mutex;

private:
    static MemoryLog *pInstance;

public:
    static MemoryLog * GetInstance();
    static const char *Version();

};


#ifdef __cplusplus
extern "C" {
#endif

/* func_declare_begin */
void pushmsgbyname(const char *key, void *msg, unsigned int msglen, char *fmt, ...);
void pushlogbyname(const char *key, char *fmt, ...);
void showmlogbyname( const char *key);
void showmlogall();
void clearmlogbyname( const char *key);
void clearmlogall();
void showmlogkeys();
void findmlogkeys(const char *name);
void savemlog2filebyname(const char *key, const char *filewithpath);
void savemlog2fileall(const char *filewithpath);
void savemlog2filekeys();
void set_mlogmaxsize(unsigned int _mlogmaxsize);
unsigned int get_mlogmaxsize(void);
void set_mlogformat(int _mlogformat);
int get_mlogformat(void);
void mloghelp();
/* func_declare_end */


#ifdef __cplusplus
}
#endif

#endif // MEMORYLOG_H
