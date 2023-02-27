#pragma once

#include <assert.h>

class Chunk
{
private:
	unsigned char* prough_data_;
	unsigned char first_useable_block;
	unsigned char useable_block_count;
public:
	Chunk(std::size_t block_size, unsigned char blocks)
	{
		// 기본 초기화
		prough_data_ = NULL;
		first_useable_block = 0;
		useable_block_count = 0;

		// 에러 체크
		if (0 == block_size)
			return;

		if (0 == blocks)
			return;

		// 작업 초기화

		// 할당
		prough_data_ = new unsigned char[block_size * blocks];
		first_useable_block = 0;
		useable_block_count = blocks;

		// 인덱스를 이용한 단방향 링크드 리스트 구성 시킨다.
		unsigned char i = 0;
		while (i < blocks)
		{
			unsigned char* p = &prough_data_[i * block_size];
			/*
			* 블록 별 맨 앞에 인덱스 정보를 저장한다.
			*/
			*p = ++i;
		}
	}

	virtual ~Chunk()
	{
		delete prough_data_;
	}

public:
	void* alloc(std::size_t block_size)
	{
		// 에러 체크
		if (0 == useable_block_count)
			return 0;

		// 인덱스로 접근하여, 사용할 수 있는 메모리 얻어옴
		unsigned char* prough_block = &prough_data_[block_size * first_useable_block];

		// 단방향 링크드 재구성
		// 인덱스를 가져와, 현재 사용 중인 블록 정보를 갱신한다.
		first_useable_block = *prough_block;
		--useable_block_count;

		return prough_block;
	}

	void delloc(void* pdelloc_target, std::size_t block_size)
	{
		// 에러 체크
		// 블록 사이즈가 0이면 종료
		assert(0 != block_size);
		// 할당 데이터 영역 주소보다 값이 작으면 종료
		assert(prough_data_ <= pdelloc_target);

		// 계산을 편하게 하기 위해서 형변환
		unsigned char* to_release = static_cast<unsigned char*>(pdelloc_target);

		// 에러 체크
		// 해제할 주소 덩어리가 block 단위가 아니라면
		assert(((to_release - prough_data_) % block_size) == 0);

		// 실제로 해제하는 작업은 없다. 단지 단방향 링크드 리스트 재구성만 할뿐
		*to_release = first_useable_block;
		first_useable_block = (unsigned char)((to_release - prough_data_) / block_size);
		++useable_block_count;

		/*  모든 에러를 잡아 주지 않는다.
			첫번째로 할당한 메모리를 빼오고, 다시 해제 하려 할 때,
			block_size를 아무것이나 줘도 정상 처리 되는 것을 확인할 수 있으며,
			더 있을 것이다.(책에서도 이런 말을 한다.)

			예)

			Chunk myChunk(4,20);

			void *p = myChunk.alloc(4);
			myChunk.delloc(p, 4); // 정상
			myChunk.delloc(p, 2); // 비정상 상태이나 에러 발생하지 않는다.
		*/
	}

	bool is_useable()
	{
		return 0 != useable_block_count;
	}

	bool is_has(void* p, std::size_t block_size, std::size_t block_num)
	{
		unsigned char* ptarget = static_cast<unsigned char*>(p);

		if (prough_data_ > ptarget)
			return false;

		if (&prough_data_[block_size * block_num] <= ptarget)
			return false;

		return true;
	}
};