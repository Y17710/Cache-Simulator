/**
 * ��ǻ�ͱ��� 1�й� Cache simulator
 * @ author		:	32202970 ������
 * @ date		:	22-12-11
**/

#pragma once
#ifndef CACHE_H
#define CACHE_H

#define TAG_SIZE 5			// �±� ��Ʈ ������
#define BLCK_NUM 4			// ���(����) ����
#define SET_NUM 256			// ��Ʈ ����
#define MAX_BUFF 15			// Ʈ���̽� ���� �Է� ������
#define BLKC_BYTE 16		// ��� �޸� ������

/* �ùķ��̼� ��� ���� */
extern int load_hit;
extern int load_miss;
extern int store_hit;
extern int store_miss;
extern int cycle;

/* struct CacheBlock */
typedef struct cacheblock {
	char valid;			// valid bit, 0=unvalid 1=valid
	char dirtyBit;		// dirtyBit, 0=off 1=on
	int time;			// ������ ���� �ð�, 0=not valid, 1=�ּڰ�
	int tag;			// tag bit, 10���� ǥ��
} CacheBlock;

/* struct cache */
typedef struct cache {
	CacheBlock** set;	// set * block
} Cache;

/* struct trace */
// trace ������ �����صδ� ����ü
typedef struct trace {
	char flag_ls;		// 'l'=load 's'=store
	int tag;			// tag bit, 10���� ǥ��
	int set;			// set bit, 10���� ǥ��
	int offset;			// offset bit, 10���� ǥ��
	int blck;			// ��Ʈ ���� ��� �ε���
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
