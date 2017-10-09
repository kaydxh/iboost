/*
选取比较常见的接收2个参数的情况。
支持普通函数/函数指针、成员函数指针。
兼容函数对象、函数适配器/boost::bind
*/

#include <iostream>

template <typename R, typename T1, typename T2>
class Base {
public:
    virtual ~Base() {

    }

    virtual R operator () (T1, T2) = 0;
};

//实现一个普通函数／函数指针的版本
template <typename R, typename T1, typename T2>
class Func : public Base<R, T1, T2> {
public:
    Func(R (*ptr)(T1, T2))
        : ptr_(ptr) {

        }

    virtual R operator() (T1 a, T2 b) {
        return ptr_(a, b);
    }

private:
    R (*ptr_) (T1, T2);

};

//实现成员函数指针的版本
template <typename R, typename Class, typename T>
class Member : public Base<R, Class, T> { 

};

template <typename R, typename Class, typename T>
class Member<R, Class*, T> : public Base<R, Class*, T> {
public:
    Member(R (Class::*ptr) (T))
        : ptr_(ptr) {

        }

    virtual R operator () (Class *obj, T a) {
        return (obj->*ptr_)(a);
    }

private:
    R (Class::*ptr_) (T); //成员函数指针类型，调用时，例如传&Point::get
};

//实现函数对象的版本
template <typename T, typename R, typename T1, typename T2>
class Functor : public Base<R, T1, T2> {
public:
    Functor(const T& obj)
        : obj_(obj) {

        }

    virtual R operator() (T1 a, T2 b) {
        return obj_(a, b);
    }   
private:
    T obj_;
};

template <typename T>
class Function {//空类的作用：主要是利用模板偏特化来进行类型萃取，正常的Function声明的时候，比如Function<int (int, int)>而不是Func<int, int, int>。

};

template <typename R, typename T1, typename T2>
class Function<R (T1, T2) > {
public:
    template <typename Class, typename _R, typename _T2>
    Function(_R (Class::*ptr)(_T2))
        : ptr_(new Member<R, T1, T2>(ptr)) {

        }

    template <typename _R, typename _T1, typename _T2>
    Function(_R (*ptr) (_T1, _T2))
        : ptr_(new Func<R, T1, T2>(ptr)) {

        }

    template <typename T>
    Function(const T& obj)
        : ptr_(new Functor<T, R, T1, T2>(obj)) {

    }

    ~Function() {
        delete ptr_;
    }

    virtual R operator() (T1 a, T2 b) {
        return ptr_->operator()(a, b);
    }

private:
    Base<R, T1, T2> *ptr_;

};

//test
int get(int a, int b) {
    std::cout << a + b << std::endl;
}

class Point {
public:
    int get(int a) {
        std::cout << "Point::get called: a = " << a << std::endl;
        return a;
    }
 
    int doit(int a, int b) {
        std::cout << "Point::get called: a + b = " << a + b << std::endl;
        return a + b;
    }

};

int main() {
    Function<int(int ,int)> foo(get);
    foo(10.1, 10.3);

    Function<int (Point*, int)> bar(&Point::get);
    Point point;
    bar(&point, 30);

  //  Function<int (int, int)> obj(boost::bind(&Point::doit, &point, _1, _2));
   // obj(90, 100);
}

/*
[xhding@centos-linux iboost]$ ./function
20
Point::get called: a = 30
*/
