#pragma once

#include "KamataEngine.h"
#include "helper.hpp"
#include "BaseEffect.h"

class GuardEffect final : public BaseEffect {
private:
	enum class Status {
		kFadeIn,
		kFadeOut,
		kFinished,
	};

	KamataEngine::WorldTransform circleWorldTransform_;
	Status status_ = Status::kFadeIn;
	float timer_ = 0;
	float duration_ = 50.f; // フェードの継続時間（フレーム）
	float alpha_ = 1.f;

	static KamataEngine::Model* model_;
	static KamataEngine::Camera* camera_;


public:
	void Initialize(KamataEngine::Vector3 position) override;
	void Update() override;
	void Draw() override;

	static void SetModel(KamataEngine::Model* model) { model_ = model; }	
	static void SetCamera(KamataEngine::Camera* camera) { camera_ = camera; }
	void SetRotation(KamataEngine::Vector3 rotation) { circleWorldTransform_.rotation_ = rotation; }
	static GuardEffect* Create(KamataEngine::Vector3 position);

	bool IsFinished() const override { return timer_ >= duration_; }

};
