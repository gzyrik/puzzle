#include "PickImage.h"
void pickImage(const std::function<void()>& transition,
               const std::function<void(cocos2d::Texture2D*)>& completion)
{
    completion(nullptr);
}