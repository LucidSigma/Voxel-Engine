#include "Block.h"

bool IsTransparent(const Block block) noexcept
{
	return block == Block::Air || block == Block::Leaves || block == Block::Glass;
}