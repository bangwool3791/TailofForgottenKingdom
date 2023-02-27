#include <iostream>

#include "chunk.h"
#include <vector>

/*
* Ŭ���� ��� �޾Ƽ�, Prefab, GameObject�뵵 ���� ���� ��.
*/
class ChunkGroup
{
public:
	/*
	* �� ��ü ������ ��� �޸� ����
	*/
	~ChunkGroup()
	{
		for (int i = 0; i < chunks_.size(); ++i)
		{
			delete chunks_[i];
			chunks_[i] = NULL;
		}

		chunks_.clear();
	}

	ChunkGroup(std::size_t block_size, int block_num)
	{
		block_size_ = block_size;
		block_num_ = block_num;

		pcurrent_chunk_ = NULL;
	}

public:
	void* alloc()
	{
		if (pcurrent_chunk_ == NULL ||
			false == pcurrent_chunk_->is_useable())
		{
			// ûũ�� �߿��� ��� �� �� �ִ� ���� �ִ��� ã�´�.
			std::vector<Chunk*>::iterator bit = chunks_.begin();
			std::vector<Chunk*>::iterator eit = chunks_.end();

			for (; bit != eit; ++bit)
			{
				if (false == (*bit)->is_useable())
					continue;

				pcurrent_chunk_ = *bit;
				break;
			}

			// ���ٸ� �����Ѵ�.
			if (bit == eit)
			{
				Chunk* p = new Chunk(block_size_, block_num_);
				pcurrent_chunk_ = p;
				chunks_.push_back(p);
			}
		}

		return pcurrent_chunk_->alloc(block_size_);
	}

	void delloc(void* pdelloc_target)
	{
		assert(pcurrent_chunk_ != NULL);

		// �ֱ� ûũ���� ������ �ִ� �޸����� �˻��Ѵ�.
		if (true == pcurrent_chunk_->is_has(pdelloc_target, block_size_, block_num_))
		{
			pcurrent_chunk_->delloc(pdelloc_target, block_size_);
			return;
		}

		// ûũ�� �߿� �ִ��� ã�� ����.
		std::vector<Chunk*>::iterator bit = chunks_.begin();
		std::vector<Chunk*>::iterator eit = chunks_.end();

		for (; bit != eit; ++bit)
		{
			if (false == (*bit)->is_has(pdelloc_target, block_size_, block_num_))
				continue;

			(*bit)->delloc(pdelloc_target, block_size_);
			pcurrent_chunk_ = *bit;
			break;
		}

		// ������ ���� ���� �޸𸮸� �����Ϸ� �ߴ��� üũ
		assert(bit != eit);

	}

private:
	std::size_t block_size_;
	unsigned char block_num_;

	std::vector<Chunk*> chunks_;
	Chunk* pcurrent_chunk_;
};
