#pragma once
#include "KamataEngine.h"
#include "helper.hpp"
#include "BaseEnemy.h"
#include <algorithm>
#include <array>
#include <numbers>

enum Corner {
	kRightBottom,
	kLeftBottom,
	kRightTop,
	kLeftTop,

	kNumCorner
};

struct CollisionMapInfo {
	bool ceilingCollided = false;
	bool grounded = false;
	bool wallCollided = false;
	KamataEngine::Vector3 moveValue;
};

class MapChipField;
class BaseEnemy;

class Player {
public:
	enum class Behavior {
		kUnknown,
		kRoot,
		kAttack,
		kNockBack,
	};

	enum class AttackPhace {
		kStart,
		kAttack,
		kEnd,
	};
	;

private:
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;

	KamataEngine::WorldTransform worldTransformAttack_;
	KamataEngine::Model* modelAttack_ = nullptr;

	uint32_t textureHandle_ = 0u;

	KamataEngine::Camera* camera_ = nullptr;

	KamataEngine::Vector3 velocity_ = {0.0f, 0.0f, 0.0f};

	static inline const float kAcceleration = 0.01f;
	static inline const float kAttenuation = 0.2f;
	static inline const float kAttenuationLanding = 0.2f;
	static inline const float kAttenuationWall = 0.3f;
	static inline const float kLimitRunSpeed = 0.2f;
	static inline const float kTimeTurn = 0.3f;
	static inline const float kGravityAcceleration = 0.02f;
	static inline const float kLimitFallSpeed = 0.3f;
	static inline const float kJumpAcceleration = 0.35f;
	static inline const float kWidth = 0.4f;
	static inline const float kHeight = 0.4f;

	float baseSpinSpeed_ = 0.1f;;
	float spinSpeed_ = baseSpinSpeed_;

	bool onGround_ = true;

	LRDirection lrDirection_ = LRDirection::kRight;

	float turnFirstRotationY_ = 0.0f;
	float turnTimer_ = 0.0f;

	MapChipField* mapChipField_ = nullptr;

	bool pushed_ = false;
	bool isDead_ = false;

	int DeadAnimationCounter_ = 0;
	int DeadAnimationDuration_ = 120;

	Behavior behavior_ = Behavior::kRoot;
	Behavior behaviorRequest_ = Behavior::kUnknown;

	uint32_t attackParameter_ = 0;
	const UINT32 attackParameterMax_ = 18;
	const float kattackStartDuration = 5.f;
	const float kattackAttackDuration = 15.f;
	const float kattackEndDuration = 5.f;
	AttackPhace attackPhace_ = AttackPhace::kStart;
	bool canAttack_ = true;
	bool isAttacking_ = false;

	uint32_t knockBackParameter_ = 0;
	const UINT32 knockBackParameterMax_ = 5;
	bool knockBackRequest_ = false;

	float angle_ = 0.f;

public:
	Player() {}
	~Player() {}

	void MoveInput() {
		// 移動入力
		if (KamataEngine::Input::GetInstance()->PushKey(DIK_UP) || KamataEngine::Input::GetInstance()->PushKey(DIK_DOWN)) {
			if (KamataEngine::Input::GetInstance()->PushKey(DIK_DOWN)) {
				worldTransform_.rotation_.x += 0.03f ;
			}
			else if (KamataEngine::Input::GetInstance()->PushKey(DIK_UP)) {
				worldTransform_.rotation_.x -= 0.03f ;
			}
		}
		
	}

	KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner) {
		KamataEngine::Vector3 offsetTable[kNumCorner] = {
		    {+kWidth / 2.f, -kHeight / 2.f, 0.f}, // kRightBottom
		    {-kWidth / 2.f, -kHeight / 2.f, 0.f}, // kLeftBottom
		    {+kWidth / 2.f, +kHeight / 2.f, 0.f}, // kRightTop
		    {-kWidth / 2.f, +kHeight / 2.f, 0.f}  // kLeftTop
		};
		return KamataEngine::Vector3{
		    center.x + offsetTable[static_cast<uint32_t>(corner)].x, center.y + offsetTable[static_cast<uint32_t>(corner)].y, center.z + offsetTable[static_cast<uint32_t>(corner)].z};
	}

	void Initialize(KamataEngine::Model* model, KamataEngine::Model* modelAttack, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
		assert(model);
		assert(camera);

		// 3D モデルの生成
		model_ = model;
		modelAttack_ = modelAttack;

		// ワールドトランスフォームの初期化
		worldTransform_.Initialize();
		worldTransform_.translation_ = position;
		worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
		worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.f;

		worldTransform_.matWorld_ = CreateAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
		// 定数バッファに転送する
		worldTransform_.TransferMatrix();

		worldTransformAttack_.Initialize();
		worldTransformAttack_.translation_ = position;
		worldTransformAttack_.scale_ = {0.5f, 0.5f, 0.5f};
		worldTransformAttack_.rotation_.y = std::numbers::pi_v<float> / 2.f;
		worldTransformAttack_.matWorld_ = CreateAffineMatrix(worldTransformAttack_.scale_, worldTransformAttack_.rotation_, worldTransformAttack_.translation_);
		worldTransformAttack_.TransferMatrix();
		// カメラのセット
		camera_ = camera;
		isDead_ = false;
	}

	void Update() {
		MoveInput();

		worldTransform_.rotation_.x = std::clamp(worldTransform_.rotation_.x, -std::numbers::pi_v<float> / 2.f, std::numbers::pi_v<float> / 2.f);
		
		KamataEngine::Vector3 direction = {   0, -std::sin(worldTransform_.rotation_.x),  0};

		//KamataEngine::DebugText::GetInstance()->ConsolePrintf("direction: %f, %f, %f, translation: %f, %f, %f\n", direction.x, direction.y, direction.z, worldTransform_.translation_.x, worldTransform_.translation_.y, worldTransform_.translation_.z);

		worldTransform_.translation_ = worldTransform_.translation_ + direction * 0.5f;
		worldTransform_.translation_.y = std::clamp(worldTransform_.translation_.y, 4.0f, 16.0f);

		worldTransform_.rotation_.z += spinSpeed_;
		worldTransform_.matWorld_ = CreateAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
		worldTransform_.TransferMatrix();
	}

	void Draw() {
		// 3Dモデルの描画
		if (isDead_) {
			return;
		}
		model_->Draw(worldTransform_, *camera_);
		if (behavior_ == Behavior::kAttack) {
			modelAttack_->Draw(worldTransformAttack_, *camera_);
		}
	}

	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }
	const KamataEngine::Vector3& GetWorldPosition() const { return worldTransform_.translation_; }
	const KamataEngine::Vector3& GetVelocity() const { return velocity_; }

	AABB GetAABB() {
		KamataEngine::Vector3 worldPos = GetWorldPosition();
		AABB aabb;
		aabb.min.x = worldPos.x - kWidth / 2.f;
		aabb.min.y = worldPos.y - kHeight / 2.f;
		aabb.min.z = worldPos.z - kWidth / 2.f;
		aabb.max.x = worldPos.x + kWidth / 2.f;
		aabb.max.y = worldPos.y + kHeight / 2.f;
		aabb.max.z = worldPos.z + kWidth / 2.f;
		return aabb;
	}

	void AddVelocity(const KamataEngine::Vector3& addVelocity) {
		velocity_ += addVelocity;
		pushed_ = true;
	}
	void SetVelocity(const KamataEngine::Vector3& velocity) { velocity_ = velocity; }

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	void OnCollision(BaseEnemy* enemy) {

		(void)enemy;
		// velocity_ += Vector3(0, kJumpAcceleration, 0);

		if (isAttacking_ || enemy->IsBonus()) {
			return;
		}

		isDead_ = true;
	}

	bool IsAlive() const { return !isDead_; }

	bool IsAttacking() const { return isAttacking_; }
	LRDirection GetLRDirection() const { return lrDirection_; }

	float GetBaseSpinSpeed() const { return baseSpinSpeed_; }
	void SetSpinSpeed(float speed) { spinSpeed_ = speed; }
};