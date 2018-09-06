////////////////////////////////////////////////////////////////////////////////////////
// Copyright(C) 1999 - 2005 Id Software, Inc.
// Copyright(C) 2000 - 2006 Tim Angus
// Copyright(C) 2011 - 2018 Dusan Jocic <dusanjocic@msn.com>
//
// This file is part of OpenWolf.
//
// OpenWolf is free software; you can redistribute it
// and / or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the License,
// or (at your option) any later version.
//
// OpenWolf is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with OpenWolf; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110 - 1301  USA
//
// -------------------------------------------------------------------------------------
// File name:   cg_main.cpp
// Version:     v1.00
// Created:
// Compilers:   Visual Studio 2015
// Description: initialization and primary entry point for cgame
// -------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////

#include <cgame/cg_precompiled.h>

// display context for new ui stuff
displayContextDef_t cgDC;

extern itemDef_t* g_bindItem;
extern bool g_waitingForKey;

idCGameLocal cgameLocal;
idCGame* cgame = &cgameLocal;

cg_t        cg;
cgs_t       cgs;
centity_t   cg_entities[ MAX_GENTITIES ];

weaponInfo_t    cg_weapons[ 32 ];
upgradeInfo_t   cg_upgrades[ 32 ];

buildableInfo_t cg_buildables[ BA_NUM_BUILDABLES ];

vmCvar_t  cg_version;
vmCvar_t  cg_teslaTrailTime;
vmCvar_t  cg_centertime;
vmCvar_t  cg_runpitch;
vmCvar_t  cg_runroll;
vmCvar_t  cg_swingSpeed;
vmCvar_t  cg_shadows;
vmCvar_t  cg_drawTimer;
vmCvar_t  cg_drawClock;
vmCvar_t  cg_drawFPS;
vmCvar_t  cg_drawSpeed;
vmCvar_t  cg_drawDemoState;
vmCvar_t  cg_drawSnapshot;
vmCvar_t  cg_drawChargeBar;
vmCvar_t  cg_drawCrosshair;
vmCvar_t  cg_drawCrosshairNames;
vmCvar_t  cg_crosshairSize;
vmCvar_t  cg_drawAmmoStack;
vmCvar_t  cg_draw2D;
vmCvar_t  cg_drawStatus;
vmCvar_t  cg_animSpeed;
vmCvar_t  cg_debugAnim;
vmCvar_t  cg_debugAlloc;
vmCvar_t  cg_debugPosition;
vmCvar_t  cg_debugEvents;
vmCvar_t  cg_errorDecay;
vmCvar_t  cg_nopredict;
vmCvar_t  cg_debugMove;
vmCvar_t  cg_noPlayerAnims;
vmCvar_t  cg_showmiss;
vmCvar_t  cg_footsteps;
vmCvar_t  cg_addMarks;
vmCvar_t  cg_brassTime;
vmCvar_t  cg_viewsize;
vmCvar_t  cg_drawGun;
vmCvar_t  cg_gun_frame;
vmCvar_t  cg_gun_x;
vmCvar_t  cg_gun_y;
vmCvar_t  cg_gun_z;
vmCvar_t  cg_tracerChance;
vmCvar_t  cg_tracerWidth;
vmCvar_t  cg_tracerLength;
vmCvar_t  cg_autoswitch;
vmCvar_t  cg_thirdPerson;
vmCvar_t  cg_thirdPersonShoulderViewMode;
vmCvar_t  cg_thirdPersonPitchFollow;
vmCvar_t  cg_thirdPersonRange;
vmCvar_t  cg_shoulderViewOverride;
vmCvar_t  cg_shoulderViewUp;
vmCvar_t  cg_shoulderViewRight;
vmCvar_t  cg_shoulderViewForward;
vmCvar_t  cg_stereoSeparation;
vmCvar_t  cg_lagometer;
vmCvar_t  cg_synchronousClients;
vmCvar_t  cg_stats;
vmCvar_t  cg_paused;
vmCvar_t  cg_blood;
vmCvar_t  cg_drawFriend;
vmCvar_t  cg_teamChatsOnly;
vmCvar_t  cg_noPrintDuplicate;
vmCvar_t  cg_noVoiceChats;
vmCvar_t  cg_noVoiceText;
vmCvar_t  cg_hudFiles;
vmCvar_t  cg_hudFilesEnable;
vmCvar_t  cg_smoothClients;
vmCvar_t  pmove_fixed;
vmCvar_t  pmove_msec;
vmCvar_t  cg_cameraMode;
vmCvar_t  cg_timescaleFadeEnd;
vmCvar_t  cg_timescaleFadeSpeed;
vmCvar_t  cg_timescale;
vmCvar_t  cg_noTaunt;
vmCvar_t  cg_drawSurfNormal;
vmCvar_t  cg_drawBBOX;
vmCvar_t  cg_wwSmoothTime;
vmCvar_t  cg_flySpeed;
vmCvar_t  cg_depthSortParticles;
vmCvar_t  cg_bounceParticles;
vmCvar_t  cg_consoleLatency;
vmCvar_t  cg_lightFlare;
vmCvar_t  cg_debugParticles;
vmCvar_t  cg_debugTrails;
vmCvar_t  cg_debugPVS;
vmCvar_t  cg_disableWarningDialogs;
vmCvar_t  cg_disableUpgradeDialogs;
vmCvar_t  cg_disableBuildDialogs;
vmCvar_t  cg_disableCommandDialogs;
vmCvar_t  cg_disableScannerPlane;
vmCvar_t  cg_tutorial;
vmCvar_t  cg_painBlendUpRate;
vmCvar_t  cg_painBlendDownRate;
vmCvar_t  cg_painBlendMax;
vmCvar_t  cg_painBlendScale;
vmCvar_t  cg_painBlendZoom;
vmCvar_t  cg_debugVoices;
vmCvar_t  cg_stickySpec;
vmCvar_t  cg_alwaysSprint;
vmCvar_t  cg_unlagged;
vmCvar_t  ui_currentClass;
vmCvar_t  ui_carriage;
vmCvar_t  ui_stage;
vmCvar_t  ui_dialog;
vmCvar_t  ui_voteActive;
vmCvar_t  ui_alienTeamVoteActive;
vmCvar_t  ui_humanTeamVoteActive;
vmCvar_t  cg_debugRandom;
vmCvar_t  cg_optimizePrediction;
vmCvar_t  cg_projectileNudge;
vmCvar_t  cg_drawBuildableStatus;
vmCvar_t  cg_hideHealthyBuildableStatus;
vmCvar_t  cg_drawTeamStatus;
vmCvar_t  cg_hideHealthyTeamStatus;
vmCvar_t  cg_suppressWAnimWarnings;
vmCvar_t  cg_voice;
vmCvar_t  cg_emoticons;
vmCvar_t  cg_drawAlienFeedback;


typedef struct
{
    vmCvar_t*  vmCvar;
    UTF8*      cvarName;
    UTF8*      defaultString;
    S32       cvarFlags;
} cvarTable_t;

static cvarTable_t cvarTable[ ] =
{
    { &cg_version, "cg_version", PRODUCT_NAME, CVAR_ROM | CVAR_USERINFO },
    { &cg_autoswitch, "cg_autoswitch", "1", CVAR_ARCHIVE },
    { &cg_drawGun, "cg_drawGun", "1", CVAR_ARCHIVE },
    { &cg_viewsize, "cg_viewsize", "100", CVAR_ARCHIVE },
    { &cg_stereoSeparation, "cg_stereoSeparation", "0.4", CVAR_ARCHIVE  },
    { &cg_shadows, "cg_shadows", "4", CVAR_ARCHIVE  },
    { &cg_draw2D, "cg_draw2D", "1", CVAR_ARCHIVE  },
    { &cg_drawStatus, "cg_drawStatus", "1", CVAR_ARCHIVE  },
    { &cg_drawTimer, "cg_drawTimer", "1", CVAR_ARCHIVE  },
    { &cg_drawClock, "cg_drawClock", "1", CVAR_ARCHIVE  },
    { &cg_drawFPS, "cg_drawFPS", "1", CVAR_ARCHIVE  },
    { &cg_drawSpeed, "cg_drawSpeed", "0", CVAR_ARCHIVE  },
    { &cg_drawDemoState, "cg_drawDemoState", "1", CVAR_ARCHIVE  },
    { &cg_drawSnapshot, "cg_drawSnapshot", "0", CVAR_ARCHIVE  },
    { &cg_drawChargeBar, "cg_drawChargeBar", "1", CVAR_ARCHIVE  },
    { &cg_drawCrosshair, "cg_drawCrosshair", "1", CVAR_ARCHIVE },
    { &cg_drawCrosshairNames, "cg_drawCrosshairNames", "1", CVAR_ARCHIVE },
    { &cg_crosshairSize, "cg_crosshairSize", "1", CVAR_ARCHIVE },
    { &cg_drawAmmoStack, "cg_drawAmmoStack", "1", CVAR_ARCHIVE },
    { &cg_brassTime, "cg_brassTime", "2500", CVAR_ARCHIVE },
    { &cg_addMarks, "cg_marks", "1", CVAR_ARCHIVE },
    { &cg_lagometer, "cg_lagometer", "1", CVAR_ARCHIVE },
    { &cg_teslaTrailTime, "cg_teslaTrailTime", "250", CVAR_ARCHIVE  },
    { &cg_gun_x, "cg_gunX", "0", CVAR_CHEAT },
    { &cg_gun_y, "cg_gunY", "0", CVAR_CHEAT },
    { &cg_gun_z, "cg_gunZ", "0", CVAR_CHEAT },
    { &cg_centertime, "cg_centertime", "3", CVAR_CHEAT },
    { &cg_runpitch, "cg_runpitch", "0.002", CVAR_ARCHIVE},
    { &cg_runroll, "cg_runroll", "0.005", CVAR_ARCHIVE },
    { &cg_swingSpeed, "cg_swingSpeed", "0.3", CVAR_CHEAT },
    { &cg_animSpeed, "cg_animspeed", "1", CVAR_CHEAT },
    { &cg_debugAnim, "cg_debuganim", "0", CVAR_CHEAT },
    { &cg_debugAlloc, "cg_debugAlloc", "0", CVAR_CHEAT },
    { &cg_debugPosition, "cg_debugposition", "0", CVAR_CHEAT },
    { &cg_debugEvents, "cg_debugevents", "0", CVAR_CHEAT },
    { &cg_errorDecay, "cg_errordecay", "100", 0 },
    { &cg_nopredict, "cg_nopredict", "0", 0 },
    { &cg_debugMove, "cg_debugMove", "0", 0 },
    { &cg_noPlayerAnims, "cg_noplayeranims", "0", CVAR_CHEAT },
    { &cg_showmiss, "cg_showmiss", "0", 0 },
    { &cg_footsteps, "cg_footsteps", "1", CVAR_CHEAT },
    { &cg_tracerChance, "cg_tracerchance", "0.4", CVAR_CHEAT },
    { &cg_tracerWidth, "cg_tracerwidth", "1", CVAR_CHEAT },
    { &cg_tracerLength, "cg_tracerlength", "100", CVAR_CHEAT },
    { &cg_thirdPersonRange, "cg_thirdPersonRange", "75", CVAR_ARCHIVE },
    { &cg_thirdPerson, "cg_thirdPerson", "0", CVAR_CHEAT },
    { &cg_thirdPersonPitchFollow, "cg_thirdPersonPitchFollow", "0", 0 },
    { &cg_thirdPersonShoulderViewMode, "cg_thirdPersonShoulderViewMode", "1", CVAR_ARCHIVE },
    { &cg_shoulderViewOverride, "cg_shoulderViewOverride", "0", 0 },
    { &cg_shoulderViewUp, "cg_shoulderViewUp", "0", 0 },
    { &cg_shoulderViewRight, "cg_shoulderViewRight", "0", 0 },
    { &cg_shoulderViewForward, "cg_shoulderViewForward", "0", 0 },
    { &cg_stats, "cg_stats", "0", 0 },
    { &cg_drawFriend, "cg_drawFriend", "1", CVAR_ARCHIVE },
    { &cg_teamChatsOnly, "cg_teamChatsOnly", "0", CVAR_ARCHIVE },
    { &cg_noPrintDuplicate, "cg_noPrintDuplicate", "0", CVAR_ARCHIVE },
    { &cg_noVoiceChats, "cg_noVoiceChats", "0", CVAR_ARCHIVE },
    { &cg_noVoiceText, "cg_noVoiceText", "0", CVAR_ARCHIVE },
    { &cg_drawSurfNormal, "cg_drawSurfNormal", "0", CVAR_CHEAT },
    { &cg_drawBBOX, "cg_drawBBOX", "0", CVAR_CHEAT },
    { &cg_wwSmoothTime, "cg_wwSmoothTime", "300", CVAR_ARCHIVE },
    { NULL, "cg_wwFollow", "1", CVAR_ARCHIVE | CVAR_USERINFO },
    { NULL, "cg_wwToggle", "1", CVAR_ARCHIVE | CVAR_USERINFO },
    { NULL, "cg_flySpeed", "600", CVAR_ARCHIVE | CVAR_USERINFO },
    { &cg_flySpeed, "cg_flySpeed", "500", CVAR_ARCHIVE | CVAR_USERINFO },
    { &cg_stickySpec, "cg_stickySpec", "1", CVAR_ARCHIVE | CVAR_USERINFO },
    { &cg_alwaysSprint, "cg_alwaysSprint", "0", CVAR_ARCHIVE | CVAR_USERINFO },
    { &cg_unlagged, "cg_unlagged", "1", CVAR_ARCHIVE | CVAR_USERINFO },
    { &cg_depthSortParticles, "cg_depthSortParticles", "1", CVAR_ARCHIVE },
    { &cg_bounceParticles, "cg_bounceParticles", "0", CVAR_ARCHIVE },
    { &cg_consoleLatency, "cg_consoleLatency", "3000", CVAR_ARCHIVE },
    { &cg_lightFlare, "cg_lightFlare", "3", CVAR_ARCHIVE },
    { &cg_debugParticles, "cg_debugParticles", "0", CVAR_CHEAT },
    { &cg_debugTrails, "cg_debugTrails", "0", CVAR_CHEAT },
    { &cg_debugPVS, "cg_debugPVS", "0", CVAR_CHEAT },
    { &cg_disableWarningDialogs, "cg_disableWarningDialogs", "0", CVAR_ARCHIVE },
    { &cg_disableUpgradeDialogs, "cg_disableUpgradeDialogs", "0", CVAR_ARCHIVE },
    { &cg_disableBuildDialogs, "cg_disableBuildDialogs", "0", CVAR_ARCHIVE },
    { &cg_disableCommandDialogs, "cg_disableCommandDialogs", "0", CVAR_ARCHIVE },
    { &cg_disableScannerPlane, "cg_disableScannerPlane", "0", CVAR_ARCHIVE },
    { &cg_tutorial, "cg_tutorial", "1", CVAR_ARCHIVE },
    { &cg_hudFiles, "cg_hudFiles", "gui/hud.txt", CVAR_ARCHIVE},
    { &cg_hudFilesEnable, "cg_hudFilesEnable", "0", CVAR_ARCHIVE},
    { NULL, "cg_alienConfig", "", CVAR_ARCHIVE },
    { NULL, "cg_humanConfig", "", CVAR_ARCHIVE },
    { NULL, "cg_spectatorConfig", "", CVAR_ARCHIVE },
    { &cg_painBlendUpRate, "cg_painBlendUpRate", "10.0", 0 },
    { &cg_painBlendDownRate, "cg_painBlendDownRate", "0.5", 0 },
    { &cg_painBlendMax, "cg_painBlendMax", "0.7", 0 },
    { &cg_painBlendScale, "cg_painBlendScale", "7.0", 0 },
    { &cg_painBlendZoom, "cg_painBlendZoom", "0.65", 0 },
    
    { &cg_debugVoices, "cg_debugVoices", "0", 0 },
    
    { &ui_currentClass, "gui_currentClass", "0", 0 },
    { &ui_carriage, "gui_carriage", "", 0 },
    { &ui_stage, "gui_stage", "0", 0 },
    { &ui_dialog, "gui_dialog", "Text not set", 0 },
    { &ui_voteActive, "gui_voteActive", "0", 0 },
    { &ui_humanTeamVoteActive, "gui_humanTeamVoteActive", "0", 0 },
    { &ui_alienTeamVoteActive, "gui_alienTeamVoteActive", "0", 0 },
    
    { &cg_debugRandom, "cg_debugRandom", "0", 0 },
    
    { &cg_optimizePrediction, "cg_optimizePrediction", "1", CVAR_ARCHIVE },
    { &cg_projectileNudge, "cg_projectileNudge", "1", CVAR_ARCHIVE },
    
    { &cg_drawBuildableStatus, "cg_drawBuildableStatus", "1", CVAR_ARCHIVE },
    { &cg_hideHealthyBuildableStatus, "cg_hideHealthyBuildableStatus", "1", CVAR_ARCHIVE },
    { &cg_drawTeamStatus, "cg_drawTeamStatus", "1", CVAR_USERINFO | CVAR_ARCHIVE },
    { &cg_hideHealthyTeamStatus, "cg_hideHealthyTeamStatus", "1", CVAR_ARCHIVE },
    { NULL, "teamoverlay", "1", CVAR_USERINFO | CVAR_ARCHIVE }, //if this is 0 the server will not send tinfo and teammate health will not work
    
    // the following variables are created in other parts of the system,
    // but we also reference them here
    
    { &cg_paused, "cl_paused", "0", CVAR_ROM },
    { &cg_blood, "com_blood", "1", CVAR_ARCHIVE },
    { &cg_synchronousClients, "g_synchronousClients", "0", 0 }, // communicated by systeminfo
    { &cg_timescaleFadeEnd, "cg_timescaleFadeEnd", "1", 0},
    { &cg_timescaleFadeSpeed, "cg_timescaleFadeSpeed", "0", 0},
    { &cg_timescale, "timescale", "1", 0},
    { &cg_smoothClients, "cg_smoothClients", "0", CVAR_USERINFO | CVAR_ARCHIVE},
    { &cg_cameraMode, "com_cameraMode", "0", CVAR_CHEAT},
    
    { &pmove_fixed, "pmove_fixed", "0", 0},
    { &pmove_msec, "pmove_msec", "8", 0},
    { &cg_noTaunt, "cg_noTaunt", "0", CVAR_ARCHIVE},
    
    { &cg_suppressWAnimWarnings, "cg_suppressWAnimWarnings", "1", CVAR_ARCHIVE},
    
    { &cg_voice, "voice", "default", CVAR_USERINFO | CVAR_ARCHIVE},
    
    { &cg_emoticons, "cg_emoticons", "1", CVAR_LATCH | CVAR_ARCHIVE},
    { &cg_drawAlienFeedback, "cg_drawAlienFeedback", "1", 0}
};

static S32 cvarTableSize = sizeof( cvarTable ) / sizeof( cvarTable[0] );

/*
=================
CG_RegisterCvars
=================
*/
void idCGameLocal::RegisterCvars( void )
{
    S32         i;
    cvarTable_t* cv;
    UTF8        var[ MAX_TOKEN_CHARS ];
    
    for( i = 0, cv = cvarTable; i < cvarTableSize; i++, cv++ )
    {
        trap_Cvar_Register( cv->vmCvar, cv->cvarName, cv->defaultString, cv->cvarFlags );
    }
    
    //repress standard Q3 console
    trap_Cvar_Set( "con_notifytime", "-2" );
    
    // see if we are also running the server on this machine
    trap_Cvar_VariableStringBuffer( "sv_running", var, sizeof( var ) );
    cgs.localServer = ( bool )atoi( var );
    
    // override any existing version cvar
    trap_Cvar_Set( "cg_version", PRODUCT_NAME );
}


/*
===============
CG_SetUIVars

Set some cvars used by the UI
===============
*/
void idCGameLocal::SetUIVars( void )
{
    S32   i, upgrades = 0;
    
    if( !cg.snap )
        return;
        
    //determine what the player is carrying
    for( i = UP_NONE + 1; i < UP_NUM_UPGRADES; i++ )
    {
        if( bggame->InventoryContainsUpgrade( i, cg.snap->ps.stats ) && bggame->Upgrade( ( upgrade_t )i )->purchasable )
            upgrades |= ( 1 << i );
    }
    
    trap_Cvar_Set( "gui_carriage", va( "%d %d %d", cg.snap->ps.stats[ STAT_WEAPON ], upgrades, cg.snap->ps.persistant[ PERS_CREDIT ] ) );
    
    switch( cg.snap->ps.stats[ STAT_TEAM ] )
    {
        case TEAM_NONE:
            trap_Cvar_Set( "gui_stage", "0" );
            break;
        case TEAM_ALIENS:
            trap_Cvar_Set( "gui_stage", va( "%d", cgs.alienStage ) );
            break;
        case TEAM_HUMANS:
            trap_Cvar_Set( "gui_stage", va( "%d", cgs.humanStage ) );
            break;
    }
}

/*
===============
CG_SetPVars

Set the p_* cvars
===============
*/
void idCGameLocal::SetPVars( void )
{
    playerState_t* ps;
    
    if( !cg.snap )
        return;
        
    ps = &cg.snap->ps;
    
    trap_Cvar_Set( "p_hp", va( "%d", ps->stats[ STAT_HEALTH ] ) );
    trap_Cvar_Set( "p_maxhp", va( "%d", ps->stats[ STAT_MAX_HEALTH ] ) );
    trap_Cvar_Set( "p_team", va( "%d", ps->stats[ STAT_TEAM ] ) );
    switch( ps->stats[ STAT_TEAM ] )
    {
        case TEAM_NONE:
            trap_Cvar_Set( "p_teamname", "^3Spectator" );
            trap_Cvar_Set( "p_stage", "0" );
            break;
        case TEAM_ALIENS:
            trap_Cvar_Set( "p_teamname", "^1Alien" );
            trap_Cvar_Set( "p_stage", va( "%d", cgs.alienStage ) );
            break;
        case TEAM_HUMANS:
            trap_Cvar_Set( "p_teamname", "^4Human" );
            trap_Cvar_Set( "p_stage", va( "%d", cgs.humanStage ) );
            break;
    }
    trap_Cvar_Set( "p_class", va( "%d", ps->stats[ STAT_CLASS ] ) );
    trap_Cvar_Set( "p_classname", bggame->ClassConfig( ( class_t )ps->stats[ STAT_CLASS ] )->humanName );
    trap_Cvar_Set( "p_weapon", va( "%d", ps->stats[ STAT_WEAPON ] ) );
    trap_Cvar_Set( "p_weaponname", bggame->Weapon( ( weapon_t )ps->stats[ STAT_WEAPON ] )->humanName );
    trap_Cvar_Set( "p_ammo", va( "%d", ps->ammo ) );
    trap_Cvar_Set( "p_clips", va( "%d", ps->clips ) );
    trap_Cvar_Set( "p_credits", va( "%d", ps->persistant[ PERS_CREDIT ] ) );
    trap_Cvar_Set( "p_score", va( "%d", ps->persistant[ PERS_SCORE ] ) );
    trap_Cvar_Set( "p_attacker", va( "%d", cgameLocal.LastAttacker( ) ) );
    if( cgameLocal.LastAttacker( ) != -1 )
        trap_Cvar_Set( "p_attackername", cgs.clientinfo[cgameLocal.LastAttacker( ) ].name );
    trap_Cvar_Set( "p_crosshair", va( "%d", cgameLocal.CrosshairPlayer( ) ) );
    if( cgameLocal.CrosshairPlayer( ) != -1 )
        trap_Cvar_Set( "p_crosshairrname", cgs.clientinfo[cgameLocal.CrosshairPlayer( ) ].name );
}

/*
=================
CG_UpdateCvars
=================
*/
void idCGameLocal::UpdateCvars( void )
{
    S32         i;
    cvarTable_t* cv;
    
    for( i = 0, cv = cvarTable; i < cvarTableSize; i++, cv++ )
        if( cv->vmCvar )
            trap_Cvar_Update( cv->vmCvar );
            
    // check for modications here
    
    SetUIVars( );
    SetPVars( );
}


S32 idCGameLocal::CrosshairPlayer( void )
{
    if( cg.time > ( cg.crosshairClientTime + 1000 ) )
        return -1;
        
    return cg.crosshairClientNum;
}


S32 idCGameLocal::LastAttacker( void )
{
    if( !cg.attackerTime )
        return -1;
        
    return cg.snap->ps.persistant[ PERS_ATTACKER ];
}


/*
=================
CG_RemoveNotifyLine
=================
*/
void idCGameLocal::RemoveNotifyLine( void )
{
    S32 i, offset, totalLength;
    
    if( cg.numConsoleLines == 0 )
        return;
        
    offset = cg.consoleLines[ 0 ].length;
    totalLength = strlen( cg.consoleText ) - offset;
    
    //slide up consoleText
    for( i = 0; i <= totalLength; i++ )
        cg.consoleText[ i ] = cg.consoleText[ i + offset ];
        
    //pop up the first consoleLine
    for( i = 0; i < cg.numConsoleLines; i++ )
        cg.consoleLines[ i ] = cg.consoleLines[ i + 1 ];
        
    cg.numConsoleLines--;
}

/*
=================
CG_AddNotifyText
=================
*/
void idCGameLocal::AddNotifyText( void )
{
    UTF8 buffer[ BIG_INFO_STRING ];
    S32 bufferLen, textLen;
    
    trap_LiteralArgs( buffer, BIG_INFO_STRING );
    
    if( !buffer[ 0 ] )
    {
        cg.consoleText[ 0 ] = '\0';
        cg.numConsoleLines = 0;
        return;
    }
    
    bufferLen = strlen( buffer );
    textLen = strlen( cg.consoleText );
    
    // Ignore console messages that were just printed
    if( cg_noPrintDuplicate.integer && textLen >= bufferLen &&
            !strcmp( cg.consoleText + textLen - bufferLen, buffer ) )
        return;
        
    if( cg.numConsoleLines == MAX_CONSOLE_LINES )
        RemoveNotifyLine( );
        
    Q_strcat( cg.consoleText, MAX_CONSOLE_TEXT, buffer );
    cg.consoleLines[ cg.numConsoleLines ].time = cg.time;
    cg.consoleLines[ cg.numConsoleLines ].length = bufferLen;
    cg.numConsoleLines++;
}

void idCGameLocal::Printf( StringEntry msg, ... )
{
    va_list argptr;
    UTF8    text[ 1024 ];
    
    va_start( argptr, msg );
    Q_vsnprintf( text, sizeof( text ), msg, argptr );
    va_end( argptr );
    
    trap_Print( text );
}

void idCGameLocal::Error( StringEntry msg, ... )
{
    va_list argptr;
    UTF8    text[ 1024 ];
    
    va_start( argptr, msg );
    Q_vsnprintf( text, sizeof( text ), msg, argptr );
    va_end( argptr );
    
    trap_Error( text );
}

void Com_Error( S32 level, StringEntry error, ... )
{
    va_list argptr;
    UTF8    text[1024];
    
    va_start( argptr, error );
    Q_vsnprintf( text, sizeof( text ), error, argptr );
    va_end( argptr );
    
    cgameLocal.Error( "%s", text );
}

void Com_Printf( StringEntry msg, ... )
{
    va_list   argptr;
    UTF8    text[1024];
    
    va_start( argptr, msg );
    Q_vsnprintf( text, sizeof( text ), msg, argptr );
    va_end( argptr );
    
    cgameLocal.Printf( "%s", text );
}



/*
================
CG_Argv
================
*/
StringEntry idCGameLocal::Argv( S32 arg )
{
    static UTF8 buffer[ MAX_STRING_CHARS ];
    
    trap_Argv( arg, buffer, sizeof( buffer ) );
    
    return buffer;
}


//========================================================================

/*
=================
CG_FileExists

Test if a specific file exists or not
=================
*/
bool idCGameLocal::FileExists( UTF8* filename )
{
    return trap_FS_FOpenFile( filename, NULL, FS_READ );
}

/*
=================
CG_RegisterSounds

called during a precache command
=================
*/
void idCGameLocal::RegisterSounds( void )
{
    S32         i;
    UTF8        name[ MAX_QPATH ];
    StringEntry  soundName;
    
    cgs.media.alienStageTransition  = trap_S_RegisterSound( "sound/announcements/overmindevolved.wav", true );
    cgs.media.humanStageTransition  = trap_S_RegisterSound( "sound/announcements/reinforcement.wav", true );
    
    cgs.media.alienOvermindAttack   = trap_S_RegisterSound( "sound/announcements/overmindattack.wav", true );
    cgs.media.alienOvermindDying    = trap_S_RegisterSound( "sound/announcements/overminddying.wav", true );
    cgs.media.alienOvermindSpawns   = trap_S_RegisterSound( "sound/announcements/overmindspawns.wav", true );
    
    cgs.media.alienL1Grab           = trap_S_RegisterSound( "sound/player/level1/grab.wav", true );
    cgs.media.alienL4ChargePrepare  = trap_S_RegisterSound( "sound/player/level4/charge_prepare.wav", true );
    cgs.media.alienL4ChargeStart    = trap_S_RegisterSound( "sound/player/level4/charge_start.wav", true );
    
    cgs.media.tracerSound           = trap_S_RegisterSound( "sound/weapons/tracer.wav", false );
    cgs.media.selectSound           = trap_S_RegisterSound( "sound/weapons/change.wav", false );
    cgs.media.turretSpinupSound     = trap_S_RegisterSound( "sound/buildables/mgturret/spinup.wav", false );
    cgs.media.weaponEmptyClick      = trap_S_RegisterSound( "sound/weapons/click.wav", false );
    
    cgs.media.talkSound             = trap_S_RegisterSound( "sound/misc/talk.wav", false );
    cgs.media.alienTalkSound        = trap_S_RegisterSound( "sound/misc/alien_talk.wav", false );
    cgs.media.humanTalkSound        = trap_S_RegisterSound( "sound/misc/human_talk.wav", false );
    cgs.media.landSound             = trap_S_RegisterSound( "sound/player/land1.wav", false );
    
    cgs.media.watrInSound           = trap_S_RegisterSound( "sound/player/watr_in.wav", false );
    cgs.media.watrOutSound          = trap_S_RegisterSound( "sound/player/watr_out.wav", false );
    cgs.media.watrUnSound           = trap_S_RegisterSound( "sound/player/watr_un.wav", false );
    
    cgs.media.disconnectSound       = trap_S_RegisterSound( "sound/misc/disconnect.wav", false );
    
    for( i = 0; i < 4; i++ )
    {
        Com_sprintf( name, sizeof( name ), "sound/player/footsteps/step%i.wav", i + 1 );
        cgs.media.footsteps[ FOOTSTEP_NORMAL ][ i ] = trap_S_RegisterSound( name, false );
        
        Com_sprintf( name, sizeof( name ), "sound/player/footsteps/flesh%i.wav", i + 1 );
        cgs.media.footsteps[ FOOTSTEP_FLESH ][ i ] = trap_S_RegisterSound( name, false );
        
        Com_sprintf( name, sizeof( name ), "sound/player/footsteps/splash%i.wav", i + 1 );
        cgs.media.footsteps[ FOOTSTEP_SPLASH ][ i ] = trap_S_RegisterSound( name, false );
        
        Com_sprintf( name, sizeof( name ), "sound/player/footsteps/clank%i.wav", i + 1 );
        cgs.media.footsteps[ FOOTSTEP_METAL ][ i ] = trap_S_RegisterSound( name, false );
    }
    
    for( i = 1 ; i < MAX_SOUNDS ; i++ )
    {
        soundName = ConfigString( CS_SOUNDS + i );
        
        if( !soundName[ 0 ] )
            break;
            
        if( soundName[ 0 ] == '*' )
            continue; // custom sound
            
        cgs.gameSounds[ i ] = trap_S_RegisterSound( soundName, false );
    }
    
    cgs.media.jetpackDescendSound     = trap_S_RegisterSound( "sound/upgrades/jetpack/low.wav", false );
    cgs.media.jetpackIdleSound        = trap_S_RegisterSound( "sound/upgrades/jetpack/idle.wav", false );
    cgs.media.jetpackAscendSound      = trap_S_RegisterSound( "sound/upgrades/jetpack/hi.wav", false );
    
    cgs.media.medkitUseSound          = trap_S_RegisterSound( "sound/upgrades/medkit/medkit.wav", false );
    
    cgs.media.alienEvolveSound        = trap_S_RegisterSound( "sound/player/alienevolve.wav", false );
    
    cgs.media.alienBuildableExplosion = trap_S_RegisterSound( "sound/buildables/alien/explosion.wav", false );
    cgs.media.alienBuildableDamage    = trap_S_RegisterSound( "sound/buildables/alien/damage.wav", false );
    cgs.media.alienBuildablePrebuild  = trap_S_RegisterSound( "sound/buildables/alien/prebuild.wav", false );
    
    cgs.media.humanBuildableExplosion = trap_S_RegisterSound( "sound/buildables/human/explosion.wav", false );
    cgs.media.humanBuildablePrebuild  = trap_S_RegisterSound( "sound/buildables/human/prebuild.wav", false );
    
    for( i = 0; i < 4; i++ )
        cgs.media.humanBuildableDamage[ i ] = trap_S_RegisterSound(
                va( "sound/buildables/human/damage%d.wav", i ), false );
                
    cgs.media.hardBounceSound1        = trap_S_RegisterSound( "sound/misc/hard_bounce1.wav", false );
    cgs.media.hardBounceSound2        = trap_S_RegisterSound( "sound/misc/hard_bounce2.wav", false );
    
    cgs.media.repeaterUseSound        = trap_S_RegisterSound( "sound/buildables/repeater/use.wav", false );
    
    cgs.media.buildableRepairSound    = trap_S_RegisterSound( "sound/buildables/human/repair.wav", false );
    cgs.media.buildableRepairedSound  = trap_S_RegisterSound( "sound/buildables/human/repaired.wav", false );
    
    cgs.media.lCannonWarningSound     = trap_S_RegisterSound( "models/weapons/lcannon/warning.wav", false );
    cgs.media.lCannonWarningSound2    = trap_S_RegisterSound( "models/weapons/lcannon/warning2.wav", false );
}


//===================================================================================


/*
=================
CG_RegisterGraphics

This function may execute for a couple of minutes with a slow disk.
=================
*/
void idCGameLocal::RegisterGraphics( void )
{
    S32 i;
    static UTF8* sb_nums[ 11 ] =
    {
        "gfx/2d/numbers/zero_32b",
        "gfx/2d/numbers/one_32b",
        "gfx/2d/numbers/two_32b",
        "gfx/2d/numbers/three_32b",
        "gfx/2d/numbers/four_32b",
        "gfx/2d/numbers/five_32b",
        "gfx/2d/numbers/six_32b",
        "gfx/2d/numbers/seven_32b",
        "gfx/2d/numbers/eight_32b",
        "gfx/2d/numbers/nine_32b",
        "gfx/2d/numbers/minus_32b",
    };
    static UTF8* buildWeaponTimerPieShaders[ 8 ] =
    {
        "gui/assets/neutral/1_5pie",
        "gui/assets/neutral/3_0pie",
        "gui/assets/neutral/4_5pie",
        "gui/assets/neutral/6_0pie",
        "gui/assets/neutral/7_5pie",
        "gui/assets/neutral/9_0pie",
        "gui/assets/neutral/10_5pie",
        "gui/assets/neutral/12_0pie",
    };
    static UTF8* alienAttackFeedbackShaders[ 11 ] =
    {
        "gui/assets/alien/feedback/scratch_00",
        "gui/assets/alien/feedback/scratch_01",
        "gui/assets/alien/feedback/scratch_02",
        "gui/assets/alien/feedback/scratch_03",
        "gui/assets/alien/feedback/scratch_04",
        "gui/assets/alien/feedback/scratch_05",
        "gui/assets/alien/feedback/scratch_06",
        "gui/assets/alien/feedback/scratch_07",
        "gui/assets/alien/feedback/scratch_08",
        "gui/assets/alien/feedback/scratch_09",
        "gui/assets/alien/feedback/scratch_10"
    };
    static UTF8* alienAttackFeedbackShadersFlipped[ 11 ] =
    {
        "gui/assets/alien/feedback/scratchr_00",
        "gui/assets/alien/feedback/scratchr_01",
        "gui/assets/alien/feedback/scratchr_02",
        "gui/assets/alien/feedback/scratchr_03",
        "gui/assets/alien/feedback/scratchr_04",
        "gui/assets/alien/feedback/scratchr_05",
        "gui/assets/alien/feedback/scratchr_06",
        "gui/assets/alien/feedback/scratchr_07",
        "gui/assets/alien/feedback/scratchr_08",
        "gui/assets/alien/feedback/scratchr_09",
        "gui/assets/alien/feedback/scratchr_10"
    };
    static UTF8* alienRangedAttackFeedbackShaders[ 11 ] =
    {
        "gui/assets/alien/feedback/rangefeedback_00",
        "gui/assets/alien/feedback/rangefeedback_01",
        "gui/assets/alien/feedback/rangefeedback_02",
        "gui/assets/alien/feedback/rangefeedback_03",
        "gui/assets/alien/feedback/rangefeedback_04",
        "gui/assets/alien/feedback/rangefeedback_05",
        "gui/assets/alien/feedback/rangefeedback_06",
        "gui/assets/alien/feedback/rangefeedback_07",
        "gui/assets/alien/feedback/rangefeedback_08",
        "gui/assets/alien/feedback/rangefeedback_09",
        "gui/assets/alien/feedback/rangefeedback_10"
    };
    
    // clear any references to old media
    ::memset( &cg.refdef, 0, sizeof( cg.refdef ) );
    trap_R_ClearScene( );
    
    trap_R_LoadWorldMap( cgs.mapname );
    UpdateMediaFraction( 0.66f );
    
    for( i = 0; i < 11; i++ )
        cgs.media.numberShaders[ i ] = trap_R_RegisterShader( sb_nums[ i ] );
        
    cgs.media.viewBloodShader           = trap_R_RegisterShader( "gfx/damage/fullscreen_painblend" );
    
    cgs.media.connectionShader          = trap_R_RegisterShader( "gfx/2d/net" );
    
    cgs.media.creepShader               = trap_R_RegisterShader( "creep" );
    
    cgs.media.scannerBlipShader         = trap_R_RegisterShader( "gfx/2d/blip" );
    cgs.media.scannerLineShader         = trap_R_RegisterShader( "gfx/2d/stalk" );
    
    cgs.media.tracerShader              = trap_R_RegisterShader( "gfx/misc/tracer" );
    
    cgs.media.backTileShader            = trap_R_RegisterShader( "console" );
    
    // building shaders
    cgs.media.greenBuildShader          = trap_R_RegisterShader( "gfx/misc/greenbuild" );
    cgs.media.redBuildShader            = trap_R_RegisterShader( "gfx/misc/redbuild" );
    cgs.media.humanSpawningShader       = trap_R_RegisterShader( "models/buildables/telenode/rep_cyl" );
    
    for( i = 0; i < 8; i++ )
        cgs.media.buildWeaponTimerPie[ i ] = trap_R_RegisterShader( buildWeaponTimerPieShaders[ i ] );
    for( i = 0; i < 11; i++ )
        cgs.media.alienAttackFeedbackShaders[i] = trap_R_RegisterShader( alienAttackFeedbackShaders[i] );
    for( i = 0; i < 11; i++ )
        cgs.media.alienAttackFeedbackShadersFlipped[i] = trap_R_RegisterShader( alienAttackFeedbackShadersFlipped[i] );
    for( i = 0; i < 11; i++ )
        cgs.media.alienRangedAttackFeedbackShaders[i] = trap_R_RegisterShader( alienRangedAttackFeedbackShaders[i] );
        
    // player health cross shaders
    cgs.media.healthCross               = trap_R_RegisterShader( "gui/assets/neutral/cross.tga" );
    cgs.media.healthCross2X             = trap_R_RegisterShader( "gui/assets/neutral/cross2.tga" );
    cgs.media.healthCross3X             = trap_R_RegisterShader( "gui/assets/neutral/cross3.tga" );
    cgs.media.healthCrossMedkit         = trap_R_RegisterShader( "gui/assets/neutral/cross_medkit.tga" );
    cgs.media.healthCrossPoisoned       = trap_R_RegisterShader( "gui/assets/neutral/cross_poison.tga" );
    
    // squad markers
    cgs.media.squadMarkerH              = trap_R_RegisterShader( "gui/assets/neutral/squad_h" );
    cgs.media.squadMarkerV              = trap_R_RegisterShader( "gui/assets/neutral/squad_v" );
    
    cgs.media.upgradeClassIconShader    = trap_R_RegisterShader( "icons/icona_upgrade.tga" );
    
    cgs.media.balloonShader             = trap_R_RegisterShader( "gfx/sprites/chatballoon" );
    
    cgs.media.disconnectPS              = RegisterParticleSystem( "disconnectPS" );
    
    UpdateMediaFraction( 0.7f );
    
    ::memset( cg_weapons, 0, sizeof( cg_weapons ) );
    ::memset( cg_upgrades, 0, sizeof( cg_upgrades ) );
    
    cgs.media.shadowMarkShader          = trap_R_RegisterShader( "gfx/marks/shadow" );
    cgs.media.wakeMarkShader            = trap_R_RegisterShader( "gfx/marks/wake" );
    
    cgs.media.poisonCloudPS             = RegisterParticleSystem( "firstPersonPoisonCloudPS" );
    cgs.media.poisonCloudedPS           = RegisterParticleSystem( "poisonCloudedPS" );
    cgs.media.alienEvolvePS             = RegisterParticleSystem( "alienEvolvePS" );
    cgs.media.alienAcidTubePS           = RegisterParticleSystem( "alienAcidTubePS" );
    
    cgs.media.jetPackDescendPS          = RegisterParticleSystem( "jetPackDescendPS" );
    cgs.media.jetPackHoverPS            = RegisterParticleSystem( "jetPackHoverPS" );
    cgs.media.jetPackAscendPS           = RegisterParticleSystem( "jetPackAscendPS" );
    
    cgs.media.humanBuildableDamagedPS   = RegisterParticleSystem( "humanBuildableDamagedPS" );
    cgs.media.alienBuildableDamagedPS   = RegisterParticleSystem( "alienBuildableDamagedPS" );
    cgs.media.humanBuildableHitSmallPS   = RegisterParticleSystem( "humanBuildableHitSmallPS" );
    cgs.media.alienBuildableHitSmallPS   = RegisterParticleSystem( "alienBuildableHitSmallPS" );
    cgs.media.humanBuildableHitLargePS   = RegisterParticleSystem( "humanBuildableHitLargePS" );
    cgs.media.alienBuildableHitLargePS   = RegisterParticleSystem( "alienBuildableHitLargePS" );
    cgs.media.humanBuildableDestroyedPS = RegisterParticleSystem( "humanBuildableDestroyedPS" );
    cgs.media.alienBuildableDestroyedPS = RegisterParticleSystem( "alienBuildableDestroyedPS" );
    
    cgs.media.alienBleedPS              = RegisterParticleSystem( "alienBleedPS" );
    cgs.media.humanBleedPS              = RegisterParticleSystem( "humanBleedPS" );
    
    BuildableStatusParse( "gui/assets/human/buildstat.cfg", &cgs.humanBuildStat );
    BuildableStatusParse( "gui/assets/alien/buildstat.cfg", &cgs.alienBuildStat );
    
    // register the inline models
    cgs.numInlineModels = trap_CM_NumInlineModels( );
    
    for( i = 1; i < cgs.numInlineModels; i++ )
    {
        UTF8    name[ 10 ];
        vec3_t  mins, maxs;
        S32     j;
        
        Com_sprintf( name, sizeof( name ), "*%i", i );
        
        cgs.inlineDrawModel[ i ] = trap_R_RegisterModel( name );
        trap_R_ModelBounds( cgs.inlineDrawModel[ i ], mins, maxs );
        
        for( j = 0 ; j < 3 ; j++ )
            cgs.inlineModelMidpoints[ i ][ j ] = mins[ j ] + 0.5 * ( maxs[ j ] - mins[ j ] );
    }
    
    // register all the server specified models
    for( i = 1; i < MAX_MODELS; i++ )
    {
        StringEntry modelName;
        
        modelName = ConfigString( CS_MODELS + i );
        
        if( !modelName[ 0 ] )
            break;
            
        cgs.gameModels[ i ] = trap_R_RegisterModel( modelName );
    }
    
    UpdateMediaFraction( 0.8f );
    
    // register all the server specified shaders
    for( i = 1; i < MAX_GAME_SHADERS; i++ )
    {
        StringEntry shaderName;
        
        shaderName = ConfigString( CS_SHADERS + i );
        
        if( !shaderName[ 0 ] )
            break;
            
        cgs.gameShaders[ i ] = trap_R_RegisterShader( shaderName );
    }
    
    UpdateMediaFraction( 0.9f );
    
    // register all the server specified particle systems
    for( i = 1; i < MAX_GAME_PARTICLE_SYSTEMS; i++ )
    {
        StringEntry psName;
        
        psName = ConfigString( CS_PARTICLE_SYSTEMS + i );
        
        if( !psName[ 0 ] )
            break;
            
        cgs.gameParticleSystems[ i ] = RegisterParticleSystem( ( UTF8* )psName );
    }
}


/*
=======================
CG_BuildSpectatorString
=======================
*/
void idCGameLocal::BuildSpectatorString( void )
{
    S32 i;
    
    cg.spectatorList[ 0 ] = 0;
    
    for( i = 0; i < MAX_CLIENTS; i++ )
    {
        if( cgs.clientinfo[ i ].infoValid && cgs.clientinfo[ i ].team == TEAM_NONE )
            Q_strcat( cg.spectatorList, sizeof( cg.spectatorList ), va( "%s     " S_COLOR_WHITE, cgs.clientinfo[ i ].name ) );
    }
    
    i = strlen( cg.spectatorList );
    
    if( i != cg.spectatorLen )
    {
        cg.spectatorLen = i;
        cg.spectatorWidth = -1;
    }
}



/*
===================
CG_RegisterClients
===================
*/
void idCGameLocal::RegisterClients( void )
{
    S32   i;
    
    cg.charModelFraction = 0.0f;
    
    //precache all the models/sounds/etc
    for( i = PCL_NONE + 1; i < PCL_NUM_CLASSES;  i++ )
    {
        PrecacheClientInfo( ( class_t )i, bggame->ClassConfig( ( class_t )i )->modelName, bggame->ClassConfig( ( class_t )i )->skinName );
        
        cg.charModelFraction = ( F32 )i / ( F32 )PCL_NUM_CLASSES;
        trap_UpdateScreen( );
    }
    
    cgs.media.larmourHeadSkin    = trap_R_RegisterSkin( "models/players/human_base/head_light.skin" );
    cgs.media.larmourLegsSkin    = trap_R_RegisterSkin( "models/players/human_base/lower_light.skin" );
    cgs.media.larmourTorsoSkin   = trap_R_RegisterSkin( "models/players/human_base/upper_light.skin" );
    
    cgs.media.jetpackModel       = trap_R_RegisterModel( "models/players/human_base/jetpack.md3" );
    cgs.media.jetpackFlashModel  = trap_R_RegisterModel( "models/players/human_base/jetpack_flash.md3" );
    cgs.media.battpackModel      = trap_R_RegisterModel( "models/players/human_base/battpack.md3" );
    
    cg.charModelFraction = 1.0f;
    trap_UpdateScreen( );
    
    //load all the clientinfos of clients already connected to the server
    for( i = 0; i < MAX_CLIENTS; i++ )
    {
        StringEntry  clientInfo;
        
        clientInfo = ConfigString( CS_PLAYERS + i );
        if( !clientInfo[ 0 ] )
            continue;
            
        NewClientInfo( i );
    }
    
    BuildSpectatorString( );
}

//===========================================================================

/*
=================
CG_ConfigString
=================
*/
StringEntry idCGameLocal::ConfigString( S32 index )
{
    if( index < 0 || index >= MAX_CONFIGSTRINGS )
        Error( "CG_ConfigString: bad index: %i", index );
        
    return cgs.gameState.stringData + cgs.gameState.stringOffsets[ index ];
}

//==================================================================

/*
======================
CG_StartMusic
======================
*/
void idCGameLocal::StartMusic( void )
{
    UTF8*  s;
    UTF8  parm1[ MAX_QPATH ], parm2[ MAX_QPATH ];
    
    // start the background music
    s = ( UTF8* )ConfigString( CS_MUSIC );
    Q_strncpyz( parm1, COM_Parse( &s ), sizeof( parm1 ) );
    Q_strncpyz( parm2, COM_Parse( &s ), sizeof( parm2 ) );
    
    if( strlen( parm1 ) )
    {
        trap_S_StartBackgroundTrack( parm1, parm2, 0 );
    }
}

/*
======================
CG_PlayerCount
======================
*/
S32 idCGameLocal::PlayerCount( void )
{
    S32 i, count = 0;
    
    RequestScores( );
    
    for( i = 0; i < cg.numScores; i++ )
    {
        if( cg.scores[ i ].team == TEAM_ALIENS ||
                cg.scores[ i ].team == TEAM_HUMANS )
            count++;
    }
    
    return count;
}

/*
==============================
new hud stuff ( mission pack )
==============================
*/
UTF8* idCGameLocal::GetMenuBuffer( StringEntry filename )
{
    S32           len;
    fileHandle_t  f;
    static UTF8   buf[ MAX_MENUFILE ];
    
    len = trap_FS_FOpenFile( filename, &f, FS_READ );
    
    if( !f )
    {
        trap_Print( va( S_COLOR_RED "menu file not found: %s, using default\n", filename ) );
        return NULL;
    }
    
    if( len >= MAX_MENUFILE )
    {
        trap_Print( va( S_COLOR_RED "menu file too large: %s is %i, max allowed is %i",
                        filename, len, MAX_MENUFILE ) );
        trap_FS_FCloseFile( f );
        return NULL;
    }
    
    trap_FS_Read( buf, len, f );
    buf[len] = 0;
    trap_FS_FCloseFile( f );
    
    return buf;
}

bool idCGameLocal::Asset_Parse( S32 handle )
{
    pc_token_t token;
    StringEntry tempStr;
    
    if( !trap_PC_ReadToken( handle, &token ) )
        return false;
        
    if( Q_stricmp( token.string, "{" ) != 0 )
        return false;
        
    while( 1 )
    {
        if( !trap_PC_ReadToken( handle, &token ) )
            return false;
            
        if( Q_stricmp( token.string, "}" ) == 0 )
            return true;
            
        // font
        if( Q_stricmp( token.string, "font" ) == 0 )
        {
            S32 pointSize;
            
            if( !PC_String_Parse( handle, &tempStr ) || !PC_Int_Parse( handle, &pointSize ) )
                return false;
                
            cgDC.registerFont( tempStr, pointSize, &cgDC.Assets.textFont );
            continue;
        }
        
        // smallFont
        if( Q_stricmp( token.string, "smallFont" ) == 0 )
        {
            S32 pointSize;
            
            if( !PC_String_Parse( handle, &tempStr ) || !PC_Int_Parse( handle, &pointSize ) )
                return false;
                
            cgDC.registerFont( tempStr, pointSize, &cgDC.Assets.smallFont );
            continue;
        }
        
        // font
        if( Q_stricmp( token.string, "bigfont" ) == 0 )
        {
            S32 pointSize;
            
            if( !PC_String_Parse( handle, &tempStr ) || !PC_Int_Parse( handle, &pointSize ) )
                return false;
                
            cgDC.registerFont( tempStr, pointSize, &cgDC.Assets.bigFont );
            continue;
        }
        
        // gradientbar
        if( Q_stricmp( token.string, "gradientbar" ) == 0 )
        {
            if( !PC_String_Parse( handle, &tempStr ) )
                return false;
                
            cgDC.Assets.gradientBar = trap_R_RegisterShaderNoMip( tempStr );
            continue;
        }
        
        // enterMenuSound
        if( Q_stricmp( token.string, "menuEnterSound" ) == 0 )
        {
            if( !PC_String_Parse( handle, &tempStr ) )
                return false;
                
            cgDC.Assets.menuEnterSound = trap_S_RegisterSound( tempStr, true );
            continue;
        }
        
        // exitMenuSound
        if( Q_stricmp( token.string, "menuExitSound" ) == 0 )
        {
            if( !PC_String_Parse( handle, &tempStr ) )
                return false;
                
            cgDC.Assets.menuExitSound = trap_S_RegisterSound( tempStr, true );
            continue;
        }
        
        // itemFocusSound
        if( Q_stricmp( token.string, "itemFocusSound" ) == 0 )
        {
            if( !PC_String_Parse( handle, &tempStr ) )
                return false;
                
            cgDC.Assets.itemFocusSound = trap_S_RegisterSound( tempStr, true );
            continue;
        }
        
        // menuBuzzSound
        if( Q_stricmp( token.string, "menuBuzzSound" ) == 0 )
        {
            if( !PC_String_Parse( handle, &tempStr ) )
                return false;
                
            cgDC.Assets.menuBuzzSound = trap_S_RegisterSound( tempStr, false );
            continue;
        }
        
        if( Q_stricmp( token.string, "cursor" ) == 0 )
        {
            if( !PC_String_Parse( handle, &cgDC.Assets.cursorStr ) )
                return false;
                
            cgDC.Assets.cursor = trap_R_RegisterShaderNoMip( cgDC.Assets.cursorStr );
            continue;
        }
        
        if( Q_stricmp( token.string, "fadeClamp" ) == 0 )
        {
            if( !PC_Float_Parse( handle, &cgDC.Assets.fadeClamp ) )
                return false;
                
            continue;
        }
        
        if( Q_stricmp( token.string, "fadeCycle" ) == 0 )
        {
            if( !PC_Int_Parse( handle, &cgDC.Assets.fadeCycle ) )
                return false;
                
            continue;
        }
        
        if( Q_stricmp( token.string, "fadeAmount" ) == 0 )
        {
            if( !PC_Float_Parse( handle, &cgDC.Assets.fadeAmount ) )
                return false;
                
            continue;
        }
        
        if( Q_stricmp( token.string, "shadowX" ) == 0 )
        {
            if( !PC_Float_Parse( handle, &cgDC.Assets.shadowX ) )
                return false;
                
            continue;
        }
        
        if( Q_stricmp( token.string, "shadowY" ) == 0 )
        {
            if( !PC_Float_Parse( handle, &cgDC.Assets.shadowY ) )
                return false;
                
            continue;
        }
        
        if( Q_stricmp( token.string, "shadowColor" ) == 0 )
        {
            if( !PC_Color_Parse( handle, &cgDC.Assets.shadowColor ) )
                return false;
                
            cgDC.Assets.shadowFadeClamp = cgDC.Assets.shadowColor[ 3 ];
            continue;
        }
    }
    
    return false;
}

void idCGameLocal::ParseMenu( StringEntry menuFile )
{
    pc_token_t  token;
    S32         handle;
    
    handle = trap_PC_LoadSource( menuFile );
    
    if( !handle )
        handle = trap_PC_LoadSource( "gui/testhud.menu" );
        
    if( !handle )
        return;
        
    while( 1 )
    {
        if( !trap_PC_ReadToken( handle, &token ) )
            break;
            
        //if ( Q_stricmp( token, "{" ) ) {
        //  Com_Printf( "Missing { in menu file\n" );
        //  break;
        //}
        
        //if ( menuCount == MAX_MENUS ) {
        //  Com_Printf( "Too many menus!\n" );
        //  break;
        //}
        
        if( token.string[ 0 ] == '}' )
            break;
            
        if( Q_stricmp( token.string, "assetGlobalDef" ) == 0 )
        {
            if( Asset_Parse( handle ) )
                continue;
            else
                break;
        }
        
        
        if( Q_stricmp( token.string, "menudef" ) == 0 )
        {
            // start a new menu
            Menu_New( handle );
        }
    }
    
    trap_PC_FreeSource( handle );
}

bool idCGameLocal::Load_Menu( UTF8** p )
{
    UTF8* token;
    
    token = COM_ParseExt( p, true );
    
    if( token[ 0 ] != '{' )
        return false;
        
    while( 1 )
    {
        token = COM_ParseExt( p, true );
        
        if( Q_stricmp( token, "}" ) == 0 )
            return true;
            
        if( !token || token[ 0 ] == 0 )
            return false;
            
        ParseMenu( token );
    }
    return false;
}

void idCGameLocal::LoadMenus( StringEntry menuFile )
{
    UTF8*          token;
    UTF8*          p;
    S32           len, start;
    fileHandle_t  f;
    static UTF8   buf[ MAX_MENUDEFFILE ];
    
    start = trap_Milliseconds( );
    
    len = trap_FS_FOpenFile( menuFile, &f, FS_READ );
    
    if( !f )
    {
        trap_Error( va( S_COLOR_YELLOW "menu file not found: %s, using default\n", menuFile ) );
        len = trap_FS_FOpenFile( "gui/hud.txt", &f, FS_READ );
        
        if( !f )
            trap_Error( va( S_COLOR_RED "default menu file not found: ui/hud.txt, unable to continue!\n" ) );
    }
    
    if( len >= MAX_MENUDEFFILE )
    {
        trap_Error( va( S_COLOR_RED "menu file too large: %s is %i, max allowed is %i",
                        menuFile, len, MAX_MENUDEFFILE ) );
        trap_FS_FCloseFile( f );
        return;
    }
    
    trap_FS_Read( buf, len, f );
    buf[ len ] = 0;
    trap_FS_FCloseFile( f );
    
    COM_Compress( buf );
    
    Menu_Reset( );
    
    p = buf;
    
    while( 1 )
    {
        token = COM_ParseExt( &p, true );
        
        if( !token || token[ 0 ] == 0 || token[ 0 ] == '}' )
            break;
            
        if( Q_stricmp( token, "}" ) == 0 )
            break;
            
        if( Q_stricmp( token, "loadmenu" ) == 0 )
        {
            if( Load_Menu( &p ) )
                continue;
            else
                break;
        }
    }
    
    //Com_Printf( "gui menu load time = %d milli seconds\n", trap_Milliseconds( ) - start );
}

bool idCGameLocal::OwnerDrawHandleKey( S32 ownerDraw, S32 flags, F32* special, S32 key )
{
    return false;
}


S32 idCGameLocal::FeederCount( F32 feederID )
{
    S32 i, count = 0;
    
    if( feederID == FEEDER_ALIENTEAM_LIST )
    {
        for( i = 0; i < cg.numScores; i++ )
        {
            if( cg.scores[ i ].team == TEAM_ALIENS )
                count++;
        }
    }
    else if( feederID == FEEDER_HUMANTEAM_LIST )
    {
        for( i = 0; i < cg.numScores; i++ )
        {
            if( cg.scores[ i ].team == TEAM_HUMANS )
                count++;
        }
    }
    
    return count;
}


void idCGameLocal::SetScoreSelection( void* p )
{
    menuDef_t*     menu = ( menuDef_t* )p;
    playerState_t* ps = &cg.snap->ps;
    S32           i, alien, human;
    S32           feeder;
    
    alien = human = 0;
    
    for( i = 0; i < cg.numScores; i++ )
    {
        if( cg.scores[ i ].team == TEAM_ALIENS )
            alien++;
        else if( cg.scores[ i ].team == TEAM_HUMANS )
            human++;
            
        if( ps->clientNum == cg.scores[ i ].client )
            cg.selectedScore = i;
    }
    
    if( menu == NULL )
        // just interested in setting the selected score
        return;
        
    feeder = FEEDER_ALIENTEAM_LIST;
    i = alien;
    
    if( cg.scores[ cg.selectedScore ].team == TEAM_HUMANS )
    {
        feeder = FEEDER_HUMANTEAM_LIST;
        i = human;
    }
    
    Menu_SetFeederSelection( menu, feeder, i, NULL );
}

// FIXME: might need to cache this info
clientInfo_t* idCGameLocal::InfoFromScoreIndex( S32 index, S32 team, S32* scoreIndex )
{
    S32 i, count;
    count = 0;
    
    for( i = 0; i < cg.numScores; i++ )
    {
        if( cg.scores[ i ].team == team )
        {
            if( count == index )
            {
                *scoreIndex = i;
                return &cgs.clientinfo[ cg.scores[ i ].client ];
            }
            count++;
        }
    }
    
    *scoreIndex = index;
    return &cgs.clientinfo[ cg.scores[ index ].client ];
}

bool idCGameLocal::ClientIsReady( S32 clientNum )
{
    // CS_CLIENTS_READY is a hex string, each character of which is 4 bits
    // the highest bit of the first UTF8 is a toggle for client 0, the second
    // highest for client 1, etc.
    // there are exactly four bits of information in each character
    S32 val;
    StringEntry s = ConfigString( CS_CLIENTS_READY );
    
    // select the appropriate character without passing the end of the string
    for( val = clientNum / 4; *s && val > 0; s++, val-- );
    
    // convert hex -> S32
    // FIXME: replace with sscanf when it supports width conversions (or some
    // other appropriate library function)
    if( isdigit( *s ) )
        val = *s - '0';
    else if( *s >= 'a' && *s <= 'f' )
        val = 10 + *s - 'a';
    else if( *s >= 'A' && *s <= 'F' )
        val = 10 + *s - 'A';
    else
        return false;
        
    // select appropriate bit
    return ( ( val & 1 << ( 3 - clientNum % 4 ) ) != 0 );
}

StringEntry idCGameLocal::FeederItemText( F32 feederID, S32 index, S32 column, qhandle_t* handle )
{
    S32           scoreIndex = 0;
    clientInfo_t*  info = NULL;
    S32           team = -1;
    score_t*       sp = NULL;
    bool      showIcons = false;
    
    *handle = -1;
    
    if( feederID == FEEDER_ALIENTEAM_LIST )
        team = TEAM_ALIENS;
    else if( feederID == FEEDER_HUMANTEAM_LIST )
        team = TEAM_HUMANS;
        
    info = InfoFromScoreIndex( index, team, &scoreIndex );
    sp = &cg.scores[ scoreIndex ];
    
    if( cg.intermissionStarted && ClientIsReady( sp->client ) )
        showIcons = false;
    else if( cg.snap->ps.pm_type == PM_SPECTATOR || cg.snap->ps.pm_flags & PMF_FOLLOW ||
             team == cg.snap->ps.stats[ STAT_TEAM ] || cg.intermissionStarted )
        showIcons = true;
        
    if( info && info->infoValid )
    {
        switch( column )
        {
            case 0:
                if( showIcons )
                {
                    if( sp->weapon != WP_NONE )
                        *handle = cg_weapons[ sp->weapon ].weaponIcon;
                }
                break;
                
            case 1:
                if( showIcons )
                {
                    if( sp->team == TEAM_HUMANS && sp->upgrade != UP_NONE )
                        *handle = cg_upgrades[ sp->upgrade ].upgradeIcon;
                    else if( sp->team == TEAM_ALIENS )
                    {
                        switch( sp->weapon )
                        {
                            case WP_ABUILD2:
                            case WP_ALEVEL1_UPG:
                            case WP_ALEVEL2_UPG:
                            case WP_ALEVEL3_UPG:
                                *handle = cgs.media.upgradeClassIconShader;
                                break;
                                
                            default:
                                break;
                        }
                    }
                }
                break;
                
            case 2:
                if( cg.intermissionStarted && ClientIsReady( sp->client ) )
                    return "Ready";
                break;
                
            case 3:
                return info->name;
                break;
                
            case 4:
                return va( "%d", sp->score );
                break;
                
            case 5:
                return va( "%4d", sp->time );
                break;
                
            case 6:
                if( sp->ping == -1 )
                    return "";
                    
                return va( "%4d", sp->ping );
                break;
        }
    }
    
    return "";
}

qhandle_t idCGameLocal::FeederItemImage( F32 feederID, S32 index )
{
    return 0;
}

void idCGameLocal::FeederSelection( F32 feederID, S32 index )
{
    S32 i, count;
    S32 team = ( feederID == FEEDER_ALIENTEAM_LIST ) ? TEAM_ALIENS : TEAM_HUMANS;
    count = 0;
    
    for( i = 0; i < cg.numScores; i++ )
    {
        if( cg.scores[ i ].team == team )
        {
            if( index == count )
                cg.selectedScore = i;
                
            count++;
        }
    }
}

F32 idCGameLocal::Cvar_Get( StringEntry cvar )
{
    UTF8 buff[ 128 ];
    
    ::memset( buff, 0, sizeof( buff ) );
    trap_Cvar_VariableStringBuffer( cvar, buff, sizeof( buff ) );
    return atof( buff );
}

void idCGameLocal::Text_PaintWithCursor( F32 x, F32 y, F32 scale, vec4_t color, StringEntry text, S32 cursorPos, UTF8 cursor, S32 limit, S32 style )
{
    UI_Text_Paint( x, y, scale, color, text, 0, limit, style );
}

S32 idCGameLocal::OwnerDrawWidth( S32 ownerDraw, F32 scale )
{
    switch( ownerDraw )
    {
        case CG_KILLER:
            return UI_Text_Width( GetKillerText( ), scale, 0 );
            break;
    }
    
    return 0;
}

S32 idCGameLocal::PlayCinematic( StringEntry name, F32 x, F32 y, F32 w, F32 h )
{
    return trap_CIN_PlayCinematic( name, x, y, w, h, CIN_loop );
}

void idCGameLocal::StopCinematic( S32 handle )
{
    trap_CIN_StopCinematic( handle );
}

void idCGameLocal::DrawCinematic( S32 handle, F32 x, F32 y, F32 w, F32 h )
{
    trap_CIN_SetExtents( handle, x, y, w, h );
    trap_CIN_DrawCinematic( handle );
}

void idCGameLocal::RunCinematicFrame( S32 handle )
{
    trap_CIN_RunCinematic( handle );
}

// hack to prevent warning
bool idCGameLocal::OwnerDrawVisible( S32 parameter )
{
    return false;
}

/*
=================
CG_LoadHudMenu
=================
*/
void idCGameLocal::LoadHudMenu( void )
{
    //UTF8        buff[ 1024 ];
    StringEntry  hudSet;
    
    cgDC.aspectScale = ( ( 640.0f * cgs.glconfig.vidHeight ) /
                         ( 480.0f * cgs.glconfig.vidWidth ) );
    cgDC.xscale = cgs.glconfig.vidWidth / 640.0f;
    cgDC.yscale = cgs.glconfig.vidHeight / 480.0f;
    
    cgDC.registerShaderNoMip  = &trap_R_RegisterShaderNoMip;
    cgDC.setColor             = &trap_R_SetColor;
    cgDC.drawHandlePic        = &cgameLocal.DrawPic;
    cgDC.drawStretchPic       = &trap_R_DrawStretchPic;
    cgDC.registerModel        = &trap_R_RegisterModel;
    cgDC.modelBounds          = &trap_R_ModelBounds;
    cgDC.fillRect             = &cgameLocal.FillRect;
    cgDC.drawRect             = &cgameLocal.DrawRect;
    cgDC.fillRoundedRect      = &cgameLocal.FillRoundedRect;
    cgDC.drawRoundedRect      = &cgameLocal.DrawRoundedRect;
    cgDC.drawSides            = &cgameLocal.DrawSides;
    cgDC.drawTopBottom        = &cgameLocal.DrawTopBottom;
    cgDC.clearScene           = &trap_R_ClearScene;
    cgDC.addRefEntityToScene  = &trap_R_AddRefEntityToScene;
    cgDC.renderScene          = &trap_R_RenderScene;
    cgDC.registerFont         = &trap_R_RegisterFont;
    cgDC.ownerDrawItem        = &cgameLocal.OwnerDraw;
    cgDC.getValue             = &cgameLocal.GetValue;
    cgDC.ownerDrawVisible     = &cgameLocal.OwnerDrawVisible;
    cgDC.runScript            = &cgameLocal.RunMenuScript;
    cgDC.setCVar              = trap_Cvar_Set;
    cgDC.getCVarString        = trap_Cvar_VariableStringBuffer;
    cgDC.getCVarValue         = cgameLocal.Cvar_Get;
    //cgDC.setOverstrikeMode    = &trap_Key_SetOverstrikeMode;
    //cgDC.getOverstrikeMode    = &trap_Key_GetOverstrikeMode;
    cgDC.startLocalSound      = &trap_S_StartLocalSound;
    cgDC.ownerDrawHandleKey   = &cgameLocal.OwnerDrawHandleKey;
    cgDC.feederCount          = &cgameLocal.FeederCount;
    cgDC.feederItemImage      = &cgameLocal.FeederItemImage;
    cgDC.feederItemText       = &cgameLocal.FeederItemText;
    cgDC.feederSelection      = &cgameLocal.FeederSelection;
    //cgDC.setBinding           = &trap_Key_SetBinding;
    //cgDC.getBindingBuf        = &trap_Key_GetBindingBuf;
    //cgDC.keynumToStringBuf    = &trap_Key_KeynumToStringBuf;
    //cgDC.executeText          = &trap_Cmd_ExecuteText;
    cgDC.Error                = &Com_Error;
    cgDC.Print                = &Com_Printf;
    cgDC.ownerDrawWidth       = &cgameLocal.OwnerDrawWidth;
    //cgDC.ownerDrawText        = &CG_OwnerDrawText;
    //cgDC.Pause                = &CG_Pause;
    cgDC.registerSound        = &trap_S_RegisterSound;
    cgDC.startBackgroundTrack = &trap_S_StartBackgroundTrack;
    cgDC.stopBackgroundTrack  = &trap_S_StopBackgroundTrack;
    cgDC.playCinematic        = &cgameLocal.PlayCinematic;
    cgDC.stopCinematic        = &cgameLocal.StopCinematic;
    cgDC.drawCinematic        = &cgameLocal.DrawCinematic;
    cgDC.runCinematicFrame    = &cgameLocal.RunCinematicFrame;
    cgDC.getFileList          = &trap_FS_GetFileList;
    cgDC.hudloading           = true;
    Init_Display( &cgDC );
    
    Menu_Reset( );
    
//  trap_Cvar_VariableStringBuffer( "cg_hudFiles", buff, sizeof( buff ) );
//  hudSet = buff;

//  if( !cg_hudFilesEnable.integer || hudSet[ 0 ] == '\0' )
    hudSet = "gui/hud.txt";
    
    LoadMenus( hudSet );
    cgDC.hudloading = false;
}

void idCGameLocal::AssetCache( void )
{
    S32 i;
    
    cgDC.Assets.gradientBar         = trap_R_RegisterShaderNoMip( ASSET_GRADIENTBAR );
    cgDC.Assets.scrollBar           = trap_R_RegisterShaderNoMip( ASSET_SCROLLBAR );
    cgDC.Assets.scrollBarArrowDown  = trap_R_RegisterShaderNoMip( ASSET_SCROLLBAR_ARROWDOWN );
    cgDC.Assets.scrollBarArrowUp    = trap_R_RegisterShaderNoMip( ASSET_SCROLLBAR_ARROWUP );
    cgDC.Assets.scrollBarArrowLeft  = trap_R_RegisterShaderNoMip( ASSET_SCROLLBAR_ARROWLEFT );
    cgDC.Assets.scrollBarArrowRight = trap_R_RegisterShaderNoMip( ASSET_SCROLLBAR_ARROWRIGHT );
    cgDC.Assets.scrollBarThumb      = trap_R_RegisterShaderNoMip( ASSET_SCROLL_THUMB );
    cgDC.Assets.sliderBar           = trap_R_RegisterShaderNoMip( ASSET_SLIDER_BAR );
    cgDC.Assets.sliderThumb         = trap_R_RegisterShaderNoMip( ASSET_SLIDER_THUMB );
    cgDC.Assets.cornerIn            = trap_R_RegisterShaderNoMip( ASSET_CORNERIN );
    cgDC.Assets.cornerOut           = trap_R_RegisterShaderNoMip( ASSET_CORNEROUT );
    
    if( cg_emoticons.integer )
    {
        cgDC.Assets.emoticonCount = bggame->LoadEmoticons( cgDC.Assets.emoticons,
                                    cgDC.Assets.emoticonWidths );
    }
    else
        cgDC.Assets.emoticonCount = 0;
        
    for( i = 0; i < cgDC.Assets.emoticonCount; i++ )
    {
        cgDC.Assets.emoticonShaders[ i ] = trap_R_RegisterShaderNoMip( va( "emoticons/%s_%dx1.tga", cgDC.Assets.emoticons[ i ], cgDC.Assets.emoticonWidths[ i ] ) );
    }
}

idCGameLocal::idCGameLocal( void )
{

}

idCGameLocal::~idCGameLocal( void )
{

}

/*
=================
CG_Init

Called after every level change or subsystem restart
Will perform callbacks to make the loading info screen update.
=================
*/
void idCGameLocal::Init( S32 serverMessageNum, S32 serverCommandSequence, S32 clientNum, bool demoPlayback )
{
    StringEntry  s;
    
    // clear everything
    ::memset( &cgs, 0, sizeof( cgs ) );
    ::memset( &cg, 0, sizeof( cg ) );
    ::memset( &cg.pmext, 0, sizeof( cg.pmext ) );
    ::memset( cg_entities, 0, sizeof( cg_entities ) );
    
    cg.clientNum = clientNum;
    
    cgs.processedSnapshotNum = serverMessageNum;
    cgs.serverCommandSequence = serverCommandSequence;
    
    // get the rendering configuration from the client system
    trap_GetGlconfig( &cgs.glconfig );
    cgs.screenXScale = cgs.glconfig.vidWidth / 640.0f;
    cgs.screenYScale = cgs.glconfig.vidHeight / 480.0f;
    
    // load a few needed things before we do any screen updates
    cgs.media.whiteShader     = trap_R_RegisterShader( "white" );
    cgs.media.charsetShader   = trap_R_RegisterShader( "gfx/2d/bigchars" );
    cgs.media.outlineShader   = trap_R_RegisterShader( "outline" );
    
    // load overrides
    bggame->InitClassConfigs( );
    bggame->InitBuildableConfigs( );
    bggame->InitAllowedGameElements( );
    
    // Dynamic memory
    bggame->InitMemory( );
    
    RegisterCvars( );
    
    InitConsoleCommands( );
    
    String_Init( );
    
    AssetCache( );
    LoadHudMenu( );
    cg.feedbackAnimation = 0;
    cg.feedbackAnimationType = 0;
    
    cg.weaponSelect = WP_NONE;
    
    // old servers
    
    // get the gamestate from the client system
    trap_GetGameState( &cgs.gameState );
    
    // check version
    s = ConfigString( CS_GAME_VERSION );
    
    if( strcmp( s, GAME_VERSION ) )
        Error( "Client/Server game mismatch: %s/%s", GAME_VERSION, s );
        
    s = ConfigString( CS_LEVEL_START_TIME );
    cgs.levelStartTime = atoi( s );
    
    ParseServerinfo( );
    
    // load the new map
    trap_CM_LoadMap( cgs.mapname );
    
    cg.loading = true;   // force players to load instead of defer
    
    LoadTrailSystems( );
    UpdateMediaFraction( 0.05f );
    
    LoadParticleSystems( );
    UpdateMediaFraction( 0.05f );
    
    RegisterSounds( );
    UpdateMediaFraction( 0.60f );
    
    RegisterGraphics( );
    UpdateMediaFraction( 0.90f );
    
    InitWeapons( );
    UpdateMediaFraction( 0.95f );
    
    InitUpgrades( );
    UpdateMediaFraction( 1.0f );
    
    InitBuildables( );
    
    cgs.voices = bggame->VoiceInit( );
    bggame->PrintVoices( cgs.voices, cg_debugVoices.integer );
    
    RegisterClients( );   // if low on memory, some clients will be deferred
    
    cg.loading = false;  // future players will be deferred
    
    InitMarkPolys( );
    
    // remove the last loading update
    cg.infoScreenText[ 0 ] = 0;
    
    // Make sure we have update values (scores)
    SetConfigValues( );
    
    StartMusic( );
    
    ShaderStateChanged( );
    
    trap_S_ClearLoopingSounds( );
}

/*
=================
idCGameLocal::Shutdown

Called before every level change or subsystem restart
=================
*/
void idCGameLocal::Shutdown( void )
{
    collisionModelManager->FreeMap();
    
    // some mods may need to do cleanup work here,
    // like closing files or archiving session data
    
    // Reset cg_version
    trap_Cvar_Set( "cg_version", "" );
}

bool idCGameLocal::CheckExecKey( S32 key )
{
    //if( !cg.showFireteamMenu )
    //{
    //    return false;
    //}
    
    return true; //CG_FireteamCheckExecKey( key, false );
}

void idCGameLocal::ConsoleText( void )
{
    UTF8 buffer[ BIG_INFO_STRING ];
    
    trap_LiteralArgs( buffer, BIG_INFO_STRING );
    
    if( !buffer[ 0 ] )
    {
        cg.consoleText[ 0 ] = '\0';
        cg.numConsoleLines = 0;
        return;
    }
    
    if( cg.numConsoleLines == MAX_CONSOLE_LINES )
    {
        RemoveNotifyLine( );
    }
    
    Q_strcat( cg.consoleText, MAX_CONSOLE_TEXT, buffer );
    cg.consoleLines[ cg.numConsoleLines ].time = cg.time;
    cg.consoleLines[ cg.numConsoleLines ].length = strlen( buffer );
    cg.numConsoleLines++;
}

bool idCGameLocal::WantsBindKeys( void )
{
    return ( g_waitingForKey && g_bindItem ) ? true : false;
}

