#ifndef __PICK_IMAGE_H__
#define __PICK_IMAGE_H__
#include <functional>
/**
 * ѡȡͼ�񲢱�������,��ɺ���� completion(sucess).
 * ���й�������ͼ�����л����� transition()
 */
void pickImage(const std::function<void()>& transition,
               const std::function<void(cocos2d::Texture2D*)>& completion);

#endif // __PICK_IMAGE_H__