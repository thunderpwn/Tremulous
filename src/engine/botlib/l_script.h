////////////////////////////////////////////////////////////////////////////////////////
// Copyright(C) 1999 - 2010 id Software LLC, a ZeniMax Media company.
// Copyright(C) 2011 - 2018 Dusan Jocic <dusanjocic@msn.com>
//
// This file is part of the OpenWolf GPL Source Code.
// OpenWolf Source Code is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// OpenWolf Source Code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with OpenWolf Source Code.  If not, see <http://www.gnu.org/licenses/>.
//
// In addition, the OpenWolf Source Code is also subject to certain additional terms.
// You should have received a copy of these additional terms immediately following the
// terms and conditions of the GNU General Public License which accompanied the
// OpenWolf Source Code. If not, please request a copy in writing from id Software
// at the address below.
//
// If you have questions concerning this license or the applicable additional terms,
// you may contact in writing id Software LLC, c/o ZeniMax Media Inc.,
// Suite 120, Rockville, Maryland 20850 USA.
//
// -------------------------------------------------------------------------------------
// File name:   l_script.h
// Version:     v1.00
// Created:
// Compilers:   Visual Studio 2015
// Description:
// -------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////

#ifndef __L_SCRIPT_H__
#define __L_SCRIPT_H__

//undef if binary numbers of the form 0b... or 0B... are not allowed
#define BINARYNUMBERS
//undef if not using the token.intvalue and token.floatvalue
#define NUMBERVALUE
//use dollar sign also as punctuation
#define DOLLAR

//maximum token length
#define MAX_TOKEN					1024

//script flags
#define SCFL_NOERRORS				0x0001
#define SCFL_NOWARNINGS				0x0002
#define SCFL_NOSTRINGWHITESPACES	0x0004
#define SCFL_NOSTRINGESCAPECHARS	0x0008
#define SCFL_PRIMITIVE				0x0010
#define SCFL_NOBINARYNUMBERS		0x0020
#define SCFL_NONUMBERVALUES		0x0040

//token types
#define TT_STRING						1			// string
#define TT_LITERAL					2			// literal
#define TT_NUMBER						3			// number
#define TT_NAME						4			// name
#define TT_PUNCTUATION				5			// punctuation

//string sub type
//---------------
//		the length of the string
//literal sub type
//----------------
//		the ASCII code of the literal
//number sub type
//---------------
#define TT_DECIMAL					0x0008	// decimal number
#define TT_HEX							0x0100	// hexadecimal number
#define TT_OCTAL						0x0200	// octal number
#ifdef BINARYNUMBERS
#define TT_BINARY						0x0400	// binary number
#endif //BINARYNUMBERS
#define TT_FLOAT						0x0800	// floating point number
#define TT_INTEGER					0x1000	// integer number
#define TT_LONG						0x2000	// long number
#define TT_UNSIGNED					0x4000	// unsigned number
//punctuation sub type
//--------------------
#define P_RSHIFT_ASSIGN				1
#define P_LSHIFT_ASSIGN				2
#define P_PARMS						3
#define P_PRECOMPMERGE				4

#define P_LOGIC_AND					5
#define P_LOGIC_OR					6
#define P_LOGIC_GEQ					7
#define P_LOGIC_LEQ					8
#define P_LOGIC_EQ					9
#define P_LOGIC_UNEQ					10

#define P_MUL_ASSIGN					11
#define P_DIV_ASSIGN					12
#define P_MOD_ASSIGN					13
#define P_ADD_ASSIGN					14
#define P_SUB_ASSIGN					15
#define P_INC							16
#define P_DEC							17

#define P_BIN_AND_ASSIGN			18
#define P_BIN_OR_ASSIGN				19
#define P_BIN_XOR_ASSIGN			20
#define P_RSHIFT						21
#define P_LSHIFT						22

#define P_POINTERREF					23
#define P_CPP1							24
#define P_CPP2							25
#define P_MUL							26
#define P_DIV							27
#define P_MOD							28
#define P_ADD							29
#define P_SUB							30
#define P_ASSIGN						31

#define P_BIN_AND						32
#define P_BIN_OR						33
#define P_BIN_XOR						34
#define P_BIN_NOT						35

#define P_LOGIC_NOT					36
#define P_LOGIC_GREATER				37
#define P_LOGIC_LESS					38

#define P_REF							39
#define P_COMMA						40
#define P_SEMICOLON					41
#define P_COLON						42
#define P_QUESTIONMARK				43

#define P_PARENTHESESOPEN			44
#define P_PARENTHESESCLOSE			45
#define P_BRACEOPEN					46
#define P_BRACECLOSE					47
#define P_SQBRACKETOPEN				48
#define P_SQBRACKETCLOSE			49
#define P_BACKSLASH					50

#define P_PRECOMP						51
#define P_DOLLAR						52
//name sub type
//-------------
//		the length of the name

//punctuation
typedef struct punctuation_s
{
    UTF8* p;						//punctuation character(s)
    S32 n;							//punctuation indication
    struct punctuation_s* next;		//next punctuation
} punctuation_t;

//token
typedef struct token_s
{
    UTF8 string[MAX_TOKEN];			//available token
    S32 type;						//last read token type
    S32 subtype;					//last read token sub type
#ifdef NUMBERVALUE
    U64 intvalue;	//integer value
    F64 floatvalue;			//floating point value
#endif //NUMBERVALUE
    UTF8* whitespace_p;				//start of white space before token
    UTF8* endwhitespace_p;			//start of white space before token
    S32 line;						//line the token was on
    S32 linescrossed;				//lines crossed in white space
    struct token_s* next;			//next token in chain
} token_t;

//script file
typedef struct script_s
{
    UTF8 filename[1024];			//file name of the script
    UTF8* buffer;					//buffer containing the script
    UTF8* script_p;					//current pointer in the script
    UTF8* end_p;					//pointer to the end of the script
    UTF8* lastscript_p;				//script pointer before reading token
    UTF8* whitespace_p;				//begin of the white space
    UTF8* endwhitespace_p;			//end of the white space
    S32 length;						//length of the script in bytes
    S32 line;						//current line in script
    S32 lastline;					//line before reading token
    S32 tokenavailable;				//set by UnreadLastToken
    S32 flags;						//several script flags
    punctuation_t* punctuations;	//the punctuations used in the script
    punctuation_t** punctuationtable;
    token_t token;					//available token
    struct script_s* next;			//next script in a chain
} script_t;

//read a token from the script
S32 PS_ReadToken( script_t* script, token_t* token );
//expect a certain token
S32 PS_ExpectTokenString( script_t* script, UTF8* string );
//expect a certain token type
S32 PS_ExpectTokenType( script_t* script, S32 type, S32 subtype, token_t* token );
//expect a token
S32 PS_ExpectAnyToken( script_t* script, token_t* token );
//returns true when the token is available
S32 PS_CheckTokenString( script_t* script, UTF8* string );
//returns true an reads the token when a token with the given type is available
S32 PS_CheckTokenType( script_t* script, S32 type, S32 subtype, token_t* token );
//skip tokens until the given token string is read
S32 PS_SkipUntilString( script_t* script, UTF8* string );
//unread the last token read from the script
void PS_UnreadLastToken( script_t* script );
//unread the given token
void PS_UnreadToken( script_t* script, token_t* token );
//returns the next character of the read white space, returns NULL if none
UTF8 PS_NextWhiteSpaceChar( script_t* script );
//remove any leading and trailing double quotes from the token
void StripDoubleQuotes( UTF8* string );
//remove any leading and trailing single quotes from the token
void StripSingleQuotes( UTF8* string );
//read a possible signed integer
S64 ReadSignedInt( script_t* script );
//read a possible signed floating point number
F64 ReadSignedFloat( script_t* script );
//set an array with punctuations, NULL restores default C/C++ set
void SetScriptPunctuations( script_t* script, punctuation_t* p );
//set script flags
void SetScriptFlags( script_t* script, S32 flags );
//get script flags
S32 GetScriptFlags( script_t* script );
//reset a script
void ResetScript( script_t* script );
//returns true if at the end of the script
S32 EndOfScript( script_t* script );
//returns a pointer to the punctuation with the given number
UTF8* PunctuationFromNum( script_t* script, S32 num );
//load a script from the given file at the given offset with the given length
script_t* LoadScriptFile( const UTF8* filename );
//load a script from the given memory with the given length
script_t* LoadScriptMemory( UTF8* ptr, S32 length, UTF8* name );
//free a script
void FreeScript( script_t* script );
//set the base folder to load files from
void PS_SetBaseFolder( UTF8* path );
//print a script error with filename and line number
void ScriptError( script_t* script, UTF8* str, ... );
//print a script warning with filename and line number
void ScriptWarning( script_t* script, UTF8* str, ... );

#endif //!__L_SCRIPT_H__