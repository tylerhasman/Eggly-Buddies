#pragma once

#include "engine/common.hpp"
#include "engine/tiny_ecs.hpp"
#include "engine/components.hpp"
#include "engine/tiny_ecs_registry.hpp"

// A simple physics system that moves rigid bodies and checks for collision
class PhysicsSystem
{
public:
	void step(float elapsed_ms);

	PhysicsSystem()
	{
	}

private:
    float maxPhysicsStep = 1000.0f / 60.0f;
};