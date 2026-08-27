#pragma once
#include "2d/ImGuiManager.h"
#include "KamataEngine.h"
#include "helper.hpp"
#include "vector"
#include "Fade.h"
#include "Skydome.h"
class TitleScene {
public:
	enum class Phase {
		kFadeIn,
		kMain,
		kFadeOut,
	};

private:
	KamataEngine::Model* playerModel_ = nullptr;
	KamataEngine::WorldTransform playerWorldTransform_;

	KamataEngine::Model* titleModel_ = nullptr;
	KamataEngine::WorldTransform titleWorldTransform_;

	KamataEngine::Camera* camera_ = nullptr;

	Fade* fade_ = nullptr;

	Phase phase_ = Phase::kFadeIn;

	int score_ = 0;

		// スカイドーム
	Skydome* skydome_ = nullptr;
	KamataEngine::Model* skydomeModel_ = nullptr;

	KamataEngine::Vector2 scorePos = {465.0f, 350.0f };
	KamataEngine::Vector2 startPos = {575.0f, 350.0f };
	uint32_t PointSoundHandle_ = 0;

	int frameCounter_ = 0;
	bool isFinished_ = false;

	uint32_t numsTextureHandle = 0;
	KamataEngine::Sprite* numsSprite[10] = {nullptr};
	uint32_t ScoreTextureHandle = 0;
	KamataEngine::Sprite* scoreSprite = nullptr;
	uint32_t tutorialTextureHandle = 0;

public:
	~TitleScene() {
		delete playerModel_;
		delete titleModel_;
		delete camera_;
		delete fade_;
	}

	void Initialize();
	void Update();
	void Draw();

	void SetScore(int score) { score_ = score; }

	bool IsFinished() const { return isFinished_; }
};