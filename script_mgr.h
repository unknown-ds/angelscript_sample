#ifndef	_SCRIPT_MGR_H_
#define	_SCRIPT_MGR_H_

#include	"./sdk/angelscript/include/angelscript.h"

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint;
typedef unsigned long uint32;
typedef unsigned long long uint64;
typedef char int8;
typedef short int16;
typedef long int32;
typedef long long int64;


//==========================================================================
class CScriptBuilder;

//--------------------------------------------------------------------------
// スクリプトマネージャクラス
class CScriptMgr
{
	CScriptMgr(const CScriptMgr&);
	const CScriptMgr& operator =(const CScriptMgr&);

public:
	// シングルトン取得
	static CScriptMgr*	GetInstance() {
		static CScriptMgr _sObject;
		return &_sObject;
	}

public:
	CScriptMgr();
	virtual ~CScriptMgr();

	// 初期化処理
	void		Init();
	// 終了処理
	void		Final();

	// コンパイル時のインクルードディレクティブコールバック
	static int IncludeCallBack(
					const char*		include,
					const char*		from,
					CScriptBuilder*	builder,
					void*			userParam
				);

	// コンパイル時のメッセージコールバック関数
	static void MessageCallback(
					const asSMessageInfo *pMsg,		// 
					void *pParam					// 
					);

	// スクリプトのコンパイル
	int		CompileScript(
				const char* pFilePath,			// スクリプトファイルパス
				const char* pModuleName,		// モジュール名
				const char* pSectionName		// セクション名
			);

	// コンパイル済スクリプトファイルの読み込み
	int		LoadScript(
				const char* _pBinScriptFn,	// コンパイル済スクリプトファイルパス
				const char* pModuleName		// モジュール名
			);


	// 実行
	int ExecuteMain(
				const char* pModuleName		// モジュール名
			);

	// スクリプトエンジンの取得
	asIScriptEngine*	GetEngine() { return(mpEngine); }
	// スクリプトビルダーの取得
	CScriptBuilder*		GetBuilder() { return(mpScriptBuilder); }

protected:
	asIScriptEngine*	mpEngine;			// スクリプトエンジン
	CScriptBuilder*		mpScriptBuilder;	// スクリプトビルダー


protected:
	//-----------------------------------------
	// スクリプトバイトコードストリーム
	class CBytecodeStream : public asIBinaryStream
	{
	public:
		CBytecodeStream(FILE *fp) : f(fp) {}

		// 書き出し
		int Write(const void *pPtr, asUINT _Size)
		{
			if (_Size == 0) return(0);
			fwrite(pPtr, _Size, 1, f);

			return(_Size);
		}

		// 読み込み
		int Read(void *pPtr, asUINT _Size)
		{
			if (_Size == 0) return(0);
			fread(pPtr, _Size, 1, f);

			return(_Size);
		}

	protected:
		FILE *f;
	};

};


#endif		// _SCRIPT_MGR_H_
