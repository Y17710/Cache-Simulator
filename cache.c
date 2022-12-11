/**
 * ��ǻ�ͱ��� 1�й� Cache simulator
 * @ author		:	32202970 ������
 * @ date		:	22-12-11
**/

/* Ʈ���̽� ����
 * ex) s 0x1fffff50 1
 *		��������(1)		�±�(5)		��Ʈ �ε���(2)	������(1)	third field
 *			s			 0x1ffff		f5				0			1
 * 
 * ��Ʈ �ε���: �ùķ��̼ǿ��� ����ϴ� ��Ʈ ���� 256�̹Ƿ� 2�ڸ�(16����)
 * ������: ����� ������ ũ�Ⱑ 16����Ʈ�̹Ƿ� 1�ڸ�(16����)
 * �±�: 16���� �ּҿ��� ��Ʈ �ε����� �������� ������ ������ ��Ʈ ���̹Ƿ� 5�ڸ�(16����)
**/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include "cache.h"

/* < init_cache >
* @desc : ĳ�� ����ü �޸� �Ҵ�
* @param : Cache* myCache (�ʱ�ȭ�� ĳ�� ����ü)
* @return : void
*/
void init_cache(Cache* myCache)
{
	myCache->set = (CacheBlock**)malloc(sizeof(CacheBlock*) * SET_NUM);
	for (int i = 0; i < SET_NUM; i++) {
		myCache->set[i] = (CacheBlock*)malloc(sizeof(CacheBlock) * BLCK_NUM);

		for (int j = 0; j < BLCK_NUM; j++) {
			// ��� �ʱ�ȭ
			init_block(&(myCache->set[i][j]));
		}
	}
}


/* < init_block >
* @desc : ��� �ʱ�ȭ�ϴ� �Լ�
* @param : CacheBlock* block (�ʱ�ȭ�� ���)
* @return : void
*/
void init_block(CacheBlock* block)
{
	block->valid = 0;
	block->dirtyBit = 0;
	block->time = 0;
	block->tag = -1;
}


/* < create_trace >
* @desc : Ʈ���̽� ����ü ���� �� �ʱ�ȭ
* @param : void
* @return : Trace* (�ʱ�ȭ�� Ʈ���̽� ����ü)
*/
Trace* create_trace(void)
{
	Trace* trace = (Trace*)malloc(sizeof(Trace));
	trace->flag_ls = 0;
	trace->offset = 0;
	trace->set = 0;
	trace->tag = 0;
	trace->blck = -1;
}


/* < get_trace >
* @desc : Ʈ���̽� ��� �а� �� �����ϴ� �Լ�
* @param : FILE* fp_trace (Ʈ���̽� ��� ����), Trace* trace (Ʈ���̽� ��� ������ ����ü)
* @return : ���� - true, EOF ���� - false
*/
bool get_trace(FILE* fp_trace, Trace* trace)
{
	char buff[MAX_BUFF] = { 0, };		//�Է� ����

	// ĳ�� �ùķ����Ϳ� �ʿ��� ũ��(MAX_BUFF)��ŭ �о ���ۿ� ����
	for (int i = 0; i < MAX_BUFF - 1; i++) {
		buff[i] = fgetc(fp_trace);
		// EOF
		if (buff[i] == -1)
			return false;
	}
	buff[MAX_BUFF - 1] = 0;				// ������ '\0'
	while (fgetc(fp_trace) != '\n');	// ���� ���ڰ� �ִٸ� �ٹٲޱ��� ����

	/* Ʈ���̽� �� ���� */
	// load or store
	if (buff[0] == 'l') trace->flag_ls = 'l';
	else if (buff[0] == 's') trace->flag_ls = 's';

	// tag, set, offset
	trace->tag = tag_to_int(buff);
	trace->set = set_to_int(buff);
	trace->offset = offset_to_int(buff);

	return true;
}


/* < tag_to_int >
* @desc : Ʈ���̽� ����� �±�(16����)�� 10������ ��ȯ�ϴ� �Լ�
* @param : char* buff_tag (�о�� ����)
* @return : int (�±� 10���� ��ȯ��)
*/
int tag_to_int(char* buff_tag)
{
	int tag = 0;			// �±� 10���� ��ȯ��
	int tag_bit = 0;		// �±� ��Ʈ 1�ڸ�

	for (int i = 4; i < 9; i++)
	{
		if (isdigit(buff_tag[i])) tag_bit = buff_tag[i] - '0';
		else if (isalpha(buff_tag[i])) tag_bit = buff_tag[i] - 'a' + 10;

		// �±� ��Ʈ�� �� �ڸ��� 0�̸� ���� �ڸ� �о��
		if (tag_bit == 0) continue;

		tag += (int)pow(16, 8 - i) * tag_bit;			// ��Ʋ ����� ��Ŀ� �����Ͽ� ��ȯ
	}

	return tag;
}


/* < set_to_int >
* @desc : Ʈ���̽� ����� ��Ʈ �ε���(16����)�� 10������ ��ȯ�ϴ� �Լ�
* @param : char* buff_tag (�о�� ����)
* @return : int (��Ʈ �ε��� 10���� ��ȯ��)
*/
int set_to_int(char* buff_set)
{
	int set = 0;			// ��Ʈ �ε��� 10���� ��ȯ��
	int set_bit = 0;		// ��Ʈ �ε��� ��Ʈ 1�ڸ�

	for (int i = 9; i < 11; i++)
	{
		if (isdigit(buff_set[i])) set_bit = buff_set[i] - '0';
		else if (isalpha(buff_set[i])) set_bit = buff_set[i] - 'a' + 10;

		// ��Ʈ �ε��� ��Ʈ�� �� �ڸ��� 0�̸� ���� �ڸ� �о��
		if (set_bit == 0) continue;

		set += (int)pow(16, 10 - i) * set_bit;		// ��Ʋ ����� ��Ŀ� �����Ͽ� ��ȯ
	}

	return set;
}


/* < offset_to_int >
* @desc : Ʈ���̽� ����� ������(16����)�� 10������ ��ȯ�ϴ� �Լ�
* @param : char* buff_offset (�о�� ����)
* @return : int (������ 10���� ��ȯ��)
*/
int offset_to_int(char* buff_offset)
{
	int offset = -1;			// ������ 10���� ��ȯ��

	if (isdigit(buff_offset[12])) offset = buff_offset[12] - '0';
	else if (isalpha(buff_offset[12])) offset = buff_offset[12] - 'a' + 10;

	return offset;
}


/* < search_cach >
* @desc : Ʈ���̽� ���� ���� ĳ�� Ž���ϴ� �Լ�
* @param : Cache* myCache (Ž���� ĳ��), Trace* trace (Ʈ���̽� ���)
* @return : char, 0 = miss / 1 = hit
*/
char search_cache(Cache* myCache, Trace* trace)
{
	CacheBlock* nowSet = myCache->set[trace->set];		// ���� Ž���� ��Ʈ
	for (int i = 0; i < BLCK_NUM; i++)
	{
		// �±װ� ��ġ�� ���
		if (nowSet[i].tag == trace->tag)
		{
			trace->blck = i;			// ��� �ε��� ����

			// valid�� ��ȿ�� ���, hit
			if (nowSet[i].valid == 1)
				return 1;
			// �ƴ� ��, miss
			else
				return 0;
		}
	}
	// Ž�� ���� ��, miss
	return 0;
}


/* < load_cache >
* @desc : ĳ�� �ε忬�� ó���ϴ� �Լ�
* @param : Cache* myCache (Ž���� ĳ��), Trace* trace (Ʈ���̽� ���), char flag_hit (��Ʈ ����)
* @return : void
*/
void load_cache(Cache* myCache, Trace* trace, char flag_hit)
{
	int count = (BLKC_BYTE - (trace->offset)) / 4;		// �ε� Ƚ�� (4byte ����)

	/* Hit */
	if (flag_hit == 1) {
		// ĳ�� -> ���μ��� �ε�
		load_hit++;				// �ε� ��Ʈ ī��Ʈ
		cycle += 1 * count;		// �ε� Ƚ����ŭ ����Ŭ �߰�
	}
	/* Miss */
	else if (flag_hit == 0) {
		load_miss++;			// �ε� �̽� ī��Ʈ

		// �޸𸮿��� ������ �ҷ��� ĳ�ÿ� ������ ��ġ(valid�� �ƴ� ���) Ž��
		for (int i = 0; i < BLCK_NUM; i++) {
			if (myCache->set[trace->set][i].valid == 0) {
				trace->blck = i;
				break;
			}
		}
		// ���� ��ġ Ž�� �����Ͽ��� ���
		if (trace->blck == -1) {
			//LRU ���, ĳ�� ��� ���� �� �ش� ��� ��ġ ���Ϲ���
			trace->blck = evict_block(myCache, trace->set);
		}

		// �޸� -> ĳ�� -> ���μ���
		for (int i = 0; i < count; i++) {
			cycle += 100;		// �޸� -> ĳ��
			cycle += 1;			// ĳ�� -> ���μ���
		}

		// �޸𸮿��� ���� �ε��Ͽ����Ƿ� valid bit, tag ����
		myCache->set[trace->set][trace->blck].valid = 1;
		myCache->set[trace->set][trace->blck].tag = trace->tag;
	}

	// �ٸ� valid ����� ���� �ð� ������Ʈ, ���� ������ ����� ���� �ð� 1�� ����
	update_time(myCache);
	myCache->set[trace->set][trace->blck].time = 1;
}


/* < store_cache >
* @desc : ĳ�� ���忬�� ó���ϴ� �Լ�
* @param : Cache* myCache (Ž���� ĳ��), Trace* trace (Ʈ���̽� ���), char flag_hit (��Ʈ ����)
* @return : void
*/
void store_cache(Cache* myCache, Trace* trace, char flag_hit)
{
	int count = (BLKC_BYTE - (trace->offset)) / 4;			// ���� Ƚ�� (4byte ����)

	/* Hit: write - back (ĳ�ÿ��� ����, ĳ�ÿ��� ������ �� �޸𸮿� ����) */
	if (flag_hit == 1) {
		store_hit++;			// ���� ��Ʈ ī��Ʈ
		cycle += 1 * count;
		myCache->set[trace->set][trace->blck].dirtyBit = 1;		// dirty bit on
	}

	/* Miss: write - allocate (ĳ�ÿ� �޸� ���ÿ� ����) */
	else if (flag_hit == 0) {
		store_miss++;			// ���� �̽� ī��Ʈ

		// ĳ�ÿ� ������ ������ ��ġ(valid�� �ƴ� ���) Ž��
		for (int i = 0; i < BLCK_NUM; i++) {
			if (myCache->set[trace->set][i].valid == 0) {
				trace->blck = i;
				break;
			}
		}

		// ���� ��ġ Ž�� �����Ͽ��� ���
		if (trace->blck == -1) {
			//LRU ���, ĳ�� ��� ���� �� �ش� ��� ��ġ ���Ϲ���
			trace->blck = evict_block(myCache, trace->set);
		}

		// ���μ��� -> ĳ�� -> �޸�
		for (int i = 0; i < count; i++) {
			cycle += 1;			// ���μ��� -> ĳ��
			cycle += 100;		// ĳ�� -> �޸�
		}

		// ���� �����͸� �����Ͽ����Ƿ� valid bit, tag ����
		myCache->set[trace->set][trace->blck].valid = 1;
		myCache->set[trace->set][trace->blck].tag = trace->tag;
	}

	// �ٸ� valid ����� ���� �ð� ������Ʈ, ���� ������ ����� ���� �ð� 1�� ����
	update_time(myCache);
	myCache->set[trace->set][trace->blck].time = 1;
}


/* < evict_block >
* @desc : ĳ�� ��� �����ϴ� �Լ�
* @param : Cache* myCache (Ž���� ĳ��), int set_index (������ ����� ã�� ��Ʈ �ε���)
* @return : int (������ ����� �ε���)
*/
int evict_block(Cache* myCache, int set_index) {
	int blck_index = 0;			// ������ ����� �ε���
	int old = 0;				// ���� ������ ���� �ð�
	CacheBlock* nowSet = myCache->set[set_index];		// ���� ������ ��Ʈ

	// LRU(least recently used)��� ���, ���ٵ� �� ���� ������ ��� Ž��
	for (int i = 0; i < BLCK_NUM; i++) {
		if (nowSet[i].time > old) {
			old = nowSet[i].time;
			blck_index = i;
		}
	}

	// dirty bit�� Ȱ��ȭ�Ǿ� �ִٸ� write - back ����� ���� �޸𸮿� ������ ����
	if (nowSet[blck_index].dirtyBit == 1)
		cycle += 100 * 4;

	// ������ ����� �� �ʱ�ȭ
	nowSet[blck_index].dirtyBit = 0;
	nowSet[blck_index].tag = -1;
	nowSet[blck_index].time = 0;
	nowSet[blck_index].valid = 0;

	return blck_index;
}


/* < update_time >
* @desc : load / store ���� �� valid ����� ������ ���� �ð� ������Ʈ�ϴ� �Լ�
* @param : Cache* myCache (Ž���� ĳ��)
* @return : void
*/
void update_time(Cache* myCache)
{
	CacheBlock* nowSet = NULL;

	// valid ����̶�� ������ ���� �ð� 1 ����
	for (int i = 0; i < SET_NUM; i++) {
		nowSet = myCache->set[i];
		for (int j = 0; j < BLCK_NUM; j++)
			if (nowSet[j].valid == 1) nowSet[j].time++;
	}
}