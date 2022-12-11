/**
 * 컴퓨터구조 1분반 Cache simulator
 * @ author		:	32202970 윤예진
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
* @desc : 캐시 시뮬레이터 실행
* @param : void
* @return : int
*/
int main(void)
{
	// 트레이스 파일 열기
	FILE* fp_trace;
	fopen_s(&fp_trace, "./trace/gcc.trace", "r");

	// 캐시 및 트레이스 구조체 생성, 초기화
	Trace* trace = create_trace();
	Cache* myCache = (Cache*)malloc(sizeof(Cache));
	init_cache(myCache);

	char flag_hit = -1;			// 히트 여부, 0=miss 1=hit

	// 더 읽어올 트레이스 기록이 없을 때까지
	while (get_trace(fp_trace, trace) == true)
	{
		// 트레이스 기록에 따라 캐시 탐색, 히트 여부 저장
		flag_hit = search_cache(myCache, trace);

		// load or store에 따라 작업 수행
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

		// 트레이스 및 히트 플래그 초기화
		trace->blck = -1;
		flag_hit = -1;
	}

	// 시뮬레이션 결과 출력
	print_result();

	fclose(fp_trace);
	free(trace);

	for (int i = 0; i < SET_NUM; i++)
		free(myCache->set[i]);
	free(myCache->set);
	free(myCache);
}


/* < print_result >
* @desc : 시뮬레이션 결과 출력하는 함수
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