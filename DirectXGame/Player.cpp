#include "Player.h"
#include "cassert"
#include "ImGuiManager.h"
#include "compute/compute.h"
#define _USE_MATH_DEFINES
#include <math.h>

Player::Player() {}

Player::~Player() {}

void Player::Initialize(const std::vector<Model*>& models) {
	input_ = Input::GetInstance();

	//基底クラスの初期化
	BaseCharacter::Initialize(models);
	
	//// 受け取ったモデルが読み込まれているかチェック
	//assert(modelBody);
	//assert(modelHead);
	//assert(modelL_arm);
	//assert(modelR_arm);
	//// 引数からモデルとテクスチャハンドルを受け取る
	//modelFighterBody_ = modelBody;
	//modelFighterHead_ = modelHead;
	//modelFighterL_arm_ = modelL_arm;
	//modelFighterR_arm_ = modelR_arm;

	// x,y,z方向のスケーリングを設定
	worldTransformBase_.scale_ = {1.0f, 1.0f, 1.0f};
	// x,y,z方向の回転を設定
	worldTransformBase_.rotation_ = {0.0f, 0.0f, 0.0f};
	// x,y,zの座標を設定
	worldTransformBase_.translation_ = {0.0f, 2.5f, 0.0f};

	worldTransformHead_.translation_ = {0.0f, 1.5f, 0.0f};
	worldTransformL_arm_.translation_ = {-0.5f, 1.3f, 0.0f}; 
	worldTransformR_arm_.translation_ = {0.5f, 1.3f, 0.0f};

	// ワールドトランスフォーム初期化
	worldTransformBase_.Initialize();
	worldTransformBody_.Initialize();
	worldTransformHead_.Initialize();
	worldTransformL_arm_.Initialize();
	worldTransformR_arm_.Initialize();

	//浮遊ギミックの初期化
	InitializeFloatingGimmick();
}

void Player::Update() { 
	// ワールド行列の更新
	//worldTransform_.UpdateMatrix();

	worldTransformBody_.parent_ = &worldTransformBase_;
	worldTransformHead_.parent_ = &worldTransformBody_;
	worldTransformL_arm_.parent_ = &worldTransformBody_;
	worldTransformR_arm_.parent_ = &worldTransformBody_;

	// キャラクターの移動ベクトル
	Vector3 move = {0, 0, 0};

	// キャラクターの移動速さ
	const float kCharacterSpeed = 0.3f;

#pragma region キーボード操作(移動)

	// 押した方向で移動ベクトルを変更(左右)
	if (input_->PushKey(DIK_LEFT)) {
		move.x -= kCharacterSpeed;
	} else if (input_->PushKey(DIK_RIGHT)) {
		move.x += kCharacterSpeed;
	}

	// 押した方向で移動ベクトルを変更(上下)
	if (input_->PushKey(DIK_UP)) {
		move.z += kCharacterSpeed;
	} else if (input_->PushKey(DIK_DOWN)) {
		move.z -= kCharacterSpeed;
	}
#pragma endregion

#pragma region コントローラーの操作
	// ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	// ゲームパッド状態取得
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		/*move.x += (float)joyState.Gamepad.sThumbLX / SHRT_MAX * kCharacterSpeed;
		move.z += (float)joyState.Gamepad.sThumbLY / SHRT_MAX * kCharacterSpeed;*/

		// 移動量
		move = {
		    (float)joyState.Gamepad.sThumbLX / SHRT_MAX * kCharacterSpeed,
			0.0f,
		    (float)joyState.Gamepad.sThumbLY / SHRT_MAX * kCharacterSpeed};

		// 移動量の速さを反映
		move = Multiply(kCharacterSpeed, Normalize(move));

		// カメラの角度から回転行列を計算する
		Matrix4x4 rotateXMatrix = MakeRotateXmatrix(viewProjection_->rotation_.x);
		Matrix4x4 rotateYMatrix = MakeRotateYmatrix(viewProjection_->rotation_.y);
		Matrix4x4 rotateZMatrix = MakeRotateZmatrix(viewProjection_->rotation_.z);
		Matrix4x4 rotateXYZMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));

		// 移動量に速さを反映(0度の移動ベクトル)
		move = Transform(move, rotateXYZMatrix);

		// Y軸周りの角度(0y)
		if (move.y != 0 || move.z != 0) {
			worldTransformBase_.rotation_.y = std::atan2(move.x, move.z);
		}

		// 座標移動(ベクトルの加算)
		worldTransformBase_.translation_.x += move.x;
		worldTransformBase_.translation_.y += move.y;
		worldTransformBase_.translation_.z += move.z;
	}
	// ワールド行列の更新
	worldTransformBase_.UpdateMatrix();
	// モデルの座標の更新
	worldTransformBody_.UpdateMatrix();
	worldTransformHead_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();

#pragma endregion

	//// 移動限界座標
	//const float kMoveLimitX = 30;
	//const float kMoveLimitY = 20;

	//// 範囲を超えない処理
	//worldTransformBase_.translation_.x = max(worldTransformBase_.translation_.x, -kMoveLimitX);
	//worldTransformBase_.translation_.x = min(worldTransformBase_.translation_.x, +kMoveLimitX);
	//worldTransformBase_.translation_.y = max(worldTransformBase_.translation_.y, -kMoveLimitY);
	//worldTransformBase_.translation_.y = min(worldTransformBase_.translation_.y, +kMoveLimitY);

	// 座標移動(ベクトルの加算)
	worldTransformBase_.translation_.x += move.x;
	worldTransformBase_.translation_.y += move.y;
	worldTransformBase_.translation_.z += move.z;

	//浮遊ギミックの更新
	UpdateFloatingGimmick();

	//腕のアニメーション
	UpdateArmAnimation();

#ifdef _DEBUG
	// キャラクターの座標を画面表示する処理
	ImGui::Begin("Debug");
	float playerPos[] = {
	    worldTransformBase_.translation_.x, worldTransformBase_.translation_.y,
	    worldTransformBase_.translation_.z};
	ImGui::SliderFloat3("PlayerPos", playerPos, 0.0f, 128.0f);
	ImGui::End();

	worldTransformBase_.translation_.x = playerPos[0];
	worldTransformBase_.translation_.y = playerPos[1];
	worldTransformBase_.translation_.z = playerPos[2];
#endif // _DEBUG
}

void Player::Draw(ViewProjection& viewProjection) {
	modelFighterBody_->Draw(worldTransformBody_, viewProjection);
	modelFighterHead_->Draw(worldTransformHead_, viewProjection);
	modelFighterL_arm_->Draw(worldTransformL_arm_, viewProjection);
	modelFighterR_arm_->Draw(worldTransformR_arm_, viewProjection);
}

const WorldTransform& Player::GetWorldTransform() {
	// TODO: return ステートメントをここに挿入します
	return worldTransformBase_;
}

void Player::InitializeFloatingGimmick() { 
	floatingParameter_ = 0.0f;
}

void Player::UpdateFloatingGimmick() {
	//浮遊移動のサイクル<frame>
	const uint16_t kPeriod = uint16_t(kFloatingFPS);
	//1フレームでのパラメータ加算値
	const float kStep = 2.0f * float(M_PI) / float(kPeriod);

	//パラメータを1ステップ分加算
	floatingParameter_ += kStep;
	//2πを超えたら0に戻す
	floatingParameter_ = std::fmod(floatingParameter_, 2.0f * float(M_PI));

	// 浮遊の振幅<m>
	const float floatingSwing = 0.1f;
	//浮遊を座標に反映
	worldTransformBody_.translation_.y = std::sin(floatingParameter_) * floatingSwing;

	ImGui::Begin("Player");
	float head[] = {
	    worldTransformHead_.translation_.x, worldTransformHead_.translation_.y,
	    worldTransformHead_.translation_.z};
	float L_arm[] = {
	    worldTransformL_arm_.translation_.x, worldTransformL_arm_.translation_.y,
	    worldTransformL_arm_.translation_.z};
	float R_arm[] = {
	    worldTransformR_arm_.translation_.x, worldTransformR_arm_.translation_.y,
	    worldTransformR_arm_.translation_.z};

	ImGui::SliderFloat3("Head Translation", head, -10.0f, 10.0f);
	ImGui::SliderFloat3("ArmL Translation", L_arm, -10.0f, 10.0f);
	ImGui::SliderFloat3("ArmR Translation", R_arm, -10.0f, 10.0f);

	ImGui::End();

	//頭
	worldTransformHead_.translation_.x = head[0];
	worldTransformHead_.translation_.y = head[1];
	worldTransformHead_.translation_.z = head[2];
	//左腕
	worldTransformL_arm_.translation_.x = L_arm[0];
	worldTransformL_arm_.translation_.y = L_arm[1];
	worldTransformL_arm_.translation_.z = L_arm[2];
	//右腕
	worldTransformR_arm_.translation_.x = R_arm[0];
	worldTransformR_arm_.translation_.y = R_arm[1];
	worldTransformR_arm_.translation_.z = R_arm[2];
}

void Player::UpdateArmAnimation() {
	// 浮遊移動のサイクル<frame>
	const uint16_t kPeriod = uint16_t(kArmFPS);
	// 1フレームでのパラメータ加算値
	const float kStep = 2.0f * float(M_PI) / float(kPeriod);

	// 
	armParameter_ += kStep;
	//
	armParameter_ = std::fmod(armParameter_, 2.0f * float(M_PI));

	//振幅
	const float armSwing = 0.5f;

	 worldTransformL_arm_.rotation_.x = std::sin(armParameter_) * armSwing;
	worldTransformR_arm_.rotation_.x = std::sin(armParameter_) * armSwing;
}

Vector3 Player::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransformBase_.matWorld_.m[3][0];
	worldPos.y = worldTransformBase_.matWorld_.m[3][1];
	worldPos.z = worldTransformBase_.matWorld_.m[3][2];

	return worldPos;
}

