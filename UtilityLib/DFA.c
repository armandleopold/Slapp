/*
 * DFA.c : very simple deterministic finate automata library
 *
 * Written by T.Pierron, June 2006.
 */


#include "SWL.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Datatypes are private */
typedef struct DFA_State_t *    DFA_State;

struct DFA_State_t
{
	uint16_t ds_Next;
	uint16_t ds_Down;
	TEXT     ds_Transition;
	long     ds_Value;
};

struct DFA_t
{
	DFA                da_Next;
	int                da_Usage;
	int                da_Size;
	struct DFA_State_t da_States[0];
};

DLLIMP DFA DFA_Create(void)
{
	DFA dfa = malloc(sizeof *dfa + 1000 * sizeof *dfa->da_States);

	if (dfa)
	{
		dfa->da_Usage = 0;
		dfa->da_Size  = 1000;
		dfa->da_Next  = NULL;
	}
	return dfa;
}

DLLIMP long DFA_Search(DFA dfa, STRPTR key, long insert)
{
	DFA_State s = NULL;
	DFA_State prev = NULL;
	DFA       d;
	Bool      down = False;
	int       i;

	for (d = dfa, s = d->da_States, i = 0; *key; prev = s)
	{
		down = True;
		if (i == d->da_Usage)
		{
			i = 0xffff;
		}
		else if (tolower(*key) == tolower(s->ds_Transition))
		{
			i = s->ds_Down;
			key ++;
			if (*key == 0)
			{
				if (insert) s->ds_Value = insert;
				return s->ds_Value;
			}
		}
		else prev = s, i = s->ds_Next, down = False;
		if (i == 0xffff)
		{
			if (insert)
			{
				for (d = dfa, i = 0; d->da_Next; i += d->da_Usage, d = d->da_Next);
				if (d->da_Usage == d->da_Size)
				{
					d->da_Next = DFA_Create();
					i += d->da_Usage;
					d = d->da_Next;
					s = d->da_States;
				}
				else s = d->da_States + d->da_Usage;
				s->ds_Next = 0xffff;
				s->ds_Down = 0xffff;
				s->ds_Transition = *key++;
				s->ds_Value = (*key == 0 ? insert : 0L);
				if (prev) {
					if (down) prev->ds_Down = i + d->da_Usage;
					else      prev->ds_Next = i + d->da_Usage;
				}
				down = True;
				d->da_Usage ++;
				i = d->da_Usage;
			}
			else return 0L;
		}
		else
		{
			for (d = dfa; i >= d->da_Usage; i -= dfa->da_Usage, d = d->da_Next);
			s = d->da_States + i;
		}
	}
	return 0L;
}


DLLIMP void DFA_Free(DFA dfa)
{
	DFA next;

	for ( ; dfa; dfa = next)
	{
		next = dfa->da_Next;
		free(dfa);
	}
}
