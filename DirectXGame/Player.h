#pragma once
#include "Input.h"
#include "Model.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

#include "BaseCharacter.h"

class Player : public BaseCharacter {
private:
	enum ModelParts {
		kModelIndexBody, 
		kModelIndexHead,
		kModelIndexL_arm,
		kModelIndexR_arm
	};

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
	void Initialize(const std::vector<Model*>& models) override;

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const ViewProjection& viewProjection) override;

	const WorldTransform& GetWorldTransform();

private:
	//浮遊ギミック初期化
	void InitializeFloatingGimmick();

	//浮遊ギミック更新
	void UpdateFloatingGimmick();

	void UpdateArmAnimation();

public:
	// ワールド座標を取得
	Vector3 GetWorldPosition();

	void SetViewProjection(const ViewProjection* viewProjection) {
		viewProjection_ = viewProjection;
	}

private:
	// カメラのビュープロジェクション
	const ViewProjection* viewProjection_ = nullptr;

private:
	Input* input_ = nullptr;

	// ワールド変換データ
	WorldTransform worldTransformBase_;
	WorldTransform worldTransformBody_;
	WorldTransform worldTransformHead_;
	WorldTransform worldTransformL_arm_;
	WorldTransform worldTransformR_arm_;


	// モデル
	Model* modelFighterBody_;
	Model* modelFighterHead_;
	Model* modelFighterL_arm_;
	Model* modelFighterR_arm_;

	// テクスチャハンドル
	//uint32_t textureHandle_ = 0;

	//浮遊ギミックの媒介変数
	float floatingParameter_ = 0.0f;

	//腕のアニメーションの変数
	float armParameter_ = 0.0f;

	//フレーム数
	const float kFloatingFPS = 60.0f;

	const float kArmFPS = 60.0f;
};
