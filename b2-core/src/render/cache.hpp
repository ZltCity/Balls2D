#pragma once

#include <map>
#include <memory>

namespace b2::render
{

template<class T>
class Cache
{
public:
	Cache() = default;
	Cache(const Cache &) = delete;
	Cache(Cache &&) noexcept = default;

	Cache &operator=(const Cache &) = delete;
	Cache &operator=(Cache &&) noexcept = default;

	void put(const std::string &id, T value);

	std::shared_ptr<T> get(const std::string &id) const;

private:
	std::map<std::string, std::shared_ptr<T>> collection;
};

template<class T>
void Cache<T>::put(const std::string &id, T value)
{
	collection.insert_or_assign(id, std::make_shared<T>(std::move(value)));
}

template<class T>
std::shared_ptr<T> Cache<T>::get(const std::string &id) const
{
	auto it = collection.find(id);

	return it == collection.end() ? nullptr : it->second;
}

} // namespace b2::render
