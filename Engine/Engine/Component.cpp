#include "Component.h"

Component::Component(GameObject* owner, ComponentType type) : active(true), gameObject(owner), type(type)
{
}

Component::~Component()
{
}

void Component::Update()
{
}

void Component::OnEditor()
{
}

void Component::Enable()
{
}

void Component::Disable()
{
}