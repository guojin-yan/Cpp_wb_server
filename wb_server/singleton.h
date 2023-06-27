#ifndef _YGJ_SINGLETON_H_
#define _YGJ_SINGLETON_H_

#include <memory>
namespace ygj_server {

template<class T, class X = void, int N = 0>
class Singleton {
public:
	static T* get_instance() {
		static T v;
		return&v;
	}
};
template<class T, class X = void, int N = 0>
class SingletonPtr {
public:
	static  std::shared_ptr<T> get_instance() {
		static std::shared_ptr<T> v(new T);
		return v;
	}
};
}
#endif //_YGJ_SINGLETON_H_
