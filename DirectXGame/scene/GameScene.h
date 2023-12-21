#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "DebugCamera.h"

#include <memory>

#include "Player.h"
#include "Enemy.h"
#include "Skydome.h"
#include "Ground.h"
#include "FollowCamera.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

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

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
	/// 
	/// // ワールドトランスフォーム
	WorldTransform worldTransform_;
	// ビュープロジェクション
	ViewProjection viewProjection_;

	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;
	// デバッグカメラ
	std::unique_ptr<DebugCamera> debugCamera_; 
	
	// フォローカメラ
	std::unique_ptr<FollowCamera> followCamera_;

	// 自キャラ
	 std::unique_ptr<Player> player_;
	//Player* player_ = nullptr;
	// 3Dモデル
	//std::unique_ptr<Model> modelPlayer_;
	std::unique_ptr<Model> modelFighterBody_;
	std::unique_ptr<Model> modelFighterHead_;
	std::unique_ptr<Model> modelFighterL_arm_;
	std::unique_ptr<Model> modelFighterR_arm_;
	std::unique_ptr<Model> modelFighterWeapon_;

	//敵キャラ
	std::unique_ptr<Enemy> enemy_;
	//モデル
	std::unique_ptr<Model> modelEnemyBody_;
	std::unique_ptr<Model> modelEnemyL_arm_;
	std::unique_ptr<Model> modelEnemyR_arm_;
	
	// テクスチャハンドル
	//uint32_t playerTextureHandle_ = 0;

	// 天球
	std::unique_ptr<Skydome> skydome_;
	//Skydome* skydome_ = nullptr;
	//  3Dモデル
	std::unique_ptr<Model> modelSkydome_;

	//地面
	std::unique_ptr<Ground> ground_;
	// Skydome* skydome_ = nullptr;
	//   3Dモデル
	std::unique_ptr<Model> modelGround_;
};
