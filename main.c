/**
 * ��ǻ�ͱ��� 1�й� Cache simulator
 * @ author		:	32202970 ������
 * @ date		:	22-12-11
**/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include "cache.h"

int load_hit = 0;
int load_miss = 0;
int store_hit = 0;
int store_miss = 0;
int cycle = 0;

void print_result(void);

/* < main >
* @desc : ĳ�� �ùķ����� ����
* @param : void
* @return : int
*/
int main(void)
{
	// Ʈ���̽� ���� ����
	FILE* fp_trace;
	fopen_s(&fp_trace, "./trace/gcc.trace", "r");

	// ĳ�� �� Ʈ���̽� ����ü ����, �ʱ�ȭ
	Trace* trace = create_trace();
	Cache* myCache = (Cache*)malloc(sizeof(Cache));
	init_cache(myCache);

	char flag_hit = -1;			// ��Ʈ ����, 0=miss 1=hit

	// �� �о�� Ʈ���̽� ����� ���� ������
	while (get_trace(fp_trace, trace) == true)
	{
		// Ʈ���̽� ��Ͽ� ���� ĳ�� Ž��, ��Ʈ ���� ����
		flag_hit = search_cache(myCache, trace);

		// load or store�� ���� �۾� ����
		if (trace->flag_ls == 'l') {
			load_cache(myCache, trace, flag_hit);
		}
		else if (trace->flag_ls == 's') {
			store_cache(myCache, trace, flag_hit);
		}
		else {
			printf("flag error\n");
			exit(1);
		}

		// Ʈ���̽� �� ��Ʈ �÷��� �ʱ�ȭ
		trace->blck = -1;
		flag_hit = -1;
	}

	// �ùķ��̼� ��� ���
	print_result();

	fclose(fp_trace);
	free(trace);

	for (int i = 0; i < SET_NUM; i++)
		free(myCache->set[i]);
	free(myCache->set);
	free(myCache);
}


/* < print_result >
* @desc : �ùķ��̼� ��� ����ϴ� �Լ�
* @param : void
* @return : void
*/
void print_result(void)
{
	printf("Total loads:\t%d\n", load_hit + load_miss);
	printf("Total stores:\t%d\n", store_hit + store_miss);
	printf("Load hits:\t%d\n", load_hit);
	printf("Load misses:\t%d\n", load_miss);
	printf("Store hits:\t%d\n", store_hit);
	printf("Store misses:\t%d\n", store_miss);
	printf("Total cycles:\t%d\n", cycle);
}