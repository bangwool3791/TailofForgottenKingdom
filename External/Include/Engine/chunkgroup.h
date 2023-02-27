#include <iostream>

#include "chunk.h"
#include <vector>

/*
* 클래스 상속 받아서, Prefab, GameObject용도 따로 만들 것.
*/
class ChunkGroup
{
public:
	/*
	* 한 객체 삭제시 모든 메모리 삭제
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
			// 청크들 중에서 사용 할 수 있는 것이 있는지 찾는다.
			std::vector<Chunk*>::iterator bit = chunks_.begin();
			std::vector<Chunk*>::iterator eit = chunks_.end();

			for (; bit != eit; ++bit)
			{
				if (false == (*bit)->is_useable())
					continue;

				pcurrent_chunk_ = *bit;
				break;
			}

			// 없다면 생성한다.
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

		// 최근 청크에서 가지고 있는 메모리인지 검사한다.
		if (true == pcurrent_chunk_->is_has(pdelloc_target, block_size_, block_num_))
		{
			pcurrent_chunk_->delloc(pdelloc_target, block_size_);
			return;
		}

		// 청크들 중에 있는지 찾아 본다.
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

		// 가지고 있지 않은 메모리를 해제하려 했는지 체크
		assert(bit != eit);

	}

private:
	std::size_t block_size_;
	unsigned char block_num_;

	std::vector<Chunk*> chunks_;
	Chunk* pcurrent_chunk_;
};
