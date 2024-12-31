#include "GameObject.h"
#include <iostream> // Para los logs

GameObject::GameObject(const char* name, GameObject* parent) : parent(parent), name(name)
{
    transform = new ComponentTransform(this);
    mesh = new ComponentMesh(this);
    material = new ComponentMaterial(this);

    AddComponent(transform);
}

GameObject::~GameObject()
{
    std::cout << "Destroying GameObject: " << this << std::endl;
}

void GameObject::Update()
{
    if (isActive)
    {
        for (auto* component : components)
        {
            component->Update();
        }
        for (auto* child : children)
        {
            child->Update();
        }
    }
}

Component* GameObject::AddComponent(Component* component)
{
    components.push_back(component);

    return component;
}

Component* GameObject::GetComponent(ComponentType type)
{
    for (auto* component : components)
    {
        if (component->type == type)
        {
            return component;
        }
    }

    return nullptr;
}