#pragma once
#include "Graphics.h"
#include "RectI.h"
#include "Sound.h"

class MemeField
{
public:
	enum class PlayerState
	{
		Fucked,
		Winner,
		Memeing
	};
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
		void Draw(const Vei2& screenPos, Graphics& gfx, const PlayerState playerState) const;
		bool HasMeme() const;
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
	MemeField(const Vei2& center);
	void Draw(Graphics& gfx) const;
	void OnRevealClick(const Vei2& screenPos);
	void OnFlagClick(const Vei2& screenPos);
	int CountNeighborMemes(const Vei2& gridPos);
	bool GameIsWon();
private:
	Tile& TileAt(const Vei2& gridPos);
	RectI GetRect() const;
	Vei2 ScreenToGrid(const Vei2& screenPos);
	void DrawBorder(Graphics& gfx) const;
	PlayerState GetPlayerState() const;
private:
	Vei2 fieldTopLeft;
	static constexpr int nTilesAcross = 4;
	static constexpr int nTilesDown = 3;
	static constexpr int numberMemes = 2;
	static constexpr int borderThickness = 10;
	static constexpr Color borderColor = Colors::Blue;
	Tile field[nTilesAcross * nTilesDown];
	PlayerState playerState = PlayerState::Memeing;
	Sound playerFuckedSound = Sound(L"spayed.wav");
};