#ifndef _COLLISION_STATE_H_
#define _COLLISION_STATE_H_

enum CollisionLocation
{
	HITPILLAR,
	HITNOTHING,
	HITPASSAGE
};

struct CollisionState
{
	CollisionLocation location;
	int pillarPassageId;
};


#endif