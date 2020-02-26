#pragma once
#ifndef I_NONCOPYABLE_H
#define I_NONCOPYABLE_H

class INoncopyable
{
protected:
	INoncopyable() = default;
	virtual ~INoncopyable() noexcept = default;

	INoncopyable(const INoncopyable&) = delete;
	INoncopyable& operator =(const INoncopyable&) = delete;
};

#endif