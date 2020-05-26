#pragma once

#include <list>
#include <map>
#include <memory>
#include "CSysSync.h"

//T是缓存的类型
template <class T>
class CResourceCache
{
private:
	DISALLOW_COPY_AND_ASSIGN(CResourceCache);

public:
	CResourceCache() { INIT_LOCK(&cs); }
	virtual ~CResourceCache() { DESTROY_LOCK(&cs); }

public:
	std::shared_ptr<T> tryLoadFromCache(const char* filename);
	void addToCache(const char* filename, std::shared_ptr<T> item);
	void flushCache();

protected:
	using T_UseMap = std::map<std::string, std::shared_ptr<T>>;

	T_UseMap UseMap;
	lock_type cs;
};

template <class T>
std::shared_ptr<T> CResourceCache<T>::tryLoadFromCache(const char* filename)
{
	assert(!isAbsoluteFileName(filename) && isNormalized(filename) && isLowerFileName(filename));

	BEGIN_LOCK(&cs);

	auto itrUse = UseMap.find(filename);
	if (itrUse != UseMap.end())
	{
		std::shared_ptr<T> t = itrUse->second;
		END_LOCK(&cs);
		return t;
	}

	END_LOCK(&cs);
	return nullptr;
}

template <class T>
void CResourceCache<T>::addToCache(const char* filename, std::shared_ptr<T> item)
{
	assert(!isAbsoluteFileName(filename) && isNormalized(filename) && isLowerFileName(filename));

	BEGIN_LOCK(&cs);

	assert(UseMap.find(filename) == UseMap.end());
	UseMap[filename] = item;

	END_LOCK(&cs);
}

template <class T>
void CResourceCache<T>::flushCache()
{
	for (auto itr : UseMap)
	{
		auto count = itr.second.use_count();
		assert(itr.second.use_count() == 1);
		if (itr.second.use_count() == 1)
			itr.second.reset();
	}
}



