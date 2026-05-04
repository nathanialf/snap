#include "common.h"

typedef u64 OSTime;
typedef void *OSMesg;

typedef struct OSMesgQueue_s {
    s32 dummy[6];
} OSMesgQueue;

typedef struct OSTimer_s {
    struct OSTimer_s *next;
    struct OSTimer_s *prev;
    OSTime interval;
    OSTime value;
    OSMesgQueue *mq;
    OSMesg msg;
} OSTimer;

extern OSTime func_800381E8(OSTimer *);
extern void func_80038174(OSTime);
extern OSTimer *D_80042DA0;

#define __osInsertTimer  func_800381E8
#define __osSetTimerIntr func_80038174
#define __osTimerList    D_80042DA0

s32 func_8013BC80(OSTimer *t, u64 countdown, u64 interval, OSMesgQueue *mq, OSMesg msg) {
    OSTime time;

    t->next = 0;
    t->prev = 0;
    t->interval = interval;
    t->value = (countdown != 0) ? countdown : interval;
    t->mq = mq;
    t->msg = msg;

    time = __osInsertTimer(t);
    if (__osTimerList->next == t) {
        __osSetTimerIntr(time);
    }
    return 0;
}
