#include "GameScene.h"
#include "2d/ImGuiManager.h"
#include "BaseEnemy.h"

using namespace KamataEngine;

GameScene::GameScene() {}
GameScene::~GameScene() {
	// プレイヤーの解放
	delete player_;

	// ブロックの解放
	// 敵キャラクターの解放
	for (BaseEnemy* enemy : enemies_) {
		delete enemy;
	}
	enemies_.clear();
	
	// デバッグカメラの解放
	delete debugCamera_;

	// スカイドームの解放
	delete skydome_;

	// カメラコントローラーの解放
	delete cameraController_;

	// デスパーティクルの解放
	delete deathParticles_;

	// フェードの解放
	delete fade_;

	// ヒットエフェクトの解放
	for (BaseEffect* hitEffect : baseEffects_) {
		delete hitEffect;
	}
	baseEffects_.clear();
	delete hitEffectModel_;
	delete guardEffectModel_;

	Audio::GetInstance()->StopWave(BGMPlayHandle_);
}

void GameScene::Initialize() {
	// ゲームシーンの状態を初期化
	phase_ = Phase::kFadeIn;

	// カメラコントローラーの生成
	cameraController_ = new CameraController();
	cameraController_->Initialize();
	cameraController_->SetMode(CameraController::Mode::kFollow);

	Rect CameraMovableArea = {
	    11.0f, 110.f, 10.0f, 10};
	// Rect CameraMovableArea = { 11.0f, (MapChipField::GetNumBlockHorizontal() - 11.f) * mapChipField_->GetBlockWidth(), -60.0f, (MapChipField::GetNumBlockVirtical() + 1) *
	// mapChipField_->GetBlockHeight()};

	cameraController_->SetMovableArea(CameraMovableArea);

	BGMSoundHandle_ = Audio::GetInstance()->LoadWave("Sounds/BGM.mp3");
	BoomSoundHandle_ = Audio::GetInstance()->LoadWave("Sounds/bomb.mp3");
	PointSoundHandle_ = Audio::GetInstance()->LoadWave("Sounds/point.mp3");

	BGMPlayHandle_ = Audio::GetInstance()->PlayWave(BGMSoundHandle_, true, 0.5f);

	// プレイヤーの生成と初期化
	playerModel_ = Model::CreateFromOBJ("player");
	playerAttackModel_ = Model::CreateFromOBJ("hit_effect");
	player_ = new Player();
	player_->Initialize(playerModel_, playerAttackModel_, cameraController_->GetCamera(), {4.0f, 10.0f, 0.0f});	
	cameraController_->SetTarget(player_);

	// tate敵キャラクターの生成と初期化
	shieldEnemyModel_ = Model::CreateFromOBJ("shieldEnemy");

	// ブロックモデルの生成と初期化
	blockModel_ = Model::CreateFromOBJ("block");
	blockModel_->SetAlpha(0.7f);
	bonusBlockModel_ = Model::CreateFromOBJ("BonusBlock");

	//  デバッグカメラの生成と初期化
	debugCamera_ = new DebugCamera(1280, 720);

	// スカイドームの生成と初期化
	skydomeModel_ = Model::CreateFromOBJ("skyDome", true);
	skydome_ = new Skydome();
	skydome_->Initialize(skydomeModel_, cameraController_->GetCamera());

	// デスパーティクルの生成と初期化
	deathParticleModel_ = Model::CreateFromOBJ("deathParticle");
	deathParticles_ = new DeathParticles();
	deathParticles_->Initialize(deathParticleModel_, cameraController_->GetCamera(), player_->GetWorldPosition());

	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::kFadeIn, 1.0f);

	hitEffectModel_ = Model::CreateFromOBJ("particle");
	HitEffect::SetCamera(cameraController_->GetCamera());
	HitEffect::SetModel(bonusBlockModel_);

	guardEffectModel_ = Model::CreateFromOBJ("player");
	guardEffectModel_->SetAlpha(0.04f);
	GuardEffect::SetCamera(cameraController_->GetCamera());
	GuardEffect::SetModel(guardEffectModel_);

	numsTextureHandle = TextureManager::Load("Front/nums.png");
	for (int i = 0; i < 10; i++) {
		numsSprite[i] = Sprite::Create(numsTextureHandle, {50.0f, 50.0f});		
	}
	ScoreTextureHandle = TextureManager::Load("Front/score.png");
	scoreSprite = Sprite::Create(ScoreTextureHandle, {50.0f, 100.0f});
	tutorialTextureHandle = TextureManager::Load("Front/tutorial.png");
	tutorialSprite = Sprite::Create(tutorialTextureHandle, {157.0f, 32.0f});

	/*BaseEnemy* enemy = new BaseEnemy();
	enemy->Initialize(1.0f, 5.0f, blockModel_, cameraController_->GetCamera(), {20.0f, 10.0f, 0.0f}, this);
	enemies_.push_back(enemy);*/
}

void GameScene::ChangePhase() {
	switch (phase_) {
	case Phase::kFadeIn: {
		if (fade_->IsFinished()) {
			phase_ = Phase::kPlay;
		}
		fade_->Update();

		// カメラコントローラーの更新
		cameraController_->Update();

		if (isDebugCameraActive_) {
			// デバッグカメラ更新
			debugCamera_->Update();
			// カメラにデバッグカメラをセット
			cameraController_->GetCamera()->matView = debugCamera_->GetCamera().matView;
			cameraController_->GetCamera()->matProjection = debugCamera_->GetCamera().matProjection;
			cameraController_->GetCamera()->TransferMatrix();
		} else {
			// 通常カメラ更新
			cameraController_->GetCamera()->UpdateMatrix();
		}

		// スカイドームの更新
		skydome_->Update();
		break;
	}
	case Phase::kPlay: {
		// プレイヤーの更新
		frameCounter_++;
		if (frameCounter_ % 60 == 0) {
			score_++;
		}
		if (frameCounter_ % 3 == 0) {
			CreateGuardEffect(player_->GetWorldPosition());
		}

		if (frameCounter_ >= 30 * 60) { 
			if (speedfactor_ < 2.f) {
				speedfactor_ += 0.001f;
			} else {
				speedfactor_ += 0.0004f;
				speedfactor_ = min(speedfactor_, 3.0f);
				if (frameCounter_ % int(ObstacleSpawnInterval_) == 0) {
					ObstacleSpawnInterval_ -= 2.f;
					ObstacleSpawnInterval_ = max(ObstacleSpawnInterval_, 30.f);
				}
				
			}
			
			player_->SetSpinSpeed(player_->GetBaseSpinSpeed() * speedfactor_);
			skydome_->SetSpeedFactor(speedfactor_);
		}



		player_->Update();

		HandleSpawningObstacles();

		// ヒットエフェクトの更新
		for (BaseEffect* hitEffect : baseEffects_) {
			hitEffect->Update();
			hitEffect->SetSpeedX(0.1f*speedfactor_);
		}

		CheckAllCollisions();

		for (BaseEnemy* enemy : enemies_) {
			enemy->Update();
			enemy->SetVelocity({enemy->GetBaseVelocity().x * speedfactor_, 0, 0});
		}
		enemies_.remove_if([](BaseEnemy* enemy) {
			if (enemy->IsDead()) {
				delete enemy;
				return true;
			}
			return false;
		});

		baseEffects_.remove_if([](BaseEffect* effect) {
			if (effect->IsFinished()) {
				delete effect;
				return true;
			}
			return false;
		});

		// カメラコントローラーの更新
		cameraController_->Update();

		// スカイドームの更新
		skydome_->Update();

#ifdef _DEBUG
		if (Input::GetInstance()->TriggerKey(DIK_C)) {
			isDebugCameraActive_ = !isDebugCameraActive_;
		}
#endif // _DEBUG

		if (isDebugCameraActive_) {
			// デバッグカメラ更新
			debugCamera_->Update();
			// カメラにデバッグカメラをセット
			cameraController_->GetCamera()->matView = debugCamera_->GetCamera().matView;
			cameraController_->GetCamera()->matProjection = debugCamera_->GetCamera().matProjection;
			cameraController_->GetCamera()->TransferMatrix();
		} else {
			// 通常カメラ更新
			cameraController_->GetCamera()->UpdateMatrix();
		}

		if (player_->IsAlive() == false) {
			phase_ = Phase::kDeath;
			deathParticles_->Initialize(deathParticleModel_, cameraController_->GetCamera(), player_->GetWorldPosition());
		}

		break;
	}
	case Phase::kDeath: {
		// スカイドームの更新
		skydome_->Update();


		// ヒットエフェクトの更新
		for (BaseEffect* hitEffect : baseEffects_) {
			hitEffect->Update();
		}

		for (BaseEnemy* enemy : enemies_) {
			enemy->Update();
		}
		enemies_.remove_if([](BaseEnemy* enemy) {
			if (enemy->IsDead()) {
				delete enemy;
				return true;
			}
			return false;
		});

		baseEffects_.remove_if([](BaseEffect* effect) {
			if (effect->IsFinished()) {
				delete effect;
				return true;
			}
			return false;
		});

		// デスパーティクルの更新
		if (deathParticles_) {
			deathParticles_->Update();
		}

#ifdef _DEBUG
		if (Input::GetInstance()->TriggerKey(DIK_C)) {
			isDebugCameraActive_ = !isDebugCameraActive_;
		}
#endif // _DEBUG
		if (isDebugCameraActive_) {
			// デバッグカメラ更新
			debugCamera_->Update();
			// カメラにデバッグカメラをセット
			cameraController_->GetCamera()->matView = debugCamera_->GetCamera().matView;
			cameraController_->GetCamera()->matProjection = debugCamera_->GetCamera().matProjection;
			cameraController_->GetCamera()->TransferMatrix();
		} else {
			// 通常カメラ更新
			cameraController_->GetCamera()->UpdateMatrix();
		}

		if (deathParticles_ && deathParticles_->IsFinished()) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::kFadeOut, 1.0f);
		}

		break;
	}
	case Phase::kFadeOut: {
		fade_->Update();
		if (fade_->IsFinished()) {
			isFinished_ = true;
		}
		Audio::GetInstance()->StopWave(BGMPlayHandle_);
		break;
	}
	}
}

void GameScene::Update() {

	#ifdef _DEBUG
	ImGui::Begin("GameScene Debug");

	if (ImGui::Button("Reload GameScene")) {
		reloadRequested_ = true;
	}	

	ImGui::End();
	#endif

	ChangePhase();
	
}

void GameScene::Draw() {
	//  3Dモデル描画前処理
	Model::PreDraw();

	// スカイドームの描画
	skydome_->Draw();

	// ブロックの描画
	for (auto& row : blockWorldTransforms_) {
		for (WorldTransform* transform : row) {
			if (!transform) {
				continue;
			}
			blockModel_->Draw(*transform, *cameraController_->GetCamera());
		}
	}

	if (deathParticles_) {
		deathParticles_->Draw();
	}

	// プレイヤーの描画
	player_->Draw();

	// 敵キャラクターの描画
	for (BaseEnemy* enemy : enemies_) {
		enemy->Draw();
	}

	// ヒットエフェクトの描画
	for (BaseEffect* baseEffect : baseEffects_) {
		baseEffect->Draw();
	}

	//  3Dモデル描画後処理
	Model::PostDraw();
	KamataEngine::Sprite::PreDraw();
	fade_->Draw();

	// draw 2D elements
	// - draw score
	// get all digits of score
	int score = score_;
	int digits[10] = {0};
	for (int i = 0; i < 10; ++i) {
		digits[i] = score % 10;
		score /= 10;
	}
	const Vector2 scorePos = {50.0f, 650.0f};
	scoreSprite->SetSize({111.f, 32.f});
	scoreSprite->SetPosition(scorePos);
	scoreSprite->Draw();

	const Vector2 startPos = {160.0f, 650.0f};
	for (int i = 9; i >= 0; --i) {
		numsSprite[i]->SetSize({32.f, 32.f});
		numsSprite[i]->SetPosition({startPos.x + (9 - i) * 20.0f, startPos.y});
		numsSprite[i]->SetTextureRect({float(digits[i]%10*32), 0.f}, {32.f, 32.f});
		numsSprite[i]->Draw();
	}

	// ease in and out effect for tutorial sprite
	tutorialFrameCounter_++;
	float t = static_cast<float>(tutorialFrameCounter_) / tutorialFrameDuration_;
	if (t > 1.0f) {
		t = 1.0f;
	}
	tutorialSprite->SetSize({EaseInOut(tutorialBigSize_.x, tutorialBaseSize_.x, t), EaseInOut(tutorialBigSize_.y, tutorialBaseSize_.y, t)});
	tutorialSprite->SetPosition({EaseInOut(tutorialFirstPosition_.x, tutorialBasePosition_.x, t), EaseInOut(tutorialFirstPosition_.y, tutorialBasePosition_.y, t)});
	tutorialSprite->Draw();

	
	KamataEngine::Sprite::PostDraw();
}

	void GameScene::CreateHitEffect(const KamataEngine::Vector3& position) {
	for (int i = 0; i < 5; ++i) {
		//Vector3 randomOffset = {static_cast<float>(rand() % 30 - 10) / 10.f, static_cast<float>(rand() % 30 - 10) / 10.f, static_cast<float>(rand() % 30 - 10) / 10.f};
		Vector3 randomOffset = {0, 0, 0};
		HitEffect* hitEffect = HitEffect::Create({position.x + randomOffset.x, position.y + randomOffset.y, position.z + randomOffset.z});
		baseEffects_.push_back(hitEffect);
	}
}

	void GameScene::CreateGuardEffect(const KamataEngine::Vector3& position) {
		GuardEffect* guardEffect = GuardEffect::Create(position);
	    guardEffect->SetRotation(player_->GetWorldTransform().rotation_);
		baseEffects_.push_back(guardEffect);
	}

	void GameScene::CheckAllCollisions() {
#pragma region Player vs MapChipField
		{
		    AABB aabb1, aabb2;
		    aabb1 = player_->GetAABB();

		    for (BaseEnemy* e : enemies_) {
			    if (e->IsCollisionDisabled()) {
				    continue;
			    }
			    aabb2 = e->GetAABB();
			    if (AABB::CheckAABBCollision(aabb1, aabb2)) {
				    player_->OnCollision(e);
				    e->OnCollision(player_);
			    }
		    }

		}
#pragma endregion

#pragma region Player vs items

#pragma endregion

#pragma region PlayerBullet vs enemies

#pragma endregion
	}

	int GameScene::SpawnMultipleObstacleArea(bool isBonuce1) {
	    int ObstacleAreaCount = rand() % 4 + 5; // 5から8までのランダムな整数を生成
	    const float gapBetweenObstacles = 3.f;  // 障害物エリア間の距離
	    const float StartY = 2.f;
	    const float EndY = 18.f;

	    // Curve parameters
	    const float curveAmplitude = 3.0f;                     // How much the curve moves up/down
	    //const float curveFrequency = 0.3f;                     // How wavy the curve is
	    float startHoleY = static_cast<float>(rand() % 6 + 7); // Start hole position (7-12)
	    float holeHeight = (rand() % 15)/10.f + 3.0f;                         // Fixed hole height for consistency

	    // Random curve direction (up or down)
	    float curveDirection = (rand() % 2 == 0) ? 1.0f : -1.0f;
	    bool isBonuce =  (rand() %4 == 0); 
		if (isBonuce1 || isBonuce) {
		    isBonuce = true;
		    holeHeight = 1.0f;
	    } // If it's a bonus area, make the hole height smaller

	    for (int i = 0; i < ObstacleAreaCount; ++i) {
		    float xPos = 30.0f + i * gapBetweenObstacles;

		    // Calculate hole Y position using sine wave for smooth curve
		    float curveProgress = static_cast<float>(i) / static_cast<float>(ObstacleAreaCount - 1);
		    float curveOffset = std::sin(curveProgress * std::numbers::pi_v<float>) * curveAmplitude * curveDirection;
		    float holeY = startHoleY + curveOffset;

		    // Clamp hole position to ensure it stays within bounds
		    holeY = std::clamp(holeY, StartY + 1.0f, EndY - holeHeight - 1.0f);

			if (isBonuce && (i == 0 || i == ObstacleAreaCount - 1)) {
			    // Spawn block above the hole
			    float blockAboveHeight = holeY - StartY;
			    if (blockAboveHeight > 0.1f) {
				    BaseEnemy* blockAboveHole = new BaseEnemy();
				    blockAboveHole->Initialize(1.0f, blockAboveHeight, blockModel_, cameraController_->GetCamera(), {xPos, StartY + blockAboveHeight / 2.f, 0.0f}, this);
				    enemies_.push_back(blockAboveHole);
			    }

			    // Spawn block below the hole
			    float blockBelowHeight = EndY - (holeY + holeHeight+1.5f);
			    if (blockBelowHeight > 0.1f) {
				    BaseEnemy* blockBelowHole = new BaseEnemy();
				    blockBelowHole->Initialize(1.0f, blockBelowHeight, blockModel_, cameraController_->GetCamera(), {xPos, holeY + holeHeight+1.5f + blockBelowHeight / 2.f, 0.0f}, this);
				    enemies_.push_back(blockBelowHole);
			    }
		    } else if (!isBonuce) {
				// Spawn block above the hole
				float blockAboveHeight = holeY - StartY;
				if (blockAboveHeight > 0.1f) {
					BaseEnemy* blockAboveHole = new BaseEnemy();
					blockAboveHole->Initialize(1.0f, blockAboveHeight, blockModel_, cameraController_->GetCamera(), {xPos, StartY + blockAboveHeight / 2.f, 0.0f}, this);
					enemies_.push_back(blockAboveHole);
				}
				// Spawn block below the hole
				float blockBelowHeight = EndY - (holeY + holeHeight);
				if (blockBelowHeight > 0.1f) {
					BaseEnemy* blockBelowHole = new BaseEnemy();
					blockBelowHole->Initialize(1.0f, blockBelowHeight, blockModel_, cameraController_->GetCamera(), {xPos, holeY + holeHeight + blockBelowHeight / 2.f, 0.0f}, this);
					enemies_.push_back(blockBelowHole);
				}
		    }

		    // Optional: Spawn bonus block at the tightest point of the curve (middle)
		    if (i == ObstacleAreaCount / 2 || isBonuce) {
			    BaseEnemy* bonusBlock = new BaseEnemy();
			    bonusBlock->Initialize(1.0f, holeHeight, bonusBlockModel_, cameraController_->GetCamera(), {xPos, holeY + holeHeight / 2.f, 0.0f}, this, true);
			    enemies_.push_back(bonusBlock);
		    }
	    }
	    return ObstacleAreaCount;
    }

	void GameScene::SpawnOneHoleObstacle() { 
		const float StartY = 2.f;
	    const float EndY = 18.f;

		float holeY = static_cast<float>(rand() % 11 + 4); // 4から17までのランダムな整数を生成
	    float holeHeight = static_cast<float>(rand() % 4 + 4); // 4から7までのランダムな整数を生成

		// spawn a block above the hole
	    BaseEnemy* blockAboveHole = new BaseEnemy();
		blockAboveHole->Initialize(1.0f, holeY - StartY, blockModel_, cameraController_->GetCamera(), {30.0f, StartY + (holeY - StartY) / 2.f, 0.0f}, this);
	    enemies_.push_back(blockAboveHole);

		// spawn a block below the hole
	    BaseEnemy* blockBelowHole = new BaseEnemy();
	    blockBelowHole->Initialize(1.0f, EndY - (holeY + holeHeight), blockModel_, cameraController_->GetCamera(), {30.0f, holeY + holeHeight + (EndY - (holeY + holeHeight)) / 2.f, 0.0f}, this);
	    enemies_.push_back(blockBelowHole);

		if (stageFlag_ == 0) {
	    // spawn a bonus block in the hole
			BaseEnemy* bonusBlock = new BaseEnemy();
			bonusBlock->Initialize(1.0f, holeHeight, bonusBlockModel_, cameraController_->GetCamera(), {30.0f, holeY + holeHeight / 2.f, 0.0f}, this, true);
		    enemies_.push_back(bonusBlock);
		    stageFlag_ = 1; // Set stageFlag_ to 1 after spawning the bonus block
		}
    }

	void GameScene::SpawnTwoHoleObstacle() {
		const float StartY = 2.f;
		const float EndY = 18.f;
		float firstHoleY = static_cast<float>(rand() % 11 + 4); // 4から14までのランダムな整数を生成
		float firstHoleHeight = static_cast<float>(rand() % 2 + 2); // 2から4までのランダムな整数を生成
		float secondHoleY = static_cast<float>(rand() % 11 + 4); // 4から14までのランダムな整数を生成
		float secondHoleHeight = static_cast<float>(rand() % 4 + 2); // 2から4までのランダムな整数を生成
	
		// Ensure the holes do not overlap
		while ((secondHoleY < firstHoleY + firstHoleHeight) && (firstHoleY < secondHoleY + secondHoleHeight)) {
			secondHoleY = static_cast<float>(rand() % 11 + 4);
			secondHoleHeight = static_cast<float>(rand() % 4 + 2);
		}

		// Sort holes by Y position
		float lowerHoleY, lowerHoleHeight, upperHoleY, upperHoleHeight;
		if (firstHoleY < secondHoleY) {
			lowerHoleY = firstHoleY;
			lowerHoleHeight = firstHoleHeight;
			upperHoleY = secondHoleY;
			upperHoleHeight = secondHoleHeight;
		} else {
			lowerHoleY = secondHoleY;
			lowerHoleHeight = secondHoleHeight;
			upperHoleY = firstHoleY;
			upperHoleHeight = firstHoleHeight;
		}

		// spawn a block above the lower hole
		BaseEnemy* blockAboveLowerHole = new BaseEnemy();
		blockAboveLowerHole->Initialize(1.0f, lowerHoleY - StartY, blockModel_, cameraController_->GetCamera(), {30.0f, StartY + (lowerHoleY - StartY) / 2.f, 0.0f}, this);
		enemies_.push_back(blockAboveLowerHole);

		// spawn a block between the two holes
		float middleBlockHeight = upperHoleY - (lowerHoleY + lowerHoleHeight);
		if (middleBlockHeight > 0.1f) { // Only spawn if there's space between holes
			BaseEnemy* blockBetweenHoles = new BaseEnemy();
			blockBetweenHoles->Initialize(1.0f, middleBlockHeight, blockModel_, cameraController_->GetCamera(), {30.0f, lowerHoleY + lowerHoleHeight + middleBlockHeight / 2.f, 0.0f}, this);
			enemies_.push_back(blockBetweenHoles);
		}

		// spawn a block below the upper hole
		BaseEnemy* blockBelowUpperHole = new BaseEnemy();
		blockBelowUpperHole->Initialize(1.0f, EndY - (upperHoleY + upperHoleHeight), blockModel_, cameraController_->GetCamera(), {30.0f, upperHoleY + upperHoleHeight + (EndY - (upperHoleY + upperHoleHeight)) / 2.f, 0.0f}, this);
		enemies_.push_back(blockBelowUpperHole);

		// find the smallest hole and spawn a bonus block there
		if (lowerHoleHeight < upperHoleHeight) {
			BaseEnemy* bonusBlock = new BaseEnemy();
			bonusBlock->Initialize(1.0f, lowerHoleHeight, bonusBlockModel_, cameraController_->GetCamera(), {30.0f, lowerHoleY + lowerHoleHeight / 2.f, 0.0f}, this, true);
			enemies_.push_back(bonusBlock);
		} else {
			BaseEnemy* bonusBlock = new BaseEnemy();
			bonusBlock->Initialize(1.0f, upperHoleHeight, bonusBlockModel_, cameraController_->GetCamera(), {30.0f, upperHoleY + upperHoleHeight / 2.f, 0.0f}, this, true);
			enemies_.push_back(bonusBlock);
	    }
	}

	void GameScene::HandleSpawningObstacles() {
		ObstacleSpawnTimer_ -= 1.f; // Assuming Update is called at 60 FPS
		if (ObstacleSpawnTimer_ <= 0.f) {
		    int randomChoice = rand() % 3; // Randomly choose between 0, 1, and 2
		    if (stageFlag_ <= 1) {
				randomChoice = rand() % 2; // Only choose between 0 and 1 for stageFlag_ <= 1
			    if (stageFlag_ == 0) {
				    SpawnOneHoleObstacle();
				    ObstacleSpawnTimer_ = ObstacleSpawnInterval_;
				    return;
			    }				    
				if (frameCounter_ >= 15*60) {
				    stageFlag_ = 2; // After 15 frames, allow multiple obstacles
				    SpawnMultipleObstacleArea(true);
				    ObstacleSpawnTimer_ = ObstacleSpawnInterval_ * 2.2f;
				    return;
			    }
		    }

			if (randomChoice == 0) {
			    randomChoice = rand() % 3; // Randomly choose between 0 and 1 for one or two hole obstacles
			    if (randomChoice == 0) {
				    SpawnTwoHoleObstacle();
			    } else {
				    SpawnOneHoleObstacle();
				}
				
			    ObstacleSpawnTimer_ = ObstacleSpawnInterval_;
			} else if (randomChoice == 1) {
				SpawnTwoHoleObstacle();
			    ObstacleSpawnTimer_ = ObstacleSpawnInterval_;
			} else {
				int count = SpawnMultipleObstacleArea();
			    ObstacleSpawnTimer_ = ObstacleSpawnInterval_*2.2f;
			    if (ObstacleSpawnTimer_ < count * 14.f) {
				    ObstacleSpawnTimer_ = count * 14.f; // Ensure a minimum interval of 1 second
			    }
			}
			 // Reset the timer
		}
    }

	void BaseEnemy::OnCollision(Player* player) {
	    (void)player; // 未使用の引数を無視するためのキャスト
	    if (isBonus_) {
		    // ボーナスアイテムに触れた場合の処理
		    gameScene_->PlayPointSound();
		    gameScene_->AddScore(50); // 例: スコアを50点加算
		    isDead_ = true;
		    isCollisionDiabled_ = true;
		    gameScene_->CreateHitEffect(worldTransform_.translation_);

		    // プレイヤーにボーナスを与える処理をここに追加
	    } else {
		    gameScene_->PlayBoomSound();
	    }
    }