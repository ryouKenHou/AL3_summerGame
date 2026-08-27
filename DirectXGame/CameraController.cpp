#include "CameraController.h"
#include "Player.hpp"
#include "algorithm"


using namespace KamataEngine;

CameraController::CameraController() {}
CameraController::~CameraController() {
	// 解放処理
	delete camera_;
}

void CameraController::Initialize() {
	// カメラの初期化処理
	camera_ = new KamataEngine::Camera();
	camera_->Initialize();
}

void CameraController::Reset() {
	// カメラのリセット処理
	const WorldTransform& targetTransform = target_->GetWorldTransform();
	camera_->translation_ = targetTransform.translation_ + offset_;
}

void CameraController::Update() {
	if (!target_->IsAlive()) return;
	
	const WorldTransform& targetTransform = target_->GetWorldTransform();
	;
	if (mode_ == Mode::kFollow) {
		// カメラの位置をターゲットの位置＋オフセットに設定
		Vector3 targetVelocity = target_->GetVelocity();
		targetPosition_ = targetTransform.translation_ + offset_ + targetVelocity * kVelocityBias;

		// カメラの位置を補間してなめらかに移動
		camera_->translation_ = Lerp(camera_->translation_, targetPosition_, kInterpolationRate);

		// marginAreaを考慮したカメラ位置調整
		camera_->translation_.x = max(camera_->translation_.x, targetTransform.translation_.x + marginArea.left);
		camera_->translation_.x = min(camera_->translation_.x, targetTransform.translation_.x + marginArea.right);
		camera_->translation_.y = max(camera_->translation_.y, targetTransform.translation_.y + marginArea.bottom);
		camera_->translation_.y = min(camera_->translation_.y, targetTransform.translation_.y + marginArea.top);
		
	} else if (mode_ == Mode::kForcedScroll) {
		// 強制スクロールモードでは、カメラの位置をターゲットの位置＋オフセットに設定
		camera_->translation_.x = camera_->translation_.x + kForcedScrollSpeed;

		if (targetTransform.translation_.x < camera_->translation_.x - 10.5f) {
			if (target_->GetVelocity().x < 0.f) {
				target_->SetVelocity({kForcedScrollSpeed, 0.f, 0.f});
			} else {
				target_->AddVelocity({kForcedScrollSpeed, 0.f, 0.f});
			}			
		}
	}

	// カメラの位置を可動範囲内に制限
	camera_->translation_.x = std::clamp(camera_->translation_.x, movableArea_.left, movableArea_.right);
	camera_->translation_.y = std::clamp(camera_->translation_.y, movableArea_.bottom, movableArea_.top);

	// カメラの注視点をターゲットの位置に設定
	camera_->UpdateViewMatrix();
}