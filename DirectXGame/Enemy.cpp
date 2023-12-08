#include "Enemy.h"

void Enemy::Initialize(const std::vector<Model*>& models)
{
	// x,y,z方向のスケーリングを設定
	worldTransformBase_.scale_ = {1.0f, 1.0f, 1.0f};
	// x,y,z方向の回転を設定
	worldTransformBase_.rotation_ = {0.0f, 0.0f, 0.0f};
	// x,y,zの座標を設定
	worldTransformBase_.translation_ = {0.0f, 0.0f, 0.0f};

	worldTransformBody_.translation_ = {0.0f, 0.0f, 0.0f};
	worldTransformL_arm_.translation_ = {-0.9f, 1.0f, 0.0f};
	worldTransformR_arm_.translation_ = {0.9f, 1.0f, 0.0f};

	velocity_ = {0.0f, 0.0f, 0.3f};

	// 基底クラスの初期化
	BaseCharacter::Initialize(models);

	// ワールドトランスフォーム初期化
	worldTransformBase_.Initialize();
	worldTransformBody_.Initialize();
	worldTransformL_arm_.Initialize();
	worldTransformR_arm_.Initialize();
}

void Enemy::Update() {
	//親子関係
	worldTransformBody_.parent_ = &worldTransformBase_;
	worldTransformL_arm_.parent_ = &worldTransformBody_;
	worldTransformR_arm_.parent_ = &worldTransformBody_;

	switch (phase_) {
	case Phase::Approach: // 接近
		Approach();
		break;
	case Phase::Leave: // 離脱
		Leave();
		break;
	}

	// ワールド行列の更新
	worldTransformBase_.UpdateMatrix();
	// モデルの座標の更新
	worldTransformBody_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();
}

void Enemy::Draw(ViewProjection& viewProjection) {
	models_[kModelIndexBody]->Draw(worldTransformBody_, viewProjection);
	models_[kModelIndexL_arm]->Draw(worldTransformL_arm_, viewProjection);
	models_[kModelIndexR_arm]->Draw(worldTransformR_arm_, viewProjection);
}

void Enemy::Approach() {
	// 移動(ベクトルを加算)
	worldTransformBase_.translation_.z += velocity_.z;
	// 規定の位置に到達したら離脱
	if (worldTransformBase_.translation_.z > 100.0f) {
		phase_ = Phase::Leave;
	}
}

void Enemy::Leave() {
	// 移動(ベクトルを加算)
	worldTransformBase_.translation_.z -= velocity_.z;
	// 規定の位置に到達したら離脱
	if (worldTransformBase_.translation_.z < 0.0f) {
		phase_ = Phase::Approach;
	}
}
