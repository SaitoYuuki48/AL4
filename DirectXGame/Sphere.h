#pragma once

#include "Model.h"
#include "WorldTransform.h"

class Sphere {
public:
	/// <summary>
	/// コンストクラタ
	/// </summary>
	Sphere();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Sphere();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, Vector3 position);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const ViewProjection& viewProjection);

public:
	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision();

	// ワールド座標を取得
	Vector3 GetWorldPosition();

public:
	bool IsDead() const { return isDead_; }

	// デスフラグ
	bool isDead_ = false;

private: // 変数
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
};
