#pragma once
#ifndef BLOCK_H
#define BLOCK_H

enum class Block
	: unsigned short
{
	Air,
	Stone,
	Dirt,
	Grass,
	Bedrock,
	Log,
	Leaves,
	Planks,
	Cobblestone,
	Glass,
	Pumpkin
};

enum Direction
	: unsigned char
{
	North = 1,
	East = 2,
	South = 4,
	West = 8,
	Zenith = 16,
	Nadir = 32
};

extern bool IsTransparent(const Block block) noexcept;

#endif