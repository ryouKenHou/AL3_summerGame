#include "HitEffect.h"
#include <algorithm>

KamataEngine::Model* HitEffect::model_ = nullptr;
KamataEngine::Camera* HitEffect::camera_ = nullptr;

HitEffect* HitEffect::Create(KamataEngine::Vector3 position) {
	HitEffect* instance = new HitEffect();
	assert(instance);
	instance->Initialize(position);

	return instance;
}

void HitEffect::Initialize(KamataEngine::Vector3 position) {
	circleWorldTransform_.Initialize();
	circleWorldTransform_.translation_ = position;
	circleWorldTransform_.scale_ = {0.5f, 0.5f, 0.5f};
	circleWorldTransform_.matWorld_ = CreateAffineMatrix(circleWorldTransform_.scale_, circleWorldTransform_.rotation_, circleWorldTransform_.translation_);
	circleWorldTransform_.TransferMatrix();

	// ellipseWorldTransforms_
	Random random;
	random.Initialize();

	// ランダムな回転速度と移動速度を設定
	rotateVelocity = {random.GetRandomFloat(-0.2f, 0.2f), random.GetRandomFloat(-0.2f, 0.2f), random.GetRandomFloat(-0.2f, 0.2f)};
	translateVelocity = {random.GetRandomFloat(-0.3f, 0.01f), random.GetRandomFloat(-0.2f, 0.2f), random.GetRandomFloat(-0.2f, 0.2f)};
}

void HitEffect::Update() {
	timer_++;

	circleWorldTransform_.rotation_ += rotateVelocity;
	circleWorldTransform_.translation_ += translateVelocity;

	rotateVelocity = {rotateVelocity.x * 0.95f, rotateVelocity.y * 0.95f, rotateVelocity.z * 0.95f};
	translateVelocity = {translateVelocity.x * 0.95f, translateVelocity.y * 0.95f, translateVelocity.z * 0.95f};
	circleWorldTransform_.scale_ = {EaseOut(0.5f, 0.0f, timer_ / duration_), EaseOut(0.5f, 0.0f, timer_ / duration_), EaseOut(0.5f, 0.0f, timer_ / duration_)};

	circleWorldTransform_.matWorld_ = CreateAffineMatrix(circleWorldTransform_.scale_, circleWorldTransform_.rotation_, circleWorldTransform_.translation_);
	circleWorldTransform_.TransferMatrix();

}

void HitEffect::Draw() {
	if (model_ && camera_) {
		//model_->SetAlpha(alpha_);
		KamataEngine::DebugText::GetInstance()->ConsolePrintf("draw\n");
		model_->Draw(circleWorldTransform_, *camera_);
	}
}