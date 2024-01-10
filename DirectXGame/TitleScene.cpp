#include "TitleScene.h"
TitleScene::TitleScene() {}

TitleScene::~TitleScene() {}

void TitleScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// テクスチャ
	uint32_t textureTitle = TextureManager::Load("./Resources/title.png");
	// ルールテクスチャ
	uint32_t luletexture = TextureManager::Load("./Resources/tutorial.png");

	// スプライト生成
	spriteTitle_ =
	    Sprite::Create(textureTitle, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	// スプライト生成
	spriteLule_ = Sprite::Create(luletexture, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
}

void TitleScene::Update() {
	// ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

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

		if (isLule == true) {
			if (waitTimer_ <= 0) {
				if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
					isSceneEnd_ = true;
				}
			}
		}
	}

	if (input_->TriggerKey(DIK_SPACE)) {
		isSceneEnd_ = true;
	}
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

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void TitleScene::sceneReset() {
	waitTimer_ = kWaitTimer_;

	isLule = false;

	isSceneEnd_ = false; 
}