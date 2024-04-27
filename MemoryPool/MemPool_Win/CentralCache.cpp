#include "CentralCache.h"
#include "PageCache.h"

// 获取中心缓存的唯一实例
CentralCache* CentralCache::get_instance()
{
	static CentralCache* instance = new CentralCache();
	return instance;
}

// 从中心缓存获取一定数量的对象给线程缓存
size_t CentralCache::fetch_range_objs(void*& start, void*& end, size_t batch_num, size_t align_size)
{
	// 根据内存块大小获取对应的SpanList
	size_t index = SizeClass::index(align_size);
	SpanList& span_list = central_cache_span_list_[index];

	//span_list.get_mutex().lock(); // 加锁

	Span* span = get_one_span(span_list, align_size);
	assert(span != nullptr);		// 该SpanList中有Span
	assert(span->list_ != nullptr); // 该Span的list_挂中有内存块

	start = span->list_;	// 起始位置
	end = start;			// 结束位置
	size_t actual_num = 1;	// 实际取出的内存块个数
	// 从span中取出内存块
	while (next_obj(end) != nullptr && actual_num < batch_num - 1)
	{
		end = next_obj(end);
		++actual_num;
	}
	span->list_ = next_obj(end);	// 更新span的list_指向
	next_obj(end) = nullptr;		// 将取出的内存块从span中断开

	//span_list.get_mutex().unlock(); // 解锁

	return actual_num;
}

// 从SpanList获取一个非空Span
Span* CentralCache::get_one_span(SpanList& span_list, size_t align_size)
{
	Span* begin = span_list.begin();
	// 遍历SpanList 找到一个非空的Span
	while (begin != span_list.end())
	{
		// 如果该Span中有内存块 则直接返回
		if (begin->list_ != nullptr)
			return begin;
		begin = begin->next_;
	}

	// 如果SpanList中没有非空的Span 则从PageCache中获取一个Span
	PageCache* page_cache = PageCache::get_instance(); // 获取页缓存的唯一实例
	Span* span = page_cache->new_span(SizeClass::num_move_page(align_size)); // 从页缓存获取一个Span
	assert(span != nullptr);

	char* start = (char*)(span->page_id_ << PAGE_SHIFT);	// 页号转换为起始地址
	size_t bytes = span->page_num_ << PAGE_SHIFT;			// 内存块总大小
	char* end = start + bytes;								// 结束地址

	// 将大块内存切分成小块内存
	size_t num = bytes / align_size;						// 内存块个数
	char* cur = start;								// 当前位置
	char* next = cur + align_size;						// 下一个位置
	for (size_t i = 0; i < num - 1; ++i)
	{
		next_obj(cur) = next;	// 将下一个位置挂到当前位置
		cur = next;				// 移动当前位置
		next = cur + align_size;		// 移动下一个位置
	}
	next_obj(cur) = nullptr;		// 最后一个位置指向nullptr

	span->list_ = start;	// span的list_指向内存块链表
	
	// 将span插入到SpanList中
	//span_list.get_mutex().lock();	// 加锁
	span_list.push_front(span);		// 插入到SpanList中
	//span_list.get_mutex().unlock(); // 解锁

	return span;
}

