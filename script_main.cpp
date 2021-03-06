// as_compiler.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "script_mgr.h"

using namespace std;

//==========================================================================
// メイン
int main(int argc, char *argv[])
{

	string _ScriptFn = "./test00.as";


	// 初期化処理
	CScriptMgr::GetInstance()->Init();

	// ファイル名取り出し
	size_t path_i = _ScriptFn.find_last_of("/") + 1;
	size_t ext_i = _ScriptFn.find_last_of(".");
	string _SecName = _ScriptFn.substr(path_i, _ScriptFn.size() - path_i);

	// スクリプトのコンパイル
	int _Ret = CScriptMgr::GetInstance()->CompileScript(_ScriptFn.c_str(), "TestProj", _SecName.c_str());
	if ( _Ret < 0) {
		printf("Failed to Compile script.\n");
		assert(1);
	}
	else {
		printf("success to Compile script.\n");
	}

	// 実行
	CScriptMgr::GetInstance()->ExecuteMain("TestProj");

	// 終了処理
	CScriptMgr::GetInstance()->Final();

	getchar();

    return 0;
}

