#include	"stdafx.h"
#include "./script_func.h"
#include "./script_mgr.h"

using namespace std;


//==========================================================================
//--------------------------------------------------------------------------
// printf関数
void asPrintf0(string &fmt)
{
	printf("%s", fmt.c_str());
}

void asPrintf0Generic(
	asIScriptGeneric *pGen
	)
{
	string* _pInput = static_cast<string*>(pGen->GetArgObject(0));

	asPrintf0(*_pInput);
}

//--------------------------------------------------------------------------
// printf関数
void asPrintf1(
	string& fmt,
	void*	_pRef,
	int		_TypeId
	)
{

	switch (_TypeId) {
	case asETypeIdFlags::asTYPEID_BOOL: {	printf(fmt.c_str(), *reinterpret_cast<bool*>(_pRef));	}break;
	case asETypeIdFlags::asTYPEID_INT8: {	printf(fmt.c_str(), *reinterpret_cast<int8*>(_pRef));	}break;
	case asETypeIdFlags::asTYPEID_INT16: {	printf(fmt.c_str(), *reinterpret_cast<int16*>(_pRef));	}break;
	case asETypeIdFlags::asTYPEID_INT32: {	printf(fmt.c_str(), *reinterpret_cast<int32*>(_pRef));	}break;
	case asETypeIdFlags::asTYPEID_INT64: {	printf(fmt.c_str(), *reinterpret_cast<int64*>(_pRef));	}break;
	case asETypeIdFlags::asTYPEID_UINT8: {	printf(fmt.c_str(), *reinterpret_cast<uint8*>(_pRef));	}break;
	case asETypeIdFlags::asTYPEID_UINT16: {	printf(fmt.c_str(), *reinterpret_cast<uint16*>(_pRef));	}break;
	case asETypeIdFlags::asTYPEID_UINT32: {	printf(fmt.c_str(), *reinterpret_cast<uint32*>(_pRef));	}break;
	case asETypeIdFlags::asTYPEID_UINT64: {	printf(fmt.c_str(), *reinterpret_cast<uint64*>(_pRef));	}break;
	case asETypeIdFlags::asTYPEID_FLOAT: {	printf(fmt.c_str(), *reinterpret_cast<float*>(_pRef));	}break;
	case asETypeIdFlags::asTYPEID_DOUBLE: {	printf(fmt.c_str(), *reinterpret_cast<double*>(_pRef));	}break;
	// string型登録時の返り値 RegisterObjectType()
	case (asETypeIdFlags::asTYPEID_APPOBJECT + 0x0d): {
		printf(fmt.c_str(), (*reinterpret_cast<string*>(_pRef)).c_str());
	}break;
	default: { printf(fmt.c_str()); }break;
	}

}

void asPrintf1Generic(
	asIScriptGeneric *pGen
)
{
	string& _Fmt = *static_cast<string*>(pGen->GetArgObject(0));
	int _Prm1TypeId = pGen->GetArgTypeId(1);
	void* _pPrm1Ptr = pGen->GetArgAddress(1);
	
	asPrintf1(_Fmt, _pPrm1Ptr, _Prm1TypeId);
}

//---------------------------------------------------------------------------
// スクリプトにバインドするテスト関数１
float testBindFunc1(
	float _Lhs,			// 引数１
	float _Rhs			// 引数２
	)
{
	//printf("execute %s.\n",__FUNCTION__);
	return (_Lhs + _Rhs);
}


// Generic
void testBindFunc1Generic(
	asIScriptGeneric *pGen
	)
{
	//printf("execute %s.\n", __FUNCTION__);

	float _Lhs = *reinterpret_cast<float*>(pGen->GetAddressOfArg(0));
	float _Rhs = *reinterpret_cast<float*>(pGen->GetAddressOfArg(1));

	pGen->SetReturnFloat(testBindFunc1(_Lhs, _Rhs));
}

//-------------------------------------------------------------------------
// スクリプトにバインドするテスト関数２
int testBindFunc2(
	void*	_pIntArray,		// 配列ポインタ
	int		_Param			// int型引数
	)
{
	CScriptArray* _pArray = (CScriptArray*)_pIntArray;

	int _Sum = 0;
	// 配列を展開して加算
	for (int _Lp = 0; _Lp < (int)_pArray->GetSize(); _Lp++) {
		_Sum += *((int*)_pArray->At(_Lp));
	}
	_Sum *= _Param;

	return(_Sum);
}

// Generic
void testBindFunc2Generic(
	asIScriptGeneric *pGen
	)
{
	void* _pIntArray = pGen->GetArgObject(0);
	int _Param = *reinterpret_cast<int*>(pGen->GetAddressOfArg(1));

	pGen->SetReturnDWord(testBindFunc2(_pIntArray, _Param));
}


//--------------------------------------------------------------------------
constexpr int myhash(char const *s){ return( *s ? *s + 7*myhash(s + 1) : 0 ); }

// スクリプトにバインドするテスト関数３
string testBindFunc3(
	string _Input			// string型引数
	)
{
	
	switch(myhash(_Input.c_str())){		// defaultを使う場合ハッシュ値の衝突に注意
	case myhash("0"):{	return("Zero");	}break;
	case myhash("1"):{	return("One");	}break;
	case myhash("2"):{	return("Two");	}break;
	case myhash("3"):{	return("Three");	}break;
	default:{}
	}
	
	return("???");
}

// Generic
void testBindFunc3Generic(
	asIScriptGeneric *pGen
	)
{
	string* _pInput = static_cast<string*>(pGen->GetArgObject(0));

	pGen->SetReturnObject(&testBindFunc3(*_pInput));
}


//==========================================================================
//==========================================================================
// 内積計算
constexpr float mathVec3DotProduct(const CVector3& _Vec1, const CVector3& _Vec2 ){
	return(_Vec1.x * _Vec2.x + _Vec1.y * _Vec2.y + _Vec1.z * _Vec2.z);
}

//--------------------------------------------------------------------------
// コンストラクタ(Native)
static void Construct1CVector3(
	CVector3*			thisPointer
	)
{
	new(thisPointer) CVector3();
}

// コンストラクタ(Native)
static void Construct2CVector3(
	float				_xx,
	float				_yy,
	float				_zz,
	CVector3*			thisPointer
	)
{
	new(thisPointer) CVector3(_xx, _yy, _zz);
}

// コピーコンストラクタ(Native)
static void CopyConstructCVector3(
	const CVector3&		other,
	CVector3*			thisPointer
	)
{
	new(thisPointer) CVector3(other);
}

// デストラクタ(Native)
static void DestructCVector3(
	CVector3*	thisPointer
	)
{
	thisPointer->~CVector3();
}

// 内積(Native)
float CVector3::DotProduct(
	const CVector3& _Vec		// 対象ベクトル
	) const
{
	return( mathVec3DotProduct(*this, _Vec) );
}


//--------------------------------------------------------------------------
// コンストラクタ(Generic)
static void Construct1CVector3Generic(
	asIScriptGeneric *_pGen
	)
{
	new (_pGen->GetObject()) CVector3();
}

// コンストラクタ(Generic)
static void Construct2CVector3Generic(
	asIScriptGeneric *_pGen
	)
{
	float	_xx = *reinterpret_cast<float*>(_pGen->GetAddressOfArg(0));
	float	_yy = *reinterpret_cast<float*>(_pGen->GetAddressOfArg(1));
	float	_zz = *reinterpret_cast<float*>(_pGen->GetAddressOfArg(2));
	
	new (_pGen->GetObject()) CVector3(_xx, _yy, _zz);
}

// コピーコンストラクタ(Generic)
static void CopyConstructCVector3Generic(
	asIScriptGeneric *_pGen
	)
{
	CVector3* ptr = static_cast<CVector3*>(_pGen->GetArgObject(0));
	new (_pGen->GetObject()) CVector3(*ptr);
}

// デストラクタ(Generic)
static void DestructCVector3Generic(
	asIScriptGeneric *_pGen
	)
{
	CVector3* ptr = static_cast<CVector3*>(_pGen->GetObject());
	ptr->~CVector3();
}

// 初期化(Generic)
static void SetVectorCVector3Generic(
	asIScriptGeneric *_pGen
	)
{
	float	_xx = *reinterpret_cast<float*>(_pGen->GetAddressOfArg(0));
	float	_yy = *reinterpret_cast<float*>(_pGen->GetAddressOfArg(1));
	float	_zz = *reinterpret_cast<float*>(_pGen->GetAddressOfArg(2));
	CVector3* _Self = static_cast<CVector3*>(_pGen->GetObject());
	
	_Self->SetVector(_xx, _yy, _zz);
}

// 代入演算子(=)(Generic)
static void AssignCVector3Generic(
	asIScriptGeneric *_pGen
	)
{
	CVector3* _Other = static_cast<CVector3*>(_pGen->GetArgObject(0));
	CVector3* _Self = static_cast<CVector3*>(_pGen->GetObject());
	*_Self = *_Other;
	_pGen->SetReturnAddress(_Self);
}

// 代入演算子(+=)(Generic)
static void AddAssignCVector3Generic(
	asIScriptGeneric *_pGen
	)
{
	CVector3* _Other = static_cast<CVector3*>(_pGen->GetArgObject(0));
	CVector3* _Self = static_cast<CVector3*>(_pGen->GetObject());
	*_Self += *_Other;
	_pGen->SetReturnAddress(_Self);
}

// 代入演算子(-=)(Generic)
static void SubAssignCVector3Generic(
	asIScriptGeneric *_pGen
	)
{
	CVector3* _Other = static_cast<CVector3*>(_pGen->GetArgObject(0));
	CVector3* _Self = static_cast<CVector3*>(_pGen->GetObject());
	*_Self -= *_Other;
	_pGen->SetReturnAddress(_Self);
}

// 代入演算子(*=)(Generic)
static void MulAssignCVector3Generic(
	asIScriptGeneric *_pGen
	)
{
	CVector3* _Other = static_cast<CVector3*>(_pGen->GetArgObject(0));
	CVector3* _Self = static_cast<CVector3*>(_pGen->GetObject());
	*_Self *= *_Other;
	_pGen->SetReturnAddress(_Self);
}

// 代入演算子(/=)(Generic)
static void DivAssignCVector3Generic(
	asIScriptGeneric *_pGen
	)
{
	CVector3* _Other = static_cast<CVector3*>(_pGen->GetArgObject(0));
	CVector3* _Self = static_cast<CVector3*>(_pGen->GetObject());
	*_Self /= *_Other;
	_pGen->SetReturnAddress(_Self);
}

// 2項演算子(+)(Generic)
static void AddCVector3Generic(
	asIScriptGeneric *_pGen
	)
{
	CVector3* _Other = static_cast<CVector3*>(_pGen->GetArgObject(0));
	CVector3* _Self = static_cast<CVector3*>(_pGen->GetObject());
	
	CVector3 _Ret = CVector3(_Self->x+_Other->x, _Self->y+_Other->y, _Self->z+_Other->z);
	
	_pGen->SetReturnObject(&_Ret);
}

// 2項演算子(-)(Generic)
static void SubCVector3Generic(
	asIScriptGeneric *_pGen
	)
{
	CVector3* _Other = static_cast<CVector3*>(_pGen->GetArgObject(0));
	CVector3* _Self = static_cast<CVector3*>(_pGen->GetObject());
	
	CVector3 _Ret = CVector3(_Self->x-_Other->x, _Self->y-_Other->y, _Self->z-_Other->z);
	
	_pGen->SetReturnObject(&_Ret);
}

// 2項演算子(*)(Generic)
static void MulCVector3Generic(
	asIScriptGeneric *_pGen
	)
{
	float _Rate = *reinterpret_cast<float*>(_pGen->GetAddressOfArg(0));
	CVector3* _Self = static_cast<CVector3*>(_pGen->GetObject());
	
	CVector3 _Ret = CVector3(_Self->x*_Rate, _Self->y*_Rate, _Self->z*_Rate);
	
	_pGen->SetReturnObject(&_Ret);
}

// 2項演算子(/)(Generic)
static void DivCVector3Generic(
	asIScriptGeneric *_pGen
	)
{
	float _Rate = *reinterpret_cast<float*>(_pGen->GetAddressOfArg(0));
	CVector3* _Self = static_cast<CVector3*>(_pGen->GetObject());
	
	CVector3 _Ret = CVector3(_Self->x/_Rate, _Self->y/_Rate, _Self->z/_Rate);
	
	_pGen->SetReturnObject(&_Ret);
}

// 比較演算子(==)(Generic)
static void EqualsCVector3Generic(
	asIScriptGeneric *_pGen
	)
{
	CVector3* _Other = static_cast<CVector3*>(_pGen->GetArgObject(0));
	CVector3* _Self = static_cast<CVector3*>(_pGen->GetObject());
	
	bool _Ret = ( (_Self->x==_Other->x)&&(_Self->y==_Other->y)&&(_Self->z==_Other->z) );
	
	_pGen->SetReturnByte(_Ret);
}

// 正規化(Generic)
static void NormalizeCVector3Generic(
	asIScriptGeneric *_pGen
	)
{
	CVector3* _Self = static_cast<CVector3*>(_pGen->GetObject());

	_Self->Normalize();

	_pGen->SetReturnAddress(_Self);
}

// 長さ取得(Generic)
static void LengthCVector3Generic(
	asIScriptGeneric *_pGen
	)
{
	CVector3* _Self = static_cast<CVector3*>(_pGen->GetObject());
	
	float _Len = _Self->Length();

	_pGen->SetReturnFloat(_Len);
}


// 内積(Generic)
static void DotProductCVector3Generic(
	asIScriptGeneric *_pGen
	)
{
	CVector3* _Self = static_cast<CVector3*>(_pGen->GetObject());
	CVector3* _Vec = static_cast<CVector3*>(_pGen->GetArgObject(0));
	
	_pGen->SetReturnFloat( mathVec3DotProduct(*_Self, *_Vec) );
}

//==========================================================================
//--------------------------------------------------------------------------
// Vector型の登録
void RegistClass_CVector3(asIScriptEngine* _pEngine)
{
	int _Result = 0;


	// オブジェクトタイプの登録
	_Result = _pEngine->RegisterObjectType("CVector3", sizeof(CVector3), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CDAK | asOBJ_APP_CLASS_ALLFLOATS); assert(_Result >= 0);


	if (strstr(asGetLibraryOptions(), "AS_MAX_PORTABILITY"))
	{
		//-----------
		// Generic
		//-----------
		// オペレータの登録
		_Result = _pEngine->RegisterObjectBehaviour("CVector3", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(Construct1CVector3Generic), asCALL_GENERIC); assert(_Result >= 0);
		_Result = _pEngine->RegisterObjectBehaviour("CVector3", asBEHAVE_CONSTRUCT, "void f(const float, const float, const float)", asFUNCTION(Construct2CVector3Generic), asCALL_GENERIC); assert(_Result >= 0);
		_Result = _pEngine->RegisterObjectBehaviour("CVector3", asBEHAVE_CONSTRUCT, "void f(const CVector3 &in)", asFUNCTION(CopyConstructCVector3Generic), asCALL_GENERIC); assert(_Result >= 0);
		_Result = _pEngine->RegisterObjectBehaviour("CVector3", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(DestructCVector3Generic), asCALL_GENERIC); assert(_Result >= 0);
		// メソッドの登録
		_Result = _pEngine->RegisterObjectMethod("CVector3", "CVector3& opAssign(const CVector3 &in)", asFUNCTION(AssignCVector3Generic), asCALL_GENERIC); assert(_Result >= 0);
		_Result = _pEngine->RegisterObjectMethod("CVector3", "CVector3& opAddAssign(const CVector3 &in)", asFUNCTION(AddAssignCVector3Generic), asCALL_GENERIC); assert(_Result >= 0);
		_Result = _pEngine->RegisterObjectMethod("CVector3", "CVector3& opSubAssign(const CVector3 &in)", asFUNCTION(SubAssignCVector3Generic), asCALL_GENERIC); assert(_Result >= 0);
		_Result = _pEngine->RegisterObjectMethod("CVector3", "CVector3& opMulAssign(const CVector3 &in)", asFUNCTION(MulAssignCVector3Generic), asCALL_GENERIC); assert(_Result >= 0);
		_Result = _pEngine->RegisterObjectMethod("CVector3", "CVector3& opDivAssign(const CVector3 &in)", asFUNCTION(DivAssignCVector3Generic), asCALL_GENERIC); assert(_Result >= 0);
		_Result = _pEngine->RegisterObjectMethod("CVector3", "CVector3 opAdd(const CVector3 &in) const", asFUNCTION(AddCVector3Generic), asCALL_GENERIC); assert(_Result >= 0);
		_Result = _pEngine->RegisterObjectMethod("CVector3", "CVector3 opSub(const CVector3 &in) const", asFUNCTION(SubCVector3Generic), asCALL_GENERIC); assert(_Result >= 0);
		_Result = _pEngine->RegisterObjectMethod("CVector3", "CVector3 opMul(float) const", asFUNCTION(MulCVector3Generic), asCALL_GENERIC); assert(_Result >= 0);
		_Result = _pEngine->RegisterObjectMethod("CVector3", "CVector3 opDiv(float) const", asFUNCTION(DivCVector3Generic), asCALL_GENERIC); assert(_Result >= 0);
		_Result = _pEngine->RegisterObjectMethod("CVector3", "bool opEquals(const CVector3 &in) const", asFUNCTION(EqualsCVector3Generic), asCALL_GENERIC); assert(_Result >= 0);
		
		_Result = _pEngine->RegisterObjectMethod("CVector3", "CVector3& SetVector(const float, const float, const float)", asFUNCTION(SetVectorCVector3Generic), asCALL_GENERIC); assert(_Result >= 0);
		_Result = _pEngine->RegisterObjectMethod("CVector3", "CVector3& Normalize()", asFUNCTION(NormalizeCVector3Generic), asCALL_GENERIC); assert(_Result >= 0);
		_Result = _pEngine->RegisterObjectMethod("CVector3", "float Length()", asFUNCTION(LengthCVector3Generic), asCALL_GENERIC); assert(_Result >= 0);
		_Result = _pEngine->RegisterObjectMethod("CVector3", "float DotProduct(const CVector3 &in)", asFUNCTION(DotProductCVector3Generic), asCALL_GENERIC); assert(_Result >= 0);
	}
	else
	{
		//-----------
		// Native
		//-----------
		// オペレータの登録
		_Result = _pEngine->RegisterObjectBehaviour("CVector3", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(Construct1CVector3), asCALL_CDECL_OBJLAST); assert(_Result >= 0);
		_Result = _pEngine->RegisterObjectBehaviour("CVector3", asBEHAVE_CONSTRUCT, "void f(const float, const float, const float)", asFUNCTION(Construct2CVector3), asCALL_CDECL_OBJLAST); assert(_Result >= 0);
		_Result = _pEngine->RegisterObjectBehaviour("CVector3", asBEHAVE_CONSTRUCT, "void f(const CVector3 &in)", asFUNCTION(CopyConstructCVector3), asCALL_CDECL_OBJLAST); assert(_Result >= 0);
		_Result = _pEngine->RegisterObjectBehaviour("CVector3", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(DestructCVector3), asCALL_CDECL_OBJLAST); assert(_Result >= 0);
		// メソッドの登録
		_Result = _pEngine->RegisterObjectMethod("CVector3", "CVector3& opAssign(const CVector3 &in)", asMETHODPR(CVector3, operator =, (const CVector3&), CVector3&), asCALL_THISCALL); assert(_Result >= 0);
		_Result = _pEngine->RegisterObjectMethod("CVector3", "CVector3& opAddAssign(const CVector3 &in)", asMETHODPR(CVector3, operator+=, (const CVector3&), CVector3&), asCALL_THISCALL); assert(_Result >= 0);
		_Result = _pEngine->RegisterObjectMethod("CVector3", "CVector3& opSubAssign(const CVector3 &in)", asMETHODPR(CVector3, operator-=, (const CVector3&), CVector3&), asCALL_THISCALL); assert(_Result >= 0);
		_Result = _pEngine->RegisterObjectMethod("CVector3", "CVector3& opMulAssign(const CVector3 &in)", asMETHODPR(CVector3, operator*=, (const CVector3&), CVector3&), asCALL_THISCALL); assert(_Result >= 0);
		_Result = _pEngine->RegisterObjectMethod("CVector3", "CVector3& opDivAssign(const CVector3 &in)", asMETHODPR(CVector3, operator/=, (const CVector3&), CVector3&), asCALL_THISCALL); assert(_Result >= 0);
		_Result = _pEngine->RegisterObjectMethod("CVector3", "CVector3 opAdd(const CVector3 &in) const", asMETHODPR(CVector3, operator+, (const CVector3&) const, CVector3), asCALL_THISCALL); assert(_Result >= 0);
		_Result = _pEngine->RegisterObjectMethod("CVector3", "CVector3 opSub(const CVector3 &in) const", asMETHODPR(CVector3, operator-, (const CVector3&) const, CVector3), asCALL_THISCALL); assert(_Result >= 0);
		_Result = _pEngine->RegisterObjectMethod("CVector3", "CVector3 opMul(float) const", asMETHODPR(CVector3, operator*, (float) const, CVector3), asCALL_THISCALL); assert(_Result >= 0);
		_Result = _pEngine->RegisterObjectMethod("CVector3", "CVector3 opDiv(float) const", asMETHODPR(CVector3, operator/, (float) const, CVector3), asCALL_THISCALL); assert(_Result >= 0);
		_Result = _pEngine->RegisterObjectMethod("CVector3", "bool opEquals(const CVector3 &in) const", asMETHODPR(CVector3, operator==, (const CVector3&) const, bool), asCALL_THISCALL); assert(_Result >= 0);
		
		_Result = _pEngine->RegisterObjectMethod("CVector3", "CVector3& SetVector(const float, const float, const float)", asMETHODPR(CVector3, SetVector, (float, float, float), CVector3&), asCALL_THISCALL); assert(_Result >= 0);
		_Result = _pEngine->RegisterObjectMethod("CVector3", "CVector3& Normalize()", asMETHODPR(CVector3, Normalize, (), CVector3&), asCALL_THISCALL); assert(_Result >= 0);
		_Result = _pEngine->RegisterObjectMethod("CVector3", "float Length()", asMETHODPR(CVector3, Length, () const, float), asCALL_THISCALL); assert(_Result >= 0);
		_Result = _pEngine->RegisterObjectMethod("CVector3", "float DotProduct(const CVector3 &in)", asMETHODPR(CVector3, DotProduct, (const CVector3&) const, float), asCALL_THISCALL); assert(_Result >= 0);
	}

	// オブジェクトプロパティの登録
	_Result = _pEngine->RegisterObjectProperty("CVector3", "float x",	asOFFSET(CVector3, x));		assert(_Result >= 0);
	_Result = _pEngine->RegisterObjectProperty("CVector3", "float y",	asOFFSET(CVector3, y));		assert(_Result >= 0);
	_Result = _pEngine->RegisterObjectProperty("CVector3", "float z",	asOFFSET(CVector3, z));		assert(_Result >= 0);

}
