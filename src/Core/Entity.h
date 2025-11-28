#pragma once

#include "Core.h"

using ComponentTuple = std::tuple<
	CCollision,
	CInput,
	CLifespan,
	CScore,
	CShape,
	CTransform
>;

class Entity
{
	ComponentTuple m_components;
	bool m_alive = true;
	std::string m_tag = "default";
	size_t m_id = 0;

public:
	Entity() {}
	Entity(size_t id, const std::string& tag) : m_id(id), m_tag(tag) {}

	template <typename T>
	T& get()
	{
		return std::get<T>(m_components);
	}

	template <typename T>
	const T& get() const
	{
		return std::get<T>(m_components);
	}

	template <typename T>
	bool has() const
	{
		return get<T>().exists;
	}

	template <typename T>
	void remove()
	{
		get<T>() = T();
	}

	template <typename T, typename... TArgs>
	T& add(TArgs&&... mArgs)
	{
		auto& component = get<T>();
		component = T(std::forward<TArgs>(mArgs)...);
		component.exists = true;
		return component;
	}

	size_t id() const
	{
		return m_id;
	}

	bool isAlive() const
	{
		return m_alive;
	}

	std::string& tag()
	{
		return m_tag;
	}

	void destroy()
	{
		m_alive = false;
	}
};