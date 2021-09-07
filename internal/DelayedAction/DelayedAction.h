#pragma once
#include <functional>
#include <Windows.h>
#include <vector>
typedef std::function<void()> callback;
struct action
{
	callback func;
	float    time;
	bool     executed;
	action(callback f, float t)
	{
		func = f;
		time = GetTickCount64() + t;
		executed = false;
	}
};
class delayAction
{
protected:
	std::vector<action> actionList;
public:
	void add(action a)
	{
		actionList.push_back(a);
	}
	void clear()
	{
		actionList.clear();
	}
	size_t size()
	{
		return actionList.size();
	}
	bool isReady()
	{
		for (auto& a : actionList) {
			if (!a.executed) {
				return false;
			}
		}
		return true;
	}
	void update(float time)
	{
		for (auto iter = actionList.begin(); iter != actionList.end(); ) {
			if ((*iter).executed == false && (*iter).time <= time) {
				(*iter).func();
				(*iter).executed = true;
				iter = actionList.erase(iter);
			}
			else ++iter;
		}
	}
};
extern delayAction DelayedAction;