#include "common.h"

typedef struct OSThread_s {
    s32 dummy[1];
} OSThread;

typedef void *OSMesg;

typedef struct OSMesgQueue_s {
    OSThread *mtqueue;
    OSThread *fullqueue;
    s32 validCount;
    s32 first;
    s32 msgCount;
    OSMesg *msg;
} OSMesgQueue;

extern struct __osThreadTail {
    OSThread *next;
    s32 priority;
} D_80042CB0;
#define __osThreadTail D_80042CB0

void func_80137700(OSMesgQueue *mq, OSMesg *msg, s32 msgCount) {
    mq->mtqueue = (OSThread *) &__osThreadTail.next;
    mq->fullqueue = (OSThread *) &__osThreadTail.next;
    mq->validCount = 0;
    mq->first = 0;
    mq->msgCount = msgCount;
    mq->msg = msg;
}
