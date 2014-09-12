/*************************************************************************
	> File Name: func.c
	> Author: gjn135120
	> Mail:765164038@qq.com 
	> Created Time: Fri 12 Sep 2014 09:27:48 AM CST
 ************************************************************************/
#include "head.h"
int get_task(pGROUP grp, pWORK task)
{
	pWORK pnew;
	int flag;
	pthread_mutex_lock(&grp->lock);
	while(!(grp->over)&&grp ->first == NULL)
		pthread_cond_wait(&grp->on,&grp->lock);
	if(grp->over == 1)
		flag = 1;
	else
	{		
		pnew = grp->first;
		*task = *pnew;
		grp->first = grp->first ->next;
		free(pnew);
		if(grp->first == NULL)
		{
			grp->last = NULL;
		}
		flag = 0;
	}
	pthread_mutex_unlock(&grp->lock);
	return flag;
}
void add_task(pGROUP grp, pWORK task)
{
	pthread_mutex_lock(&grp->lock);
	if(grp->first == NULL)
	{
		grp->first = task;
		grp->last = task;
	}
	else
	{
		grp->last->next = task;
		grp->last = grp->last->next;
	}
	grp->cnt ++;
	pthread_cond_broadcast(&grp->on);
	pthread_mutex_unlock(&grp->lock);
}

void find(char *path, char *buf)
{
	FILE *p = NULL;
	p = fopen(path, "rb");
	if(p == NULL )
	{
		printf("open file error!!\n");
		exit(1);
	}
	char msg[1024];
	while(memset(msg, 0, 1024), fgets(msg, 1024, p))
	{
		if(strstr(msg, buf) != NULL)
		{
			printf("%s\n", path);
			fclose(p);
			return ;
		}
	}
	fclose(p);
}
void make_task(pGROUP grp, pWORK task)
{
	struct stat msg;
	memset(&msg, 0, sizeof(msg));
	if(lstat(task->path, &msg) == -1)
	{
		printf("lstat error !!\n");
		return ;
	}
	if(S_ISDIR(msg.st_mode))
	{
		DIR *fp = NULL;
		struct dirent *p;
		if((fp = opendir(task->path)) == NULL)
		{
			printf("opendir error!!\n");
			exit(1);
		}
	   while((p = readdir(fp))!=NULL)
	   {
		   if(strncmp(p->d_name,".",1) == 0 || strncmp(p->d_name,"..",2) == 0 )
			   continue;
		   pWORK pnew = (pWORK)calloc(1, sizeof(WORK));
		   strcpy(pnew->path, task->path);
		   strcat(pnew->path, "/");
		   strcat(pnew->path, p->d_name);
		   strcpy(pnew->buf, task->buf);
		   add_task(grp, pnew);
	   }	
	   closedir(fp);
	}
	else if(S_ISREG(msg.st_mode))
	{
		find(task->path, task->buf);
	}
	else
		return;
}

int back(pGROUP grp)
{
	int flag;
	pthread_mutex_lock(&grp->lock);
	grp->cnt --;
	if(grp->cnt <= 0)
	{
		grp->over = 1;
		pthread_cond_broadcast(&grp->on);	
		flag = 1;
	}
	else
		flag = 0;
	pthread_mutex_unlock(&grp->lock);
	return flag;
}
void* hand(void *arg)
{
//	pthread_detach(pthread_self());
	pWORKER worker = (pWORKER)arg;
	WORK task;
	while(1)
	{
		if(get_task(worker->grp, &task) == 1)
			break;
		make_task(worker->grp, &task);
		if(back(worker->grp) == 1)
			break;
	}
}
void group_init(pGROUP grp)
{
	grp->first = NULL;
	grp->last = NULL;
	pthread_mutex_init(&grp->lock, NULL);
	pthread_cond_init(&grp->on, NULL);
	pthread_cond_init(&grp->off, NULL);
	grp->cnt = 0;
	grp->over = 0;
	int index;
	for(index = 0; index < CNT; index ++)
	{
		grp->arr[index].grp = grp;
		pthread_create(&(grp->arr[index].id), NULL, hand, (void*)&(grp->arr[index]));
	}
}

void group_start(pGROUP grp, char *path, char *buf)
{
	pWORK pnew = (pWORK)calloc(1, sizeof(WORK));
	strcpy(pnew->path, path);
	strcpy(pnew->buf, buf);
	add_task(grp, pnew);
}

void group_stop(pGROUP grp)
{
	pthread_mutex_destroy(&grp->lock);
	pthread_cond_destroy(&grp->on);
	pthread_cond_destroy(&grp->off);
}
