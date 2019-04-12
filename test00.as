

//==========================================================================
// テストクラス
class CTest{
	// コンストラクタ
	CTest(){
		a = 0;
		b = 0.f;
	}
	CTest(int _a, float _b=0.f){
		a = _a;
		b = _b;
	}
	// デストラクタ
	~CTest(){
	}
	
	// メソッド(public)
	int GetA() { return(a); }
	float GetB() { return(b); }
	
	// メソッド(protected)
	protected void Pow(float _Rate){
		b = pow(b, _Rate);
	}
	
	// メンバ
	int a;
	float b;
};


//==========================================================================
int gGlobalVal;

// スクリプト型のテスト関数１
void testFunction1()
{
	gGlobalVal = 0;
	
	array<float> arr = {1.0f, 2.0f, 3.0f};
	
	// 配列の加算
	float sum = 0.0f;
	for(uint i=0; i<arr.length();i++){
		sum += arr[i];
	}
	
	float ret1 = testBindFunc1(arr[0], sum);
	
	printf("ret1 = %f\n", ret1);
}

// スクリプト型のテスト関数２
float testFunction2(float lhs, float rhs)
{
	// クラスインスタンス
	CTest _cTest1 = CTest();
	CTest _cTest2 = CTest(10, float(20));
	//printf("CTest1 : %f\n", _cTest1.GetB());
	//printf("CTest2 : %f\n", _cTest2.GetB());
	
	// CVector3型
	CVector3 _Vec1; _Vec1.SetVector(1.0f, 1.0f, 0.0f);
	CVector3 _Vec2; _Vec2.SetVector(1.0f, 1.0f, 0.0f);
	//printf("DotProduct = %f\n", _Vec1.DotProduct(_Vec2));
	
	
	array<int> arr = {2, 3, 4};
	int ret2 = testBindFunc2(arr, 2);
	printf("ret2 = %d\n", ret2);
	

	return( lhs+rhs );
}

// スクリプト側のテスト関数３
string testFunction3(int param)
{
	string ret3;
	switch(param){
	case 0:{	ret3 = testBindFunc3("0"); }break;
	case 1:{	ret3 = testBindFunc3("1"); }break;
	case 2:{	ret3 = testBindFunc3("2"); }break;
	case 3:{	ret3 = testBindFunc3("3"); }break;
	}
	
	printf("ret3 = %s\n", ret3);
	
	
	return(ret3);
}
