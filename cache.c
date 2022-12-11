/**
 * 컴퓨터구조 1분반 Cache simulator
 * @ author		:	32202970 윤예진
 * @ date		:	22-12-11
**/

/* 트레이스 예시
 * ex) s 0x1fffff50 1
 *		연산종류(1)		태그(5)		세트 인덱스(2)	오프셋(1)	third field
 *			s			 0x1ffff		f5				0			1
 * 
 * 세트 인덱스: 시뮬레이션에서 사용하는 세트 수는 256이므로 2자리(16진수)
 * 오프셋: 블록의 데이터 크기가 16바이트이므로 1자리(16진수)
 * 태그: 16진수 주소에서 세트 인덱스와 오프셋을 제외한 나머지 비트 수이므로 5자리(16진수)
**/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include "cache.h"

/* < init_cache >
* @desc : 캐시 구조체 메모리 할당
* @param : Cache* myCache (초기화할 캐시 구조체)
* @return : void
*/
void init_cache(Cache* myCache)
{
	myCache->set = (CacheBlock**)malloc(sizeof(CacheBlock*) * SET_NUM);
	for (int i = 0; i < SET_NUM; i++) {
		myCache->set[i] = (CacheBlock*)malloc(sizeof(CacheBlock) * BLCK_NUM);

		for (int j = 0; j < BLCK_NUM; j++) {
			// 블록 초기화
			init_block(&(myCache->set[i][j]));
		}
	}
}


/* < init_block >
* @desc : 블록 초기화하는 함수
* @param : CacheBlock* block (초기화할 블록)
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
* @desc : 트레이스 구조체 생성 및 초기화
* @param : void
* @return : Trace* (초기화된 트레이스 구조체)
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
* @desc : 트레이스 기록 읽고 값 저장하는 함수
* @param : FILE* fp_trace (트레이스 기록 파일), Trace* trace (트페이스 기록 저장할 구조체)
* @return : 정상 - true, EOF 도달 - false
*/
bool get_trace(FILE* fp_trace, Trace* trace)
{
	char buff[MAX_BUFF] = { 0, };		//입력 버퍼

	// 캐시 시뮬레이터에 필요한 크기(MAX_BUFF)만큼 읽어서 버퍼에 저장
	for (int i = 0; i < MAX_BUFF - 1; i++) {
		buff[i] = fgetc(fp_trace);
		// EOF
		if (buff[i] == -1)
			return false;
	}
	buff[MAX_BUFF - 1] = 0;				// 마지막 '\0'
	while (fgetc(fp_trace) != '\n');	// 남은 문자가 있다면 줄바꿈까지 읽음

	/* 트레이스 값 저장 */
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
* @desc : 트레이스 기록의 태그(16진수)를 10진수로 변환하는 함수
* @param : char* buff_tag (읽어온 버퍼)
* @return : int (태그 10진수 변환값)
*/
int tag_to_int(char* buff_tag)
{
	int tag = 0;			// 태그 10진수 변환값
	int tag_bit = 0;		// 태그 비트 1자리

	for (int i = 4; i < 9; i++)
	{
		if (isdigit(buff_tag[i])) tag_bit = buff_tag[i] - '0';
		else if (isalpha(buff_tag[i])) tag_bit = buff_tag[i] - 'a' + 10;

		// 태그 비트의 한 자리가 0이면 다음 자리 읽어옴
		if (tag_bit == 0) continue;

		tag += (int)pow(16, 8 - i) * tag_bit;			// 리틀 엔디안 방식에 유의하여 변환
	}

	return tag;
}


/* < set_to_int >
* @desc : 트레이스 기록의 세트 인덱스(16진수)를 10진수로 변환하는 함수
* @param : char* buff_tag (읽어온 버퍼)
* @return : int (세트 인덱스 10진수 변환값)
*/
int set_to_int(char* buff_set)
{
	int set = 0;			// 세트 인덱스 10진수 변환값
	int set_bit = 0;		// 세트 인덱스 비트 1자리

	for (int i = 9; i < 11; i++)
	{
		if (isdigit(buff_set[i])) set_bit = buff_set[i] - '0';
		else if (isalpha(buff_set[i])) set_bit = buff_set[i] - 'a' + 10;

		// 세트 인덱스 비트의 한 자리가 0이면 다음 자리 읽어옴
		if (set_bit == 0) continue;

		set += (int)pow(16, 10 - i) * set_bit;		// 리틀 엔디안 방식에 유의하여 변환
	}

	return set;
}


/* < offset_to_int >
* @desc : 트레이스 기록의 오프셋(16진수)를 10진수로 변환하는 함수
* @param : char* buff_offset (읽어온 버퍼)
* @return : int (오프셋 10진수 변환값)
*/
int offset_to_int(char* buff_offset)
{
	int offset = -1;			// 오프셋 10진수 변환값

	if (isdigit(buff_offset[12])) offset = buff_offset[12] - '0';
	else if (isalpha(buff_offset[12])) offset = buff_offset[12] - 'a' + 10;

	return offset;
}


/* < search_cach >
* @desc : 트레이스 값에 따라 캐시 탐색하는 함수
* @param : Cache* myCache (탐색할 캐시), Trace* trace (트레이스 기록)
* @return : char, 0 = miss / 1 = hit
*/
char search_cache(Cache* myCache, Trace* trace)
{
	CacheBlock* nowSet = myCache->set[trace->set];		// 현재 탐색할 세트
	for (int i = 0; i < BLCK_NUM; i++)
	{
		// 태그가 일치할 경우
		if (nowSet[i].tag == trace->tag)
		{
			trace->blck = i;			// 블록 인덱스 저장

			// valid가 유효할 경우, hit
			if (nowSet[i].valid == 1)
				return 1;
			// 아닐 시, miss
			else
				return 0;
		}
	}
	// 탐색 실패 시, miss
	return 0;
}


/* < load_cache >
* @desc : 캐시 로드연산 처리하는 함수
* @param : Cache* myCache (탐색할 캐시), Trace* trace (트레이스 기록), char flag_hit (히트 여부)
* @return : void
*/
void load_cache(Cache* myCache, Trace* trace, char flag_hit)
{
	int count = (BLKC_BYTE - (trace->offset)) / 4;		// 로드 횟수 (4byte 단위)

	/* Hit */
	if (flag_hit == 1) {
		// 캐시 -> 프로세서 로드
		load_hit++;				// 로드 히트 카운트
		cycle += 1 * count;		// 로드 횟수만큼 사이클 추가
	}
	/* Miss */
	else if (flag_hit == 0) {
		load_miss++;			// 로드 미스 카운트

		// 메모리에서 데이터 불러와 캐시에 저장할 위치(valid가 아닌 블록) 탐색
		for (int i = 0; i < BLCK_NUM; i++) {
			if (myCache->set[trace->set][i].valid == 0) {
				trace->blck = i;
				break;
			}
		}
		// 저장 위치 탐색 실패하였을 경우
		if (trace->blck == -1) {
			//LRU 기법, 캐시 블록 해제 후 해당 블록 위치 리턴받음
			trace->blck = evict_block(myCache, trace->set);
		}

		// 메모리 -> 캐시 -> 프로세서
		for (int i = 0; i < count; i++) {
			cycle += 100;		// 메모리 -> 캐시
			cycle += 1;			// 캐시 -> 프로세서
		}

		// 메모리에서 새로 로드하였으므로 valid bit, tag 설정
		myCache->set[trace->set][trace->blck].valid = 1;
		myCache->set[trace->set][trace->blck].tag = trace->tag;
	}

	// 다른 valid 블록의 접근 시간 업데이트, 새로 가져온 블록의 접근 시간 1로 설정
	update_time(myCache);
	myCache->set[trace->set][trace->blck].time = 1;
}


/* < store_cache >
* @desc : 캐시 저장연산 처리하는 함수
* @param : Cache* myCache (탐색할 캐시), Trace* trace (트레이스 기록), char flag_hit (히트 여부)
* @return : void
*/
void store_cache(Cache* myCache, Trace* trace, char flag_hit)
{
	int count = (BLKC_BYTE - (trace->offset)) / 4;			// 저장 횟수 (4byte 단위)

	/* Hit: write - back (캐시에만 저장, 캐시에서 해제할 시 메모리에 저장) */
	if (flag_hit == 1) {
		store_hit++;			// 저장 히트 카운트
		cycle += 1 * count;
		myCache->set[trace->set][trace->blck].dirtyBit = 1;		// dirty bit on
	}

	/* Miss: write - allocate (캐시와 메모리 동시에 쓰기) */
	else if (flag_hit == 0) {
		store_miss++;			// 저장 미스 카운트

		// 캐시에 데이터 저장할 위치(valid가 아닌 블록) 탐색
		for (int i = 0; i < BLCK_NUM; i++) {
			if (myCache->set[trace->set][i].valid == 0) {
				trace->blck = i;
				break;
			}
		}

		// 저장 위치 탐색 실패하였을 경우
		if (trace->blck == -1) {
			//LRU 기법, 캐시 블록 해제 후 해당 블록 위치 리턴받음
			trace->blck = evict_block(myCache, trace->set);
		}

		// 프로세서 -> 캐시 -> 메모리
		for (int i = 0; i < count; i++) {
			cycle += 1;			// 프로세서 -> 캐시
			cycle += 100;		// 캐시 -> 메모리
		}

		// 새로 데이터를 저장하였으므로 valid bit, tag 설정
		myCache->set[trace->set][trace->blck].valid = 1;
		myCache->set[trace->set][trace->blck].tag = trace->tag;
	}

	// 다른 valid 블록의 접근 시간 업데이트, 새로 가져온 블록의 접근 시간 1로 설정
	update_time(myCache);
	myCache->set[trace->set][trace->blck].time = 1;
}


/* < evict_block >
* @desc : 캐시 블록 해제하는 함수
* @param : Cache* myCache (탐색할 캐시), int set_index (해제할 블록을 찾을 세트 인덱스)
* @return : int (해제한 블록의 인덱스)
*/
int evict_block(Cache* myCache, int set_index) {
	int blck_index = 0;			// 해제한 블록의 인덱스
	int old = 0;				// 가장 오래된 접근 시간
	CacheBlock* nowSet = myCache->set[set_index];		// 현재 접근할 세트

	// LRU(least recently used)기법 사용, 접근된 지 가장 오래된 블록 탐색
	for (int i = 0; i < BLCK_NUM; i++) {
		if (nowSet[i].time > old) {
			old = nowSet[i].time;
			blck_index = i;
		}
	}

	// dirty bit가 활성화되어 있다면 write - back 기법에 따라 메모리에 데이터 저장
	if (nowSet[blck_index].dirtyBit == 1)
		cycle += 100 * 4;

	// 해제한 블록의 값 초기화
	nowSet[blck_index].dirtyBit = 0;
	nowSet[blck_index].tag = -1;
	nowSet[blck_index].time = 0;
	nowSet[blck_index].valid = 0;

	return blck_index;
}


/* < update_time >
* @desc : load / store 연산 시 valid 블록의 마지막 접근 시간 업데이트하는 함수
* @param : Cache* myCache (탐색할 캐시)
* @return : void
*/
void update_time(Cache* myCache)
{
	CacheBlock* nowSet = NULL;

	// valid 블록이라면 마지막 접근 시간 1 증가
	for (int i = 0; i < SET_NUM; i++) {
		nowSet = myCache->set[i];
		for (int j = 0; j < BLCK_NUM; j++)
			if (nowSet[j].valid == 1) nowSet[j].time++;
	}
}