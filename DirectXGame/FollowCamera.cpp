#include "FollowCamera.h"
#include "ImGuiManager.h"

#include "compute/compute.h"

void FollowCamera::Initialize() {

	viewProjection_.Initialize();

	// x,y,z方向の回転を設定
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
	// x,y,zの方向のを設定
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};
}

void FollowCamera::Update() {

	//追従対象がいれば
	if (target_) {
		//追従対象カメラまでのオフセット
		Vector3 offset = {0.0f, 2.0f, -20.0f};

		//カメラの角度から回転行列を計算する
		Matrix4x4 rotateXMatrix = MakeRotateXmatrix(viewProjection_.rotation_.x);
		Matrix4x4 rotateYMatrix = MakeRotateYmatrix(viewProjection_.rotation_.y);
		Matrix4x4 rotateZMatrix = MakeRotateZmatrix(viewProjection_.rotation_.z);
		Matrix4x4 rotateXYZMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));

		//オフセットをカメラの回転に合わせて回転させる
		offset = Transform(offset, rotateXYZMatrix);

		//座標をコピーしてオフセット分、ずらす
		viewProjection_.translation_ = Add(target_->translation_, offset);
	}

	// ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	// ゲームパッド状態取得
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		const float rotationSpeed = 0.03f;

		viewProjection_.rotation_.y += (float)joyState.Gamepad.sThumbRX / SHRT_MAX * rotationSpeed;
	}

	 viewProjection_.UpdateMatrix();
}
