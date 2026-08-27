#pragma once
#include "KamataEngine.h"
#include "helper.hpp"
#include <algorithm>
#include <array>
#include <numbers>
#include <assert.h>

class GameScene;
class Player;

class BaseEnemy {
private:
	float width_ = 1.0f;
	float height_ = 1.0f;

	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::Model* model_ = nullptr;

	KamataEngine::Camera* camera_ = nullptr;

	KamataEngine::Vector3 velocity_ = {0.0f, 0.0f, 0.0f};
	bool isDead_ = false;
	bool isCollisionDiabled_ = false;
	GameScene* gameScene_ = nullptr;

	float deadTimer_ = 0.f;
	float deadDuration_ = 0.5f;
	float walkTimer_ = 0.f;
	static inline const float kWalkSpeed = 0.1f;


	bool isBonus_ = false;

public:
	virtual ~BaseEnemy() { KamataEngine::DebugText::GetInstance()->ConsolePrintf("BaseEnemy destructor called\n");
	}

	virtual void Initialize(float width, float height, KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position, GameScene* gameScene, bool isBonus = false) {
		assert(model);
		assert(camera);
		assert(gameScene);

		gameScene_ = gameScene;
		width_ = width;
		height_ = height;

		// 3D モデルの生成
		model_ = model;

		// ワールドトランスフォームの初期化
		worldTransform_.Initialize();
		worldTransform_.translation_ = position;
		worldTransform_.scale_ = {width, height, 1.0f};
		worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.f;

		worldTransform_.matWorld_ = CreateAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
		// 定数バッファに転送する
		worldTransform_.TransferMatrix();

		// カメラのセット
		camera_ = camera;

		// 初速度の設定
		velocity_ = {-kWalkSpeed, 0, 0};

		walkTimer_ = 0.f;

		isDead_ = false;
		isCollisionDiabled_ = false;
		isBonus_ = isBonus;

		if (isBonus_) {
			// ボーナスアイテムの場合、少し上に浮かせる
			model_->SetAlpha(0.8f);
			worldTransform_.scale_ = {width*0.8f, height, 0.8f};
		}
	}

	bool IsBonus() const { return isBonus_; }

	virtual void Update() {
		// ワールドトランスフォームの更新
		worldTransform_.translation_.x += velocity_.x;

		if (isBonus_) {
			// ボーナスアイテムは上下に揺れる
			worldTransform_.rotation_.y += 0.05f;
		}

		worldTransform_.matWorld_ = CreateAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
		worldTransform_.TransferMatrix();

		if (worldTransform_.translation_.x < -10.f) {
			isDead_ = true;
		}
	
	}

	virtual void Draw() {
		// 3Dモデルの描画
		model_->Draw(worldTransform_, *camera_);
	}

	virtual void OnCollision(Player* player);

	virtual AABB GetAABB() {
		AABB aabb;
		aabb.min.x = worldTransform_.translation_.x - width_ / 2.f;
		aabb.min.y = worldTransform_.translation_.y - height_ / 2.f;
		aabb.min.z = worldTransform_.translation_.z - width_ / 2.f;
		aabb.max.x = worldTransform_.translation_.x + width_ / 2.f;
		aabb.max.y = worldTransform_.translation_.y + height_ / 2.f;
		aabb.max.z = worldTransform_.translation_.z + width_ / 2.f;
		return aabb;
	}

	bool IsCollisionDisabled() const { return isCollisionDiabled_; }
	bool IsDead() const { return isDead_; }

	void SetVelocity(const KamataEngine::Vector3& velocity) { velocity_ = velocity; }
	KamataEngine::Vector3 GetBaseVelocity() const { return {-kWalkSpeed, 0, 0}; }
};