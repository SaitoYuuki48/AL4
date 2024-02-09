﻿#pragma once
#pragma once
#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Scene.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

#include "FixedCamera.h"

#include "Skydome.h"
#include "Ground.h"
#include "Fade.h"

class TitleScene {
public:
	/// <summary>
	/// コンストクラタ
	/// </summary>
	TitleScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~TitleScene();

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

	void sceneReset();

	bool isSceneEnd_ = false;

	bool IsSceneEnd() { return isSceneEnd_; }
	SceneType NextScene() { return SceneType::kGamePlay; }

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	// ビュープロジェクション
	ViewProjection viewProjection_;

	// カメラ
	std::unique_ptr<FixedCamera> fixedCamera_; 

	//切り替え用
	static const int32_t kWaitTimer_ = 30;
	int32_t waitTimer_ = kWaitTimer_;

	bool isLule = false;

	// 天球
	std::unique_ptr<Skydome> skydome_;
	//  3Dモデル
	std::unique_ptr<Model> modelSkydome_;

	// 地面
	std::unique_ptr<Ground> ground_;
	//   3Dモデル
	std::unique_ptr<Model> modelGround_;

	//  スプライト
	Sprite* spriteTitle_ = nullptr;
	Sprite* spriteLule_ = nullptr;

	// フェード
	std::unique_ptr<Fade> fade_;
	bool fadeTimerFlag_;
	const float kFadeTimer_ = 1.657f * 60.0f;
	float fadeTimer_ = kFadeTimer_;

};