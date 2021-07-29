#pragma once
#include "Graphics.h"
#include "RectI.h"

class MemeField
{
private:
	class Tile
	{
	public:
		enum class State
		{
			Hidden,
			Flagged,
			Revealed
		};
	public:
		void Draw(const Vei2& screenPos, Graphics& gfx, bool fucked) const;
		bool HasMeme();
		void SpawnMeme();
		void Reveal();
		bool IsRevealed() const;
		void ToggleFlag();
		void UntoggleFlag();
		bool IsFlagged() const;
		void SetNNeighborMemes(const int memeCount);
	private:
		State state = State::Hidden;
		bool hasMeme = false;
		int nNeighborMemes = -1;
	};
public:
	MemeField(const Vei2& center, int numberMemes);
	void Draw(Graphics& gfx) const;
	void OnRevealClick(const Vei2& screenPos);
	void OnFlagClick(const Vei2& screenPos);
	int CountNeighborMemes(const Vei2& gridPos);
private:
	Tile& TileAt(const Vei2& gridPos);
	RectI GetRect() const;
	Vei2 ScreenToGrid(const Vei2& screenPos);
	void DrawBorder(Graphics& gfx) const;
private:
	Vei2 fieldTopLeft;
	static constexpr int nTilesAcross = 20;
	static constexpr int nTilesDown = 16;
	static constexpr int borderThickness = 10;
	static constexpr Color borderColor = Colors::Blue;
	Tile field[nTilesAcross * nTilesDown];
	bool isFucked = false;
};