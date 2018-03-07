#ifndef __PICK_IMAGE_H__
#define __PICK_IMAGE_H__
#include <functional>
/**
 * 选取图像并保存纹理,完成后调用 completion(sucess).
 * 其中过程中视图发生切换调用 transition()
 */
void pickImage(const std::function<void()>& transition,
               const std::function<void(cocos2d::Texture2D*)>& completion);

#endif // __PICK_IMAGE_H__