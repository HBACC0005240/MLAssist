#pragma once
enum DiagonalMovement
{
	None=0,
	Always = 1,
	Never = 2,
	IfAtMostOneObstacle = 3,
	OnlyWhenNoObstacles = 4
};