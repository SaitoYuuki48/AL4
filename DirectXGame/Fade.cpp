#include "Fade.h"
#include <TextureManager.h>

void Fade::Initialize() {
	// フェードの初期化
	uint32_t fadeTexHandle = TextureManager::Load("fade.png");
	fadeSprite_ = Sprite::Create(fadeTexHandle, {0, 0});
}

void Fade::Update() {
	// フェードの更新
	//フェードイン
	if (fadeFlag == FadeState::KFadeIn) {
		fadeColor_.w -= 0.01f;
		if (fadeColor_.w <= 0) {
			fadeFlag = FadeState::kNul;
		}
	}

	//フェードアウト
	if (fadeFlag == FadeState::kFadeOut) {
		fadeColor_.w += 0.01f;
		if (fadeColor_.w >= 1) {
			fadeFlag = FadeState::kNul;
		}
	}

	if (fadeFlag == FadeState::kNul) {
		fadeColor_.w = 0.0f;
	}
}

void Fade::Draw() {
	fadeSprite_->SetColor(fadeColor_);
	// フェードの描画
	fadeSprite_->Draw();
}

void Fade::FadeInStart() { 
	fadeFlag = FadeState::KFadeIn;
	fadeColor_.w = 1.0f;
}

void Fade::FadeOutStart() {
	fadeFlag = FadeState::kFadeOut;
	fadeColor_.w = 0.0f;
}
