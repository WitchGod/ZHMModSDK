#pragma once

#include "Common.h"
#include "EngineFunction.h"

class ZActor;
class ZDynamicObject;
class ZString;

class ZHMSDK_API Functions
{
public:
	static EngineFunction<void(ZActor* th)>* ZActor_OnOutfitChanged;
	static EngineFunction<void(ZActor* th)>* ZActor_ReviveActor;
	static EngineFunction<void(ZDynamicObject* th, ZString* a2)>* ZDynamicObject_ToString;
};