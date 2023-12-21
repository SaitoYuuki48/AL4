#include "Enemy.h"
#include "compute/compute.h"

void Enemy::Initialize(const std::vector<Model*>& models)
{
	// x,y,z方向のスケーリングを設定
	worldTransformBase_.scale_ = {1.0f, 1.0f, 1.0f};
	// x,y,z方向の回転を設定
	worldTransformBase_.rotation_ = {0.0f, 0.0f, 0.0f};
	// x,y,zの座標を設定
	worldTransformBase_.translation_ = {-5.0f, 0.0f, 5.0f};

	worldTransformBody_.translation_ = {0.0f, 0.0f, 0.0f};
	worldTransformL_arm_.translation_ = {-0.9f, 1.0f, 0.0f};
	worldTransformR_arm_.translation_ = {0.9f, 1.0f, 0.0f};

	move = {0.0f, 0.0f, 0.0f};
	
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

	//switch (phase_) {
	//case Phase::Approach: // 接近
	//	Approach();
	//	break;
	//case Phase::Leave: // 離脱
	//	Leave();
	//	break;
	//}

	worldTransformBase_.rotation_.y += 0.01f;


	// カメラの角度から回転行列を計算する
	Matrix4x4 rotateXMatrix = MakeRotateXmatrix(worldTransformBase_.rotation_.x);
	Matrix4x4 rotateYMatrix = MakeRotateYmatrix(worldTransformBase_.rotation_.y);
	Matrix4x4 rotateZMatrix = MakeRotateZmatrix(worldTransformBase_.rotation_.z);
	Matrix4x4 rotateXYZMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));
	
	// 速度　回転
	const float speed = 0.1f;
	move = {0.0f,0.0f,speed};

	// オフセットをカメラの回転に合わせて回転させる
	move = TransformNormal(move, rotateYMatrix);

	// 座標移動(ベクトルの加算)
	 worldTransformBase_.translation_.x += move.x;
	 worldTransformBase_.translation_.y += move.y;
	 worldTransformBase_.translation_.z += move.z;


	// ワールド行列の更新
	worldTransformBase_.UpdateMatrix();
	// モデルの座標の更新
	worldTransformBody_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();
}

void Enemy::Draw(const ViewProjection& viewProjection) {
	models_[kModelIndexBody]->Draw(worldTransformBody_, viewProjection);
	models_[kModelIndexL_arm]->Draw(worldTransformL_arm_, viewProjection);
	models_[kModelIndexR_arm]->Draw(worldTransformR_arm_, viewProjection);
}
