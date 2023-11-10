#include <memory>

#include "client/uimanager.hpp"

UIManager* UIManager::create()
{
    return new UIManager();
}

UIManager::UIManager()
{

}

UIManager::~UIManager()
{

}

void UIManager::init(Renderer *rend)
{

}

void UIManager::update(double dt)
{

}

void UIManager::draw()
{

}

void UIManager::addElement(std::shared_ptr<UIElement> el)
{

}

void UIManager::removeElement(std::shared_ptr<UIElement> el)
{

}
