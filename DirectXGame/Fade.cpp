#include "Fade.h"
#include <algorithm>

void Fade::Initialize() {
	// フェードの初期化処理
	sprite_ = KamataEngine::Sprite::Create(0, {0.f, 0.f});
	sprite_->SetSize({1280.f, 720.f});
	sprite_->SetColor({0.f, 0.f, 0.f, 1.f});
}

void Fade::Update() {
	// フェードの更新処理
	switch (status_) {
	case Fade::Status::kNone:
		break;
	case Fade::Status::kFadeIn:
		counter_ += 1.f / 60.f;
		if (counter_ >= duration_) {
			counter_ = duration_;
		}

		sprite_->SetColor({0.f, 0.f, 0.f, 1.f - std::clamp(counter_ / duration_, 0.f, 1.f)});

		break;
	case Fade::Status::kFadeOut:
		counter_ += 1.f / 60.f;
		if (counter_ >= duration_) {
			counter_ = duration_;
		}

		sprite_->SetColor({0.f, 0.f, 0.f, std::clamp(counter_ / duration_, 0.f, 1.f)});

		break;
	}
}

void Fade::Draw() {
	// フェードの描画処理
	if (status_ == Status::kNone) {
		return;
	}
	
	sprite_->Draw();
	
}

void Fade::Start(Status status, float duration) {
	// フェードの開始処理
	status_ = status;
	duration_ = duration;
	counter_ = 0.0f;
}

void Fade::Stop() {
	// フェードの停止処理
	status_ = Status::kNone;
}

bool Fade::IsFinished() const {
	// フェードの終了判定
	switch (status_) {
	case Status::kFadeIn:
	case Status::kFadeOut:
		return counter_ >= duration_;
	}
	return true;
}