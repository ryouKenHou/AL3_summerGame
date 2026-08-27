#pragma once
#include "KamataEngine.h"
#include "Player.hpp"
#include "Skydome.h"
#include "vector"
#include "CameraController.h"
#include "helper.hpp"
#include "list"
#include "DeathParticles.h"
#include "Fade.h"
#include "HitEffect.h"
#include "GuardEffect.h"

class GameScene {
public:
	enum class Phase{
		kFadeIn,
		kPlay,
		kDeath,
		kFadeOut,
	};

private:
	// ゲームシーンの状態
	Phase phase_;

	// プレイヤー
	Player* player_ = nullptr;
	KamataEngine::Model* playerModel_ = nullptr;
	KamataEngine::Model* playerAttackModel_ = nullptr;
	KamataEngine::WorldTransform playerWorldTransform_;

	// デスパーティクル
	DeathParticles* deathParticles_ = nullptr;
	KamataEngine::Model* deathParticleModel_ = nullptr;

	// ブロックのモデル
	KamataEngine::Model* blockModel_ = nullptr;
	KamataEngine::Model* bonusBlockModel_ = nullptr;
	std::vector<std::vector<KamataEngine::WorldTransform*>> blockWorldTransforms_;

	// tate敵キャラクター
	int shieldEnemyMax_ = 1;
	KamataEngine::Model* shieldEnemyModel_ = nullptr;

	// カメラ
	CameraController* cameraController_ = nullptr;

	// 敵キャラクター
	int enemyMax_ = 10;
	std::list<BaseEnemy*> enemies_;

	// デバッグカメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
	bool isDebugCameraActive_ = false;

	// スカイドーム
	Skydome* skydome_ = nullptr;
	KamataEngine::Model* skydomeModel_ = nullptr;

	bool isFinished_ = false;

	Fade* fade_ = nullptr;

	std::list<BaseEffect*> baseEffects_;
	KamataEngine::Model* hitEffectModel_ = nullptr;
	KamataEngine::Model* guardEffectModel_ = nullptr;

	bool reloadRequested_ = false;

	float ObstacleSpawnTimer_ = 0.f;
	float ObstacleSpawnInterval_ = 120.f; // 障害物のスポーン間隔（秒）

	uint32_t BGMSoundHandle_ = 0;
	uint32_t BoomSoundHandle_ = 0;
	uint32_t PointSoundHandle_ = 0;
	uint32_t BGMPlayHandle_ = 0;

	uint32_t numsTextureHandle = 0;
	KamataEngine::Sprite* numsSprite[10] = {nullptr};
	uint32_t ScoreTextureHandle = 0;
	KamataEngine::Sprite* scoreSprite = nullptr;
	uint32_t tutorialTextureHandle = 0;
	KamataEngine::Sprite* tutorialSprite = nullptr;

	KamataEngine::Vector2 tutorialBaseSize_ = {157.f, 32.f};
	KamataEngine::Vector2 tutorialBigSize_ = {157.f*2, 32.f*2};
	KamataEngine::Vector2 tutorialBasePosition_ = {1100.f, 650.f};
KamataEngine::Vector2 tutorialFirstPosition_ = {300.f, 400.f};
	int tutorialFrameCounter_ = 0;
	int tutorialFrameDuration_ = 250; // 3秒間表示

	int score_ = 0;
	int frameCounter_ = 0;

	int stageFlag_ = 0;

	float speedfactor_ = 1.0f;

public:
	GameScene();
	~GameScene();

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	void CheckAllCollisions();

	void ChangePhase();

	bool IsFinished() const { return isFinished_; }

	void CreateHitEffect(const KamataEngine::Vector3& position);

	void CreateGuardEffect(const KamataEngine::Vector3& position);

	bool IsReloadRequested() const { return reloadRequested_; }

	void SpawnOneHoleObstacle();
	void SpawnTwoHoleObstacle();
	int SpawnMultipleObstacleArea(bool isBonuce1 = false);

	void HandleSpawningObstacles();

	void PlayPointSound() {
		//if (PointSoundHandle_ != 0) {
			KamataEngine::Audio::GetInstance()->PlayWave(PointSoundHandle_);
		//}
	}

	void PlayBoomSound() {
		//if (BoomSoundHandle_ != 0) {
			KamataEngine::Audio::GetInstance()->PlayWave(BoomSoundHandle_, false, 0.5f);
		//}
	}

	void AddScore(int points) {
		score_ += points; }

	int GetScore() const { return score_; }
};
