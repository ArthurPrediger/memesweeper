#include "MemeField.h"
#include "Vei2.h"
#include "SpriteCodex.h"
#include <assert.h>
#include <random>

void MemeField::Tile::Draw(const Vei2& screenPos, Graphics& gfx) const
{
	switch (state)
	{
	case State::Hidden:
		SpriteCodex::DrawTileButton(screenPos, gfx);
		break;
	case State::Flagged:
		SpriteCodex::DrawTileButton(screenPos, gfx);
		SpriteCodex::DrawTileFlag(screenPos, gfx);
		break;
	case State::Revealed:
		if (!hasMeme)
		{
			SpriteCodex::DrawTile0(screenPos, gfx);
		}
		else
		{
			SpriteCodex::DrawTileBomb(screenPos, gfx);
		}
		break;
	}

}

bool MemeField::Tile::HasMeme()
{
	return hasMeme;
}

void MemeField::Tile::SpawnMeme()
{
	assert(!hasMeme);
	hasMeme = true;
}

void MemeField::Tile::Reveal()
{
	assert(state == State::Hidden);
	state = State::Revealed;
}

bool MemeField::Tile::IsRevealed() const
{
	return state == State::Revealed;
}

MemeField::MemeField(int numberMemes)
{
	assert(numberMemes > 0 && numberMemes < nTilesAcross* nTilesDown);
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> xDist(0, nTilesAcross - 1);
	std::uniform_int_distribution<int> yDist(0, nTilesAcross - 1);

	for (int nMemesSpawned = 0; nMemesSpawned < numberMemes; nMemesSpawned++)
	{
		Vei2 spawnPos;
		do
		{
			spawnPos = { xDist(rng), yDist(rng) };
		} while ( TileAt(spawnPos).HasMeme() );	

		TileAt(spawnPos).SpawnMeme();
	}
}

void MemeField::Draw(Graphics& gfx) const
{
	gfx.DrawRect(GetRect(), SpriteCodex::baseColor);

	for (int x = 0; x < nTilesAcross; x++)
	{
		for (int y = 0; y < nTilesDown; y++)
		{
			field[y * nTilesAcross + x].Draw(Vei2(x * SpriteCodex::tileSize,y * SpriteCodex::tileSize), gfx);
		}
	}
}

void MemeField::OnRevealClick(const Vei2& screenPos)
{
	if (screenPos.x >= 0 && screenPos.x <= nTilesAcross * SpriteCodex::tileSize &&
		screenPos.y >= 0 && screenPos.y <= nTilesDown * SpriteCodex::tileSize)
	{
		Vei2 gridPos = ScreenToGrid(screenPos);
		Tile& tile = TileAt(gridPos);
		if (!tile.IsRevealed())
		{
			tile.Reveal();
		}
	}
}

MemeField::Tile& MemeField::TileAt(const Vei2& gridPos)
{
	return field[gridPos.y * nTilesAcross + gridPos.x];
}

RectI MemeField::GetRect() const
{
	return RectI(0, nTilesAcross * SpriteCodex::tileSize, 0, nTilesDown * SpriteCodex::tileSize);
}

Vei2 MemeField::ScreenToGrid(const Vei2& screenPos)
{
	return screenPos / SpriteCodex::tileSize;
}
