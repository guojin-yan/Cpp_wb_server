/// @file noncopyable.h
/// @brief 不可拷贝对象封装
/// @author yanguojin
/// @email 1023438782@qq.com
/// @date 2023-07-17
#ifndef __YGJ_NONCOPYABLE_H__
#define __YGJ_NONCOPYABLE_H__

namespace ygj_server {
/// @brief 对象无法拷贝,赋值
class NonCopyable {
public:

	/// @brief 默认构造函数
    NonCopyable() = default;

	/// @brief 默认析构函数
    ~NonCopyable() = default;

	/// @brief 拷贝构造函数(禁用) 
    NonCopyable(const NonCopyable&) = delete;

	/// @brief 赋值函数(禁用)
    NonCopyable& operator=(const NonCopyable&) = delete;
};

}

#endif
