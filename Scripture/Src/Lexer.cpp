//
// Created by lsc on 20-05-18.
//

#include <Lsc/Scripture/Lexer.h>

namespace Lsc
{

Lexer::ConfigData &Lexer::Config()
{
    return mConfig;
}

#pragma region InternalCursor

/*!
 * @brief Skips white spaces character, advancing(/consuming) M pointer
 *
 * Prefix increment operator
 * @return true if M is not on EOF, false otherwise.
 */
bool Lexer::InternalCursor::operator++()
{
    if(C >= E)
        return false;
    ++C;
    while((C < E) && (isspace(*C)))
        ++C;
    return true;
}

/*!
 * @brief Skips white spaces character, advancing(/consuming) M pointer
 *
 * Postfix increment operator, just calls the prefix increment operator.
 * @return true if M is not on EOF, false otherwise.
 */
bool Lexer::InternalCursor::operator++(int)
{
    return ++(*this);
}

/*!
* @brief Skips white spaces character, advancing(/consuming) M pointer
*
* Named method, just calls the prefix increment operator.
* @return true if M is not on EOF, false otherwise.
*/
[[maybe_unused]] bool Lexer::InternalCursor::SkipWS()
{
    return ++(*this);
}

/*!
 * @brief Tests if P is on or past EOF.
 * @param P
 * @return true if P is EOF, false otherwise.
 */
bool Lexer::InternalCursor::Eof(const char *P) const
{
    if(P)
        return P > E;
    return C > E;
}

/*!
 * @brief Synchronize the Location data from the M pointer.
 *
 * @return none.
 */
void Lexer::InternalCursor::Sync()
{
    L = 1;
    while(C > B)
    {
        if((*C == '\n') || (*C == '\r'))
            ++L;
        --C;
    }
    
    Col = 1;
    while((C > B) && (*C != '\n') && (*C != '\r'))
    {
        --C;
        ++Col;
    }
}

/*!
 * @brief Get the ptrdiff between the M pointer and the beginning of the source text (B pointer).
 * @return int.
 */
long Lexer::InternalCursor::Index() const
{
    return (ptrdiff_t) (C - B);
}

/*!
 * @brief Advances/Consume the M pointer till the next NewLine{'\r'; '\n'}  code in the source text
 * @return distinct std::string of the sequence.
 */
[[maybe_unused]] std::string Lexer::InternalCursor::ScanToEol()
{
    std::string Str;
    while((C <= E) && (*C != '\n') && (*C != '\r'))
        Str += *C++;
    return Str;
}

/*!
 * @brief Get a std::string copy of the current line from the M pointer
 * @return string.
 */
std::string Lexer::InternalCursor::Line() const
{
    std::string Str;
    
    const char *lb, *eb;
    lb = eb = C;
    while((lb > B) && (*lb != '\r') && (*lb != '\n'))
        --lb;
    if(lb > B)
        ++lb;
    while((eb < E) && (*eb != '\r') && (*eb != '\n'))
        ++eb;
    --eb;
    
    for(; lb <= eb; lb++)
        Str += *lb;
    return Str;
}

/*!
 * @brief Build a line String from the current position then Mark at the current columns.

  * @return std::string
  * @note : Must be Sync()'ed before calling Mark();
 
 */
std::string Lexer::InternalCursor::Mark() const
{
    String Str = Line();
    Str << '\n';
    for(int x = 0; x < Col; x++)
        Str << ' ';
    Str << '^';
    return Str();
}

/*!
 * @brief Get the string representation of the [internal]cursor location in the source text.
 * @return std::string
 */
[[maybe_unused]] std::string Lexer::InternalCursor::Location() const
{
    String Str = "%d,%d";
    Str << L << Col;
    return Str();
}

/*!
 * @brief Advances/Consumes the M pointer to Skip till SubStr_ match.
 * @param SubStr_
 * @return Expect code.
 */
[[maybe_unused]] Rem::Int Lexer::InternalCursor::ScanTo(const char *SubStr_)
{
    
    return Rem::Int::Ok;
}

Expect<std::string> Lexer::InternalCursor::ScanString()
{
    const char  *be    = C;
    char        Quote_ = *be;
    std::string Str;
    ++be;
    
    while((be <= E) && (*be != Quote_))
        Str += *be++;
    
    if((*be != Quote_) && (be > E))
    {
        Sync();
        return (Rem::Save() << Rem::Type::Error << Rem::Int::Eof << " : Unterminated string constant:\n" << Mark());
    }
    Str += *be; // Include the rhs Quote.
    return Str;
}

Lexer::InternalCursor::InternalCursor(const char *Source_)
{
    C = Source_;
    E = C + std::strlen(C) - 1;
    B = C;
    L = Col = 1;
}

#pragma endregion InternalCursor

#pragma region NumScanner


Lexer::NumScanner::NumScanner(const char *_c, const char *_eos) : B(_c), C(_c), E(nullptr), Eos(_eos)
{}

/*!
 * @brief For now a bare minimum digit with some rough floating point scan.
 * @return true if the M pointer is consumed and advanced
 */
bool Lexer::NumScanner::operator++(int)
{
    if(C >= Eos)
        return false;
    
    if(!isdigit(*C))
    {
        if(*C == '.')
            if(!Real)
                Real = true;
            else
                return false;
        else
            return false;
    }
    E = C;
    ++C;
    return true;
}

/*!
 * @brief Implements boolean operator
 * @return true if this NumScanner was  a valid numeric sequence, false otherwise.
 */
Lexer::NumScanner::operator bool() const
{
    return E >= B;
    //return false;
}

/*!
 * @brief Computes the numeric scale and 'best gess' base.
 * @return one [combined] of {{u,i}{8,16,32,64}} | fp| oct | hex | bin.
 *
 * @note Numeric Base is omitted as of this version. Thus it only computes the Scale.
 */
Type::T Lexer::NumScanner::operator()() const
{
    if(!Real)
    {
        String   Str = String::MakeStr(B, E);
        uint64_t D;
        Str >> D;
        uint64_t                I = 0;
        std::array<uint64_t, 3> R = {0x100, 0x10000, 0x100000000};
        while(D >= R[I])
            ++I;
        std::array<Type::T, 4> Cap = {Type::U8, Type::U16, Type::U32, Type::U64};
        return Cap[I];
    }
    
    ///@todo SCAN SCIENTIFIC NOTATION !!!!!!
    return Type::Float;
}

#pragma endregion NumScanner


#pragma region Scanners

std::map<Lexer::InputPair, Lexer::ScannerFn> Lexer::_ProductionTable = {
    // Begin from empty tokens stream:
    {{Type::Null,      Type::Null},    &Lexer::_InputDefault},
    {{Type::Null,      Type::Unary},   &Lexer::_InputUnaryOperator},
    {{Type::Null,      Type::Keyword}, &Lexer::_InputKeyword},
    {{Type::Null,      Type::Binary},  &Lexer::ScanSignPrefix},
    
    
    // -----------------------------------------------------------------
    
    // --- Phase 1 association:
    //     Binary Operators:
    {{Type::ClosePair, Type::Binary},  &Lexer::_InputBinaryOperator},
    {{Type::Postfix,   Type::Binary},  &Lexer::_InputBinaryOperator},
    {{Type::Id,        Type::Binary},  &Lexer::_InputBinaryOperator},
    {{Type::Number,    Type::Binary},  &Lexer::_InputBinaryOperator},
    {{Type::Text,      Type::Binary},  &Lexer::_InputBinaryOperator},
    {{Type::Binary,    Type::Binary},  &Lexer::ScanSignPrefix},
    //...
    // ----------------------------------------------------------------
    
    // (Restricted) Factor Notation Syntax:
    {{Type::Number, Type::Null},  &Lexer::ScanFactorNotation},
    // --- Phase 1 association:
    //     Unary Operators:
    
    //...
};

Lexer::Scanner Lexer::GetScanner(Lexer::InputPair &&Pair)
{
    for(auto M : Lexer::_ProductionTable) if(M.first == Pair) return M.second; // That's it! :)
    // More to do here...
    
    return Rem::Save() << Rem::Int::Rejected << " Lexer::GetScanner : No match...";
}



Return Lexer::_InputBinaryOperator(TokenData &Token_)
{
    return Append(Token_);
}



/*!
 * @brief Unknow Input Token.
 * @return Expect<>
 */
Return Lexer::_InputDefault(TokenData &Token_)
{
    if(!ScanNumber(Token_))
    {
        if(!ScanIdentifier(Token_))
            return Rem::Save() << Rem::Type::Fatal << ": " << Rem::Int::UnExpected << " Token type " << Token_.TypeName();
    }
    
    mCursor.Sync();
    
    return Rem::Int::Implement;
}


Return Lexer::_InputUnaryOperator(TokenData &)
{
    return (Rem::Save() << Rem::Int::Implement);
}

Return Lexer::_InputPunctuation(TokenData &)
{
    return (Rem::Save() << Rem::Int::Implement);
}
Return Lexer::_InputKeyword(TokenData &)
{
    return (Rem::Save() << Rem::Int::Implement);
}
Return Lexer::_InputString(TokenData &)
{
    return (Rem::Save() << Rem::Int::Implement);
}
Return Lexer::_InputHex(TokenData &)
{
    return (Rem::Save() << Rem::Int::Implement);
}
Return Lexer::ScanNumber(TokenData &)
{
    return (Rem::Save() << Rem::Int::Implement);
}


Return Lexer::ScanIdentifier(TokenData &)
{
    
    return (Rem::Save() << Rem::Int::Implement);
}

/*!
 * @brief  Scans for std maths factor notation syntax style:
 *         4ac => 4 x a x c
 *         4(ac...) => 4 x ( a x c ...)
 *         Rejected sequences:
 *         ac4 => Id; a4c => Id ...;
 *
 * @note   Required that the Left hand side token is a Number and that the Input token is contiguous and of unknown type (Type::Null) to be scanned as an identifier.
 * @return Execp<>
 */
Return Lexer::ScanFactorNotation(TokenData &Token_)
{
    // Tokens stream is NOT EMPTY here.
    
    // Required that the next Token_ is contiguous ( no [white]space between lhs and Token_ ).
    if(mCursor.C > (mConfig.Tokens->back().mLoc.End + 1 ))
        return Rem::Int::Rejected;
    
    
    // Set _F "state" flag :
    if(!mCursor._F)
    {
        // Required that the LHS is of type Number.
        
    }
    
    return (Rem::Save() << Rem::Int::Implement);
}

Return Lexer::ScanSignPrefix(TokenData &Token_)
{
    if(Token_.M == Mnemonic::Add || Token_.M == Mnemonic::Sub)
    {
        Token_.T = Type::Prefix;
        Token_.S = (Token_.S & ~Type::Binary) | Type::Sign | Type::Unary | Type::Prefix; // Type::Operator bit already set
        return Append(Token_);
    }
    return _InputBinaryOperator(Token_);
}


Return Lexer::ScanPrefix(TokenData &)
{
    return (Rem::Save() << Rem::Int::Implement);
}
Return Lexer::ScanPostfix(TokenData &)
{
    return (Rem::Save() << Rem::Int::Implement);
}

#pragma endregion Scanners

Return Lexer::Append(TokenData &Token_)
{
    if(!Token_)
        return (Rem::Save() << Rem::Type::Error << ": Attempt to push a Null TokenData into the Tokens stream.");
    
    mCursor.Sync();
    Token_.mLoc.L = mCursor.L;
    Token_.mLoc.C = mCursor.Col;

    std::size_t sz = Token_.mLoc.End - Token_.mLoc.Begin + 1;
    Token_.mLoc.I = (ptrdiff_t) (Token_.mLoc.Begin - mCursor.B);
    mCursor.C += sz;
    mCursor.Col += sz;
    mConfig.Tokens->push_back(Token_);
    ++mCursor;
    std::cout << __PRETTY_FUNCTION__ << ":\n" << mCursor.Mark() << '\n';
    return Rem::Int::Accepted;
}

Return Lexer::operator()()
{
    return Exec();
}

Return Lexer::Exec()
{
    return Rem::Int::Implement;
}


}