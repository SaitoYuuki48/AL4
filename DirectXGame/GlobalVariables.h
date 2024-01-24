#pragma once
#include <iostream>
#include<variant>
#include <string>
#include <map>

#include "Vector3.h"
#include <json.hpp>

/// <summary>
/// グローバル
/// </summary>
class GlobalVariables final{
public:
	static GlobalVariables* GetInstance();

private:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	GlobalVariables() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GlobalVariables() = default;

	/// <summary>
	/// コピーコンストラクタ
	/// </summary>
	GlobalVariables(const GlobalVariables& obj) = delete;

	/// <summary>
	///コピー代入演算子
	/// </summary>
	GlobalVariables& operator=(const GlobalVariables& obj) = delete;

public:
   /// <summary>
   /// 毎フレーム処理
   /// </summary>
	void Update();

public:
    /// <summary>
    /// グループの作成
    /// </summary>
	/// <param name="groupName">グループ名</param>
	void CreateGroup(const std::string& groupName);

	// 値のセット(int)
	void SetValue(const std::string& groupName, const std::string& key, int32_t value);
	// 値のセット(float)
	void SetValue(const std::string& groupName, const std::string& key, float value);
	// 値のセット(Vector3)
	void SetValue(const std::string& groupName, const std::string& key, const Vector3& value);

	 /// <summary>
	/// ファイルに書き出し
	/// </summary>
	/// <param name="groupName">グループ</param>
	void SaveFile(const std::string& groupName);

private:

	using json=nlohmann::json;
  
	//グローバル変数の保存先ファイルパス
	const std::string kDirectoryPath = "Resources/GlobalVariables/";

private:
	//項目
	struct  Item {
		//項目の値
		std::variant<int32_t, float, Vector3> value;
	};

	//グループ
	struct  Group {
		std::map<std::string, Item> items;
	};

	//全データ
	std::map<std::string, Group> datas_;
};
