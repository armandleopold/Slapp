/*
 * Quick and simple calculator to parse C-like expressions.   vi:ts=4:
 *
 * Written by T.Pierron, Jan 2008.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include "SWL.h"

#define RIGHT               1
#define LEFT                2

typedef struct Operator_t *     Operator;
typedef struct Stack_t *        Stack;

struct Operator_t
{
	STRPTR token;
	int    arity;
	int    associativity;
	int    priority;
};

/**h* SWL/Calc
 * INTRODUCTION
 *	This is a self-contained module that lets you parse C-like expression,
 *	including the use of named variable (which are read/written through a
 *	callback). It supports the following operators (in decreasing precedence):
 *
 *		Op  Pri Assoc Meaning
 *		=====================
 *		-   15  Right (unary negative)
 *		~   15  Right (unary bit toggle)
 *		!   15  Right (unary logical not, returns eitheir 0 or 1)
 *		++  14  Right (pre-increment of a variable name, no post-inc).
 *		--  14  Right (pre-decrement of a variable)
 *		*   13  Left  (multiplication)
 *		/   13  Left  (division)
 *		%   13  Left  (modulus)
 *		+   12  Left  (addition)
 *		-   12  Left  (substraction)
 *		>>  11  Left  (right bit shift)
 *		<<  11  Left  (left bit shift)
 *		<   10  Left  (logicial less than, returns eitheir 0 or 1)
 *		>   10  Left  (logical greater than)
 *		>=  10  Left  (logicial greater or equal than)
 *		<=  10  Left  (logicial less or equal than)
 *		==   9  Left  (equality test)
 *		!=   9  Left  (inequality test)
 *		&    8  Left  (bit and)
 *		^    7  Left  (bit exclusive or)
 *		|    6  Left  (bit or)
 *		&&   5  Left  (logical and, short circuit useless clause)
 *		||   4  Left  (logical or)
 *		?:   3  Right (if else inline, short circuit useless clause)
 *		=    2  Right (asignment into a variable)
 *		/=, *=, %=, +=, -=, <<=, >>=, &=, ^=, |=
 *		     2  Right (operation and assignment combined)
 *
 *	Left association means that 1 + 2 - 3 + 4 is evaluated as (((1 + 2) - 3) + 4).
 *	Right asscoiation means that ~!~!1 is evaluated as ~(!(~(!1))).
 *
 *	( and ) can be used to alter default operator priority.
 *
 *	Promotion is also supported. I.e : "3/2" is the integer division between 3 and 2,
 *	which is 1, while "3/2." is the real division between 3 and 2, that is 1.5.
 *
 *	Bit related operators will convert the operand into integer, if not already
 *	(like in javascript).
 *
 *	Boolean operator &&, || and ?: support short circuit eval. I.e: if first operand
 *	of a && is false, second operand won't be evaluated, although syntax will be
 *	checked.
 *
 *	Custom functions are not supported.
 *
 *	There a few things that does not work like C expression:
 *	- Post increment not supported.
 *	- Operators * (dereference), [] (array), & (address), ., ->, (type), sizeof,
 *	  and , are not supported.
 *
 *	For a full-featured embeddable language, check out the following projects :
 *	- spidermonkey (http://www.mozilla.org/js/spidermonkey/ )
 *	- LUA (http://www.lua.org/ )
 */

static struct Operator_t OperatorList[] =
{
	{ "-",   1, RIGHT, 15 }, { "~",   1, RIGHT, 15 },
	{ "!",   1, RIGHT, 15 }, { "++",  1, RIGHT, 14 },
	{ "--",  1, RIGHT, 14 }, { "*",   2, LEFT,  13 },
	{ "/",   2, LEFT,  13 }, { "%",   2, LEFT,  13 },
	{ "+",   2, LEFT,  12 }, { "-",   2, LEFT,  12 },
	{ "<<",  2, LEFT,  11 }, { ">>",  2, LEFT,  11 },
	{ "<",   2, LEFT,  10 }, { ">",   2, LEFT,  10 },
	{ "<=",  2, LEFT,  10 }, { ">=",  2, LEFT,  10 },
	{ "==",  2, LEFT,   9 }, { "!=",  2, LEFT,   9 },
	{ "&",   2, LEFT,   8 }, { "^",   2, LEFT,   7 },
	{ "|",   2, LEFT,   6 }, { "&&",  2, LEFT,   5 },
	{ "||",  2, LEFT,   4 }, { "?",   3, RIGHT,  3 },
	{ ":",   0, RIGHT,  3 }, { "=",   2, RIGHT,  2 },
	{ "*=",  2, RIGHT,  2 }, { "/=",  2, RIGHT,  2 },
	{ "%=",  2, RIGHT,  2 }, { "+=",  2, RIGHT,  2 },
	{ "-=",  2, RIGHT,  2 }, { "<<=", 2, RIGHT,  2 },
	{ ">>=", 2, RIGHT,  2 }, { "&=",  2, RIGHT,  2 },
	{ "^=",  2, RIGHT,  2 }, { "|=",  2, RIGHT,  2 }
};

struct Stack_t
{
	void * next;
	int    type;
	union
	{
		double   real;
		long     integer;
		char     ident[32];
		Operator ope;

	} value;
};

#define	doeval       value.ident[31]

enum
{
	TOKEN_UNKNOWN,
	TOKEN_NUMBER,
	TOKEN_INCPRI,
	TOKEN_DECPRI,
	TOKEN_OPERATOR
};


static Stack NewOperator(Operator ope)
{
	Stack oper = calloc(sizeof *oper, 1);

	oper->value.ope = ope;
	oper->type      = TYPE_OPE;
	oper->doeval    = True;

	return oper;
}

static Stack NewNumber(int type, ...)
{
	va_list args;
	Stack   num = calloc(sizeof *num, 1);
	STRPTR  arg;
	int     len;

	va_start(args, type);
	num->type = type;
	switch (type) {
	case TYPE_INT: num->value.integer = va_arg(args, ULONG); break;
	case TYPE_DBL: num->value.real    = va_arg(args, double); break;
	case TYPE_IDF:
		arg = va_arg(args, STRPTR);
		len = va_arg(args, int) + 1;
		CopyString(num->value.ident, arg, MIN(sizeof num->value.ident, len));
	}
	va_end(args);
	return num;
}

// Try to parse a number
static int GetNumber(Stack * object, STRPTR * exp)
{
	STRPTR cur = *exp;
	STRPTR str = cur;
	ULONG  nbi;
	double nbf;

	// Signed number are interpreted as a unsigned preceeded by an unary -
	if (! isdigit(*str)) return 0;

	// First try if we can parse an integer (octal, dec or hexa, like in C)
	nbi = strtoul(cur, &str, 0);

	if (str > cur && (*str == 0 || strchr("eE.", *str) == NULL))
	{
		*object = NewNumber(TYPE_INT, nbi);
		*exp    = str;
		return 1;
	}

	// Try a double
	nbf = strtod(cur, &str);

	if (str > cur)
	{
		*object = NewNumber(TYPE_DBL, nbf);
		*exp    = str;
		return 1;
	}
	return 0;
}

static int GetToken(Stack * object, STRPTR * exp)
{
	STRPTR str;
	int    type = TOKEN_UNKNOWN;

	// Skip starting space
	for (str = *exp; isspace(*str); str ++);

	// Precedence arrangement
	if (*str == '(' || *str == '[')
	{
		str ++;
		type = TOKEN_INCPRI;
	}
	else if (*str == ')' || *str == ']')
	{
		str ++;
		type = TOKEN_DECPRI;
	}
	else if (GetNumber(object, &str))
	{
		type = TOKEN_NUMBER;
	}
	else if (isalpha(*str))
	{
		// Ident
		for (*exp = str ++; isalnum(*str); str ++);

		*object = NewNumber(TYPE_IDF, *exp, (int) (str - *exp));
		type    = TOKEN_NUMBER;
	}
	else // Maybe an operator
	{
		Operator best, cur;

		// Use same rule a C : longest match is winner
		for (best = NULL, cur = OperatorList; cur < OperatorList + DIM(OperatorList); cur ++)
		{
			int length = strlen(cur->token);

			if (strncmp(str, cur->token, length) == 0 && (best == NULL || strlen(best->token) < length))
				best = cur;
		}
		if (best)
		{
			str += strlen(best->token);
			*object = NewOperator(best);
			type = TOKEN_OPERATOR;
		}
		else type = TOKEN_UNKNOWN;
	}

	*exp = str;

	return type;
}

static void PushStack(Stack * top, Stack object)
{
	object->next = *top;
	(*top) = object;
}

static Stack PopStack(Stack * top)
{
	Stack ret = *top;

	if (ret) *top = ret->next;

	return ret;
}

// Transform an ident into eitheir a TYPE_INT or TYPE_DBL
static void AffectArg(Stack arg, ParseExpCb cb, APTR data)
{
	if (arg->type == TYPE_IDF)
	{
		VariantBuf v;
		memset(&v, 0, sizeof v);
		cb(arg->value.ident, &v, 0, data);
		switch (v.type) {
		case TYPE_INT: arg->type = TYPE_INT; arg->value.integer = v.v.z; break;
		case TYPE_DBL: arg->type = TYPE_DBL; arg->value.real    = v.v.r; break;
		}
	}
}

static Bool IsNull(Stack arg)
{
	switch (arg->type) {
	case TYPE_INT: return arg->value.integer == 0; break;
	case TYPE_DBL: return arg->value.real    == 0; break;
	}
	return True;
}


#if	0
static void DebugStacks(Stack values, Stack oper)
{
	Stack s;

	printf("Values: ");
	for (s = values; s; s = s->next) {
		switch (s->type) {
		case TYPE_INT: printf("%ld ", s->value.integer); break;
		case TYPE_DBL: printf("%g ", s->value.real); break;
		case TYPE_IDF: printf("%s ", s->value.ident);
		}
	}

	printf("\nOper:   ");
	for (s = oper; s; s = s->next)
		printf("%s ", s->value.ope->token);
	printf("\n================================================\n");
}
#endif

// This is the function that takes operand and perform operation according to top most operator
static int MakeOp(Stack * values, Stack * oper, ParseExpCb cb, APTR data)
{
	Stack arg1, arg2, arg3;
	Operator ope = (*oper)->value.ope;
	VariantBuf v;
	Bool eval;
	int nb, error = 0;

	memset(&v, 0, sizeof v);

	#define	THROW(err)	{ error = err; goto error_case; }

	// Do we need to evalutate anything ?
	for (arg1 = (*oper)->next, eval = True; eval && arg1; eval = arg1->doeval, arg1 = arg1->next);

	arg1 = arg2 = arg3 = NULL;
	switch (ope->arity) {
	case 3:  arg3 = PopStack(values); if (arg3 == NULL) THROW(PERR_MissingOperand);
	case 2:  arg2 = PopStack(values); if (arg2 == NULL) THROW(PERR_MissingOperand);
	default: arg1 = PopStack(values); if (arg1 == NULL) THROW(PERR_MissingOperand);
	}
	free(PopStack(oper));

	if (eval == False) // Short circuit
	{
		PushStack(values, arg1); // Push a dummy value
		arg1 = NULL;
		THROW(0);
	}

	switch (ope - OperatorList) {
	case 0: // Unary -
		AffectArg(arg1, cb, data);
		switch (arg1->type) {
		case TYPE_INT: arg1->value.integer = - arg1->value.integer; break;
		case TYPE_DBL: arg1->value.real    = - arg1->value.real;
		}
		PushStack(values, arg1);
		break;

#define	MAKE_OP(operator) \
		AffectArg(arg1, cb, data); \
		switch (arg1->type) { \
		case TYPE_INT: arg1->value.integer = operator arg1->value.integer; break; \
		case TYPE_DBL: arg1->value.integer = operator (ULONG) arg1->value.real; \
		               arg1->type = TYPE_INT; \
		} \
		PushStack(values, arg1)

	case 1: // Unary ~
		MAKE_OP(~);
		break;
	case 2: // Unary !
		MAKE_OP(!);
		break;

#undef	MAKE_OP
#define	MAKE_OP(operator, operator2) \
		AffectArg(arg1, cb, data); \
		switch (arg1->type) { \
		case TYPE_INT: \
			if (arg2->type == TYPE_INT) arg1->value.integer operator arg2->value.integer; \
			else arg1->value.real = arg1->value.integer operator2 arg2->value.real, arg1->type = TYPE_DBL; \
			break; \
		case TYPE_DBL: \
			if (arg2->type == TYPE_INT) arg1->value.real operator arg2->value.integer; \
			else arg1->value.real operator arg2->value.real; \
		} \
		free(arg2); \
		PushStack(values, arg1)

	case 5: // Binary *
		AffectArg(arg2, cb, data);
		MAKE_OP(*=, *);
		break;
	case 6: // Division /
		AffectArg(arg2, cb, data);
		if (IsNull(arg2)) THROW(PERR_DivisionByZero);
		MAKE_OP(/=, /);
		break;
	case 7: // Modulus %
		AffectArg(arg2, cb, data);
		if (IsNull(arg2)) THROW(PERR_DivisionByZero);
		AffectArg(arg1, cb, data);
		if (arg2->type == TYPE_DBL) arg2->value.integer = (long) arg2->value.real;
		if (arg1->type == TYPE_DBL) arg1->value.integer = (long) arg1->value.real;
		arg1->value.integer %= arg1->value.integer;
		arg1->type = TYPE_INT;
		free(arg2);
		PushStack(values, arg1);
		break;
	case 8: // Addition +
		AffectArg(arg2, cb, data);
		MAKE_OP(+=, +);
		break;
	case 9: // Substraction -
		AffectArg(arg2, cb, data);
		MAKE_OP(-=, -);
		break;

#undef	MAKE_OP
#define	MAKE_OP(operator1, operator2) \
		AffectArg(arg1, cb, data); \
		AffectArg(arg2, cb, data); \
		switch (arg2->type) { \
		case TYPE_INT: nb = arg2->value.integer; break; \
		case TYPE_DBL: nb = (long) arg2->value.real; break; \
		default:       nb = 0; \
		} \
\
		switch (arg1->type) { \
		case TYPE_INT: arg1->value.integer operator1 nb; break; \
		case TYPE_DBL: arg1->value.integer = (long) arg1->value.real operator2 nb; \
		               arg1->type = TYPE_INT; break; \
		} \
		free(arg2); \
		PushStack(values, arg1)

	case 10: // Left bit shifting
		MAKE_OP(<<=, <<);
		break;
	case 11: // Right bit shifting
		MAKE_OP(>>=, >>);
		break;
	case 18: // Binary and
		MAKE_OP(&=, &);
		break;
	case 19: // Binary xor
		MAKE_OP(^=, ^);
		break;
	case 20: // Binary or
		MAKE_OP(|=, |);
		break;

#undef	MAKE_OP
#define	MAKE_OP(operator) \
		AffectArg(arg1, cb, data); \
		AffectArg(arg2, cb, data); \
		switch (arg1->type) { \
		case TYPE_INT: \
			switch (arg2->type) { \
			case TYPE_INT: arg1->value.integer = (arg1->value.integer operator arg2->value.integer); break; \
			case TYPE_DBL: arg1->value.integer = ((double) arg1->value.integer operator arg2->value.real); break; \
			} \
			break; \
		case TYPE_DBL: \
			switch (arg2->type) { \
			case TYPE_INT: arg1->value.integer = (arg1->value.real operator (double) arg2->value.integer); break; \
			case TYPE_DBL: arg1->value.integer = (arg1->value.real operator arg2->value.real); break; \
			} \
			arg1->type = TYPE_INT; \
		} \
		free(arg2); \
		PushStack(values, arg1)

	case 12:
		MAKE_OP(<);
		break;
	case 13:
		MAKE_OP(>);
		break;
	case 14:
		MAKE_OP(<=);
		break;
	case 15:
		MAKE_OP(>=);
		break;
	case 16:
		MAKE_OP(==);
		break;
	case 17:
		MAKE_OP(!=);
		break;

#undef MAKE_OP

	case 21: arg1->value.integer = ! IsNull(arg1) && ! IsNull(arg2); goto case_XY; // Logical and
	case 22: arg1->value.integer = ! IsNull(arg1) || ! IsNull(arg2); // Logical or
	case_XY:
		arg1->type = TYPE_INT;
		PushStack(values, arg1);
		free(arg2);
		break;
	case 23: // ? :
		if (IsNull(arg1)) memcpy(arg1, arg3, sizeof *arg1);
		else              memcpy(arg1, arg2, sizeof *arg1);
		free(arg2); free(arg3);
		PushStack(values, arg1);
		break;

	case 25: // Assignment =
		if (arg1->type != TYPE_IDF) THROW(PERR_LValueNotModifiable);
		AffectArg(arg2, cb, data);
		v.type = arg2->type;
		switch (arg2->type) {
		case TYPE_INT: v.v.z = arg2->value.integer; break;
		case TYPE_DBL: v.v.r = arg2->value.real; break;
		}
		cb(arg1->value.ident, &v, 1, data);
		memcpy(arg1, arg2, sizeof *arg1); free(arg2);
		PushStack(values, arg1);
		break;
	case 3: // ++
	case 4: // --
		arg2 = NewNumber(TYPE_INT, 1);
		// no break;
	case 26: case 27: case 28: case 29: case 30:
	case 31: case 32: case 33: case 34: case 35:
		nb = ope - OperatorList;
		arg3 = calloc(sizeof *arg3, 1);
		memcpy(arg3, arg1, sizeof *arg3);
		PushStack(values, arg1);
		PushStack(values, arg3);
		PushStack(values, arg2);
		PushStack(oper, NewOperator(OperatorList + 25));
		PushStack(oper, NewOperator(OperatorList + nb - (nb < 24 ? -5 : nb < 31 ? 21 : 15)));
		MakeOp(values, oper, cb, data);
		error = MakeOp(values, oper, cb, data);
	}
	return 0;

	error_case:
	if (arg1) free(arg1);
	if (arg2) free(arg2);
	if (arg3) free(arg3);
	return error;
}

/**f* Calc/ParseExpression
 * NAME
 *	ParseExpression - Parse C-like expression
 *
 * SYNOPSIS
 *	int ParseExpression(STRPTR exp, ParseExpCb cb, APTR data);
 *
 * FUNCTION
 *	This function will evaluate given string 'exp' as a C-like expression. See
 *	introduction for supported features.
 *
 *	Note that error management is somewhat minimalist.
 *
 * INPUT
 *	- exp  : expression to parse, string will not be modified.
 *	- cb   : callback used to get/set custom variable and to receive result.
 *	         Expected prototype for callback is:
 *
 *	         void cb(STRPTR var, Variant v, int store, APTR data);
 *
 *	         var   : name of variable to set/get. if NULL, result is passed in v.
 *	         v     : value of variable to set (if store == 1) or get (if store == 0).
 *	         store : whether callback should set or retrieve value of named variable.
 *	         data  : custom value given to ParseExpression.
 *
 *	- data : custom value passed to callback.
 *
 * NOTE
 *	Variant has 3 meaningful fields :
 *	- v->type : must be set (or is set) to eitheir TYPE_INT or TYPE_DBL.
 *	- v->v.z  : if v->type == TYPE_INT, then you can use this field to get/set value.
 *	- v->v.r  : if v->type == TYPE_DBL, use this field.
 *
 * RESULT
 *	0 if no errors were found while parsing, PERR_* otherwise.
 */
DLLIMP int ParseExpression(STRPTR exp, ParseExpCb cb, APTR data)
{
	Operator ope;
	STRPTR   next;
	int      curpri, pri, error, tok;
	Stack    values, oper, object;

	for (curpri = error = tok = 0, values = oper = NULL, next = exp; error == 0 && *exp; exp = next)
	{
		switch (GetToken(&object, &next)) {
		case TOKEN_NUMBER: // Number => stack it
			if (tok == TOKEN_NUMBER) error = PERR_SyntaxError;
			tok = TOKEN_NUMBER;
			PushStack(&values, object);
			break;
		case TOKEN_INCPRI:
			if (tok == TOKEN_NUMBER) error = PERR_SyntaxError;
			curpri += 30;
			break;
		case TOKEN_DECPRI:
			if (tok == TOKEN_OPERATOR) error = PERR_SyntaxError;
			curpri -= 30;
			if (curpri < 0) error = PERR_TooManyClosingParens;
			break;
		case TOKEN_OPERATOR: // Operator
			ope = object->value.ope;
			pri = curpri + ope->priority - (ope->associativity == LEFT);

			if (ope == OperatorList && tok == TOKEN_NUMBER)
				ope = object->value.ope = OperatorList + 9, pri -= 3; // Binary '-' instead

			while (error == 0 && oper && pri < oper->type)
				error = MakeOp(&values, &oper, cb, data);

			tok = TOKEN_OPERATOR;
			object->type = curpri + ope->priority;

			if (ope == OperatorList + 24) // c clause of a?b:c
			{
				if (oper->value.ope != OperatorList + 23) // Misplace :
					error = PERR_SyntaxError;
				else
					oper->doeval = ! oper->doeval;
				free(object);
				break;
			}
			else if (ope == OperatorList + 23 || ope == OperatorList + 21) // b clause of a?b:c or b clause of a&&b
			{
				object->doeval = ! IsNull(values);
			}
			else if (ope == OperatorList + 22) // b clause of a||b
			{
				object->doeval = IsNull(values);
			}
			PushStack(&oper, object);
			break;
		default:
			error = PERR_SyntaxError;
		}
	}
	while (error == 0 && oper)
		error = MakeOp(&values, &oper, cb, data);

	if (error == 0 && values)
	{
		// Final result
		VariantBuf v;
		v.type = values->type;
		switch (v.type) {
		case TYPE_INT: v.v.z = values->value.integer; break;
		case TYPE_DBL: v.v.r = values->value.real;
		}
		cb(NULL, &v, 0, data);
	}
	while (oper)   free(PopStack(&oper));
	while (values) free(PopStack(&values));
	return error;
}

