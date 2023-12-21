#pragma once
#include "Input.h"
#include "Model.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include <optional>

#include "BaseCharacter.h"

class Player : public BaseCharacter {
private:
	enum ModelParts {
		kModelIndexBody, 
		kModelIndexHead,
		kModelIndexL_arm,
		kModelIndexR_arm,
		kModelIndexWeapon,
	};

	enum class Behavior {
		kRoot, // 通常状態
		kAttack, //攻撃中
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

	//腕のアニメーションの初期化
	void InitializeArmAnimation();

	void UpdateArmAnimation();

	//通常攻撃初期化
	void BehaviorRootInitialize();
	//攻撃行動初期化
	void BehaviorAttackInitialize();

	//通常行動更新
	void BehaviorRootUpdate();

	//攻撃行動更新
	void BehaviorAttackUpdate();

	/*void (Player::* Player::pBehaviorUpdateTable[])() = {

	}*/

public:
	// ワールド座標を取得
	Vector3 GetWorldPosition();

	void SetViewProjection(const ViewProjection* viewProjection) {
		viewProjection_ = viewProjection;
	}

private:
	Input* input_ = nullptr;

	// カメラのビュープロジェクション
	const ViewProjection* viewProjection_ = nullptr;
	// ワールド変換データ
	WorldTransform worldTransformBase_;
	WorldTransform worldTransformBody_;
	WorldTransform worldTransformHead_;
	WorldTransform worldTransformL_arm_;
	WorldTransform worldTransformR_arm_;
	WorldTransform worldTransformWeapon_;

	// モデル
	Model* modelFighterBody_;
	Model* modelFighterHead_;
	Model* modelFighterL_arm_;
	Model* modelFighterR_arm_;
	Model* modelFighterHammer_;

	// テクスチャハンドル
	//uint32_t textureHandle_ = 0;

	//浮遊ギミックの媒介変数
	float floatingParameter_ = 0.0f;

	//腕のアニメーションの変数
	float armParameter_ = 0.0f;

	//フレーム数
	const float kFloatingFPS = 60.0f;

	const float kArmFPS = 60.0f;

	//攻撃のアニメーションの変数
	struct AttackAnimation {
		float time;
		const int kAnimMaxTime;
		const int kAttackAllFrame;
	};

	AttackAnimation attack_{
		60.0f,
		60,
		90
	};

	//振る舞い
	Behavior behavior_ = Behavior::kRoot;

	//次の振る舞いのリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;
};
