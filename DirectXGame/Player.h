#pragma once
#include "Input.h"
#include "Model.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

class Player {
public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	Player();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model/*, uint32_t textureHandle*/);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ViewProjection& viewProjection);

	//プレイヤーを持たせる
	void SetParent(const WorldTransform* parent);

	const WorldTransform& GetWorldTransform();

public:
	// ワールド座標を取得
	Vector3 GetWorldPosition();

	void SetViewProjection(const ViewProjection* viewProjection);

private:
	// カメラのビュープロジェクション
	const ViewProjection* viewProjection_ = nullptr;

private:
	Input* input_ = nullptr;

	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;
};
