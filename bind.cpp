#include <iostream>

namespace {
	struct Placeholders1 {
	}_1;

	struct Placeholders2 {
	}_2;
}

class Base0 {
};

template <typename T1>
class Base1 : public Base0 {
	public:
		Base1(T1 data1)
			: data1_(data1) {

			}

	protected:
		T1 data1_;
};

template <typename T1, typename T2>
class Base2 : public Base1<T1> {
	public:
		Base2(T1 data1, T2 data2)
			: Base1<T1>(data1),
			  data2_(data2) {

			  }
		
	protected:
		T2 data2_;
};

class CallList0 : public Base0 {
	public:
		template <typename _T>
		_T operator [] (_T arg) {
			return arg;
		}
};

template <typename T1>
class CallList1 : public Base1<T1> {
	public:
		CallList1(T1 data1)
			: Base1<T1>(data1) {

			}

		T1 operator[] (Placeholders1 arg) {
			return Base1<T1>::data1_;
		}

		template <typename _T>
		_T operator [] (_T arg) {
			return arg;
		}
};

template <typename T1, typename T2>
class CallList2 : public Base2<T1, T2> {
	public:
		CallList2(T1 data1, T2 data2) 
			: Base2<T1, T2>(data1, data2) {

			}

		T1 operator[] (Placeholders1 arg1) {
			return Base2<T1, T2>::data1_;
		}

		T2 operator[] (Placeholders2 arg2) {
			return Base2<T1, T2>::data2_;
		}

		template <typename _T>
		_T operator[] (_T arg) {
			return arg;
		}
};

class BindList0 : public Base0 {
	public:
		template <typename Func>
		void operator() (Func func) {
			func();
		}
};

template <typename T1>
class BindList1 : public Base1<T1> {
	public:
		BindList1(T1 data1)
			: Base1<T1> (data1) {

			}

		template <typename Func, typename Call>
		void operator() (Func func, Call call) {
			func(call[Base1<T1>::data1_]);
		}
};

template <typename T1, typename T2>
class BindList2 : public Base2<T1, T2> {
	public:
		BindList2(T1 data1, T2 data2)
			: Base2<T1, T2>(data1, data2) {

			}


		template <typename Func, typename Call>
		void operator() (Func func, Call call) {
			func(call[Base2<T1, T2>::data1_], call[Base2<T1, T2>::data2_]);
		}
};

template <typename Func, typename Bind>
class BindImpl {
	public:
		BindImpl(Func func, Bind bindlist)
			: func_(func),
			  bindlist_(bindlist) {

			  }

		void operator() () {
			bindlist_(func_);
		}

		template <typename T1>
		void operator() (T1 data1) {
			bindlist_(func_, CallList1<T1>(data1));
		}

		template <typename T1, typename T2>
		void operator() (T1 data1, T2 data2) {
			bindlist_(func_, CallList2<T1, T2>(data1, data2));
		}

	protected:
		Func func_;
		Bind bindlist_;
};

template <typename Func>
BindImpl<Func, BindList0> bind(Func func) {
	return BindImpl<Func, BindList0>(func, BindList0());
}

template <typename Func, typename T1>
BindImpl<Func, BindList1<T1> > bind(Func func, T1 data1) {
	return BindImpl<Func, BindList1<T1> >(func, BindList1<T1>(data1));
}

template <typename Func, typename T1, typename T2>
BindImpl<Func, BindList2<T1, T2> > bind(Func func, T1 data1, T2 data2) {
	return BindImpl<Func, BindList2<T1, T2> >(func, BindList2<T1, T2>(data1, data2));
}


//test

int get (int a, int b) {
	std::cout << a + b << std::endl;
	return 0;
}

class Point {
	public:
		int operator() (int a, int b) {
			std::cout << "Point::operator() called: a + b = " << a + b << std::endl;
			return a+b;
		}
};

int main() {

	bind(get, _1, 10)(20);
	bind(Point(), _1, _2)(3, 4);

	return 0;
}

/*
[xhding@centos-linux iboost]$ ./bind
30
Point::operator() called: a + b = 7
*/
