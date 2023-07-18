/// @file noncopyable.h
/// @brief ���ɿ��������װ
/// @author yanguojin
/// @email 1023438782@qq.com
/// @date 2023-07-17
#ifndef __YGJ_NONCOPYABLE_H__
#define __YGJ_NONCOPYABLE_H__

namespace ygj_server {
/// @brief �����޷�����,��ֵ
class NonCopyable {
public:

	/// @brief Ĭ�Ϲ��캯��
    NonCopyable() = default;

	/// @brief Ĭ����������
    ~NonCopyable() = default;

	/// @brief �������캯��(����) 
    NonCopyable(const NonCopyable&) = delete;

	/// @brief ��ֵ����(����)
    NonCopyable& operator=(const NonCopyable&) = delete;
};

}

#endif
