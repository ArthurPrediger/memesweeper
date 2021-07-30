#include "MemeField.h"
#include "Vei2.h"
#include "SpriteCodex.h"
#include <assert.h>
#include <random>

void MemeField::Tile::Draw(const Vei2& screenPos, Graphics& gfx, const PlayerState playerState) const
{
	if (playerState == PlayerState::Memeing)
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
				SpriteCodex::DrawTileNumber(screenPos, gfx, nNeighborMemes);
			}
			else
			{
				SpriteCodex::DrawTileBomb(screenPos, gfx);
			}
			break;
		}
	}
	else
	{
		switch (state)
		{
		case State::Hidden:
			if (hasMeme)
			{
				SpriteCodex::DrawTileBomb(screenPos, gfx);
			}
			else
			{
				SpriteCodex::DrawTileButton(screenPos, gfx);
			}
			break;
		case State::Flagged:
			if (hasMeme)
			{
				SpriteCodex::DrawTileBomb(screenPos, gfx);
				SpriteCodex::DrawTileFlag(screenPos, gfx);
			}
			else
			{
				SpriteCodex::DrawTileBomb(screenPos, gfx);
				SpriteCodex::DrawTileCross(screenPos, gfx);
			}
			break;
		case State::Revealed:
			if (!hasMeme)
			{
				SpriteCodex::DrawTileNumber(screenPos, gfx, nNeighborMemes);
			}
			else
			{
				SpriteCodex::DrawTileBombRed(screenPos, gfx);
			}
			break;
		}
	}

}

bool MemeField::Tile::HasMeme() const
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

void MemeField::Tile::ToggleFlag()
{
	assert(state == State::Hidden);
	state = State::Flagged;
}

void MemeField::Tile::UntoggleFlag()
{
	assert(state == State::Flagged);
	state = State::Hidden;
}

bool MemeField::Tile::IsFlagged() const
{
	return state == State::Flagged;
}

void MemeField::Tile::SetNNeighborMemes(const int memeCount)
{
	assert(nNeighborMemes == -1);
	nNeighborMemes = memeCount;
}

MemeField::MemeField(const Vei2& center)
	:
	fieldTopLeft(center - Vei2(nTilesAcross * SpriteCodex::tileSize, nTilesDown * SpriteCodex::tileSize) / 2)
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

	for (int x = 0; x < nTilesAcross; x++)
	{
		for (int y = 0; y < nTilesDown; y++)
		{
			TileAt(Vei2{ x, y }).SetNNeighborMemes(CountNeighborMemes(Vei2{ x, y }));
		}
	}
}

void MemeField::Draw(Graphics& gfx) const
{
	DrawBorder(gfx);
	gfx.DrawRect(GetRect(), SpriteCodex::baseColor);
	
	for (int x = 0; x < nTilesAcross; x++)
	{
		for (int y = 0; y < nTilesDown; y++)
		{
			field[y * nTilesAcross + x].Draw(Vei2(x * SpriteCodex::tileSize,y * SpriteCodex::tileSize) + fieldTopLeft, gfx, playerState);
		}
	}
}

void MemeField::OnRevealClick(const Vei2& screenPos)
{
	if (screenPos.x >= fieldTopLeft.x && screenPos.x <= nTilesAcross * SpriteCodex::tileSize + fieldTopLeft.x &&
		screenPos.y >= fieldTopLeft.y && screenPos.y <= nTilesDown * SpriteCodex::tileSize + fieldTopLeft.y &&
		playerState == PlayerState::Memeing)
	{
		Vei2 gridPos = ScreenToGrid(screenPos);
		Tile& tile = TileAt(gridPos);
		if (!tile.IsRevealed() && !tile.IsFlagged())
		{
			tile.Reveal();
			
			if (tile.HasMeme())
			{
				playerState = PlayerState::Fucked;
				playerFuckedSound.Play();
			}
		}
	}
}

void MemeField::OnFlagClick(const Vei2& screenPos)
{
	if (screenPos.x >= fieldTopLeft.x && screenPos.x <= nTilesAcross * SpriteCodex::tileSize + fieldTopLeft.x &&
		screenPos.y >= fieldTopLeft.y && screenPos.y <= nTilesDown * SpriteCodex::tileSize + fieldTopLeft.y &&
		playerState == PlayerState::Memeing)
	{
		Vei2 gridPos = ScreenToGrid(screenPos);
		Tile& tile = TileAt(gridPos);
		if (!tile.IsFlagged() && !tile.IsRevealed())
		{
			tile.ToggleFlag();
		}
		else if(tile.IsFlagged())
		{
			tile.UntoggleFlag();
		}
	}
}

int MemeField::CountNeighborMemes(const Vei2& gridPos)
{
	const int xStart = std::max(0, gridPos.x - 1);
	const int yStart = std::max(0, gridPos.y - 1);
	const int xEnd = std::min(nTilesAcross - 1, gridPos.x + 1);
	const int yEnd = std::min(nTilesDown - 1, gridPos.y + 1);

	int count = 0;

	for (int x = xStart; x <= xEnd; x++)
	{
		for (int y = yStart; y <= yEnd; y++)
		{
			if (TileAt(Vei2{ x, y }).HasMeme())
			{
				count++;
			}
		}
	}

	return count;
}

MemeField::Tile& MemeField::TileAt(const Vei2& gridPos)
{
	return field[gridPos.y * nTilesAcross + gridPos.x];
}

RectI MemeField::GetRect() const
{
	return RectI(fieldTopLeft, nTilesAcross * SpriteCodex::tileSize, nTilesDown * SpriteCodex::tileSize);
}

Vei2 MemeField::ScreenToGrid(const Vei2& screenPos)
{
	return (screenPos - fieldTopLeft) / SpriteCodex::tileSize;
}

void MemeField::DrawBorder(Graphics& gfx) const
{
	int fieldRight = nTilesAcross * SpriteCodex::tileSize + fieldTopLeft.x;
	int fieldBottom = nTilesDown * SpriteCodex::tileSize + fieldTopLeft.y;
	gfx.DrawRect(fieldTopLeft.x, fieldTopLeft.y - borderThickness, fieldRight, fieldTopLeft.y, borderColor);
	gfx.DrawRect(fieldTopLeft.x, fieldBottom, fieldRight, fieldBottom + borderThickness, borderColor);
	gfx.DrawRect(fieldTopLeft.x - borderThickness, fieldTopLeft.y - borderThickness, fieldTopLeft.x, fieldBottom + borderThickness, borderColor);
	gfx.DrawRect(fieldRight, fieldTopLeft.y - borderThickness, fieldRight + borderThickness, fieldBottom + borderThickness, borderColor);
}

bool MemeField::GameIsWon()
{
	for (const Tile& t : field)
	{
		if (t.HasMeme() && !t.IsFlagged() ||
			!t.HasMeme() && !t.IsRevealed())
		{
			return false;
		}
	}
	playerState = PlayerState::Winner;
	return true;
}

MemeField::PlayerState MemeField::GetPlayerState() const
{
	return playerState;
}


