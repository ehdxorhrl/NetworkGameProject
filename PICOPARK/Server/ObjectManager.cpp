#include "stdafx.h"
#include "ObjectManager.h"

void ObjectManager::Add(CObject* _object)
{
	if (_object == nullptr)return;

	auto findit = find(objects.begin(), objects.end(), _object);
	if (findit != objects.end())return;

	objects.emplace_back(_object);
}

void ObjectManager::Remove(CObject* _object)
{
	auto it = remove(objects.begin(), objects.end(), _object);
	objects.erase(it, objects.end());

	delete _object;
}

void ObjectManager::Clear()
{
	for_each(objects.begin(), objects.end(), [=](CObject* obj) {delete obj; });
	objects.clear();
}
