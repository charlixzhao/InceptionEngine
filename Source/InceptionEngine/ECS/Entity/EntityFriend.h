#pragma once

namespace inceptionengine
{

	/*
	This class is used to transfer friendship to the vector in World so that we don't
	need to define customed vector allocator. This friendship transfer makes sure that
	only World can instantiate Entity so that user will not be able to create Entity
	by their own. All any creation must go through World::CreateEntity().
	*/
	class EntityFriend
	{
	public:
		EntityFriend() = default;

	private:
		friend class World;
	};
}