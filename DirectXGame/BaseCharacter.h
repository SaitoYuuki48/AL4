#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

class BaseCharacter {

public:
    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="models_">モデルデータ配列</param>
	virtual void Initialize(const std::vector<Model*>& models);

    /// <summary>
    /// 更新
    /// </summary>
	virtual void Update();

    /// <summary>
    /// 描画 
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション (参照渡し) </param>
	virtual void Draw(const ViewProjection& viewProjection);
    
public:
    /// <summary>
    ///  ワールド変換データを取得
    /// </summary>
	const WorldTransform& GetWorldTransform() { return worldTransform_; } 

protected:
	//モデルデータ配列
	std::vector<Model*> models_;

	WorldTransform worldTransform_;
};
