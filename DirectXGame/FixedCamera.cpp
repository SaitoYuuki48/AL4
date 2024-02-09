#include "FixedCamera.h"
#include "compute/compute.h"

void FixedCamera::Initialize() {
	// x,y,z方向の回転を設定
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
	// x,y,zの方向のを設定
	worldTransform_.translation_ = {0.0f, 8.0f, -50.0f};

	// ビュープロジェクションの初期化
	viewProjection_.Initialize();
}

void FixedCamera::Update() {
	worldTransform_.matWorld_ = MakeAffineMatrix(
	    {1.0f, 1.0f, 1.0f}, worldTransform_.rotation_, worldTransform_.translation_);

	// カメラオブジェクトのワールド行列からビュー行列を計算する
	viewProjection_.matView = Inverse(worldTransform_.matWorld_);

	// viewProjection_.UpdateMatrix();
}
