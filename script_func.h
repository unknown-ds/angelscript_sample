#ifndef	_SCRIPT_FUNC_H_
#define	_SCRIPT_FUNC_H_

#include	"./sdk/angelscript/include/angelscript.h"
#include	"sdk\add_on\scriptarray\scriptarray.h"
#include	"sdk\add_on\scriptstdstring\scriptstdstring.h"
#include	"sdk\add_on\scriptbuilder\scriptbuilder.h"


using namespace std;
//==========================================================================
//--------------------------------------------------------------------------
// printf関数
extern void asPrintf0(string &msg);
extern void asPrintf0Generic(
	asIScriptGeneric *pGen
	);

// printf関数
extern void asPrintf1(
	string& fmt,
	void*	_pRef,
	int		_TypeId
	);
extern void asPrintf1Generic(
	asIScriptGeneric *pGen
);

//--------------------------------------------------------------------------
// スクリプトにバインドするテスト関数１
extern float testBindFunc1(
	float lhs,
	float rhs
	);
extern void testBindFunc1Generic(
	asIScriptGeneric *pGen
	);


//--------------------------------------------------------------------------
// スクリプトにバインドするテスト関数２
extern int testBindFunc2(
	void*	_pIntArray,
	int		_Param
	);
extern void testBindFunc2Generic(
	asIScriptGeneric *pGen
	);

//--------------------------------------------------------------------------
// スクリプトにバインドするテスト関数３
extern string testBindFunc3(
	string _Input
	);
extern void testBindFunc3Generic(
	asIScriptGeneric *pGen
	);



//==========================================================================
//==========================================================================
// Vector型の登録
extern void RegistClass_CVector3(asIScriptEngine* _pEngine);

//--------------------------------------------------------------------------
// 3要素ベクター型クラス
class CVector3
{
public:
	// コンストラクタ
	CVector3()
	{
		x = y = z = 0.0f;
	}
	// コンストラクタ
	CVector3(float _xx, float _yy, float _zz)
	{
		x = _xx;
		y = _yy;
		z = _zz;
	}
	// コピーコンストラクタ
	CVector3(const CVector3& _Other)
		:x(_Other.x), y(_Other.y), z(_Other.z)
	{
	}

	//---------------
	// 初期化
	//---------------
	CVector3& SetVector(float _xx, float _yy, float _zz)
	{
		x = _xx; y = _yy; z = _zz;
		return(*this);
	}

	//----------------
	// 代入演算子
	//----------------
	// =
	CVector3& operator=(const CVector3& _Other)
	{
		x = _Other.x;	y = _Other.y;	z = _Other.z;
		return(*this);
	}
	// +=
	CVector3& operator+=(const CVector3& _Other)
	{
		x += _Other.x;	y += _Other.y;	z += _Other.z;
		return(*this);
	}
	// -=
	CVector3& operator-=(const CVector3& _Other)
	{
		x -= _Other.x;	y -= _Other.y;	z -= _Other.z;
		return(*this);
	}
	// *=
	CVector3& operator*=(const CVector3& _Other)
	{
		x *= _Other.x;	y *= _Other.y;	z *= _Other.z;
		return(*this);
	}
	// /=
	CVector3& operator/=(const CVector3& _Other)
	{
		x /= _Other.x;	y /= _Other.y;	z /= _Other.z;
		return(*this);
	}

	//----------------
	// 2項演算子
	//----------------
	// +
	CVector3 operator+(const CVector3& other) const {
		return( CVector3(x+other.x, y+other.y, z+other.z) );
	}
	// -
	CVector3 operator-(const CVector3& other) const {
		return( CVector3(x-other.x, y-other.y, z-other.z) );
	}
	// *
	CVector3 operator*(float rate) const {
		return( CVector3(x*rate, y*rate, z*rate) );
	}
	// /
	CVector3 operator/(float rate) const {
		return( CVector3(x/rate, y/rate, z/rate) );
	}

	//----------------
	// 比較演算子
	//----------------
	// ==
	bool operator==(const CVector3& other) const {
		return( (x==other.x) && (y==other.y) && (z==other.z) );
	}
	// !=
	bool operator!=(const CVector3& other) const {
		return( !(*this == other) );
	}

	// 正規化
	CVector3& Normalize()
	{
		float _ff = x*x + y*y + z*z;
		
		if(DBL_EPSILON >= _ff){
			x = y = z = 0.0f;
			return(*this);
		}
		
		_ff = 1.0f / sqrtf(_ff);
		x *= _ff;
		y *= _ff;
		z *= _ff;
		
		return(*this);
	}
	
	// 長さを取得
	float Length() const {
		return( sqrtf(x*x + y*y + z*z) );
	}

	// 内積
	float DotProduct(
		const CVector3& _Vec		// 対象ベクトル
		) const;

public:
	float	x{0.0f};
	float	y{0.0f};
	float	z{0.0f};
};




#endif		// _SCRIPT_FUNC_H_
