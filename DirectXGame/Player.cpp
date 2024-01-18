#include "Player.h"
#include "ImGuiManager.h"
#include "cassert"
#include "compute/compute.h"
#define _USE_MATH_DEFINES
#include "FollowCamera.h"
#include "easing.h"
#include <math.h>

Player::Player() {}

Player::~Player() {}

void Player::Initialize(const std::vector<Model*>& models) {
	input_ = Input::GetInstance();

	// x,y,z方向のスケーリングを設定
	worldTransformBase_.scale_ = {1.1f, 1.1f, 1.1f};
	// x,y,z方向の回転を設定
	worldTransformBase_.rotation_ = {0.0f, 0.0f, 0.0f};
	// x,y,zの座標を設定
	worldTransformBase_.translation_ = {0.0f, 0.0f, 0.0f};

	worldTransformHead_.scale_ = {1.1f, 1.1f, 1.1f};
	worldTransformL_arm_.scale_ = {1.1f, 1.1f, 1.1f};
	worldTransformR_arm_.scale_ = {1.1f, 1.1f, 1.1f};
	
	worldTransformHead_.translation_ = {0.0f, 1.5f, 0.0f};
	worldTransformL_arm_.translation_ = {-0.5f, 1.3f, 0.0f};
	worldTransformR_arm_.translation_ = {0.5f, 1.3f, 0.0f};
	worldTransformWeapon_.translation_ = {0.0f, 2.0f, 0.0f};

	// 親子関係
	worldTransformBody_.parent_ = &worldTransformBase_;
	worldTransformHead_.parent_ = &worldTransformBody_;
	worldTransformL_arm_.parent_ = &worldTransformBody_;
	worldTransformR_arm_.parent_ = &worldTransformBody_;
	worldTransformWeapon_.parent_ = &worldTransformBody_;

	// 基底クラスの初期化
	BaseCharacter::Initialize(models);

	// ワールドトランスフォーム初期化
	worldTransformBase_.Initialize();
	worldTransformBody_.Initialize();
	worldTransformHead_.Initialize();
	worldTransformL_arm_.Initialize();
	worldTransformR_arm_.Initialize();
	worldTransformWeapon_.Initialize();

	// 浮遊ギミックの初期化
	InitializeFloatingGimmick();
}

void Player::Update() {

	if (behaviorRequest_) {
		// 振る舞いを変更する
		behavior_ = behaviorRequest_.value();
		// 各振る舞い事の初期化を実行
		switch (behavior_) {
		case Behavior::kRoot:
		default:
			// ルートビヘイビアの初期化
			BehaviorRootInitialize();
			break;
		case Behavior::kAttack:
			// アタックビヘイビアの初期化
			BehaviorAttackInitialize();
			break;
		case Behavior::kJump:
			//ジャンプビヘイビアの初期化
			BehaviorJumpInitialize();
			break;
		}
		// 振る舞いリクエストをリセット
		behaviorRequest_ = std::nullopt;
	}

	switch (behavior_) {
		// 通常行動
	case Behavior::kRoot:
	default:
		// ルートビヘイビアの更新
		BehaviorRootUpdate();
		break;
		// 攻撃行動
	case Behavior::kAttack:
		// アタックビヘイビアの更新
		BehaviorAttackUpdate();
		break;
	case Behavior::kJump:
		//ジャンプビヘイビアの更新
		BehaviorJumpUpdate();
		break;
	}

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

	// ワールド行列の更新
	worldTransformBase_.UpdateMatrix();
	// モデルの座標の更新
	worldTransformBody_.UpdateMatrix();
	worldTransformHead_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();
	worldTransformWeapon_.UpdateMatrix();
}

void Player::Draw(const ViewProjection& viewProjection) {
	models_[kModelIndexBody]->Draw(worldTransformBody_, viewProjection);
	models_[kModelIndexHead]->Draw(worldTransformHead_, viewProjection);
	models_[kModelIndexL_arm]->Draw(worldTransformL_arm_, viewProjection);
	models_[kModelIndexR_arm]->Draw(worldTransformR_arm_, viewProjection);

	if (behavior_ == Behavior::kAttack) {
		models_[kModelIndexWeapon]->Draw(worldTransformWeapon_, viewProjection);
	}
}

const WorldTransform& Player::GetWorldTransform() {
	// TODO: return ステートメントをここに挿入します
	return worldTransformBase_;
}

void Player::InitializeFloatingGimmick() { floatingParameter_ = 0.0f; }

void Player::UpdateFloatingGimmick() {
	// 浮遊移動のサイクル<frame>
	const uint16_t kPeriod = uint16_t(kFloatingFPS);
	// 1フレームでのパラメータ加算値
	const float kStep = 2.0f * float(M_PI) / float(kPeriod);

	// パラメータを1ステップ分加算
	floatingParameter_ += kStep;
	// 2πを超えたら0に戻す
	floatingParameter_ = std::fmod(floatingParameter_, 2.0f * float(M_PI));

	// 浮遊の振幅<m>
	const float floatingSwing = 0.1f;
	// 浮遊を座標に反映
	worldTransformBody_.translation_.y = std::sin(floatingParameter_) * floatingSwing;

#ifdef _DEBUG
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

	// 頭
	worldTransformHead_.translation_.x = head[0];
	worldTransformHead_.translation_.y = head[1];
	worldTransformHead_.translation_.z = head[2];
	// 左腕
	worldTransformL_arm_.translation_.x = L_arm[0];
	worldTransformL_arm_.translation_.y = L_arm[1];
	worldTransformL_arm_.translation_.z = L_arm[2];
	// 右腕
	worldTransformR_arm_.translation_.x = R_arm[0];
	worldTransformR_arm_.translation_.y = R_arm[1];
	worldTransformR_arm_.translation_.z = R_arm[2];
#endif // _DEBUG
}

void Player::InitializeArmAnimation() { armParameter_ = 0.0f; }

void Player::UpdateArmAnimation() {
	// 浮遊移動のサイクル<frame>
	const uint16_t kPeriod = uint16_t(kArmFPS);
	// 1フレームでのパラメータ加算値
	const float kStep = 2.0f * float(M_PI) / float(kPeriod);

	//
	armParameter_ += kStep;
	//
	armParameter_ = std::fmod(armParameter_, 2.0f * float(M_PI));

	// 振幅
	const float armSwing = 0.5f;

	worldTransformL_arm_.rotation_.x = std::sin(armParameter_) * armSwing;
	worldTransformR_arm_.rotation_.x = std::sin(armParameter_) * armSwing;
}

void Player::BehaviorRootInitialize() {
	InitializeFloatingGimmick();
	InitializeArmAnimation();

	worldTransformL_arm_.rotation_.x = 0.0f;
	worldTransformR_arm_.rotation_.x = 0.0f;
	worldTransformWeapon_.rotation_.x = 0.0f;

	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();
	worldTransformWeapon_.UpdateMatrix();
}

void Player::BehaviorRootUpdate() {

	// キャラクターの移動速さ
	const float kCharacterSpeed = 0.3f;

#pragma region キーボード操作(移動)

	// 押した方向で移動ベクトルを変更(左右)
	if (input_->PushKey(DIK_LEFT)) {
		velocity_.x -= kCharacterSpeed;
	} else if (input_->PushKey(DIK_RIGHT)) {
		velocity_.x += kCharacterSpeed;
	}

	// 押した方向で移動ベクトルを変更(上下)
	if (input_->PushKey(DIK_UP)) {
		velocity_.z += kCharacterSpeed;
	} else if (input_->PushKey(DIK_DOWN)) {
		velocity_.z -= kCharacterSpeed;
	}
#pragma endregion

#pragma region コントローラーの操作
	// ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	// ゲームパッド状態取得
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {

		// 移動量
		velocity_ = {
		    (float)joyState.Gamepad.sThumbLX / SHRT_MAX * kCharacterSpeed, 0.0f,
		    (float)joyState.Gamepad.sThumbLY / SHRT_MAX * kCharacterSpeed};

		// 移動量の速さを反映
		velocity_ = Multiply(kCharacterSpeed, Normalize(velocity_));

		// カメラの角度から回転行列を計算する
		Matrix4x4 rotateXMatrix = MakeRotateXmatrix(viewProjection_->rotation_.x);
		Matrix4x4 rotateYMatrix = MakeRotateYmatrix(viewProjection_->rotation_.y);
		Matrix4x4 rotateZMatrix = MakeRotateZmatrix(viewProjection_->rotation_.z);
		Matrix4x4 rotateXYZMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));

		// 移動量に速さを反映(0度の移動ベクトル)
		velocity_ = Transform(velocity_, rotateXYZMatrix);

		// Y軸周りの角度(0y)

		if (/* move.y != 0 || move.z != 0*/ Length(velocity_) != 0) {
			worldTransformBase_.rotation_.y = std::atan2(velocity_.x, velocity_.z);
		}

		// 座標移動(ベクトルの加算)
		worldTransformBase_.translation_.x += velocity_.x;
		worldTransformBase_.translation_.y += velocity_.y;
		worldTransformBase_.translation_.z += velocity_.z;

		// 攻撃
		/*if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
			behaviorRequest_ = Behavior::kAttack;
		}*/

		//ジャンプ
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B) {
			behaviorRequest_ = Behavior::kJump;
		}
	}

#pragma endregion

	// 浮遊ギミックの更新
	UpdateFloatingGimmick();

	// 腕のアニメーション
	UpdateArmAnimation();

	//// 移動限界座標
	// const float kMoveLimitX = 30;
	// const float kMoveLimitZ = 20;

	//// 範囲を超えない処理
	// worldTransformBase_.translation_.x = max(worldTransformBase_.translation_.x, -kMoveLimitX);
	// worldTransformBase_.translation_.x = min(worldTransformBase_.translation_.x, +kMoveLimitX);
	// worldTransformBase_.translation_.z = max(worldTransformBase_.translation_.Z, -kMoveLimitZ);
	// worldTransformBase_.translation_.z = min(worldTransformBase_.translation_.Z, +kMoveLimitZ);
}

void Player::BehaviorAttackInitialize() { 
	attack_.time = 0.0f; 
}

void Player::BehaviorAttackUpdate() {

	const float kDegreeToRadian = (float)M_PI / 180.0f;

	attack_.time++;
	if (attack_.time <= attack_.kAnimMaxTime) {
		float frame = (float)(attack_.time / attack_.kAnimMaxTime);
		float easeInBack = EaseInBack(frame * frame);
		float weaponAngle = (float)((90 * kDegreeToRadian)) * easeInBack;
		float armAngle = (float)((120 * kDegreeToRadian)) * easeInBack;
		worldTransformWeapon_.rotation_.x = weaponAngle;
		worldTransformL_arm_.rotation_.x = armAngle + (float)M_PI;
		worldTransformR_arm_.rotation_.x = armAngle + (float)M_PI;
	} else if (attack_.time >= attack_.kAllFrame) {
		attack_.time = 0.0f;
		behaviorRequest_ = Behavior::kRoot;
		FollowCamera::SetShakeFlag(false);
	} else if (attack_.time >= attack_.kAnimMaxTime) {
		// アニメーションが終わったらカメラを揺らす
		FollowCamera::SetShakeFlag(true);
	}
}

void Player::BehaviorJumpInitialize() {
	worldTransformBody_.translation_.y = 0;
	worldTransformL_arm_.rotation_.x = 0;
	worldTransformR_arm_.rotation_.x = 0;

	/*worldTransformL_arm_.rotation_.x += 3.0f;
	worldTransformR_arm_.rotation_.x += 3.0f;*/

	//ジャンプ初速
	const float kJumpFirstSpeed = 1.0f;
	//ジャンプ初速を与える
	velocity_.y = kJumpFirstSpeed;
}

void Player::BehaviorJumpUpdate() {
	//移動
	worldTransformBase_.translation_ = Add(worldTransformBase_.translation_, velocity_);
	//重力加速度
	const float kGravityAcceleration = 0.05f;
	//加速度ベクトル
	Vector3 accelerationVector = {0, -kGravityAcceleration, 0};
	// 加速する
	velocity_ = Add(velocity_, accelerationVector);


	const float kDegreeToRadian = (float)M_PI / 180.0f;

	jump_.time++;
	if (jump_.time <= jump_.kAnimMaxTime) {
		float frame = (float)(jump_.time / jump_.kAnimMaxTime);
		float easeInSine = EaseInSine(frame * frame);
		float armAngle = (float)((180 * kDegreeToRadian)) * easeInSine;
		worldTransformL_arm_.rotation_.x = armAngle + (float)M_PI;
		worldTransformR_arm_.rotation_.x = armAngle + (float)M_PI;
	} else if (jump_.time >= jump_.kAllFrame) {
		jump_.time = 0.0f;
	}

	//着地
	if (worldTransformBase_.translation_.y <= 0.0f) {
		worldTransformBase_.translation_.y = 0;
		worldTransformL_arm_.rotation_.x = 0;
		worldTransformR_arm_.rotation_.x = 0;

		jump_.time = 0.0f;

		//ジャンプ終了
		behaviorRequest_ = Behavior::kRoot;
	}
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

void Player::PositionReset() {
	// x,y,z方向の回転を設定
	worldTransformBase_.rotation_ = {0.0f, 0.0f, 0.0f};
	// x,y,zの座標を設定
	worldTransformBase_.translation_ = {0.0f, 0.0f, 0.0f};
	// 振る舞い
	behavior_ = Behavior::kRoot;
}
