#pragma offload_attribute(push,_Cilk_shared)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#pragma offload_attribute(pop)

typedef struct
{
	int cardnum;
	char *name;
	int sex;
}Student ;


_Cilk_shared void offloadfunction(Student *lab ,int n)
{
	int i=0;
	printf(" \n after offload1 %d \n",lab[0].cardnum);
	printf(" \n after offload2 %s \n",lab[0].name);
	char *abc=(char *)malloc(sizeof(char )*10);
	memcpy(abc,lab[0].name,3);
	printf(" \n after offload2 %s \n",abc);
}

int main() {

	int i;
	Student *lab=(Student *)_Offload_shared_malloc(sizeof(Student)*10);
	char *namet="abc";
	for(i=0;i<10;i++)
	{
		lab[i].name=(char *)_Offload_shared_malloc(sizeof(char)*10);
		memcpy(lab[i].name,namet,3);
		lab[i].cardnum=i;
	}
	_Cilk_offload offloadfunction(lab,10);
	return 0;
}
