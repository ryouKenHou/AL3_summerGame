#include "Skydome.h"
#include "helper.hpp"

using namespace KamataEngine;

Skydome::Skydome() {}
Skydome::~Skydome() {
	// 解放処理
	delete model_;
}

void Skydome::Initialize(Model* model, Camera* camera) {
	assert(model);
	assert(camera);

	// 3D モデルの生成
	model_ = model;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// カメラのセット
	camera_ = camera;
}

void Skydome::Update() {
	// 更新処理
	worldTransform_.rotation_.y -= 0.01f * speedFactor_;
	worldTransform_.matWorld_ = CreateAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	worldTransform_.TransferMatrix();
}

void Skydome::Draw() {
	// 描画処理
	model_->Draw(worldTransform_, *camera_);
}