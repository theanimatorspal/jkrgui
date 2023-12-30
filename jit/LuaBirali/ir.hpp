#include "jit.hpp"
#include "lexer.hpp"
#include "lua.hpp"

enum BinOpr {
    /* arithmetic operators */
    ADD,
    SUB,
    MUL,
    MOD,
    POW,
    DIV,
    IDIV,
    /* bitwise operators */
    BAND,
    BOR,
    BXOR,
    SHL,
    SHR,
    /* string operator */
    CONCAT,
    /* comparison operators */
    EQ,
    LT,
    LE,
    NE,
    GT,
    GE,
    /* logical operators */
    AND,
    OR,
    NOBINOPR
};

/*
** Priority table for binary operators.
*/
constexpr struct {
    int left; /* left priority for each binary operator */
    int right; /* right priority */
} priority[] = {
    /* ORDER OPR */
    { 10, 10 }, { 10, 10 }, /* '+' '-' */
    { 11, 11 }, { 11, 11 }, /* '*' '%' */
    { 14, 13 }, /* '^' (right associative) */
    { 11, 11 }, { 11, 11 }, /* '/' '//' */
    { 6, 6 }, { 4, 4 }, { 5, 5 }, /* '&' '|' '~' */
    { 7, 7 }, { 7, 7 }, /* '<<' '>>' */
    { 9, 8 }, /* '..' (right associative) */
    { 3, 3 }, { 3, 3 }, { 3, 3 }, /* ==, <, <= */
    { 3, 3 }, { 3, 3 }, { 3, 3 }, /* ~=, >, >= */
    { 2, 2 }, { 1, 1 } /* and, or */
};

static BinOpr getbinopr(int op)
{
    switch (op) {
    case '+':
        return BinOpr::ADD;
    case '-':
        return BinOpr::SUB;
    case '*':
        return BinOpr::MUL;
    case '%':
        return BinOpr::MOD;
    case '^':
        return BinOpr::POW;
    case '/':
        return BinOpr::DIV;
    case Token::Idiv:
        return BinOpr::IDIV;
    case '&':
        return BinOpr::BAND;
    case '|':
        return BinOpr::BOR;
    case '~':
        return BinOpr::BXOR;
    case Token::Shl:
        return BinOpr::SHL;
    case Token::Shr:
        return BinOpr::SHR;
    case Token::Concat:
        return BinOpr::CONCAT;
    case Token::Ne:
        return BinOpr::NE;
    case Token::Eq:
        return BinOpr::EQ;
    case '<':
        return BinOpr::LT;
    case Token::Le:
        return BinOpr::LE;
    case '>':
        return BinOpr::GT;
    case Token::Ge:
        return BinOpr::GE;
    case Token::And:
        return BinOpr::AND;
    case Token::Or:
        return BinOpr::OR;
    default:
        return BinOpr::NOBINOPR;
    }
}

enum UnOpr {
    OPR_MINUS,
    OPR_BNOT,
    OPR_NOT,
    OPR_LEN,
    OPR_NOUNOPR
};

constexpr int UNARY_PRIORITY = 12;

static UnOpr getunopr(int op)
{
    switch (op) {
    case Token::Not:
        return OPR_NOT;
    case '-':
        return OPR_MINUS;
    case '~':
        return OPR_BNOT;
    case '#':
        return OPR_LEN;
    default:
        return OPR_NOUNOPR;
    }
}

struct LHS_assign {
    LHS_assign* previous;
    ExpDesc v;
};

class Ir : public Lexer {
    Ir(std::istream& inS)
        : Lexer(inS)
    {
    }

    bool testnext(int c)
    {
        if (ls.mt.token == c) {
            Lexer::Next();
            return 1;
        } else {
            return 0;
        }
    }

    void check(int c)
    {
        if (ls.mt.token != c)
            (SyntaxError << "Error Expected " << TokenToString(c) << " here " << ls.mlinenumber)++;
    }

    void checkmatch(int what, int who, int where)
    {
        if (not testnext(what)) {
            if (where == ls.mlinenumber)
                (SyntaxError << "Expected " << TokenToString(what) << "<>")++;
        } else {
            (SyntaxError << "Expected " << TokenToString(what) << " to close " << TokenToString(who) << "at line " << where)++;
        }
    }

    s strcheckname()
    {
        s str;
        check(Token::Name);
        str = get<s>(ls.mt.s);
        Next();
        return str;
    }

    void singlevar(ExpDesc* var)
    {
        s varname = strcheckname();
        // Find a variable with given name
        if (var->kind == ExpKind::vvoid) { /* global name */
            ExpDesc key;
            // TODO Generate Code for The Variable
            // TODO Code String
            // TODO Generate code for indexed variable
        }
    }

    void initexpression(ExpDesc* e, ExpKind k, int i)
    {
        e->kind = k;
        e->info = i;
    }

    void codestring(ExpDesc* e, sv ins)
    {
        e->kind = ExpKind::vkstr;
        e->u = s(ins);
    }

    void codename(ExpDesc* e)
    {
        codestring(e, strcheckname());
    }

    // kita simple expression kita unop subexpression yaa binop subexpr
    BinOpr subexpression(ExpDesc* v, int limit)
    {
        BinOpr binopr;
        UnOpr uopr;
        uopr = getunopr(ls.mt.token);
        if (uopr != UnOpr::OPR_NOUNOPR) /* Vanepaxi Unary ho*/
        {
            int line = ls.mlinenumber;
            Next();
            subexpression(v, UNARY_PRIORITY);
            // TODO: Generate LLVM Code
        } else {
            // TODO: Simple Expression
        }

        binopr = getbinopr(ls.mt.token);
        while (binopr != BinOpr::NOBINOPR and priority[binopr].left > limit) {
            ExpDesc v2;
            BinOpr nextOp;
            int line = ls.mlinenumber;
            Next();
            // TODO: Generate Code for infix
            nextOp = subexpression(&v2, priority[binopr].right);
            // Generated Code for postfix
            binopr = nextOp;
        }

        return binopr;
    }

    void primaryexpression(ExpDesc* v)
    {
        /* Either a name or '(' wala parent expression */
        switch (ls.mt.token) {
        case '(':
            int line = ls.mlinenumber;
            Lexer::Next();
            expression(v);
            checkmatch(')', '(', line);
            return;
        case Token::Name: {
            singlevar(v);
            return;
        }
        default:
            (SyntaxError << "Unexpected Symbol")++;
        }
    }

    void expression(ExpDesc* v)
    {
        subexpression(v, 0);
    }

    void suffixedexp(ExpDesc* v)
    {
        primaryexpression(v);
        while (true) {
            switch (ls.mt.token) {
            case '.': {
                // TODO: fieldselwtf
                break;
            }
            case '[': {
                ExpDesc key;
                // TODO: GEnerate Indexing Code
                // TODO: yindex
                // GENERATE indexing code
                break;
            }
            case ':': {
                ExpDesc key;
                Next();
                // TODO COdename
                // Generate Self Code
                // Funcargs
                break;
            }
            case '(':
            case Token::String:
            case '{': {
                // TODO Func Args
                break;
            }
            default:
                return;
            }
        }
    }

    void simpleexpression(ExpDesc* v)
    {
        switch (ls.mt.token) {
        case Token::Flt: {
            initexpression(v, ExpKind::vkflt, 0);
            v->u = get<Number>(ls.mt.s);
            break;
        }
        case Token::Int: {
            initexpression(v, ExpKind::vkint, 0);
            v->u = get<Integer>(ls.mt.s);
            break;
        }
        case Token::String: {
            codestring(v, get<s>(ls.mt.s));
            break;
        }
        case Token::Nil: {
            initexpression(v, ExpKind::vnil, 0);
            break;
        }
        case Token::True: {
            initexpression(v, ExpKind::vtrue, 0);
            break;
        }
        case Token::False: {
            initexpression(v, ExpKind::vfalse, 0);
            break;
        }
        case Token::Dots: {
            // TODO FunctionState
            // Check Condition
            // init Expression
            break;
        }
        case '{': {
            // TODO Constructor
            break;
        }
        case Token::Function: {
            Lexer::Next();
            // body
            break;
        }
        default:
            suffixedexp(v);
            return;
        }
        Lexer::Next();
    }

    void expressionstatement()
    {
        LHS_assign v;
        // TODO Suffixed Expression
        if (ls.mt.token == '=' or ls.mt.token == ',') // is assignment
        {
            v.previous = nullptr;
            // TODO: Rest Assign
        } else {
            // TODO: FunctionCall
            // Check Condition v.v.k == VCall kind
        }
    }

    void statement()
    {
        switch (ls.mt.token) {
        case ';': {
            Lexer::Next();
            break;
        }
        case Token::If: {
            // TODO: parse IF statement
            break;
        }
        case Token::While: {
            break;
        }
        case Token::Do: {
            break;
        }
        case Token::For: {
            break;
        }
        case Token::Repeat: {
            break;
        }
        case Token::Function: {
            break;
        }
        case Token::Local: {
            break;
        }
        case Token::Dbcolon: {

            break;
        }
        case Token::Return: {
            break;
        }
        case Token::Goto: {
            break;
        }
        default: {
            break;
        }
        }
    }

private:
    up<llvm::LLVMContext> mirc;
    up<llvm::IRBuilder<>> mb;
    up<llvm::Module> mmod;
    std::map<std::string, llvm::AllocaInst*> mNamedValues;
    // std::map<std::string, up<Expr::Prototype>> mFunctionPrototypes;

    llvm::ExitOnError mExitOnError;
    up<Jit> mJit;
    up<llvm::FunctionPassManager> mFunctionPassManager;
    up<llvm::LoopAnalysisManager> mLoopAnalysisManager;
    up<llvm::FunctionAnalysisManager> mFunctionAnalysisManager;
    up<llvm::CGSCCAnalysisManager> mCGSCCAnalysisManager;
    up<llvm::ModuleAnalysisManager> mModuleAnalysisManager;
    up<llvm::PassInstrumentationCallbacks> mPassInstrumentationCallbacks;
    up<llvm::StandardInstrumentations> mStandardInstrumentations;

private:
    void InitializeJit()
    {
        llvm::InitializeNativeTarget();
        llvm::InitializeNativeTargetAsmPrinter();
        llvm::InitializeNativeTargetAsmParser();
        mJit = mExitOnError(Jit::Create());
    }
    void InitializeModulePassAndMessengers ( )
    {
        mirc = mu<llvm::LLVMContext>();
        mmod = mu<llvm::Module>("LuaBiraliJit", *mirc);
        mmod->setDataLayout(mJit->GetDataLayout());
        mb = mu<llvm::IRBuilder<>>(*mirc);
        mFunctionPassManager = mu<llvm::FunctionPassManager>();
        mLoopAnalysisManager = mu<llvm::LoopAnalysisManager>();
        mFunctionAnalysisManager = mu<llvm::FunctionAnalysisManager>();
        mCGSCCAnalysisManager = mu<llvm::CGSCCAnalysisManager>();
        mModuleAnalysisManager = mu<llvm::ModuleAnalysisManager>();
        mPassInstrumentationCallbacks = mu<llvm::PassInstrumentationCallbacks>();
        mStandardInstrumentations = mu<llvm::StandardInstrumentations>(*mirc,
            /* debug logging */
            true);
        mStandardInstrumentations->registerCallbacks(*mPassInstrumentationCallbacks,
            mModuleAnalysisManager.get());
        mFunctionPassManager->addPass(llvm::RegToMemPass());
        mFunctionPassManager->addPass(llvm::InstCombinePass());
        mFunctionPassManager->addPass(llvm::ReassociatePass());
        mFunctionPassManager->addPass(llvm::GVNPass());
        mFunctionPassManager->addPass(llvm::SimplifyCFGPass());
        llvm::PassBuilder pb;
        pb.registerModuleAnalyses(*mModuleAnalysisManager);
        pb.registerFunctionAnalyses(*mFunctionAnalysisManager);
        pb.crossRegisterProxies(*mLoopAnalysisManager,
            *mFunctionAnalysisManager,
            *mCGSCCAnalysisManager,
            *mModuleAnalysisManager);
    }
    Dyndata mData;
};