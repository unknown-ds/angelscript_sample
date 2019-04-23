#include	"stdafx.h"
#include	"./script_mgr.h"
#include	"./script_func.h"

#include	"sdk\angelscript\include\angelscript.h"
#include	"sdk\add_on\scriptarray\scriptarray.h"
#include	"sdk\add_on\scriptstdstring\scriptstdstring.h"
#include	"sdk\add_on\scriptbuilder\scriptbuilder.h"
#include	"sdk\add_on\scriptmath\scriptmath.h"

using namespace std;

//void RegisterString(asIScriptEngine * engine);

// 登録するグローバル変数
int gVersion = 0;

//==========================================================================
//--------------------------------------------------------------------------
// 
namespace
{
	// メモリ確保カウンタ
	int t_allocCount = 0;
	// メモリ確保
	void* t_alloc(const size_t _Size)
	{
		++t_allocCount;
		return std::malloc(_Size);
	}
	// メモリ解放
	void t_free(void* _pPtr)
	{
		std::free(_pPtr);
		--t_allocCount;
	}
}

//==========================================================================
//--------------------------------------------------------------------------
// コンストラクタ
CScriptMgr::CScriptMgr()
{
	mpEngine = NULL;
	mpScriptBuilder = NULL;
}


//--------------------------------------------------------------------------
// デストラクタ
CScriptMgr::~CScriptMgr()
{
}


//--------------------------------------------------------------------------
// 初期化処理
void CScriptMgr::Init()
{
	if(mpEngine)return;	// 作成済
	
	int result = 0;

	// バージョン表示
	printf("%s\n", asGetLibraryVersion());

	// メモリアロケータの登録
	result = asSetGlobalMemoryFunctions(t_alloc, t_free);
	if (result < 0) {
		printf("Failed to create script engine.\n");
		return;
	}

	// スクリプトエンジンを生成
	mpEngine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	if (mpEngine == 0) {
		printf("Failed to create script engine.\n");
		return;
	}
	
	// スクリプトコンパイラメッセージコールバック関数の設定
	result = mpEngine->SetMessageCallback(asFUNCTION(CScriptMgr::MessageCallback), 0, asCALL_CDECL);
	if (result == asINVALID_ARG || result == asNOT_SUPPORTED) {
		printf("Failed to Regist compiler message callback.\n");
		return;
	}
	

	// エンジンプロパティ設定
	mpEngine->SetEngineProperty(asEP_INIT_GLOBAL_VARS_AFTER_BUILD, false);
	//mpEngine->SetEngineProperty(asEP_BUILD_WITHOUT_LINE_CUES, true);
	mpEngine->SetEngineProperty(asEP_SCRIPT_SCANNER, 1);
	mpEngine->SetEngineProperty(asEP_STRING_ENCODING, 0);


	// arrayアドオンの登録
	RegisterScriptArray(mpEngine, true);
	// stringアドオンの登録
	RegisterStdString(mpEngine);
	RegisterStdStringUtils(mpEngine);
	// 簡易版文字列アドオンの登録
//	RegisterString(mpEngine);
	// mathアドオンの登録
	RegisterScriptMath(mpEngine);

	// クラス登録
	RegistClass_CVector3(mpEngine);

	// グローバル変数の登録
	result = mpEngine->RegisterGlobalProperty("int gVersion", &gVersion); assert(result >= 0);
	
	// C++で実装した関数をスクリプトにバインド
	if (!strstr(asGetLibraryOptions(), "AS_MAX_PORTABILITY")) {
		// Native
		result = mpEngine->RegisterGlobalFunction("void printf(const string &in)", asFUNCTION(asPrintf0), asCALL_CDECL); assert(result >= 0);
		result = mpEngine->RegisterGlobalFunction("void printf(const string &in, ?&in)", asFUNCTION(asPrintf1), asCALL_CDECL); assert(result >= 0);
		result = mpEngine->RegisterGlobalFunction("float testBindFunc1(float, float)", asFUNCTION(testBindFunc1), asCALL_CDECL); assert(result >= 0);
		result = mpEngine->RegisterGlobalFunction("int testBindFunc2(array<int> &in, int)", asFUNCTION(testBindFunc2), asCALL_CDECL); assert(result >= 0);
		result = mpEngine->RegisterGlobalFunction("string testBindFunc3(string)", asFUNCTION(testBindFunc3), asCALL_CDECL); assert(result >= 0);
	}else{
		// Generic
		result = mpEngine->RegisterGlobalFunction("void printf(const string &in)", asFUNCTION(asPrintf0Generic), asCALL_GENERIC); assert(result >= 0);
		result = mpEngine->RegisterGlobalFunction("void printf(const string &in, ?&in)", asFUNCTION(asPrintf1Generic), asCALL_GENERIC); assert(result >= 0);
		result = mpEngine->RegisterGlobalFunction("float testBindFunc1Generic(float, float)", asFUNCTION(testBindFunc1Generic), asCALL_GENERIC); assert(result >= 0);
		result = mpEngine->RegisterGlobalFunction("int testBindFunc2Generic(array<int> &in, int)", asFUNCTION(testBindFunc2Generic), asCALL_GENERIC); assert(result >= 0);
		result = mpEngine->RegisterGlobalFunction("string testBindFunc3Generic(string)", asFUNCTION(testBindFunc3Generic), asCALL_GENERIC); assert(result >= 0);
	}

	// スクリプトビルダー生成
	mpScriptBuilder = new CScriptBuilder();
	mpScriptBuilder->SetIncludeCallback(CScriptMgr::IncludeCallBack, NULL);
}


//--------------------------------------------------------------------------
// 終了処理
void CScriptMgr::Final()
{
	// スクリプトビルダー始末
	if (mpScriptBuilder) {
		delete mpScriptBuilder;
		mpScriptBuilder = NULL;
	}

	// スクリプトエンジン始末
	if(mpEngine){
		mpEngine->Release();
	}
	mpEngine = NULL;

}


//--------------------------------------------------------------------------
// コンパイル時のインクルードディレクティブコールバック
int CScriptMgr::IncludeCallBack(
	const char*		include,
	const char*		from,
	CScriptBuilder*	builder,
	void*			userParam
	)
{
	return(0);
}


//--------------------------------------------------------------------------
// コンパイル時のメッセージコールバック関数
void CScriptMgr::MessageCallback(
			const asSMessageInfo*	pMsg,		// メッセージ
			void*					pParam		// パラメータ
			)
{
	if (pMsg == NULL) {
		printf("invalid args.");
		return;
	}
	const char *type = "ERROR";
	if (pMsg->type == asMSGTYPE_WARNING) {
		type = "WARNINIG";
	}
	else if (pMsg->type == asMSGTYPE_INFORMATION) {
		type = "INFORMATION";
	}
	printf("%s (%d, %d) : %s : %s\n", pMsg->section, pMsg->row, pMsg->col, type, pMsg->message);
	assert(pMsg->type != asMSGTYPE_ERROR);
}


//--------------------------------------------------------------------------
// スクリプトのコンパイル
int CScriptMgr::CompileScript(
			const char* pFilePath,			// スクリプトファイルパス
			const char* pModuleName,		// モジュール名
			const char* pSectionName		// セクション名
		)
{
	int result;

	if (!mpEngine || !pFilePath || !pModuleName || !pSectionName) {
		printf("invalid args.");
		return -1;
	}

	int len(0);
	string script;
	{ // スクリプトファイルを開く
		FILE *pFile = NULL;
		errno_t err = fopen_s(&pFile, pFilePath, "rb");
		if (err != 0 || !pFile) {
			printf("Filed to open script file [%d : %s]\n", err, pFilePath);
			return -1;
		}

		// ファイルサイズを取得
		fseek(pFile, 0, SEEK_END);
		int len = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);

		// スクリプトファイルを読み込む
		script.resize(len);
		size_t c = fread(&script[0], len, 1, pFile);
		fclose(pFile);
		if (c == 0) {
			printf("Filed to read script file [%d : %s]\n", err, pFilePath);
			return -1;
		}
	}

	
	// スクリプトビルダー設定
	// defineディレクティブを設定
	//mpScriptBuilder->DefineWord("DEBUG");

	// スクリプトモジュール生成
	result = mpScriptBuilder->StartNewModule(mpEngine, pModuleName); assert(result >= 0);
	// スクリプトセクションの追加
	result = mpScriptBuilder->AddSectionFromMemory(pSectionName, &script[0], len); assert(result >= 0);

	// コンパイル。エラーがあればasIScriptEngine::SetMessageCallbackで設定された関数が呼ばれる
	result = mpScriptBuilder->BuildModule(); assert(result >= 0);
	
	// スクリプトモジュールを取得
	asIScriptModule *pModule = mpScriptBuilder->GetModule();

#if 0
	// バイトコード書き出し
	FILE *fp;
	string _OutputFn(pFilePath);
	_OutputFn += "b";

	errno_t error;
	if ((error = fopen_s(&fp, _OutputFn.c_str(), "wb")) != 0) {
		// エラー
		printf("FileOpenError! [%s]\n", _OutputFn.c_str()); assert(0);
		return(-1);
	}else {
		// ファイル書き出し
		CBytecodeStream* _pOut = new CBytecodeStream(fp);
		pModule->SaveByteCode(_pOut);

		fclose(fp);
	}
#endif

	return(0);
}


//--------------------------------------------------------------------------
// コンパイル済スクリプトファイルの読み込み
int CScriptMgr::LoadScript(
					const char* _pBinScriptFn,	// コンパイル済スクリプトファイルパス
					const char* pModuleName		// モジュール名
					)
{
	asIScriptModule *pModule = mpEngine->GetModule(pModuleName, asGM_CREATE_IF_NOT_EXISTS);

	FILE *fp;
	if ((fopen_s(&fp, _pBinScriptFn, "rb")) != 0) {
		// エラー
		printf("FileOpenError! [%s]\n", _pBinScriptFn); assert(0);
	}else{
		// ファイル読み込み
		CBytecodeStream* _pIn = new CBytecodeStream(fp);
		pModule->LoadByteCode(_pIn);

		fclose(fp);
	}

	return(0);
}


//--------------------------------------------------------------------------
// 実行処理
int CScriptMgr::ExecuteMain(
					const char* pModuleName		// モジュール名
					)
{
	int result;
	asIScriptModule *pModule = mpEngine->GetModule(pModuleName);
	
	
	// スクリプトコンテキストの作成、（スクリプトの実行単位で必要）
	asIScriptContext *pCtx = mpEngine->CreateContext();
	if (!pCtx) {
		printf("Failed to create the context.\n");
		mpEngine->Release();
		return 0;
	}
	// グローバル変数をリセット
	pModule->ResetGlobalVars(pCtx);

	// 実行テスト１
	{
		// オブジェクトタイプを取得
		asITypeInfo *pType = pModule->GetTypeInfoByDecl("CTest");
		// ファクトリ関数を取得
		asIScriptFunction *pFactory = pType->GetFactoryByDecl("CTest @CTest(int, float)");
		// スクリプト関数設定準備（連続実行時は2回目以降省略可能）
		pCtx->Prepare(pFactory);
		// 引き数を設定
		pCtx->SetArgDWord(0, 10);
		pCtx->SetArgFloat(1, 20.0f);
		// 実行
		pCtx->Execute();

		// 作成したオブジェクトを取得
		asIScriptObject* obj = *(asIScriptObject**)pCtx->GetAddressOfReturnValue();

		// オブジェクトを保存しようとする場合、参照を増加させる（再利用時に必須）
		obj->AddRef();

		// クラスメソッドをあらわす関数オブジェクトを取得
		asIScriptFunction *func = pType->GetMethodByDecl("int GetA()");
		// メソッド呼び出しのためのコンテキストを作成
		pCtx->Prepare(func);
		// オブジェクトポインタを設定
		pCtx->SetObject(obj);
		// 呼び出し
		pCtx->Execute();
		printf("C++ CTest::GetA:%d\n", pCtx->GetReturnDWord());

		// オブジェクト始末
		obj->Release();
	}

	{
		// 関数を探す
		asIScriptFunction *pFunc0 = pModule->GetFunctionByDecl("void testFunction1()");
		// 準備
		pCtx->Prepare(pFunc0);
		// 実行
		result = pCtx->Execute();
	}
	
	// 実行テスト３
	{
		asIScriptFunction *pFunc1 = pModule->GetFunctionByDecl("string testFunction3(int)");
		pCtx->Prepare(pFunc1);
		pCtx->SetArgDWord(0, 3);
		result = pCtx->Execute();
		printf("C++ testFunction3:%s\n", ((string*)pCtx->GetReturnAddress())->c_str() );
	}


	// 実行テスト２
	asIScriptFunction *pFunc = pModule->GetFunctionByDecl("float testFunction2(float, float)");
	if (pFunc == NULL) {
		printf("Function not found.\n");
		pCtx->Release();
		mpEngine->Release();
		return 0;
	}
	result = pCtx->Prepare(pFunc);
	if (result != 0) {
		printf("Failed to prepare the context.\n");
		pCtx->Release();
		mpEngine->Release();
		return 0;
	}

	// 引き数をセット
	result = pCtx->SetArgFloat(0, 5.0f);
	if (result != 0) {
		printf("Failed to set arments.[%d]\n", result);
		pCtx->Release();
		mpEngine->Release();
		return 0;
	}
	result = pCtx->SetArgFloat(1, 3.0f);
	if (result != 0) {
		printf("Failed to set arments.[%d]\n", result);
		pCtx->Release();
		mpEngine->Release();
		return 0;
	}


	// 実行
	result = pCtx->Execute();
	if (result != asEXECUTION_FINISHED) {
		// 通常終了しなかった場合
		if (result == asEXECUTION_ABORTED){
			// 割り込み強制終了
			printf("Script aborted.");
		}else
		if (result == asEXECUTION_EXCEPTION){
			// 例外発生
			printf("Script threw Exception.\n");
			asIScriptFunction *func = pCtx->GetExceptionFunction();
			printf("Detail [%s / %s] %s[%d] %s\n"
				, func->GetDeclaration()
				, func->GetModuleName()
				, func->GetScriptSectionName()
				, pCtx->GetExceptionLineNumber()
				, pCtx->GetExceptionString());
		}else
		if (result == asEXECUTION_SUSPENDED){
			// 中断終了
			int line = pCtx->GetExceptionLineNumber();
			printf("Script suspend at L %d.\n", line);
		}else{
			// 不明なエラー
			printf("The script ended for some unforeseen reason (%d).\n", result);
		}
	}else{
		// 通常終了
		float returnValue = pCtx->GetReturnFloat();	// 返り値取得
		printf("C++ testFunction2: %f\n", returnValue);
	}


	// スクリプトコンテキストの解放
	pCtx->Release();


	return(0);
}


/*
//==========================================================================
//==========================================================================
// 文字列ファクトリ(Native)
static string StringFactory(asUINT length, const char *s)
{
	return string(s, length);
}

// 文字列クラスコンストラクタ(Native)
static void ConstructString(string *thisPointer)
{
	new(thisPointer) string();
}

// 文字列クラスコピーコンストラクタ(Native)
static void CopyConstructString(const string &other, string *thisPointer)
{
	new(thisPointer) string(other);
}

// 文字列クラスデストラクタ(Native)
static void DestructString(string *thisPointer)
{
	thisPointer->~string();
}

//--------------------------------------------------------------------------
// 文字列ファクトリ(Generic)
static void StringFactoryGeneric(asIScriptGeneric *gen)
{
	asUINT length = gen->GetArgDWord(0);
	const char *s = (const char*)gen->GetArgAddress(1);

	new (gen->GetAddressOfReturnLocation()) string(StringFactory(length, s));
}

// 文字列クラスコンストラクタ(Generic)
static void ConstructStringGeneric(asIScriptGeneric * gen)
{
	new (gen->GetObject()) string();
}

// 文字列クラスコピーコンストラクタ(Generic)
static void CopyConstructStringGeneric(asIScriptGeneric * gen)
{
	string * a = static_cast<string *>(gen->GetArgObject(0));
	new (gen->GetObject()) string(*a);
}

// 文字列クラスデストラクタ(Generic)
static void DestructStringGeneric(asIScriptGeneric * gen)
{
	string * ptr = static_cast<string *>(gen->GetObject());
	ptr->~string();
}

// 文字列クラス代入(Generic)
static void AssignStringGeneric(asIScriptGeneric *gen)
{
	string * a = static_cast<string *>(gen->GetArgObject(0));
	string * self = static_cast<string *>(gen->GetObject());
	*self = *a;
	gen->SetReturnAddress(self);
}

//--------------------------------------------------------------------------
// 文字列型の登録
void RegisterString(asIScriptEngine* engine)
{
	int r = 0;

	if (strstr(asGetLibraryOptions(), "AS_MAX_PORTABILITY"))
	{
		//-----------
		// Generic
		//-----------
		// 文字列タイプの登録
		r = engine->RegisterObjectType("string", sizeof(string), asOBJ_VALUE | asOBJ_APP_CLASS_CDAK); assert(r >= 0);
		// 文字列ファクトリの登録
		r = engine->RegisterStringFactory("string", asFUNCTION(StringFactoryGeneric), asCALL_GENERIC); assert(r >= 0);
		// 文字列オペレータの登録
		r = engine->RegisterObjectBehaviour("string", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(ConstructStringGeneric), asCALL_GENERIC); assert(r >= 0);
		r = engine->RegisterObjectBehaviour("string", asBEHAVE_CONSTRUCT, "void f(const string &in)", asFUNCTION(CopyConstructStringGeneric), asCALL_GENERIC); assert(r >= 0);
		r = engine->RegisterObjectBehaviour("string", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(DestructStringGeneric), asCALL_GENERIC); assert(r >= 0);
		r = engine->RegisterObjectMethod("string", "string &opAssign(const string &in)", asFUNCTION(AssignStringGeneric), asCALL_GENERIC); assert(r >= 0);

	}
	else
	{
		//-----------
		// Native
		//-----------
		// 文字列タイプの登録
		r = engine->RegisterObjectType("string", sizeof(string), asOBJ_VALUE | asOBJ_APP_CLASS_CDAK); assert(r >= 0);
		// 文字列ファクトリの登録
		r = engine->RegisterStringFactory("string", asFUNCTION(StringFactory), asCALL_CDECL); assert(r >= 0);
		// 文字列オペレータの登録
		r = engine->RegisterObjectBehaviour("string", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(ConstructString), asCALL_CDECL_OBJLAST); assert(r >= 0);
		r = engine->RegisterObjectBehaviour("string", asBEHAVE_CONSTRUCT, "void f(const string &in)", asFUNCTION(CopyConstructString), asCALL_CDECL_OBJLAST); assert(r >= 0);
		r = engine->RegisterObjectBehaviour("string", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(DestructString), asCALL_CDECL_OBJLAST); assert(r >= 0);
		r = engine->RegisterObjectMethod("string", "string &opAssign(const string &in)", asMETHODPR(string, operator =, (const string&), string&), asCALL_THISCALL); assert(r >= 0);

	}

}
*/

