#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Input.h"

class FixedCamera {
public:
	/// <summary
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary
	/// 更新
	/// </summary>
	void Update();

public:
	/// <summary>
	/// ビュープロジェクションを取得
	/// </summary>
	/// <returns>ビュープロジェクション</returns>
	const ViewProjection& GetViewProjection() { return viewProjection_; }

	const WorldTransform& GetWorldTransform() { return worldTransform_; }

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// ビュープロジェクション
	ViewProjection viewProjection_;
};
