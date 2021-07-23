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
		void Draw(const Vei2& screenPos, Graphics& gfx) const;
		bool HasMeme();
		void SpawnMeme();
		void Reveal();
		bool IsRevealed() const;
		void ToggleFlag();
		void UntoggleFlag();
		bool IsFlagged() const;
	private:
		State state = State::Hidden;
		bool hasMeme = false;
	};
public:
	MemeField(int numberMemes);
	void Draw(Graphics& gfx) const;
	void OnRevealClick(const Vei2& screenPos);
	void OnFlagClick(const Vei2& screenPos);
private:
	Tile& TileAt(const Vei2& gridPos);
	RectI GetRect() const;
	Vei2 ScreenToGrid(const Vei2& screenPos);
private:
	static constexpr int nTilesAcross = 20;
	static constexpr int nTilesDown = 16;
	Tile field[nTilesAcross * nTilesDown];
};