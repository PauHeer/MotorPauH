#pragma once

class App;

class Module
{
public:
	Module() {}
	Module(App* parent) : active(false), app(parent)
	{}

	virtual ~Module() {}

	void Init()
	{
		active = true;
	}

	virtual bool Awake()
	{
		return true;
	}

	virtual bool Start()
	{
		return true;
	}

	virtual bool PreUpdate(float dt)
	{
		return true;
	}

	virtual bool Update(float dt)
	{
		return true;
	}

	virtual bool PostUpdate(float dt)
	{
		return true;
	}

	virtual bool CleanUp()
	{
		return true;
	}

	void Enable() {
		if (!active)
		{
			active = true;
			Start();
		}
	}

	void Disable() {
		if (active)
		{
			active = false;
			CleanUp();
		}
	}

public:

	bool active = true;
	App* app = nullptr;
};