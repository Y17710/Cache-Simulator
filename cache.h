/**
 * 컴퓨터구조 1분반 Cache simulator
 * @ author		:	32202970 윤예진
 * @ date		:	22-12-11
**/

#pragma once
#ifndef CACHE_H
#define CACHE_H

#define TAG_SIZE 5			// 태그 비트 사이즈
#define BLCK_NUM 4			// 블록(라인) 개수
#define SET_NUM 256			// 세트 개수
#define MAX_BUFF 15			// 트레이스 한줄 입력 사이즈
#define BLKC_BYTE 16		// 블록 메모리 사이즈

/* 시뮬레이션 결과 저장 */
extern int load_hit;
extern int load_miss;
extern int store_hit;
extern int store_miss;
extern int cycle;

/* struct CacheBlock */
typedef struct cacheblock {
	char valid;			// valid bit, 0=unvalid 1=valid
	char dirtyBit;		// dirtyBit, 0=off 1=on
	int time;			// 마지막 접근 시간, 0=not valid, 1=최솟값
	int tag;			// tag bit, 10진수 표기
} CacheBlock;

/* struct cache */
typedef struct cache {
	CacheBlock** set;	// set * block
} Cache;

/* struct trace */
// trace 정보를 저장해두는 구조체
typedef struct trace {
	char flag_ls;		// 'l'=load 's'=store
	int tag;			// tag bit, 10진수 표기
	int set;			// set bit, 10진수 표기
	int offset;			// offset bit, 10진수 표기
	int blck;			// 세트 안의 블록 인덱스
} Trace;

void init_cache(Cache* myCache);
void init_block(CacheBlock* block);
Trace* create_trace(void);

bool get_trace(FILE* fp_trace, Trace* trace);
int tag_to_int(char* buff_tag);
int set_to_int(char* buff_set);
int offset_to_int(char* buff_offset);

char search_cache(Cache* myCache, Trace* trace);
void load_cache(Cache* myCache, Trace* trace, char flag_hit);
void store_cache(Cache* myCache, Trace* trace, char flag_hit);
int evict_block(Cache* myCache, int set_index);

void update_time(Cache* myCache);

#endif
