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
// File name:   sv_net_chan.cpp
// Version:     v1.00
// Created:
// Compilers:   Visual Studio 2015
// Description:
// -------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////

#ifdef DEDICATED
#include <null/null_precompiled.h>
#else
#include <OWLib/precompiled.h>
#endif

/*
==============
SV_Netchan_Encode

// first four bytes of the data are always:
S64 reliableAcknowledge;
==============
*/
static void SV_Netchan_Encode( client_t* client, msg_t* msg, UTF8* commandString )
{
    S64    reliableAcknowledge, i, index;
    U8    key, *string;
    S32     srdc, sbit;
    bool    soob;
    
    if( msg->cursize < SV_ENCODE_START )
    {
        return;
    }
    
    // NOTE: saving pos, reading reliableAck, restoring, not using it .. useless?
    srdc = msg->readcount;
    sbit = msg->bit;
    soob = msg->oob;
    
    msg->bit = 0;
    msg->readcount = 0;
    msg->oob = false;
    
    reliableAcknowledge = MSG_ReadLong( msg );
    
    msg->oob = soob;
    msg->bit = sbit;
    msg->readcount = srdc;
    
    string = ( U8* ) commandString;
    index = 0;
    // xor the client challenge with the netchan sequence number
    key = client->challenge ^ client->netchan.outgoingSequence;
    for( i = SV_ENCODE_START; i < msg->cursize; i++ )
    {
        // modify the key with the last received and with this message acknowledged client command
        if( !string[index] )
        {
            index = 0;
        }
        if( string[index] > 127 || string[index] == '%' )
        {
            key ^= '.' << ( i & 1 );
        }
        else
        {
            key ^= string[index] << ( i & 1 );
        }
        index++;
        // encode the data with this key
        *( msg->data + i ) = *( msg->data + i ) ^ key;
    }
}

/*
==============
SV_Netchan_Decode

// first 12 bytes of the data are always:
S64 serverId;
S64 messageAcknowledge;
S64 reliableAcknowledge;
==============
*/
static void SV_Netchan_Decode( client_t* client, msg_t* msg )
{
    S32     serverId, messageAcknowledge, reliableAcknowledge, i, index, srdc, sbit;
    bool    soob;
    U8    key, *string;
    
    srdc = msg->readcount;
    sbit = msg->bit;
    soob = msg->oob;
    
    msg->oob = false;
    
    serverId = MSG_ReadLong( msg );
    messageAcknowledge = MSG_ReadLong( msg );
    reliableAcknowledge = MSG_ReadLong( msg );
    
    msg->oob = soob;
    msg->bit = sbit;
    msg->readcount = srdc;
    
    string = ( U8* ) client->reliableCommands[reliableAcknowledge & ( MAX_RELIABLE_COMMANDS - 1 )];
    index = 0;
    //
    key = client->challenge ^ serverId ^ messageAcknowledge;
    for( i = msg->readcount + SV_DECODE_START; i < msg->cursize; i++ )
    {
        // modify the key with the last sent and acknowledged server command
        if( !string[index] )
        {
            index = 0;
        }
        if( string[index] > 127 || string[index] == '%' )
        {
            key ^= '.' << ( i & 1 );
        }
        else
        {
            key ^= string[index] << ( i & 1 );
        }
        index++;
        // decode the data with this key
        *( msg->data + i ) = *( msg->data + i ) ^ key;
    }
}

/*
=================
SV_Netchan_FreeQueue
=================
*/
void SV_Netchan_FreeQueue( client_t* client )
{
    netchan_buffer_t* netbuf, *next;
    
    for( netbuf = client->netchan_start_queue; netbuf; netbuf = next )
    {
        next = netbuf->next;
        Z_Free( netbuf );
    }
    
    client->netchan_start_queue = NULL;
    client->netchan_end_queue = client->netchan_start_queue;
}

/*
=================
SV_Netchan_TransmitNextFragment
=================
*/
void SV_Netchan_TransmitNextFragment( client_t* client )
{
    Netchan_TransmitNextFragment( &client->netchan );
    while( !client->netchan.unsentFragments && client->netchan_start_queue )
    {
        // make sure the netchan queue has been properly initialized (you never know)
        //% if (!client->netchan_end_queue) {
        //%     Com_Error(ERR_DROP, "netchan queue is not properly initialized in SV_Netchan_TransmitNextFragment\n");
        //% }
        // the last fragment was transmitted, check wether we have queued messages
        netchan_buffer_t* netbuf = client->netchan_start_queue;
        
        // pop from queue
        client->netchan_start_queue = netbuf->next;
        if( !client->netchan_start_queue )
        {
            client->netchan_end_queue = NULL;
        }
        
        if( !SV_GameIsSinglePlayer() )
        {
            SV_Netchan_Encode( client, &netbuf->msg, netbuf->lastClientCommandString );
        }
        Netchan_Transmit( &client->netchan, netbuf->msg.cursize, netbuf->msg.data );
        
        Z_Free( netbuf );
    }
}

/*
===============
SV_Netchan_Transmit

TTimo
show_bug.cgi?id=462
if there are some unsent fragments (which may happen if the snapshots
and the gamestate are fragmenting, and collide on send for instance)
then buffer them and make sure they get sent in correct order
================
*/
void SV_Netchan_Transmit( client_t* client, msg_t* msg )
{
    MSG_WriteByte( msg, svc_EOF );
    
    if( client->netchan.unsentFragments )
    {
        netchan_buffer_t* netbuf;
        
        //Com_DPrintf("SV_Netchan_Transmit: there are unsent fragments remaining\n");
        netbuf = ( netchan_buffer_t* ) Z_Malloc( sizeof( netchan_buffer_t ) );
        
        // store the msg, we can't store it encoded, as the encoding depends on stuff we still have to finish sending
        MSG_Copy( &netbuf->msg, netbuf->msgBuffer, sizeof( netbuf->msgBuffer ), msg );
        
        // copy the command, since the command number used for encryption is
        // already compressed in the buffer, and receiving a new command would
        // otherwise lose the proper encryption key
        strcpy( netbuf->lastClientCommandString, client->lastClientCommandString );
        
        // insert it in the queue, the message will be encoded and sent later
        //% *client->netchan_end_queue = netbuf;
        //% client->netchan_end_queue = &(*client->netchan_end_queue)->next;
        netbuf->next = NULL;
        if( !client->netchan_start_queue )
        {
            client->netchan_start_queue = netbuf;
        }
        else
        {
            client->netchan_end_queue->next = netbuf;
        }
        client->netchan_end_queue = netbuf;
        
        // emit the next fragment of the current message for now
        Netchan_TransmitNextFragment( &client->netchan );
    }
    else
    {
        if( !SV_GameIsSinglePlayer() )
        {
            SV_Netchan_Encode( client, msg, client->lastClientCommandString );
        }
        Netchan_Transmit( &client->netchan, msg->cursize, msg->data );
    }
}

/*
=================
Netchan_SV_Process
=================
*/
bool SV_Netchan_Process( client_t* client, msg_t* msg )
{
    S32 ret;
    
    ret = Netchan_Process( &client->netchan, msg );
    if( !ret )
    {
        return false;
    }
    if( !SV_GameIsSinglePlayer() )
    {
        SV_Netchan_Decode( client, msg );
    }
    return true;
}
