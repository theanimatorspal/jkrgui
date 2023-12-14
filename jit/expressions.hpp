#include <llvm-c/lto.h>

#ifndef GETTER
#define GETTER inline auto
#endif
#define mu std::make_unique
#define up std::unique_ptr
#define mv std::move
template<typename T>
using v = std::vector<T>;
using s = std::string;
using sv = std::string_view;



struct Expression
{
	virtual ~Expression() = default;
	virtual Value* codegen() = 0;
};

struct Number : Expression
{
	Number(double inValue)
		: mValue(inValue)
	{}

private:
	double mValue;
};

struct Variable : Expression
{
	Variable(const sv inName)
		: mName(inName)
	{}
	s mName;
};

struct Binary : Expression
{
	Binary(char inOp, up<Expression> inLHS, up<Expression> inRHS)
		: mOp(inOp)
		, mLHS(mv(inLHS))
		, mRHS(mv(inRHS))
	{}
	char mOp;
	up<Expression> mLHS, mRHS;
};

struct Call : Expression
{
	Call(const sv inCallee, v<up<Expression>> Args)
		: mCallee(inCallee)
		, mArgs(mv(Args))
	{}
	s mCallee;
	v<up<Expression>> mArgs;
};

struct Prototype : Expression
{
	Prototype(const sv inName, v<s> inArgs)
		: mName(inName)
		, mArgs(mv(inArgs))
	{}
	GETTER GetName() const { return mName; }
	s mName;
	v<s> mArgs;
};

struct Function : Expression
{
	Function(up<Prototype> inPrototype, up<Expression> inBody)
		: mPrototype(mv(inPrototype))
		, mBody(mv(inBody))
	{}
	up<Prototype> mPrototype;
	up<Expression> mBody;
};
