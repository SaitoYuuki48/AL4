#include "Player.h"
#include "cassert"
#include "ImGuiManager.h"
#include "compute/compute.h"

Player::Player() {}

Player::~Player() {}

void Player::Initialize(Model* model/*, uint32_t textureHandle*/) {
	input_ = Input::GetInstance();
	
	// 受け取ったモデルが読み込まれているかチェック
	assert(model);
	// 引数からモデルとテクスチャハンドルを受け取る
	model_ = model;
	//textureHandle_ = textureHandle;

	// x,y,z方向のスケーリングを設定
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	// x,y,z方向の回転を設定
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
	// x,y,zの座標を設定
	worldTransform_.translation_ = {0.0f, 2.5f, 0.0f};

	// ワールドトランスフォーム初期化
	worldTransform_.Initialize();
}

void Player::Update() { 
	// ワールド行列の更新
	//worldTransform_.UpdateMatrix();

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

		// 移動量に速さを反映(0度の移動ベクトル)
		move = Transform(move,)

		// 座標移動(ベクトルの加算)
		worldTransform_.translation_.x += move.x;
		worldTransform_.translation_.y += move.y;
		worldTransform_.translation_.z += move.z;
	}
	// ワールド行列の更新
	worldTransform_.UpdateMatrix();
	
#pragma endregion

	// 移動限界座標
	const float kMoveLimitX = 30;
	const float kMoveLimitY = 20;

	// 範囲を超えない処理
	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.y = max(worldTransform_.translation_.y, -kMoveLimitY);
	worldTransform_.translation_.y = min(worldTransform_.translation_.y, +kMoveLimitY);

	// 座標移動(ベクトルの加算)
	worldTransform_.translation_.x += move.x;
	worldTransform_.translation_.y += move.y;
	worldTransform_.translation_.z += move.z;

#ifdef _DEBUG
	// キャラクターの座標を画面表示する処理
	ImGui::Begin("Debug");
	float playerPos[] = {
	    worldTransform_.translation_.x, worldTransform_.translation_.y,
	    worldTransform_.translation_.z};
	ImGui::SliderFloat3("PlayerPos", playerPos, 0.0f, 128.0f);
	ImGui::End();

	worldTransform_.translation_.x = playerPos[0];
	worldTransform_.translation_.y = playerPos[1];
	worldTransform_.translation_.z = playerPos[2];
#endif // _DEBUG
}

void Player::Draw(ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}

void Player::SetParent(const WorldTransform* parent) {
	// 親子関係を結ぶ
	worldTransform_.parent_ = parent;
}

const WorldTransform& Player::GetWorldTransform() {
	// TODO: return ステートメントをここに挿入します
	return worldTransform_;
}

Vector3 Player::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void Player::SetViewProjection(const ViewProjection* viewProjection) {
	viewProjection_ = viewProjection;
}
