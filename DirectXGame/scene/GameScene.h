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

#include "Scene.h"

#include "Player.h"
//#include "Enemy.h"
#include "Sphere.h"
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

public:
	/// <summary>
	/// 玉の発生
	/// </summary>
	void SphereSpawn(Vector3 position);

	/// <summary>
	/// 玉発生データの読み込み
	/// </summary>
	void LoadSpherePopData();

	/// <summary>
	/// 玉発生コマンドの更新
	/// </summary>
	void UpdateSpherePopCommands();

	// シーンのリセット
	void sceneReset();

	bool isSceneEnd = false;

	bool IsSceneEnd() { return isSceneEnd; }
	SceneType NextScene() { return SceneType::kTitle; }

private:
	/// <summary>
	/// 衝突判定と応答
	/// </summary>
	void CheckAllCollisions();

	// スコアの更新処理
	void ScoreUpdate(int32_t num, int32_t kNum);

	//時間の処理
	void TimerUpdate(int32_t timer);

	//画像の初期化
	void TextureInitialize();

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
	
	// 3Dモデル
	std::unique_ptr<Model> modelFighterBody_;
	std::unique_ptr<Model> modelFighterHead_;
	std::unique_ptr<Model> modelFighterL_arm_;
	std::unique_ptr<Model> modelFighterR_arm_;
	std::unique_ptr<Model> modelFighterWeapon_;

	//玉
	//std::unique_ptr<Sphere> sphere_;
	std::list<Sphere*> spheres_;

	// 玉の3Dモデル
	std::unique_ptr<Model> modelSphere_;

	// 敵発生コマンド
	std::stringstream spherePopCommands;

	// 敵の待機中のフラグ
	bool spherePopWaitFlag = true;
	// 待機タイマー
	int32_t spherePopWaitTimer = 0;

	// クリアに必要な数
	static const int32_t kSphereNum_ = 10;
	// 玉の数
	int32_t sphereNum_ = 0;

	// 玉の取得数表示
	int32_t sphereNum2;
	int32_t sphereNum1;

	int32_t sphereNum10_0;
	int32_t sphereNum10_1;

	Sprite* textureSlash_;
	Sprite* textureNumber_[10];
	Sprite* textureNumber2_[10];
	Sprite* textureNumber10_0_[10];
	Sprite* textureNumber10_1_[10];

	//時間の表示
	int32_t timerNum1;
	int32_t timerNum0;

	Sprite* textureTimer1_[10];
	Sprite* textureTimer0_[10];


	//敵キャラ
	//std::unique_ptr<Enemy> enemy_;
	//モデル
	/*std::unique_ptr<Model> modelEnemyBody_;
	std::unique_ptr<Model> modelEnemyL_arm_;
	std::unique_ptr<Model> modelEnemyR_arm_;*/
	
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

	//制限時間
	static const int32_t kTimer_ = 60 * 60; // 1800; // 3600;
	int32_t timer_ = kTimer_;

	//クリア、ゲームオーバー
	bool clearFlag = false;
	bool gameoverFlag = false;

	Sprite* textureClear;
	Sprite* textureGameover;
};
