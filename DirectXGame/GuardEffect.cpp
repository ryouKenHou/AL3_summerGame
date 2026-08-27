#include "GuardEffect.h"
#include <algorithm>
#include <array>
#include <numbers>
KamataEngine::Model* GuardEffect::model_ = nullptr;
KamataEngine::Camera* GuardEffect::camera_ = nullptr;
GuardEffect* GuardEffect::Create(KamataEngine::Vector3 position) {
	GuardEffect* instance = new GuardEffect();
	assert(instance);
	instance->Initialize(position);

	return instance;
}

void GuardEffect::Initialize(KamataEngine::Vector3 position) {
	circleWorldTransform_.Initialize();
	circleWorldTransform_.translation_ = position;
	circleWorldTransform_.scale_ = {0.8f, 0.8f, 0.8f};
	circleWorldTransform_.matWorld_ = CreateAffineMatrix(circleWorldTransform_.scale_, circleWorldTransform_.rotation_, circleWorldTransform_.translation_);
	circleWorldTransform_.TransferMatrix();
}

void GuardEffect::Update() {
	timer_++;
	circleWorldTransform_.rotation_.x = std::clamp(circleWorldTransform_.rotation_.x, -std::numbers::pi_v<float> / 2.f, std::numbers::pi_v<float> / 2.f);
	circleWorldTransform_.translation_.x -= speedX_;
	circleWorldTransform_.matWorld_ = CreateAffineMatrix(circleWorldTransform_.scale_, circleWorldTransform_.rotation_, circleWorldTransform_.translation_);
	circleWorldTransform_.TransferMatrix();

}

void GuardEffect::Draw() {
	if (model_ && camera_) {
		//KamataEngine::DebugText::GetInstance()->ConsolePrintf("draw\n");
		model_->Draw(circleWorldTransform_, *camera_);

	}
}