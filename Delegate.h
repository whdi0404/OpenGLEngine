#pragma once

class GenericClass {};
const int SINGLE_MEMFUNCPTR_SIZE = sizeof(void (GenericClass::*)());

struct MicrosoftMFP {
	void (GenericClass::*codeptr)();
	int delta;
	int vtable_index;
};

template <int N>
struct SimpleMemFunc {
	template <class X, class XFuncType, class GenericMemFuncType>
	inline static GenericClass* Convert(X *pthis, XFuncType function_to_bind,
		GenericMemFuncType &bound_func) {
		typedef char ERROR_Unspported_member_function_pointer[N - 100];
	}
};

template <>
struct SimpleMemFunc<SINGLE_MEMFUNCPTR_SIZE>
{
	template <class X, class XFuncType, class GenericMemFuncType>
	inline static GenericClass* Convert(X *pthis, XFuncType function_to_bind, GenericMemFuncType &bound_func)
	{
		bound_func = horrible_cast<GenericMemFuncType>(function_to_bind);
		return reinterpret_cast<GenericClass *>(pthis);
	}
};

template <>
struct SimpleMemFunc<SINGLE_MEMFUNCPTR_SIZE * 2> {
	template <class X, class XFuncType, class GenericMemFuncType>
	inline static GenericClass* Convert(X *pthis, XFuncType function_to_bind,
		GenericMemFuncType &bound_func) {
		union {
			XFuncType func;
			struct {
				GenericMemFuncType func_ptr;
				int                delta;
			} s;
		} u;

		typedef int ERROR_CantUsehorrible_cast[sizeof(function_to_bind) == sizeof(u.s) ? 1 : -1];
		u.func = function_to_bind;
		bound_func = u.s.func_ptr;
		return reinterpret_cast<GenericClass*>(reinterpret_cast<char*>(pthis) + u.s.delta);
	}
};

struct GenericVirualClass : virtual public GenericClass
{
	typedef GenericVirualClass* (GenericVirualClass::*ProbePtrType)();
	GenericVirualClass* GetThis() { return this; }
};

template <>
struct SimpleMemFunc<SINGLE_MEMFUNCPTR_SIZE * 3> {
	template <class X, class XFuncType, class GenericMemFuncType>
	inline static GenericClass* Convert(X *pthis, XFuncType function_to_bind,
		GenericMemFuncType &bound_func) {
		union {
			XFuncType func;
			GenericClass *(X::*ProbeFunc)();
			MicrosoftMFP s;
		} u;
		u.func = function_to_bind;
		bound_func = reinterpret_cast<GenericMemFuncType>(u.s.codeptr);

		union {
			GenericVirualClass::ProbePtrType vfunc;
			MicrosoftMFP s;
		} u2;

		typedef int ERROR_CantUsehorrible_cast[sizeof(function_to_bind) == sizeof(u.s) &&
			sizeof(function_to_bind) == sizeof(u.ProbeFunc) &&
			sizeof(u2.vfunc) == sizeof(u2.s) ? 1 : -1];

		u2.vfunc = &GenericVirualClass::GetThis;
		u.s.codeptr = u2.s.codeptr;

		return (pthis->*u.ProbeFunc)();
	}
};

template <>
struct SimpleMemFunc<SINGLE_MEMFUNCPTR_SIZE * 4> {
	template <class X, class XFuncType, class GenericMemFuncType>
	inline static GenericClass* Convert(X *pthis, XFuncType function_to_bind,
		GenericMemFuncType &bound_func) {
		union {
			XFuncType func;
			struct {
				GenericMemFuncType func_ptr;
				int delta;
				int vtordisp;
				int vtable_index;
			} s;
		} u;

		typedef int ERROR_CantUsehorrible_cast[sizeof(function_to_bind) == sizeof(u.s) ? 1 : -1];

		u.func = function_to_bind;
		bound_func = u.s.func_ptr;

		int virtual_delta = 0;
		if (u.s.vtable_index) {
			const int* vtable = *reinterpret_cast<const int* const*> (
				reinterpret_cast<const char*>(pthis) + u.s.vtordisp);

			virtual_delta = u.s.vtordisp + *reinterpret_cast<const int*> (
				reinterpret_cast<const char*>(vtable) + u.s.vtable_index);
		}

		return reinterpret_cast<GenericClass*>(
			reinterpret_cast<char*>(pthis) + u.s.delta + virtual_delta);
	}
};

#define Delegate(retType, delName, params, putParams)\
class delName\
{\
public:\
	template <class X, class XMemFunc>\
	inline void bind(const X* pthis, XMemFunc function_to_bind)\
	{\
		pthis_ = SimpleMemFunc<sizeof(function_to_bind)>::Convert(const_cast<X*>(pthis), function_to_bind, pMemfunc_);\
	}\
	\
	template <class XMemFunc>\
	inline void bind(XMemFunc function_to_bind)\
	{\
		pthis_ = nullptr;\
		pfunc_ = horrible_cast<GenericFuncType>(function_to_bind);\
	}\
	\
	retType operator() params\
	{\
		return pthis_ != nullptr ? (pthis_->*pMemfunc_)putParams : (pfunc_)putParams;\
	}\
private:\
	typedef retType (GenericClass::*GenericMemFuncType) params;\
	typedef retType (*GenericFuncType) params;\
	GenericClass *pthis_;\
	GenericMemFuncType pMemfunc_;\
	GenericFuncType pfunc_;\
}