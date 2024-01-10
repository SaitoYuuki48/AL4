#include "GameScene.h"
#include "TextureManager.h"
#include "AxisIndicator.h"
#include <cassert>
#include <fstream>

GameScene::GameScene() {}

GameScene::~GameScene() {  
	for (Sphere* sphere : spheres_) {
		delete sphere;
	}
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// ビュープロジェクションの初期化
	viewProjection_.farZ = 600;
	viewProjection_.Initialize();

	//プレイヤー
	// モデル
	modelFighterBody_.reset(Model::CreateFromOBJ("player_Body", true));
	modelFighterHead_.reset(Model::CreateFromOBJ("player_Head", true));
	modelFighterL_arm_.reset(Model::CreateFromOBJ("player_L_arm", true));
	modelFighterR_arm_.reset(Model::CreateFromOBJ("player_R_arm", true));
	modelFighterWeapon_.reset(Model::CreateFromOBJ("hammer", true));

	std::vector<Model*> playerModels = {
	    modelFighterBody_.get(), modelFighterHead_.get(), modelFighterL_arm_.get(),
	    modelFighterR_arm_.get(), modelFighterWeapon_.get()};

	// 自キャラの生成
	player_ = std::make_unique<Player>();
	// 自キャラの初期化
	player_->Initialize(playerModels);

	//玉のモデル
	modelSphere_.reset(Model::CreateFromOBJ("sphere", true));

	//天球
	// 3Dモデルの生成
	modelSkydome_.reset(Model::CreateFromOBJ("skydome", true));
	// 天球の生成
	skydome_ = std::make_unique<Skydome>();
	// 天球の初期化
	skydome_->Initialize(modelSkydome_.get());

	//地面
	//  3Dモデルの生成
	modelGround_.reset(Model::CreateFromOBJ("Ground", true));
	// 地面の生成
	ground_ = std::make_unique<Ground>();
	// 地面の初期化
	ground_->Initialize(modelGround_.get());

	// フォローカメラの生成
	followCamera_ = std::make_unique<FollowCamera>();

	followCamera_->Initialize();

	// 自キャラのワールドトランスフォームを追従カメラにセット
	followCamera_->SetTarget(&player_->GetWorldTransform());

	player_->SetViewProjection(&followCamera_->GetViewProjection());

	// 玉発生データの読み込み
	LoadSpherePopData();

	//テクスチャの初期化
	TextureInitialize();

	// デバッグカメラの生成
	debugCamera_ = std::make_unique<DebugCamera>(1280, 720);

#ifdef _DEBUG
	// 軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方向表示が参照するビュープロジェクションを指定する(アドレス渡し)
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);
#endif // _DEBUG
}

void GameScene::Update() {

	//フォローカメラ
	followCamera_->Update();

	// デバッグカメラ
	debugCamera_->Update();

#ifdef _DEBUG

	if (input_->TriggerKey(DIK_C) && isDebugCameraActive_ == false) {
		isDebugCameraActive_ = true;
	}

	if (input_->TriggerKey(DIK_V) && isDebugCameraActive_ == true) {
		isDebugCameraActive_ = false;
	}

#endif // _DEBUG

	// カメラの処理 
	if (isDebugCameraActive_ == true) {
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		// ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();
	}
	else {
		viewProjection_.matView = followCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;
		// ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();
	}

	
	// 玉発生コマンドの更新
	UpdateSpherePopCommands();

	// 自キャラの更新
	if (clearFlag == false && gameoverFlag == false) {
		player_->Update();
	}

	// 玉の更新
	//sphere_->Update();
	for (Sphere* sphere : spheres_) {
		sphere->Update();
	}

	// 天球の更新
	skydome_->Update();

	// 地面の更新
	ground_->Update();

	// 衝突判定と応答
	CheckAllCollisions();

	ScoreUpdate(sphereNum_, kSphereNum_);

	// デスフラグの立った玉を削除
	spheres_.remove_if([](Sphere* sphere) {
		if (sphere->IsDead()) {
			delete sphere;
			return true;
		}
		return false;
	});


	// ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	TimerUpdate(timer_);

	if (sphereNum_ >= kSphereNum_){
		clearFlag = true;
		if (clearFlag == true) {
			if (Input::GetInstance()->GetJoystickState(0, joyState)) {
				if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
					isSceneEnd = true;
				}
			}
		}
	} 

	if (clearFlag == false && gameoverFlag == false) {
		timer_--;
	}

	if (timer_ <= 0) {
		gameoverFlag = true;
		if (gameoverFlag == true) {
			if (Input::GetInstance()->GetJoystickState(0, joyState)) {
				if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
					isSceneEnd = true;
				}
			}
		}
	}
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// 自キャラの描画
	player_->Draw(viewProjection_);

	//玉の描画
	//sphere_->Draw(viewProjection_);
	for (Sphere* sphere : spheres_) {
		sphere->Draw(viewProjection_);
	}

	// 天球の描画
	skydome_->Draw(viewProjection_);

	//地面の描画
	ground_->Draw(viewProjection_);

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	//取得数、必要数
	textureNumber2_[sphereNum2]->Draw();
	textureNumber_[sphereNum1]->Draw();
	textureSlash_->Draw();
	textureNumber10_1_[sphereNum10_1]->Draw();
	textureNumber10_0_[sphereNum10_0]->Draw();
	//時間
	textureTimer0_[timerNum0]->Draw();
	textureTimer1_[timerNum1]->Draw();

	if (clearFlag == true) {
		textureClear->Draw();
	} 
	else if (gameoverFlag == true) {
		textureGameover->Draw();
	}

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::SphereSpawn(Vector3 position) { 
	// 宣言
	Sphere* sphere = new Sphere; 
	// 玉の初期化
	sphere->Initialize(modelSphere_.get(),position);
	//リストに登録
	spheres_.push_back(sphere);
}

void GameScene::LoadSpherePopData() {
	spherePopCommands.clear();

	// ファイルを開く
	std::ifstream file;
	file.open("Resources/spherePop.csv");
	assert(file.is_open());

	// ファイルの内容を文字列ストリームにコピー
	spherePopCommands << file.rdbuf();

	// ファイルを閉じる
	file.close();
}

void GameScene::UpdateSpherePopCommands() {
	// 待機処理
	if (spherePopWaitFlag) {
		spherePopWaitTimer--;
		if (spherePopWaitTimer <= 0) {
			// 待機完了
			spherePopWaitFlag = false;
		}
		return;
	}

	// 1行分の文字列を入れる変数
	std::string line;

	/// コマンド実行ループ
	while (getline(spherePopCommands, line)) {
		// 1行文の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		std::string word;
		//,区切りで行の先頭文字列を取得
		getline(line_stream, word, ',');

		// "//"から始まる行はコメント
		if (word.find("//") == 0) {
			// コメント行を飛ばす
			continue;
		}

		// POPコマンド
		if (word.find("POP") == 0) {
			// x座標
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());

			// y座標
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());

			// z座標
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			// 敵を発生させる
			SphereSpawn(Vector3(x, y, z));
		}
		// WAITコマンド
		else if (word.find("WAIT") == 0) {
			getline(line_stream, word, ',');

			// 待ち時間
			int32_t waitTime = atoi(word.c_str());

			// 待機開始
			spherePopWaitFlag = true;
			spherePopWaitTimer = waitTime;

			// コマンドループを抜ける
			break;
		}
	}
}

void GameScene::CheckAllCollisions() {
	// 判定対象AとBの座標
	Vector3 posA, posB;
	
	//自キャラの座標
	posA = player_->GetWorldPosition();

	//自キャラと玉の当たり判定
	for (Sphere* sphere : spheres_) {
		//玉の座標
		posB = sphere->GetWorldPosition();

		float radius1 = 1.5f;
		float radius2 = 2.0f;

		float hit = (posB.x - posA.x) * (posB.x - posA.x) + (posB.y - posA.y) * (posB.y - posA.y) +
		            (posB.z - posA.z) * (posB.z - posA.z);

		float radius = (radius1 + radius2) * (radius1 + radius2);

		// 球と球の交差判定
		if (hit <= radius) {
			// 敵弾の衝突時コールバックを呼び出す
			sphere->OnCollision();

			//玉の取得数を足す
			sphereNum_++;
		}
	}
}

void GameScene::ScoreUpdate(int32_t num, int32_t kNum) {
	// 2桁
	sphereNum2 = num / 10;
	num = num % 10;
	// 1桁
	sphereNum1 = num;

	// 2桁
	sphereNum10_1 = kNum / 10;
	kNum = kNum % 10;
	// 1桁
	sphereNum10_0 = kNum;
}

void GameScene::TimerUpdate(int32_t timer) {

	timer = timer / 60;

	// 2桁
	timerNum1 = timer / 10;
	timer = timer % 10;
	// 1桁
	timerNum0 = timer;
}

void GameScene::TextureInitialize() {
	// 数字の画像
	uint32_t numHandle[10];
	numHandle[0] = TextureManager::Load("./Resources/number/0.png");
	numHandle[1] = TextureManager::Load("./Resources/number/1.png");
	numHandle[2] = TextureManager::Load("./Resources/number/2.png");
	numHandle[3] = TextureManager::Load("./Resources/number/3.png");
	numHandle[4] = TextureManager::Load("./Resources/number/4.png");
	numHandle[5] = TextureManager::Load("./Resources/number/5.png");
	numHandle[6] = TextureManager::Load("./Resources/number/6.png");
	numHandle[7] = TextureManager::Load("./Resources/number/7.png");
	numHandle[8] = TextureManager::Load("./Resources/number/8.png");
	numHandle[9] = TextureManager::Load("./Resources/number/9.png");

	uint32_t slashHandle = TextureManager::Load("./Resources/number/slash.png");

	textureSlash_ =
	    Sprite::Create(slashHandle, {100.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});

	//取得した数
	textureNumber_[0] =
	    Sprite::Create(numHandle[0], {50.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureNumber_[1] =
	    Sprite::Create(numHandle[1], {50.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureNumber_[2] =
	    Sprite::Create(numHandle[2], {50.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureNumber_[3] =
	    Sprite::Create(numHandle[3], {50.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureNumber_[4] =
	    Sprite::Create(numHandle[4], {50.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureNumber_[5] =
	    Sprite::Create(numHandle[5], {50.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureNumber_[6] =
	    Sprite::Create(numHandle[6], {50.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureNumber_[7] =
	    Sprite::Create(numHandle[7], {50.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureNumber_[8] =
	    Sprite::Create(numHandle[8], {50.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureNumber_[9] =
	    Sprite::Create(numHandle[9], {50.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});

	textureNumber2_[0] =
	    Sprite::Create(numHandle[0], {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureNumber2_[1] =
	    Sprite::Create(numHandle[1], {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureNumber2_[2] =
	    Sprite::Create(numHandle[2], {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureNumber2_[3] =
	    Sprite::Create(numHandle[3], {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureNumber2_[4] =
	    Sprite::Create(numHandle[4], {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureNumber2_[5] =
	    Sprite::Create(numHandle[5], {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureNumber2_[6] =
	    Sprite::Create(numHandle[6], {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureNumber2_[7] =
	    Sprite::Create(numHandle[7], {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureNumber2_[8] =
	    Sprite::Create(numHandle[8], {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureNumber2_[9] =
	    Sprite::Create(numHandle[9], {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});

	textureNumber10_0_[0] =
	    Sprite::Create(numHandle[0], {200.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureNumber10_0_[1] =
	    Sprite::Create(numHandle[1], {200.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureNumber10_0_[2] =
	    Sprite::Create(numHandle[2], {200.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureNumber10_0_[3] =
	    Sprite::Create(numHandle[3], {200.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureNumber10_0_[4] =
	    Sprite::Create(numHandle[4], {200.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureNumber10_0_[5] =
	    Sprite::Create(numHandle[5], {200.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureNumber10_0_[6] =
	    Sprite::Create(numHandle[6], {200.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureNumber10_0_[7] =
	    Sprite::Create(numHandle[7], {200.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureNumber10_0_[8] =
	    Sprite::Create(numHandle[8], {200.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureNumber10_0_[9] =
	    Sprite::Create(numHandle[9], {200.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});

	//必要数
	textureNumber10_1_[0] =
	    Sprite::Create(numHandle[0], {150.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureNumber10_1_[1] =
	    Sprite::Create(numHandle[1], {150.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureNumber10_1_[2] =
	    Sprite::Create(numHandle[2], {150.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureNumber10_1_[3] =
	    Sprite::Create(numHandle[3], {150.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureNumber10_1_[4] =
	    Sprite::Create(numHandle[4], {150.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureNumber10_1_[5] =
	    Sprite::Create(numHandle[5], {150.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureNumber10_1_[6] =
	    Sprite::Create(numHandle[6], {150.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureNumber10_1_[7] =
	    Sprite::Create(numHandle[7], {150.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureNumber10_1_[8] =
	    Sprite::Create(numHandle[8], {150.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureNumber10_1_[9] =
	    Sprite::Create(numHandle[9], {150.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});

	// 時間の画像
	textureTimer0_[0] = Sprite::Create(numHandle[0], {1180.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureTimer0_[1] = Sprite::Create(numHandle[1], {1180.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureTimer0_[2] = Sprite::Create(numHandle[2], {1180.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureTimer0_[3] =	Sprite::Create(numHandle[3], {1180.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureTimer0_[4] = Sprite::Create(numHandle[4], {1180.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureTimer0_[5] =	Sprite::Create(numHandle[5], {1180.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureTimer0_[6] = Sprite::Create(numHandle[6], {1180.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureTimer0_[7] = Sprite::Create(numHandle[7], {1180.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureTimer0_[8] = Sprite::Create(numHandle[8], {1180.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureTimer0_[9] =	Sprite::Create(numHandle[9], {1180.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});

	textureTimer1_[0] = Sprite::Create(numHandle[0], {1130.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureTimer1_[1] = Sprite::Create(numHandle[1], {1130.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureTimer1_[2] = Sprite::Create(numHandle[2], {1130.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureTimer1_[3] = Sprite::Create(numHandle[3], {1130.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureTimer1_[4] = Sprite::Create(numHandle[4], {1130.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureTimer1_[5] = Sprite::Create(numHandle[5], {1130.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureTimer1_[6] = Sprite::Create(numHandle[6], {1130.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureTimer1_[7] = Sprite::Create(numHandle[7], {1130.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureTimer1_[8] = Sprite::Create(numHandle[8], {1130.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureTimer1_[9] = Sprite::Create(numHandle[9], {1130.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});

	//クリア、ゲームオーバーの画像
	uint32_t clearHandle = TextureManager::Load("./Resources/CLEAR.png");
	uint32_t gameoverHandle = TextureManager::Load("./Resources/GAMEOVER.png");

	textureClear = Sprite::Create(clearHandle, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	textureGameover = Sprite::Create(gameoverHandle, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
}


void GameScene::sceneReset() { 
	isSceneEnd = false;

	clearFlag = false;

	gameoverFlag = false;

	sphereNum_ = 0; 

	timer_ = kTimer_;

	// シーン移行する際にリセットしたい値をまとめた関数用意するといい
	spheres_.clear();

	player_->PositionReset();

	// 玉発生データの読み込み
	LoadSpherePopData();
}