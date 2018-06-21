/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of OpenWOlf.

OpenWOlf is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

OpenWOlf is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OpenWolf; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

#ifndef __AI_CHAR_H__
#define __AI_CHAR_H__

S32 BotChat_EnterGame( bot_state_t* bs );
S32 BotChat_ExitGame( bot_state_t* bs );
S32 BotChat_StartLevel( bot_state_t* bs );
S32 BotChat_EndLevel( bot_state_t* bs );
S32 BotChat_HitTalking( bot_state_t* bs );
S32 BotChat_HitNoDeath( bot_state_t* bs );
S32 BotChat_HitNoKill( bot_state_t* bs );
S32 BotChat_Death( bot_state_t* bs );
S32 BotChat_Kill( bot_state_t* bs );
S32 BotChat_EnemySuicide( bot_state_t* bs );
S32 BotChat_Random( bot_state_t* bs );
// time the selected chat takes to type in
F32 BotChatTime( bot_state_t* bs );
// returns true if the bot can chat at the current position
S32 BotValidChatPosition( bot_state_t* bs );
// test the initial bot chats
void BotChatTest( bot_state_t* bs );

#endif //!__AI_CHAR_H__