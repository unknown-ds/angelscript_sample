# 要約
静的型付けスクリプト言語（グルー言語）のAngelScriptのサンプルです。  
公式サイト(https://www.angelcode.com/angelscript/)

## Luaと比較した特徴
||Lua|AngelScript|
|---|---|---|
|変数型|動的|静的|
|文法|Cライクな別物？|C++ライク|
|バインダ|toLua++など|組み込み済|
|実行速度|速い|Luaよりは少し遅い（未確認）|
|ドキュメント|豊富|少ない|



## 環境
Windows10 64bit  
Visual Studio Express2017 for Windows Desktop  
AngelScript ver.2.33.0  

## 使用アドオン
- scriptarray
- scriptbuilder
- scriptstdstring
- scriptmath 


# 実装サンプル
## 準備
### スクリプトエンジンの生成
    // スクリプトエンジンを生成
	asIScriptEngine* mpEngine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	if (mpEngine == 0) {
		printf("Failed to create script engine.\n");
		return;
	}
    
### アドオンの登録
    // arrayアドオンの登録
    RegisterScriptArray(mpEngine, true);
    // stringアドオンの登録 
    RegisterStdString(mpEngine);
	  RegisterStdStringUtils(mpEngine);
    // mathアドオンの登録
	  RegisterScriptMath(mpEngine);
    

### 自前実装のクラス登録
`省略`
### グローバル変数の登録
	// グローバル変数の登録
	result = mpEngine->RegisterGlobalProperty("int gVersion", &gVersion); assert(result >= 0);

### 自前実装の関数の登録
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


## 実行
### 準備
	int result;
	asIScriptModule *pModule = mpEngine->GetModule(pModuleName);
	
	// スクリプトコンテキストの作成、（スクリプトの実行単位で必要）
	asIScriptContext *pCtx = mpEngine->CreateContext();
	if (!pCtx) {
		printf("Failed to create the context.\n");
		mpEngine->Release();
		return 0;
	}

### スクリプト側実装の関数をC++側で実行する１
    // 関数を探す
		asIScriptFunction *pFunc0 = pModule->GetFunctionByDecl("void testFunction1()");
		// 準備
		pCtx->Prepare(pFunc0);
		// 実行
		result = pCtx->Execute();

### スクリプト側実装の関数をC++側で実行する２（エラー処理付）
    // 関数を探す
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

### スクリプト側実装のクラスのオブジェクトを生成
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
