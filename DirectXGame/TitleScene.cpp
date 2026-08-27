#include "TitleScene.h"

void TitleScene::Initialize() {
	// タイトルシーンの初期化処理
	playerModel_ = KamataEngine::Model::CreateFromOBJ("player", true);
	playerWorldTransform_.Initialize();
	playerWorldTransform_.translation_ = {0.f, -2.f, -40.f};
	playerWorldTransform_.rotation_.y = 3.141592654f;

	PointSoundHandle_ = KamataEngine::Audio::GetInstance()->LoadWave("Sounds/point.mp3");

	titleModel_ = KamataEngine::Model::CreateFromOBJ("titleFont", true);
	titleWorldTransform_.Initialize();
	titleWorldTransform_.translation_ = {0.f, 5.f, -1.f};
	// titleWorldTransform_.rotation_.y = 3.141592654f;
	titleWorldTransform_.scale_ = {1.5f, 1.5f, 1.5f};

	camera_ = new KamataEngine::Camera();
	camera_->Initialize();
	camera_->translation_.z = -15.f;
	// スカイドームの生成と初期化

	skydomeModel_ = KamataEngine::Model::CreateFromOBJ("skyDome", true);
	skydome_ = new Skydome();
	skydome_->Initialize(skydomeModel_, camera_);

	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::kFadeIn, 1.0f);

	numsTextureHandle = KamataEngine::TextureManager::Load("Front/nums.png");
	for (int i = 0; i < 10; i++) {
		numsSprite[i] = KamataEngine::Sprite::Create(numsTextureHandle, {50.0f, 50.0f});
	}
	ScoreTextureHandle = KamataEngine::TextureManager::Load("Front/score.png");
	scoreSprite = KamataEngine::Sprite::Create(ScoreTextureHandle, {50.0f, 100.0f});
}

void TitleScene::Update() {
	frameCounter_++;
	// タイトルシーンの更新処理
	playerWorldTransform_.matWorld_ = CreateAffineMatrix(playerWorldTransform_.scale_, playerWorldTransform_.rotation_, playerWorldTransform_.translation_);
	playerWorldTransform_.TransferMatrix();

	titleWorldTransform_.translation_.y += sinf(frameCounter_ * 0.05f) * 0.08f;

	titleWorldTransform_.matWorld_ = CreateAffineMatrix(titleWorldTransform_.scale_, titleWorldTransform_.rotation_, titleWorldTransform_.translation_);
	titleWorldTransform_.TransferMatrix();

	switch (phase_) {
	case Phase::kFadeIn:
		
		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}
		break;
	case Phase::kMain:
		
		if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_SPACE)) {			
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::kFadeOut, 1.0f);			
			KamataEngine::Audio::GetInstance()->PlayWave(PointSoundHandle_);
			
		}
		break;
	case Phase::kFadeOut:
		
		if (fade_->IsFinished()) {
			isFinished_ = true;
		}
		break;
	}

	fade_->Update();
	// スカイドームの更新
	skydome_->Update();
}

void TitleScene::Draw() {
	KamataEngine::Model::PreDraw();
	// タイトルシーンの描画処理
	playerModel_->Draw(playerWorldTransform_, *camera_);
	titleModel_->Draw(titleWorldTransform_, *camera_);
	skydome_->Draw();
	KamataEngine::Model::PostDraw();

	KamataEngine::Sprite::PreDraw();
	fade_->Draw();

	if (score_ > 0) {
		// Calculate sine wave offset once
		float sineOffset = sinf(frameCounter_ * 0.05f) * 8.0f; // Increased for visibility

		// スコア表示
		int score = score_;
		int digits[10] = {0};
		for (int i = 0; i < 10; ++i) {
			digits[i] = score % 10;
			score /= 10;
		}

		// Use base position + offset (don't modify the base position)
		scoreSprite->SetSize({111.f, 32.f});
		scoreSprite->SetPosition({scorePos.x, scorePos.y + sineOffset});
		scoreSprite->Draw();

		for (int i = 9; i >= 0; --i) {
			numsSprite[i]->SetSize({32.f, 32.f});
			numsSprite[i]->SetPosition({startPos.x + (9 - i) * 20.0f, startPos.y + sineOffset});
			numsSprite[i]->SetTextureRect({float(digits[i] % 10 * 32), 0.f}, {32.f, 32.f});
			numsSprite[i]->Draw();
		}
	}
	KamataEngine::Sprite::PostDraw();
}