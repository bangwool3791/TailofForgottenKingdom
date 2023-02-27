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
		// �⺻ �ʱ�ȭ
		prough_data_ = NULL;
		first_useable_block = 0;
		useable_block_count = 0;

		// ���� üũ
		if (0 == block_size)
			return;

		if (0 == blocks)
			return;

		// �۾� �ʱ�ȭ

		// �Ҵ�
		prough_data_ = new unsigned char[block_size * blocks];
		first_useable_block = 0;
		useable_block_count = blocks;

		// �ε����� �̿��� �ܹ��� ��ũ�� ����Ʈ ���� ��Ų��.
		unsigned char i = 0;
		while (i < blocks)
		{
			unsigned char* p = &prough_data_[i * block_size];
			/*
			* ��� �� �� �տ� �ε��� ������ �����Ѵ�.
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
		// ���� üũ
		if (0 == useable_block_count)
			return 0;

		// �ε����� �����Ͽ�, ����� �� �ִ� �޸� ����
		unsigned char* prough_block = &prough_data_[block_size * first_useable_block];

		// �ܹ��� ��ũ�� �籸��
		// �ε����� ������, ���� ��� ���� ��� ������ �����Ѵ�.
		first_useable_block = *prough_block;
		--useable_block_count;

		return prough_block;
	}

	void delloc(void* pdelloc_target, std::size_t block_size)
	{
		// ���� üũ
		// ��� ����� 0�̸� ����
		assert(0 != block_size);
		// �Ҵ� ������ ���� �ּҺ��� ���� ������ ����
		assert(prough_data_ <= pdelloc_target);

		// ����� ���ϰ� �ϱ� ���ؼ� ����ȯ
		unsigned char* to_release = static_cast<unsigned char*>(pdelloc_target);

		// ���� üũ
		// ������ �ּ� ����� block ������ �ƴ϶��
		assert(((to_release - prough_data_) % block_size) == 0);

		// ������ �����ϴ� �۾��� ����. ���� �ܹ��� ��ũ�� ����Ʈ �籸���� �һ�
		*to_release = first_useable_block;
		first_useable_block = (unsigned char)((to_release - prough_data_) / block_size);
		++useable_block_count;

		/*  ��� ������ ��� ���� �ʴ´�.
			ù��°�� �Ҵ��� �޸𸮸� ������, �ٽ� ���� �Ϸ� �� ��,
			block_size�� �ƹ����̳� �൵ ���� ó�� �Ǵ� ���� Ȯ���� �� ������,
			�� ���� ���̴�.(å������ �̷� ���� �Ѵ�.)

			��)

			Chunk myChunk(4,20);

			void *p = myChunk.alloc(4);
			myChunk.delloc(p, 4); // ����
			myChunk.delloc(p, 2); // ������ �����̳� ���� �߻����� �ʴ´�.
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