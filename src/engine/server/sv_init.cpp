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
// File name:   sv_init.cpp
// Version:     v1.00
// Created:
// Compilers:   Visual Studio 2015
// Description:
// -------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////

#include <server/server.h>

/*
===============
SV_SendConfigstring

Creates and sends the server command necessary to update the CS index for the
given client
===============
*/
static void SV_SendConfigstring( client_t* client, S32 index )
{
    S32 maxChunkSize = MAX_STRING_CHARS - 24, len;
    
    if( sv.configstrings[index].restricted && Com_ClientListContains(
                &sv.configstrings[index].clientList, client - svs.clients ) )
    {
        // Send a blank config string for this client if it's listed
        SV_SendServerCommand( client, "cs %i \"\"\n", index );
        return;
    }
    
    len = strlen( sv.configstrings[index].s );
    
    if( len >= maxChunkSize )
    {
        S32		sent = 0, remaining = len;
        UTF8*	cmd, buf[MAX_STRING_CHARS];
        
        while( remaining > 0 )
        {
            if( sent == 0 )
            {
                cmd = "bcs0";
            }
            else if( remaining < maxChunkSize )
            {
                cmd = "bcs2";
            }
            else
            {
                cmd = "bcs1";
            }
            Q_strncpyz( buf, &sv.configstrings[index].s[sent], maxChunkSize );
            
            SV_SendServerCommand( client, "%s %i \"%s\"\n", cmd, index, buf );
            
            sent += ( maxChunkSize - 1 );
            remaining -= ( maxChunkSize - 1 );
        }
    }
    else
    {
        // standard cs, just send it
        SV_SendServerCommand( client, "cs %i \"%s\"\n", index,
                              sv.configstrings[index].s );
    }
}

/*
===============
SV_UpdateConfigStrings
===============
*/
void SV_UpdateConfigStrings( void )
{
    S32             len, i, index, maxChunkSize = MAX_STRING_CHARS - 24;
    client_t*       client;
    
    for( index = 0; index < MAX_CONFIGSTRINGS; index++ )
    {
        if( !sv.configstringsmodified[index] )
        {
            continue;
        }
        sv.configstringsmodified[index] = false;
        
        // send it to all the clients if we aren't
        // spawning a new server
        if( sv.state == SS_GAME || sv.restarting )
        {
            // send the data to all relevent clients
            for( i = 0, client = svs.clients; i < sv_maxclients->integer; i++, client++ )
            {
                if( client->state < CS_PRIMED )
                {
                    continue;
                }
                // do not always send server info to all clients
                if( index == CS_SERVERINFO && client->gentity && ( client->gentity->r.svFlags & SVF_NOSERVERINFO ) )
                {
                    continue;
                }
                
                // RF, don't send to bot/AI
                // Gordon: Note: might want to re-enable later for bot support
                // RF, re-enabled
                // Arnout: removed hardcoded gametype
                // Arnout: added coop
                if( ( SV_GameIsSinglePlayer() || SV_GameIsCoop() ) && client->gentity && ( client->gentity->r.svFlags & SVF_BOT ) )
                {
                    continue;
                }
                
                len = strlen( sv.configstrings[index].s );
                if( len >= maxChunkSize )
                {
                    S32             sent = 0, remaining = len;
                    UTF8*           cmd, buf[MAX_STRING_CHARS];
                    
                    while( remaining > 0 )
                    {
                        if( sent == 0 )
                        {
                            cmd = "bcs0";
                        }
                        else if( remaining < maxChunkSize )
                        {
                            cmd = "bcs2";
                        }
                        else
                        {
                            cmd = "bcs1";
                        }
                        Q_strncpyz( buf, &sv.configstrings[index].s[sent], maxChunkSize );
                        
                        SV_SendServerCommand( client, "%s %i \"%s\"\n", cmd, index, buf );
                        
                        sent += ( maxChunkSize - 1 );
                        remaining -= ( maxChunkSize - 1 );
                    }
                }
                else
                {
                    // standard cs, just send it
                    SV_SendServerCommand( client, "cs %i \"%s\"\n", index, sv.configstrings[index] );
                }
            }
        }
    }
}

/*
===============
SV_SetConfigstringNoUpdate
===============
*/
void SV_SetConfigstringNoUpdate( S32 index, const UTF8* val )
{
    if( index < 0 || index >= MAX_CONFIGSTRINGS )
    {
        Com_Error( ERR_DROP, "SV_SetConfigstring: bad index %i\n", index );
    }
    
    if( !val )
    {
        val = "";
    }
    
    // don't bother broadcasting an update if no change
    if( !strcmp( val, sv.configstrings[index].s ) )
    {
        return;
    }
    
    // change the string in sv
    Z_Free( sv.configstrings[index].s );
    sv.configstrings[index].s = CopyString( val );
}

/*
===============
SV_SetConfigstring
===============
*/
void SV_SetConfigstring( S32 index, const UTF8* val )
{
    S32			i;
    client_t*	client;
    
    if( index < 0 || index >= MAX_CONFIGSTRINGS )
    {
        Com_Error( ERR_DROP, "SV_SetConfigstring: bad index %i\n", index );
    }
    
    if( !val )
    {
        val = "";
    }
    
    // don't bother broadcasting an update if no change
    if( !strcmp( val, sv.configstrings[ index ].s ) )
    {
        return;
    }
    
    // change the string in sv
    Z_Free( sv.configstrings[index].s );
    sv.configstrings[index].s = CopyString( val );
    
    // send it to all the clients if we aren't
    // spawning a new server
    if( sv.state == SS_GAME || sv.restarting )
    {
        // send the data to all relevent clients
        for( i = 0, client = svs.clients; i < sv_maxclients->integer ; i++, client++ )
        {
            if( client->state < CS_ACTIVE )
            {
                if( client->state == CS_PRIMED )
                    client->csUpdated[ index ] = true;
                continue;
            }
            // do not always send server info to all clients
            if( index == CS_SERVERINFO && client->gentity && ( client->gentity->r.svFlags & SVF_NOSERVERINFO ) )
            {
                continue;
            }
            
            SV_SendConfigstring( client, index );
        }
    }
}

/*
===============
SV_GetConfigstring
===============
*/
void SV_GetConfigstring( S32 index, UTF8* buffer, S32 bufferSize )
{
    if( bufferSize < 1 )
    {
        Com_Error( ERR_DROP, "SV_GetConfigstring: bufferSize == %i", bufferSize );
    }
    if( index < 0 || index >= MAX_CONFIGSTRINGS )
    {
        Com_Error( ERR_DROP, "SV_GetConfigstring: bad index %i\n", index );
    }
    if( !sv.configstrings[index].s )
    {
        buffer[0] = 0;
        return;
    }
    
    Q_strncpyz( buffer, sv.configstrings[index].s, bufferSize );
}

/*
===============
SV_SetConfigstringRestrictions
===============
*/
void SV_SetConfigstringRestrictions( S32 index, const clientList_t* clientList )
{
    S32				i;
    clientList_t	oldClientList = sv.configstrings[index].clientList;
    
    sv.configstrings[index].clientList = *clientList;
    sv.configstrings[index].restricted = true;
    
    for( i = 0 ; i < sv_maxclients->integer ; i++ )
    {
        if( svs.clients[i].state >= CS_CONNECTED )
        {
            if( Com_ClientListContains( &oldClientList, i ) !=
                    Com_ClientListContains( clientList, i ) )
            {
                // A client has left or joined the restricted list, so update
                SV_SendConfigstring( &svs.clients[i], index );
            }
        }
    }
}

/*
===============
SV_SetUserinfo
===============
*/
void SV_SetUserinfo( S32 index, const UTF8* val )
{
    if( index < 0 || index >= sv_maxclients->integer )
    {
        Com_Error( ERR_DROP, "SV_SetUserinfo: bad index %i\n", index );
    }
    
    if( !val )
    {
        val = "";
    }
    
    Q_strncpyz( svs.clients[index].userinfo, val, sizeof( svs.clients[index].userinfo ) );
    Q_strncpyz( svs.clients[index].name, Info_ValueForKey( val, "name" ), sizeof( svs.clients[index].name ) );
}

/*
===============
SV_GetUserinfo
===============
*/
void SV_GetUserinfo( S32 index, UTF8* buffer, S32 bufferSize )
{
    if( bufferSize < 1 )
    {
        Com_Error( ERR_DROP, "SV_GetUserinfo: bufferSize == %i", bufferSize );
    }
    if( index < 0 || index >= sv_maxclients->integer )
    {
        Com_Error( ERR_DROP, "SV_GetUserinfo: bad index %i\n", index );
    }
    Q_strncpyz( buffer, svs.clients[index].userinfo, bufferSize );
}


/*
================
SV_CreateBaseline

Entity baselines are used to compress non-delta messages
to the clients -- only the fields that differ from the
baseline will be transmitted
================
*/
void SV_CreateBaseline( void )
{
    sharedEntity_t* svent;
    S32             entnum;
    
    for( entnum = 1; entnum < sv.num_entities; entnum++ )
    {
        svent = SV_GentityNum( entnum );
        if( !svent->r.linked )
        {
            continue;
        }
        svent->s.number = entnum;
        
        //
        // take current state as baseline
        //
        sv.svEntities[entnum].baseline = svent->s;
    }
}

/*
===============
SV_BoundMaxClients
===============
*/
void SV_BoundMaxClients( S32 minimum )
{
    // get the current maxclients value
    Cvar_Get( "sv_maxclients", "20", 0 );
    
    // START    xkan, 10/03/2002
    // allow many bots in single player. note that this pretty much means all previous
    // settings will be ignored (including the one set through "seta sv_maxclients <num>"
    // in user profile's wolfconfig_mp.cfg). also that if the user subsequently start
    // the server in multiplayer mode, the number of clients will still be the number
    // set here, which may be wrong - we can certainly just set it to a sensible number
    // when it is not in single player mode in the else part of the if statement when
    // necessary
    if( SV_GameIsSinglePlayer() || SV_GameIsCoop() )
    {
        Cvar_Set( "sv_maxclients", "64" );
    }
    // END      xkan, 10/03/2002
    
    sv_maxclients->modified = false;
    
    if( sv_maxclients->integer < minimum )
    {
        Cvar_Set( "sv_maxclients", va( "%i", minimum ) );
    }
    else if( sv_maxclients->integer > MAX_CLIENTS )
    {
        Cvar_Set( "sv_maxclients", va( "%i", MAX_CLIENTS ) );
    }
}


/*
===============
SV_Startup

Called when a host starts a map when it wasn't running
one before.  Successive map or map_restart commands will
NOT cause this to be called, unless the game is exited to
the menu system first.
===============
*/
void SV_Startup( void )
{
    if( svs.initialized )
    {
        Com_Error( ERR_FATAL, "SV_Startup: svs.initialized" );
    }
    SV_BoundMaxClients( 1 );
    
    // RF, avoid trying to allocate large chunk on a fragmented zone
    svs.clients = ( client_t* )calloc( sizeof( client_t ) * sv_maxclients->integer, 1 );
    if( !svs.clients )
    {
        Com_Error( ERR_FATAL, "SV_Startup: unable to allocate svs.clients" );
    }
    
    if( com_dedicated->integer )
    {
        svs.numSnapshotEntities = sv_maxclients->integer * PACKET_BACKUP * 64;
    }
    else
    {
        // we don't need nearly as many when playing locally
        svs.numSnapshotEntities = sv_maxclients->integer * 4 * 64;
    }
    svs.initialized = true;
    
    Cvar_Set( "sv_running", "1" );
    
    // Join the ipv6 multicast group now that a map is running so clients can scan for us on the local network.
    NET_JoinMulticast6();
}

/*
==================
SV_ChangeMaxClients
==================
*/
void SV_ChangeMaxClients( void )
{
    S32             oldMaxClients, i, count;
    client_t*       oldClients;
    
    // get the highest client number in use
    count = 0;
    for( i = 0; i < sv_maxclients->integer; i++ )
    {
        if( svs.clients[i].state >= CS_CONNECTED )
        {
            if( i > count )
            {
                count = i;
            }
        }
    }
    count++;
    
    oldMaxClients = sv_maxclients->integer;
    // never go below the highest client number in use
    SV_BoundMaxClients( count );
    // if still the same
    if( sv_maxclients->integer == oldMaxClients )
    {
        return;
    }
    
    oldClients = ( client_t* )Hunk_AllocateTempMemory( count * sizeof( client_t ) );
    // copy the clients to hunk memory
    for( i = 0; i < count; i++ )
    {
        if( svs.clients[i].state >= CS_CONNECTED )
        {
            oldClients[i] = svs.clients[i];
        }
        else
        {
            ::memset( &oldClients[i], 0, sizeof( client_t ) );
        }
    }
    
    // free old clients arrays
    //Z_Free( svs.clients );
    free( svs.clients );			// RF, avoid trying to allocate large chunk on a fragmented zone
    
    // allocate new clients
    // RF, avoid trying to allocate large chunk on a fragmented zone
    svs.clients = ( client_t* )calloc( sizeof( client_t ) * sv_maxclients->integer, 1 );
    if( !svs.clients )
    {
        Com_Error( ERR_FATAL, "SV_Startup: unable to allocate svs.clients" );
    }
    
    ::memset( svs.clients, 0, sv_maxclients->integer * sizeof( client_t ) );
    
    // copy the clients over
    for( i = 0; i < count; i++ )
    {
        if( oldClients[i].state >= CS_CONNECTED )
        {
            svs.clients[i] = oldClients[i];
        }
    }
    
    // free the old clients on the hunk
    Hunk_FreeTempMemory( oldClients );
    
    // allocate new snapshot entities
    if( com_dedicated->integer )
    {
        svs.numSnapshotEntities = sv_maxclients->integer * PACKET_BACKUP * 64;
    }
    else
    {
        // we don't need nearly as many when playing locally
        svs.numSnapshotEntities = sv_maxclients->integer * 4 * 64;
    }
}


/*
====================
SV_SetExpectedHunkUsage

  Sets com_expectedhunkusage, so the client knows how to draw the percentage bar
====================
*/
void SV_SetExpectedHunkUsage( UTF8* mapname )
{
    S32             handle, len;
    UTF8*           memlistfile = "hunkusage.dat", *buf, *buftrav, *token;
    
    len = FS_FOpenFileByMode( memlistfile, &handle, FS_READ );
    if( len >= 0 ) // the file exists, so read it in, strip out the current entry for this map, and save it out, so we can append the new value
    {
        buf = ( UTF8* )Z_Malloc( len + 1 );
        memset( buf, 0, len + 1 );
        
        FS_Read( ( void* )buf, len, handle );
        FS_FCloseFile( handle );
        
        // now parse the file, filtering out the current map
        buftrav = buf;
        while( ( token = COM_Parse( &buftrav ) ) != NULL && token[0] )
        {
            if( !Q_stricmp( token, mapname ) )
            {
                // found a match
                token = COM_Parse( &buftrav );	// read the size
                if( token && token[0] )
                {
                    // this is the usage
                    com_expectedhunkusage = atoi( token );
                    Z_Free( buf );
                    return;
                }
            }
        }
        
        Z_Free( buf );
    }
    
    // just set it to a negative number,so the cgame knows not to draw the percent bar
    com_expectedhunkusage = -1;
}

/*
================
SV_ClearServer
================
*/
void SV_ClearServer( void )
{
    S32 i;
    
    for( i = 0; i < MAX_CONFIGSTRINGS; i++ )
    {
        if( sv.configstrings[i].s )
        {
            Z_Free( sv.configstrings[i].s );
        }
    }
    ::memset( &sv, 0, sizeof( sv ) );
}

/*
================
SV_TouchCGame

touch cgame so that a pure client can load it if it's in a seperate pk3
================
*/
void SV_TouchCGame( void )
{
    fileHandle_t f;
    
    FS_FOpenFileRead( "vm/cgame.qvm", &f, false );
    if( f )
    {
        FS_FCloseFile( f );
    }
}

/*
================
SV_TouchCGameDLL
  touch the cgame DLL so that a pure client (with DLL sv_pure support) can load do the correct checks
================
*/
void SV_TouchCGameDLL( void )
{
    fileHandle_t    f;
    UTF8*           filename;
    
    filename = Sys_GetDLLName( "cgame" );
    FS_FOpenFileRead_Filtered( filename, &f, false, FS_EXCLUDE_DIR );
    if( f )
    {
        FS_FCloseFile( f );
    }
    else if( sv_pure->integer )   // ydnar: so we can work the damn game
    {
        Com_Error( ERR_DROP, "Failed to locate cgame DLL for pure server mode" );
    }
}

/*
================
SV_SpawnServer

Change the server to a new map, taking all connected
clients along with it.
This is NOT called for map_restart
================
*/
void SV_SpawnServer( UTF8* server, bool killBots )
{
    S32             i, checksum;
    bool        isBot;
    const UTF8*     p;
    
    svs.queryDone = 0;
    
    // ydnar: broadcast a level change to all connected clients
    if( svs.clients && !com_errorEntered )
    {
        SV_FinalCommand( "spawnserver", false );
    }
    
#if 0 //defined(USE_HTTP)
    // Dushan - Do not allow users who are not logged in
    if( Cvar_VariableIntegerValue( "ui_logged_in" ) != 1 )
    {
        Com_Error( ERR_DROP, "You are not logged in\n" );
        return;
    }
#endif
    
    // shut down the existing game if it is running
    SV_ShutdownGameProgs();
    
    Com_Printf( "------ Server Initialization ------\n" );
    Com_Printf( "Server: %s\n", server );
    
    // if not running a dedicated server CL_MapLoading will connect the client to the server
    // also print some status stuff
    CL_MapLoading();
    
    // make sure all the client stuff is unloaded
    CL_ShutdownAll();
    
    // clear the whole hunk because we're (re)loading the server
    Hunk_Clear();
    
    // clear collision map data     // (SA) NOTE: TODO: used in missionpack
    collisionModelManager->ClearMap();
    collisionModelManager->FreeMap();
    
    // wipe the entire per-level structure
    SV_ClearServer();
    
    // MrE: main zone should be pretty much emtpy at this point
    // except for file system data and cached renderer data
#ifdef ZONE_DEBUG
    Z_LogHeap();
#endif
    
    // allocate empty config strings
    for( i = 0; i < MAX_CONFIGSTRINGS; i++ )
    {
        sv.configstrings[i].s = CopyString( "" );
        sv.configstringsmodified[i] = false;
    }
    
    // init client structures and svs.numSnapshotEntities
    if( !Cvar_VariableValue( "sv_running" ) )
    {
        SV_Startup();
    }
    else
    {
        // check for maxclients change
        if( sv_maxclients->modified )
        {
            SV_ChangeMaxClients();
        }
    }
    
    // clear pak references
    FS_ClearPakReferences( 0 );
    
    // allocate the snapshot entities on the hunk
    svs.snapshotEntities = ( entityState_t* )Hunk_Alloc( sizeof( entityState_t ) * svs.numSnapshotEntities, h_high );
    svs.nextSnapshotEntities = 0;
    
    // toggle the server bit so clients can detect that a
    // server has changed
    svs.snapFlagServerBit ^= SNAPFLAG_SERVERCOUNT;
    
    // set nextmap to the same map, but it may be overriden
    // by the game startup or another console command
    Cvar_Set( "nextmap", "map_restart 0" );
//  Cvar_Set( "nextmap", va("map %s", server) );

    // Ridah
    // DHM - Nerve :: We want to use the completion bar in multiplayer as well
    // Arnout: just always use it
    if( !SV_GameIsSinglePlayer() )
    {
        SV_SetExpectedHunkUsage( va( "maps/%s.bsp", server ) );
    }
    else
    {
        // just set it to a negative number,so the cgame knows not to draw the percent bar
        Cvar_Set( "com_expectedhunkusage", "-1" );
    }
    
    // make sure we are not paused
    Cvar_Set( "cl_paused", "0" );
    
#if !defined( DO_LIGHT_DEDICATED )
    // get a new checksum feed and restart the file system
    srand( Sys_Milliseconds() );
    sv.checksumFeed = ( ( ( S32 )rand() << 16 ) ^ rand() ) ^ Sys_Milliseconds();
    
    // DO_LIGHT_DEDICATED
    // only comment out when you need a new pure checksum string and it's associated random feed
    //Com_DPrintf("SV_SpawnServer checksum feed: %p\n", sv.checksumFeed);
    
#else							// DO_LIGHT_DEDICATED implementation below
    // we are not able to randomize the checksum feed since the feed is used as key for pure_checksum computations
    // files.c 1776 : pack->pure_checksum = Com_BlockChecksumKey( fs_headerLongs, 4 * fs_numHeaderLongs, LittleLong(fs_checksumFeed) );
    // we request a fake randomized feed, files.c knows the answer
    srand( Sys_Milliseconds() );
    sv.checksumFeed = FS_RandChecksumFeed();
#endif
    FS_Restart( sv.checksumFeed );
    
    collisionModelManager->LoadMap( va( "maps/%s.bsp", server ), false, &checksum );
    
    // set serverinfo visible name
    Cvar_Set( "mapname", server );
    
    Cvar_Set( "sv_mapChecksum", va( "%i", checksum ) );
    
    sv_newGameShlib = Cvar_Get( "sv_newGameShlib", "", CVAR_TEMP );
    
    // serverid should be different each time
    sv.serverId = com_frameTime;
    sv.restartedServerId = sv.serverId;
    sv.checksumFeedServerId = sv.serverId;
    Cvar_Set( "sv_serverid", va( "%i", sv.serverId ) );
    
    // clear physics interaction links
    SV_ClearWorld();
    
    // media configstring setting should be done during
    // the loading stage, so connected clients don't have
    // to load during actual gameplay
    sv.state = SS_LOADING;
    
    Cvar_Set( "sv_serverRestarting", "1" );
    
    // load and spawn all other entities
    SV_InitGameProgs();
    
    // don't allow a map_restart if game is modified
    // Arnout: there isn't any check done against this, obsolete
//  sv_gametype->modified = false;

    // run a few frames to allow everything to settle
    for( i = 0; i < GAME_INIT_FRAMES; i++ )
    {
        game->RunFrame( svs.time );
        SV_BotFrame( svs.time );
        svs.time += FRAMETIME;
    }
    
    // create a baseline for more efficient communications
    SV_CreateBaseline();
    
    for( i = 0; i < sv_maxclients->integer; i++ )
    {
        // send the new gamestate to all connected clients
        if( svs.clients[i].state >= CS_CONNECTED )
        {
            UTF8* denied;
            
            if( svs.clients[i].netchan.remoteAddress.type == NA_BOT )
            {
                if( killBots || SV_GameIsSinglePlayer() || SV_GameIsCoop() )
                {
                    SV_DropClient( &svs.clients[i], "" );
                    continue;
                }
                isBot = true;
            }
            else
            {
                isBot = false;
            }
            
            // connect the client again
            denied = ( UTF8* )game->ClientConnect( i, false, isBot ); // firstTime = qfalse
            if( denied )
            {
                // this generally shouldn't happen, because the client
                // was connected before the level change
                SV_DropClient( &svs.clients[i], denied );
            }
            else
            {
                if( !isBot )
                {
                    // when we get the next packet from a connected client,
                    // the new gamestate will be sent
                    svs.clients[i].state = CS_CONNECTED;
                }
                else
                {
                    client_t*       client;
                    sharedEntity_t* ent;
                    
                    client = &svs.clients[i];
                    client->state = CS_ACTIVE;
                    ent = SV_GentityNum( i );
                    ent->s.number = i;
                    client->gentity = ent;
                    
                    client->deltaMessage = -1;
                    client->nextSnapshotTime = svs.time;	// generate a snapshot immediately
                    
                    game->ClientBegin( i );
                }
            }
        }
    }
    
    // run another frame to allow things to look at all the players
    game->RunFrame( svs.time );
    SV_BotFrame( svs.time );
    svs.time += FRAMETIME;
    
    if( sv_pure->integer )
    {
        // the server sends these to the clients so they will only
        // load pk3s also loaded at the server
        p = FS_LoadedPakChecksums();
        Cvar_Set( "sv_paks", p );
        if( strlen( p ) == 0 )
        {
            Com_Printf( "WARNING: sv_pure set but no PK3 files loaded\n" );
        }
        p = FS_LoadedPakNames();
        Cvar_Set( "sv_pakNames", p );
        
        // if a dedicated pure server we need to touch the cgame because it could be in a
        // seperate pk3 file and the client will need to load the latest cgame.qvm
        if( com_dedicated->integer )
        {
            SV_TouchCGame();
        }
    }
    else
    {
        Cvar_Set( "sv_paks", "" );
        Cvar_Set( "sv_pakNames", "" );
    }
    // the server sends these to the clients so they can figure
    // out which pk3s should be auto-downloaded
    // NOTE: we consider the referencedPaks as 'required for operation'
    
    // we want the server to reference the mp_bin pk3 that the client is expected to load from
    SV_TouchCGameDLL();
    
    p = FS_ReferencedPakChecksums();
    Cvar_Set( "sv_referencedPaks", p );
    p = FS_ReferencedPakNames();
    Cvar_Set( "sv_referencedPakNames", p );
    
    // save systeminfo and serverinfo strings
    cvar_modifiedFlags &= ~CVAR_SYSTEMINFO;
    SV_SetConfigstring( CS_SYSTEMINFO, Cvar_InfoString_Big( CVAR_SYSTEMINFO ) );
    
    SV_SetConfigstring( CS_SERVERINFO, Cvar_InfoString( CVAR_SERVERINFO | CVAR_SERVERINFO_NOUPDATE ) );
    cvar_modifiedFlags &= ~CVAR_SERVERINFO;
    
    // NERVE - SMF
#if 0
    SV_SetConfigstring( CS_WOLFINFO, Cvar_InfoString( CVAR_WOLFINFO ) );
    cvar_modifiedFlags &= ~CVAR_WOLFINFO;
#endif
    
    // any media configstring setting now should issue a warning
    // and any configstring changes should be reliably transmitted
    // to all clients
    sv.state = SS_GAME;
    
    // send a heartbeat now so the master will get up to date info
    SV_Heartbeat_f();
    
    Hunk_SetMark();
    
    SV_UpdateConfigStrings();
    
    Cvar_Set( "sv_serverRestarting", "0" );
    
    Com_Printf( "-----------------------------------\n" );
}

// Update Server
#if defined (UPDATE_SERVER)
/*
====================
SV_ParseVersionMapping

  Reads versionmap.cfg which sets up a mapping of client version to installer to download
====================
*/
void SV_ParseVersionMapping( void )
{
    S32 handle, len;
    UTF8* filename = "versionmap.cfg", *buf, *buftrav, *token;
    
    len = FS_SV_FOpenFileRead( filename, &handle );
    if( len >= 0 )    // the file exists
    {
    
        buf = ( UTF8* )Z_Malloc( len + 1 );
        memset( buf, 0, len + 1 );
        
        FS_Read( ( void* )buf, len, handle );
        FS_FCloseFile( handle );
        
        // now parse the file, setting the version table info
        buftrav = buf;
        
        token = COM_Parse( &buftrav );
        if( strcmp( token, "OpenWolf-VersionMap" ) )
        {
            Z_Free( buf );
            Com_Error( ERR_FATAL, "invalid versionmap.cfg" );
            return;
        }
        
        Com_Printf( "\n------------Update Server-------------\n\nParsing version map..." );
        
        while( ( token = COM_Parse( &buftrav ) ) && token[0] )
        {
            // read the version number
            strcpy( versionMap[ numVersions ].version, token );
            
            // read the platform
            token = COM_Parse( &buftrav );
            if( token && token[0] )
            {
                strcpy( versionMap[ numVersions ].platform, token );
            }
            else
            {
                Z_Free( buf );
                Com_Error( ERR_FATAL, "error parsing versionmap.cfg, after %s", versionMap[ numVersions ].version );
                return;
            }
            
            // read the installer name
            token = COM_Parse( &buftrav );
            if( token && token[0] )
            {
                strcpy( versionMap[ numVersions ].installer, token );
            }
            else
            {
                Z_Free( buf );
                Com_Error( ERR_FATAL, "error parsing versionmap.cfg, after %s", versionMap[ numVersions ].platform );
                return;
            }
            
            numVersions++;
            if( numVersions >= MAX_UPDATE_VERSIONS )
            {
                Z_Free( buf );
                Com_Error( ERR_FATAL, "Exceeded maximum number of mappings(%d)", MAX_UPDATE_VERSIONS );
                return;
            }
            
        }
        
        Com_Printf( " found %d mapping%c\n--------------------------------------\n\n", numVersions, numVersions > 1 ? 's' : ' ' );
        
        Z_Free( buf );
    }
    else
    {
        Com_Error( ERR_FATAL, "Couldn't open versionmap.cfg" );
    }
}
#endif

/*
===============
SV_Init

Only called at main exe startup, not for each game
===============
*/
void            SV_BotInitBotLib( void );

void SV_Init( void )
{
    SV_AddOperatorCommands();
    
    // serverinfo vars
    Cvar_Get( "dmflags", "0", /*CVAR_SERVERINFO */ 0 );
    Cvar_Get( "fraglimit", "0", /*CVAR_SERVERINFO */ 0 );
    Cvar_Get( "timelimit", "0", CVAR_SERVERINFO );
    
    // Rafael gameskill
//  sv_gameskill = Cvar_Get ("g_gameskill", "3", CVAR_SERVERINFO | CVAR_LATCH );
    // done
    
    Cvar_Get( "sv_keywords", "", CVAR_SERVERINFO );
    Cvar_Get( "protocol", va( "%i", ETPROTOCOL_VERSION ), CVAR_SERVERINFO | CVAR_ARCHIVE );
    sv_mapname = Cvar_Get( "mapname", "nomap", CVAR_SERVERINFO | CVAR_ROM );
    sv_privateClients = Cvar_Get( "sv_privateClients", "0", CVAR_SERVERINFO );
    sv_hostname = Cvar_Get( "sv_hostname", "OpenWolf Host", CVAR_SERVERINFO | CVAR_ARCHIVE );
    //
#ifdef __MACOS__
    sv_maxclients = Cvar_Get( "sv_maxclients", "16", CVAR_SERVERINFO | CVAR_LATCH );	//DAJ HOG
#else
    sv_maxclients = Cvar_Get( "sv_maxclients", "20", CVAR_SERVERINFO | CVAR_LATCH );	// NERVE - SMF - changed to 20 from 8
#endif
    
    sv_maxRate = Cvar_Get( "sv_maxRate", "0", CVAR_ARCHIVE | CVAR_SERVERINFO );
    sv_minPing = Cvar_Get( "sv_minPing", "0", CVAR_ARCHIVE | CVAR_SERVERINFO );
    sv_maxPing = Cvar_Get( "sv_maxPing", "0", CVAR_ARCHIVE | CVAR_SERVERINFO );
    sv_floodProtect = Cvar_Get( "sv_floodProtect", "1", CVAR_ARCHIVE | CVAR_SERVERINFO );
    sv_allowAnonymous = Cvar_Get( "sv_allowAnonymous", "0", CVAR_SERVERINFO );
    sv_friendlyFire = Cvar_Get( "g_friendlyFire", "1", CVAR_SERVERINFO | CVAR_ARCHIVE );	// NERVE - SMF
    sv_maxlives = Cvar_Get( "g_maxlives", "0", CVAR_ARCHIVE | CVAR_LATCH | CVAR_SERVERINFO );	// NERVE - SMF
    sv_needpass = Cvar_Get( "g_needpass", "0", CVAR_SERVERINFO | CVAR_ROM );
    
    // systeminfo
    //bani - added cvar_t for sv_cheats so server engine can reference it
    sv_cheats = Cvar_Get( "sv_cheats", "1", CVAR_SYSTEMINFO | CVAR_ROM );
    sv_serverid = Cvar_Get( "sv_serverid", "0", CVAR_SYSTEMINFO | CVAR_ROM );
    sv_pure = Cvar_Get( "sv_pure", "0", CVAR_SYSTEMINFO );
    
    Cvar_Get( "sv_paks", "", CVAR_SYSTEMINFO | CVAR_ROM );
    Cvar_Get( "sv_pakNames", "", CVAR_SYSTEMINFO | CVAR_ROM );
    Cvar_Get( "sv_referencedPaks", "", CVAR_SYSTEMINFO | CVAR_ROM );
    Cvar_Get( "sv_referencedPakNames", "", CVAR_SYSTEMINFO | CVAR_ROM );
    
    // server vars
    sv_rconPassword = Cvar_Get( "rconPassword", "", CVAR_TEMP );
    sv_privatePassword = Cvar_Get( "sv_privatePassword", "", CVAR_TEMP );
    sv_fps = Cvar_Get( "sv_fps", "60", CVAR_TEMP );
    sv_timeout = Cvar_Get( "sv_timeout", "240", CVAR_TEMP );
    sv_zombietime = Cvar_Get( "sv_zombietime", "2", CVAR_TEMP );
    Cvar_Get( "nextmap", "", CVAR_TEMP );
    
    sv_allowDownload = Cvar_Get( "sv_allowDownload", "1", CVAR_ARCHIVE );
    sv_master[0] = Cvar_Get( "sv_master1", MASTER_SERVER_NAME, 0 );
    sv_master[1] = Cvar_Get( "sv_master2", "", CVAR_ARCHIVE );
    sv_master[2] = Cvar_Get( "sv_master3", "", CVAR_ARCHIVE );
    sv_master[3] = Cvar_Get( "sv_master4", "", CVAR_ARCHIVE );
    sv_master[4] = Cvar_Get( "sv_master5", "", CVAR_ARCHIVE );
    sv_reconnectlimit = Cvar_Get( "sv_reconnectlimit", "3", 0 );
    sv_tempbanmessage =
        Cvar_Get( "sv_tempbanmessage", "You have been kicked and are temporarily banned from joining this server.", 0 );
    sv_showloss = Cvar_Get( "sv_showloss", "0", 0 );
    sv_padPackets = Cvar_Get( "sv_padPackets", "0", 0 );
    sv_killserver = Cvar_Get( "sv_killserver", "0", 0 );
    sv_mapChecksum = Cvar_Get( "sv_mapChecksum", "", CVAR_ROM );
    
    sv_reloading = Cvar_Get( "g_reloading", "0", CVAR_ROM );
    
    sv_lanForceRate = Cvar_Get( "sv_lanForceRate", "1", CVAR_ARCHIVE );
    
    sv_onlyVisibleClients = Cvar_Get( "sv_onlyVisibleClients", "0", 0 );	// DHM - Nerve
    
    sv_showAverageBPS = Cvar_Get( "sv_showAverageBPS", "0", 0 );	// NERVE - SMF - net debugging
    
    // NERVE - SMF - create user set cvars
    Cvar_Get( "g_userTimeLimit", "0", 0 );
    Cvar_Get( "g_userAlliedRespawnTime", "0", 0 );
    Cvar_Get( "g_userAxisRespawnTime", "0", 0 );
    Cvar_Get( "g_maxlives", "0", 0 );
    Cvar_Get( "g_altStopwatchMode", "0", CVAR_ARCHIVE );
    Cvar_Get( "g_minGameClients", "8", CVAR_SERVERINFO );
    Cvar_Get( "g_complaintlimit", "6", CVAR_ARCHIVE );
    Cvar_Get( "gamestate", "-1", CVAR_WOLFINFO | CVAR_ROM );
    Cvar_Get( "g_currentRound", "0", CVAR_WOLFINFO );
    Cvar_Get( "g_nextTimeLimit", "0", CVAR_WOLFINFO );
    // -NERVE - SMF
    
    // TTimo - some UI additions
    // NOTE: sucks to have this hardcoded really, I suppose this should be in UI
    Cvar_Get( "g_axismaxlives", "0", 0 );
    Cvar_Get( "g_alliedmaxlives", "0", 0 );
    Cvar_Get( "g_fastres", "0", CVAR_ARCHIVE );
    Cvar_Get( "g_fastResMsec", "1000", CVAR_ARCHIVE );
    
    // ATVI Tracker Wolfenstein Misc #273
    Cvar_Get( "g_voteFlags", "0", CVAR_ROM | CVAR_SERVERINFO );
    
    // ATVI Tracker Wolfenstein Misc #263
    Cvar_Get( "g_antilag", "1", CVAR_ARCHIVE | CVAR_SERVERINFO );
    
    Cvar_Get( "g_needpass", "0", CVAR_SERVERINFO );
    
    g_gameType = Cvar_Get( "g_gametype", va( "%i", com_gameInfo.defaultGameType ), CVAR_SERVERINFO | CVAR_LATCH );
    
#if !defined (UPDATE_SERVER)
    // the download netcode tops at 18/20 kb/s, no need to make you think you can go above
    sv_dl_maxRate = Cvar_Get( "sv_dl_maxRate", "42000", CVAR_ARCHIVE );
#else
    // the update server is on steroids, sv_fps 60 and no snapshotMsec limitation, it can go up to 30 kb/s
    sv_dl_maxRate = Cvar_Get( "sv_dl_maxRate", "60000", CVAR_ARCHIVE );
#endif
    
    sv_wwwDownload = Cvar_Get( "sv_wwwDownload", "0", CVAR_ARCHIVE );
    sv_wwwBaseURL = Cvar_Get( "sv_wwwBaseURL", "", CVAR_ARCHIVE );
    sv_wwwDlDisconnected = Cvar_Get( "sv_wwwDlDisconnected", "0", CVAR_ARCHIVE );
    sv_wwwFallbackURL = Cvar_Get( "sv_wwwFallbackURL", "", CVAR_ARCHIVE );
    
    //bani
    sv_packetloss = Cvar_Get( "sv_packetloss", "0", CVAR_CHEAT );
    sv_packetdelay = Cvar_Get( "sv_packetdelay", "0", CVAR_CHEAT );
    
    // fretn - note: redirecting of clients to other servers relies on this,
    // ET://someserver.com
    sv_fullmsg = Cvar_Get( "sv_fullmsg", "Server is full.", CVAR_ARCHIVE );
    
    // initialize bot cvars so they arelisted and can be set before loading the botlib
    SV_BotInitCvars();
    
    // init the botlib here because we need the pre-compiler in the UI
    SV_BotInitBotLib();
    
    svs.serverLoad = -1;
    
#if defined(UPDATE_SERVER)
    SV_Startup();
    SV_ParseVersionMapping();
    
    // serverid should be different each time
    sv.serverId = com_frameTime + 100;
    sv.restartedServerId = sv.serverId; // I suppose the init here is just to be safe
    sv.checksumFeedServerId = sv.serverId;
    Cvar_Set( "sv_serverid", va( "%i", sv.serverId ) );
    Cvar_Set( "mapname", "Update" );
    
    // allocate empty config strings
    {
        S32 i;
        
        for( i = 0 ; i < MAX_CONFIGSTRINGS ; i++ )
        {
            sv.configstrings[i].s = CopyString( "" );
        }
    }
#endif
}


/*
==================
SV_FinalCommand

Used by SV_Shutdown to send a final message to all
connected clients before the server goes down.  The messages are sent immediately,
not just stuck on the outgoing message list, because the server is going
to totally exit after returning from this function.
==================
*/
void SV_FinalCommand( UTF8* cmd, bool disconnect )
{
    S32             i, j;
    client_t*       cl;
    
    // send it twice, ignoring rate
    for( j = 0; j < 2; j++ )
    {
        for( i = 0, cl = svs.clients; i < sv_maxclients->integer; i++, cl++ )
        {
            if( cl->state >= CS_CONNECTED )
            {
                // don't send a disconnect to a local client
                if( cl->netchan.remoteAddress.type != NA_LOOPBACK )
                {
                    //% SV_SendServerCommand( cl, "print \"%s\"", message );
                    SV_SendServerCommand( cl, "%s", cmd );
                    
                    // ydnar: added this so map changes can use this functionality
                    if( disconnect )
                    {
                        SV_SendServerCommand( cl, "disconnect" );
                    }
                }
                // force a snapshot to be sent
                cl->nextSnapshotTime = -1;
                SV_SendClientSnapshot( cl );
            }
        }
    }
}


/*
================
SV_Shutdown

Called when each game quits,
before Sys_Quit or Sys_Error
================
*/
void SV_Shutdown( UTF8* finalmsg )
{
    if( !com_sv_running || !com_sv_running->integer )
    {
        return;
    }
    
    Com_Printf( "----- Server Shutdown -----\n" );
    
    NET_LeaveMulticast6();
    
    if( svs.clients && !com_errorEntered )
    {
        SV_FinalCommand( va( "print \"%s\"", finalmsg ), true );
    }
    
    SV_MasterShutdown();
    SV_ShutdownGameProgs();
    
    // free current level
    SV_ClearServer();
    
    collisionModelManager->FreeMap();
    
    // free server static data
    if( svs.clients )
    {
        S32 index;
        
        for( index = 0; index < sv_maxclients->integer; index++ )
        {
            SV_FreeClient( &svs.clients[index] );
        }
        
        //Z_Free( svs.clients );
        free( svs.clients );		// RF, avoid trying to allocate large chunk on a fragmented zone
    }
    memset( &svs, 0, sizeof( svs ) );
    svs.serverLoad = -1;
    
    Cvar_Set( "sv_running", "0" );
    
    Com_Printf( "---------------------------\n" );
    
    // disconnect any local clients
    CL_Disconnect( false );
}
