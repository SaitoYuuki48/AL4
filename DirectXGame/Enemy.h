#pragma once
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

#include "BaseCharacter.h"

class Enemy : public BaseCharacter{

private:
	enum ModelParts { 
		kModelIndexBody,
		kModelIndexL_arm, 
		kModelIndexR_arm
	};

	enum class Phase {
		Approach, // 接近する
		Leave,    // 離脱する
	};

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const std::vector<Model*>& models) override;

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ViewProjection& viewProjection);

private:
	// 接近する
	void Approach();
	// 離脱する
	void Leave();

private:
	// ワールド変換データ
	// ワールド変換データ
	WorldTransform worldTransformBase_;
	WorldTransform worldTransformBody_;
	WorldTransform worldTransformL_arm_;
	WorldTransform worldTransformR_arm_;

	// モデル
	Model* modelFighterBody_;
	Model* modelFighterL_arm_;
	Model* modelFighterR_arm_;

	// 速度
	Vector3 velocity_;

	// フェーズ
	Phase phase_ = Phase::Approach;
};
