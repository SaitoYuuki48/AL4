#pragma once
#include "Sprite.h"
#include "Vector4.h"

class Fade {
private:
	enum FadeState {
		kNul,
		KFadeIn,
		kFadeOut,
	};

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

public:
	void FadeInStart();

	void FadeOutStart();

private:
	Sprite* fadeSprite_ = nullptr;
	Vector4 fadeColor_ = {1.0f, 1.0f, 1.0f, 0.0f};

	FadeState fadeFlag = FadeState::kNul;
};
