#pragma once

class IHitable
{
public:
	virtual ~IHitable() = default;

	virtual void GetHit(IHitable* attacker, float damage) = 0;

	virtual void BeBlocked() = 0;
};