/*************************************************************************
	> File Name: test.c
	> Author: gjn135120
	> Mail:765164038@qq.com 
	> Created Time: Fri 12 Sep 2014 10:43:47 AM CST
 ************************************************************************/
#include "head.h"
int main(int argc, char *argv[])
{
	GROUP grp;
	group_init(&grp);
	group_start(&grp, argv[1], argv[2]);
	int index;
	for(index = 0; index < CNT; index ++)
		pthread_join(grp.arr[index].id,NULL);
	return 0;
}
