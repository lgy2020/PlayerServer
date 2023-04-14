#pragma once
#include <unistd.h>
#include <sys/types.h>
#include <functional>
// CFunctionBase* m_func; 进程类里构造时候没法确认，因此需要使用CFunctionBase多态
class CFunctionBase
{
public:
	virtual ~CFunctionBase() {}
	virtual int operator()() = 0;
};

template<typename _FUNCTION_, typename... _ARGS_>
class CFunction :public CFunctionBase
{
public:
	CFunction(_FUNCTION_ func, _ARGS_... args)
		:m_binder(std::forward<_FUNCTION_>(func), std::forward<_ARGS_>(args)...)
	{}
	virtual ~CFunction() {}
	virtual int operator()() {
		return m_binder();
	}
	typename std::_Bindres_helper<int, _FUNCTION_, _ARGS_...>::type m_binder;
};