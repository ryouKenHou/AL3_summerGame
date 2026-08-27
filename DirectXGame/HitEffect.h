#pragma once

#include "KamataEngine.h"
#include "helper.hpp"
#include "BaseEffect.h"

class HitEffect final : public BaseEffect {
private:
	KamataEngine::WorldTransform circleWorldTransform_;

	float timer_ = 0;
	float duration_ = 60.f; // フェードの継続時間（フレーム）
	float alpha_ = 0.8f;

	static KamataEngine::Model* model_;
	static KamataEngine::Camera* camera_;

	KamataEngine::Vector3 rotateVelocity;
	KamataEngine::Vector3 translateVelocity;


public:
	void Initialize(KamataEngine::Vector3 position) override;
	void Update() override;
	void Draw() override;

	static void SetModel(KamataEngine::Model* model) { model_ = model; }	
	static void SetCamera(KamataEngine::Camera* camera) { camera_ = camera; }
	static HitEffect* Create(KamataEngine::Vector3 position);

	bool IsFinished() const override { return timer_ >= duration_; }
};
