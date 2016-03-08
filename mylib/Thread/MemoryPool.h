#pragma once

#include "Lock.h"
// #include <vector>
#include <list>
#include "Utils/Time.h"

typedef _W64 long OffsetType;

struct _NoExtra {

};

template <typename T, typename ExtraType = _NoExtra>
class MemoryPool: public Mutex {

protected:
	struct Block {

		T						item;

		struct {
			unsigned short		stamp;
			ExtraType			extra;
		} metadata;
	};

public:

	MemoryPool(void)
	{
		_buffer = NULL;
		_total = 0;
	}

	~MemoryPool(void)
	{

	}

	bool init(size_t blockCount)
	{
		_buffer = (Block* )malloc(sizeof(Block) * blockCount);
		if (_buffer == NULL)
			return false;

#ifdef _DEBUG
		memset(_buffer, 0, sizeof(Block) * blockCount);
#endif

		for (size_t i = 0; i < blockCount; i ++) {

			Block* blk = _buffer + i;
			_queue.push_back(blk);
		}

		_total = blockCount;
		return true;
	}

	void uninit()
	{
		if (_buffer)
			free(_buffer);

		_queue.clear();
	}

	T* allocObj(OffsetType& offset, unsigned short* stamp = NULL)
	{
		MutexGuard guard(this);

		if (_queue.empty())
			return NULL;

		Block* blk = _queue.back();
		_queue.pop_back();

		try {
			new(blk)Block();
		} catch(...) {

			_queue.push_back(blk);
			throw;
		}

		offset = blk - _buffer;
		unsigned short theStamp = genStamp();
		if (stamp != NULL)
			*stamp = theStamp;

		setStamp(blk, theStamp);

		return &blk->item;
	}

	void freeObj(T* item)
	{
		MutexGuard guard(this);

		Block* blk = (Block* )item;

		try {

			blk->~Block();
#ifdef _DEBUG
			memset(blk, 0, sizeof(Block));
#endif
		} catch (...) {

			_queue.push_back(blk);
			throw;
		}

		_queue.push_back(blk);
		setStamp(blk, 0);
	}

	T* alloc(OffsetType& offset, unsigned short* stamp = NULL)
	{
		return allocObj(offset, stamp);
	}

	void free(T* item)
	{
		freeObj(item);
	}

	T* getBlock(OffsetType offset, unsigned short* stamp = NULL)
	{
		MutexGuard guard(this);

		if ((size_t )offset > _total)
			return NULL;

		Block* blk = &_buffer[offset];

		unsigned short blkStamp = getStamp(blk);
		
		if (stamp != NULL) {

			if (blkStamp != *stamp)
				return NULL;

		} else if ( blkStamp == 0)
			return NULL;

		return &blk->item;
	}

	size_t getTotal() const 
	{ 
		return _total; 
	}

	size_t getUsed() const 
	{
		return _total - _queue.size();
	}
	
	size_t getRemain() const 
	{
		return _queue.size();
	}

	static ExtraType* getExtra(T* item)
	{
		Block* blk = (Block* )item;
		return &blk->metadata.extra
	}

protected:

	static unsigned short getStamp(Block* blk)
	{
		return blk->metadata.stamp;
	}

	static void setStamp(Block* blk, unsigned short stamp)
	{
		blk->metadata.stamp = stamp;
	}

	static unsigned short genStamp()
	{
		unsigned short stamp = (unsigned short )(Utils::getMsTime() >> 8);
		return stamp == 0 ? 1 : stamp;
	}

protected:
	Block*					_buffer;
	std::list<Block* >		_queue;
	size_t					_total;
};

//////////////////////////////////////////////////////////////////////////

#define PtrInRange(Buff, BuffSize, Ptr) \
	( ULONG_PTR(Ptr) >= ULONG_PTR(Buff) && ULONG_PTR(Ptr) < (ULONG_PTR(Buff) + ULONG_PTR(BuffSize)) )

template <typename T, int BufLvl = 3, typename ExtraType = _NoExtra>
class MemoryPoolEx: public Mutex {

protected:
	struct Block {

		T						item;

		struct {
			unsigned short		stamp;
			ExtraType			extra;
		} metadata;
	};

	struct Buffer {
		Block*					buffer;
		size_t					blkSize;
	};

public:

	MemoryPoolEx(void)
	{
		memset(_buffers, 0, sizeof(_buffers));
		_total = 0;
		_allocated = 0;
		_bufferNo = 0;
	}

	~MemoryPoolEx(void)
	{

	}

	bool init(size_t initBlocks, size_t blockCount)
	{
		assert(BufLvl >= 2);

		if (!initBuffer(_buffers[_bufferNo], initBlocks))
			return false;

		_total = blockCount;
		_allocated = initBlocks;
		_initBlks =  initBlocks;
		return true;
	}

	void uninit()
	{
		for (size_t i = 0; i <= _bufferNo; i ++)
			if (_buffers[i])
				free(_buffer[i]);

		_queue.clear();
	}

	T* allocObj(OffsetType& offset, unsigned short* stamp = NULL)
	{
		MutexGuard guard(this);

		if (_queue.empty()) {

			if (_allocated >= _total)
				return NULL;
			else if (!extend())
				return NULL;
		}

		Block* blk = _queue.back();
		_queue.pop_back();

		try {
			new(blk)Block();
		} catch(...) {

			_queue.push_back(blk);
			throw;
		}

		offset = getOffset(blk);
		assert(offset != -1);

		unsigned short theStamp = genStamp();
		if (stamp != NULL)
			*stamp = theStamp;

		setStamp(blk, theStamp);

		return &blk->item;
	}

	void freeObj(T* item)
	{
		MutexGuard guard(this);

		Block* blk = (Block* )item;

		try {

			blk->~Block();
#ifdef _DEBUG
			memset(blk, 0, sizeof(Block));
#endif
		} catch (...) {

			_queue.push_back(blk);
			throw;
		}

		_queue.push_back(blk);
		setStamp(blk, 0);
	}

	T* alloc(OffsetType& offset, unsigned short* stamp = NULL)
	{
		return allocObj(offset, stamp);
	}

	void free(T* item)
	{
		freeObj(item);
	}

	T* getBlock(OffsetType offset, unsigned short* stamp = NULL)
	{
		MutexGuard guard(this);

		if ((size_t )offset > _allocated)
			return NULL;

		size_t bufNum = getBufferNumber(offset);

		if (bufNum == -1)
			return NULL;

		Block* blk = &_buffers[bufNum].buffer[offset];

		unsigned short blkStamp = getStamp(blk);

		if (stamp != NULL) {

			if (blkStamp != *stamp)
				return NULL;

		} else if ( blkStamp == 0)
			return NULL;

		return &blk->item;
	}

	size_t getTotal() const 
	{ 
		return _total; 
	}

	size_t getUsed() const 
	{
		return _allocated - _queue.size();
	}

	size_t getAllocated() const 
	{
		return _allocated;
	}

	size_t getRemain() const 
	{
		return _total - _allocated;
	}

	static ExtraType* getExtra(T* item)
	{
		Block* blk = (Block* )item;
		return &blk->metadata.extra
	}

protected:

	static unsigned short getStamp(Block* blk)
	{
		return blk->metadata.stamp;
	}

	static void setStamp(Block* blk, unsigned short stamp)
	{
		blk->metadata.stamp = stamp;
	}

	static unsigned short genStamp()
	{
		unsigned short stamp = (unsigned short )(Utils::getMsTime() >> 8);
		return stamp == 0 ? 1 : stamp;
	}

	bool extend()
	{
		if (_bufferNo >= BufLvl - 1)
			return false;

		const size_t blk = (_total - _initBlks + BufLvl - 2) / (BufLvl - 1);
		const size_t blkSize = (_total - _allocated) > blk ? blk : _total - _allocated;

		if (!initBuffer(_buffers[++ _bufferNo], blkSize))
			return false;

		_allocated += blkSize;
		return true;
	}

	bool initBuffer(Buffer& buffer, size_t blkSize)
	{
		Block* buf = (Block* )malloc(sizeof(Block) * blkSize);
		if (buf == NULL)
			return false;

#ifdef _DEBUG
		memset(buf, 0, sizeof(Block) * blkSize);
#endif

		for (size_t i = 0; i < blkSize; i ++) {

			Block* blk = buf + i;
			_queue.push_back(blk);
		}

		buffer.buffer = buf;
		buffer.blkSize = blkSize;
		return true;
	}

	OffsetType getOffset(Block* blk) const 
	{
		size_t blkNum = 0;

		for (size_t i = 0; i <= _bufferNo; i ++) {

			size_t blkSize = _buffers[i].blkSize;

			if (PtrInRange(_buffers[i].buffer, blkSize * sizeof(Block), blk)) {

				return blkNum + (blk - _buffers[i].buffer);
			}

			blkNum += blkSize;
		}

		return -1;
	}

	size_t getBufferNumber(OffsetType& offset) const 
	{

		size_t blkNum = 0;

		for (size_t i = 0; i <= _bufferNo; i ++) {

			size_t blkSize = _buffers[i].blkSize;

			if ((size_t )offset >= blkNum && (size_t )offset < (blkNum + blkSize)) {

				offset = offset - blkNum;
				return i;
			}

			blkNum += blkSize;
		}

		return -1;
	}

protected:
	Buffer					_buffers[BufLvl];
	std::list<Block* >		_queue;
	size_t					_total;
	size_t					_allocated;
	size_t					_initBlks;
	size_t					_bufferNo;
};
