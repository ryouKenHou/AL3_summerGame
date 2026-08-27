#pragma once
#include <KamataEngine.h>

class Skydome {
private:
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;

	float speedFactor_ = 1.f; // 回転速度の係数

public:
	Skydome();
	~Skydome();
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera);
	void Update();
	void Draw();

	void SetSpeedFactor(float speedFactor) { speedFactor_ = speedFactor; }
};