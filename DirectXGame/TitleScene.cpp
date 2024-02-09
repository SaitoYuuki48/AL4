#include "TitleScene.h"
#include "ImGuiManager.h"

TitleScene::TitleScene() {}

TitleScene::~TitleScene() {}

void TitleScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	viewProjection_.Initialize();

	// テクスチャ
	uint32_t textureTitle = TextureManager::Load("./Resources/titleName.png");
	// ルールテクスチャ
	uint32_t luletexture = TextureManager::Load("./Resources/tutorial2.png");

	// スプライト生成
	spriteTitle_ =
	    Sprite::Create(textureTitle, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	// スプライト生成
	spriteLule_ = Sprite::Create(luletexture, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});

	// 天球
	//  3Dモデルの生成
	modelSkydome_.reset(Model::CreateFromOBJ("skydome", true));
	// 天球の生成
	skydome_ = std::make_unique<Skydome>();
	// 天球の初期化
	skydome_->Initialize(modelSkydome_.get());

	// 地面
	//   3Dモデルの生成
	modelGround_.reset(Model::CreateFromOBJ("Ground", true));
	// 地面の生成
	ground_ = std::make_unique<Ground>();
	// 地面の初期化
	ground_->Initialize(modelGround_.get());

	
	// フェードの生成
	fade_ = std::make_unique<Fade>();
	// フェードの初期化
	fade_->Initialize();

	// カメラの生成
	fixedCamera_ = std::make_unique<FixedCamera>();
	// カメラの初期化
	fixedCamera_->Initialize();

	fade_->FadeInStart();
}

void TitleScene::Update() {
	// ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	// カメラの更新
	fixedCamera_->Update();

	viewProjection_.matView = fixedCamera_->GetViewProjection().matView;
	viewProjection_.matProjection = fixedCamera_->GetViewProjection().matProjection;
	// ビュープロジェクション行列の転送
	viewProjection_.TransferMatrix();

	// 天球の更新
	skydome_->Update();
	skydome_->DemoUpdate();

	// 地面の更新
	ground_->Update();
	ground_->DemoUpdate(); 

	// ゲームパッド状態取得
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		// 切り替え
		waitTimer_--;
		if (waitTimer_ <= 0 &&isLule == false) {
			if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
				waitTimer_ = kWaitTimer_;
				isLule = true;
			}
		}

		//遊び方
		if (isLule == true) {
			if (waitTimer_ <= 0) {
				if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A && fadeTimerFlag_ == false) {
					fadeTimerFlag_ = true;
					fade_->FadeOutStart();
				}
			}
		}

		if (fadeTimerFlag_ == true) {
			fadeTimer_--;
		}
			
		if (fadeTimer_ <= 0) {
			isSceneEnd_ = true;
		}	

		// フェードの更新
		fade_->Update();
	}

	if (input_->TriggerKey(DIK_SPACE)) {
		isSceneEnd_ = true;
	}
#ifdef _DEBUG
	ImGui::Begin("Debug");
	//ImGui::SliderFloat3("PlayerPos", playerPos, 0.0f, 128.0f);
	ImGui::End();
#endif //DEBUG
}

void TitleScene::Draw() {
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	
	// 天球の描画
	skydome_->Draw(viewProjection_);

	// 地面の描画
	ground_->Draw(viewProjection_);

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	spriteTitle_->Draw();
	if (isLule == true) {
		spriteLule_->Draw();
	}

	// フェードの描画
	fade_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void TitleScene::sceneReset() {
	waitTimer_ = kWaitTimer_;

	isLule = false;

	isSceneEnd_ = false; 
	
	fadeTimer_ = kFadeTimer_;

	fadeTimerFlag_ = false;

	fade_->FadeInStart();
}