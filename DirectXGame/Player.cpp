#include "Player.h"
#include "cassert"

Player::Player() {}

Player::~Player() {}

void Player::Initialize(Model* model) {
	// 受け取ったモデルが読み込まれているかチェック
	assert(model);
	// 引数からモデルとテクスチャハンドルを受け取る
	model_ = model;

	textureHandle_ = TextureManager::Load("player/player.png");

	// ワールドトランスフォーム初期化
	worldTransform_.Initialize();
}

void Player::Update() { worldTransform_.TransferMatrix(); }

void Player::Draw(ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}
