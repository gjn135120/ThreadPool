/*************************************************************************
	> File Name: head.h
	> Author: gjn135120
	> Mail:765164038@qq.com 
	> Created Time: Thu 11 Sep 2014 10:44:44 PM CST
 ************************************************************************/
#ifndef _HEAD_H__
#define _HEAD_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <dirent.h>
#define SIZE 4
#define CNT 100
typedef struct tag_work
{
	char path[256];
	char buf[128];
	struct tag_work *next;
}WORK,*pWORK;

typedef struct tag_worker
{
	pthread_t id;
	struct tag_crew *grp;
}WORKER,*pWORKER;

typedef struct tag_crew
{
	pWORK first,last;
	pthread_mutex_t lock;
	pthread_cond_t on;
	pthread_cond_t off;
	int cnt;
	int over;
	WORKER arr[CNT];
}GROUP,*pGROUP;

void group_init(pGROUP grp);
void group_start(pGROUP grp, char *path, char *buf);
size_t path_len;
size_t name_len;
#endif

