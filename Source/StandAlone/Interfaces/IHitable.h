#pragma once

class IHitable
{
public:
	virtual ~IHitable() = default;

	virtual void GetHit() = 0;
};