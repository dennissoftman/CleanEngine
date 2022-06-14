#include "client/dummyuimanager.hpp"

UIManager *UIManager::create()
{
    return new DummyUIManager();
}

DummyUIManager::DummyUIManager()
{

}

void DummyUIManager::init(Renderer *rend)
{
    (void)rend;
}

void DummyUIManager::update(double dt)
{
    (void)dt;
}

void DummyUIManager::draw()
{
    
}

void DummyUIManager::addElement(std::shared_ptr<UIElement> el)
{
    (void)el;
}

void DummyUIManager::removeElement(std::shared_ptr<UIElement> el)
{
    (void)el;
}
