#pragma once
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <input.h>

/// <summary>
/// 追従カメラ
/// </summary>
class FollowCamera {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

public:
	void SetTarget(const WorldTransform* target) { target_ = target; }

	static void SetShakeFlag(bool shakeFlag) { shakeFlag_ = shakeFlag; }

private:
	void CameraShake();

public:
	/// <summary>
	/// ビュープロジェクションを取得
	/// </summary>
	/// <returns>ビュープロジェクション</returns>
	const ViewProjection& GetViewProjection() { return viewProjection_; }

	const WorldTransform& GetWorldTransform() { return worldTransform_; }

private:
	Input* input_ = nullptr;

	// ワールド変換データ
	WorldTransform worldTransform_;
	//ビュープロジェクション
	ViewProjection viewProjection_;

	//追従対象
	const WorldTransform* target_ = nullptr;

	static bool shakeFlag_; 
};
