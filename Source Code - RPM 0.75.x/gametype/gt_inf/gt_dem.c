// Copyright (C) 2001-2002 Raven Software.
//

#include "gt_local.h"

							
#define TRIGGER_DEMOSITE_1		200
#define TRIGGER_DEMOSITE_2		201

#define ITEM_BOMB				100
#define ITEM_PLANTED_BOMB		301

void	GT_UpdateCvars  ( void );
void	GT_RegisterCvars	( void );

//gametypeLocals_t	gametype;

extern vmCvar_t	gt_bombTimer;
extern vmCvar_t	gt_bombDefuseTime;
extern vmCvar_t	gt_bombPlantTime;
extern vmCvar_t	gt_customType;
extern vmCvar_t	gt_simpleScoring;

void GT_Init_DEM ( void )
{
	gtTriggerDef_t	triggerDef;
	gtItemDef_t		itemDef;

	memset ( &gametype, 0, sizeof(gametype) );

	// Register all cvars for this gametype
	//GT_RegisterCvars ( );

	gametype.bombTakenSound    = trap_Cmd_RegisterGlobalSound ( "sound/ctf_flag.mp3" );
	gametype.bombExplodedSound = trap_Cmd_RegisterGlobalSound ( "sound/ctf_win.mp3" );
	gametype.bombPlantedSound  = trap_Cmd_RegisterGlobalSound ( "sound/ctf_base.mp3" );
	gametype.caseReturnSound  = trap_Cmd_RegisterGlobalSound ( "sound/ctf_return.mp3" );

	//gametype.bombBeepSound	   = trap_Cmd_RegisterGlobalSound ( "sound/misc/c4/beep" );
	//gametype.bombBeepSound	   = trap_Cmd_RegisterGlobalSound ( "sound/ambience/generic/alarm04" );
	//gametype.bombBeepSound	   = trap_Cmd_RegisterGlobalSound ( "sound/ambience/generic/alarm05" );
	gametype.bombBeepSound	   = trap_Cmd_RegisterGlobalSound ( "sound/ambience/nyc/beep2.mp3" );

	gametype.bombExplodeEffect = trap_Cmd_RegisterEffect ( "explosions/mushroom_explosion.efx" );

	// Register the triggers
	memset ( &triggerDef, 0, sizeof(triggerDef) );
	triggerDef.use		= qtrue;
	triggerDef.useTime	= gt_bombPlantTime.integer * 600;
	//triggerDef.useIcon	= trap_Cmd_RegisterIcon ( "gfx/menus/hud/tnt" );
	//triggerDef.useSound = trap_Cmd_RegisterGlobalSound ( "sound/misc/c4/c4_loop" );
	//triggerDef.size		= sizeof(triggerDef);
	trap_Cmd_RegisterTrigger ( TRIGGER_DEMOSITE_1, "briefcase_destination_blue", &triggerDef );
	trap_Cmd_RegisterTrigger ( TRIGGER_DEMOSITE_2, "briefcase_destination_red", &triggerDef );

	//Regitser Items
	memset ( &itemDef, 0, sizeof(itemDef) );
//	itemDef.size = sizeof(itemDef);
	itemDef.use = qtrue;
	itemDef.useTime = gt_bombDefuseTime.integer * 600;
//	itemDef.useSound = triggerDef.useSound;
//	itemDef.useIcon	= trap_Cmd_RegisterIcon ( "gfx/menus/hud/wire_cutters" );

	trap_Cmd_RegisterItem ( ITEM_BOMB, "briefcase", &itemDef );
	//trap_Cmd_RegisterItem ( ITEM_PLANTED_BOMB, "armed_c4", &itemDef );
	//trap_Cmd_RegisterItem ( ITEM_PLANTED_BOMB, "briefcase", &itemDef );

//	gametype.iconBombPlanted[0] = trap_Cmd_RegisterIcon ( "gfx/menus/hud/dem_planted" );
//	gametype.iconBombPlanted[1] = trap_Cmd_RegisterIcon ( "gfx/menus/hud/dem_planted1" );
//	gametype.iconBombPlanted[2] = trap_Cmd_RegisterIcon ( "gfx/menus/hud/dem_planted2" );
//	gametype.iconBombPlanted[3] = trap_Cmd_RegisterIcon ( "gfx/menus/hud/dem_planted3" );
//	gametype.iconBombPlanted[4] = trap_Cmd_RegisterIcon ( "gfx/menus/hud/dem_planted4" );
//	gametype.iconBombPlanted[5] = trap_Cmd_RegisterIcon ( "gfx/menus/hud/dem_planted5" );
//	gametype.iconBombPlanted[6] = trap_Cmd_RegisterIcon ( "gfx/menus/hud/dem_planted6" );
}


void GT_RunFrame_DEM ( int time )
{
	gametype.time = time;

	if(gametype.paused && gametype.bombPlantTime )
	{
		gametype.bombPlantTime  += 50;
	}
	else
	{
		if ( gametype.bombPlantTime )
		{
			static const int slowTime = 1000;
			static const int fastTime = 100;

			if ( !gametype.bombBeepTime || gametype.time > gametype.bombBeepTime ) 
			{
				float addTime;

				addTime = (float)(gametype.bombPlantTime - gametype.time) / (float)(gt_bombTimer.integer * 1000);
				addTime = fastTime + (addTime * (float)(slowTime - fastTime) );

				gametype.bombBeepTime = gametype.time + (int)addTime;

				trap_Cmd_StartGlobalSound ( gametype.bombBeepSound);
				addTime = (float)(gametype.bombPlantTime - gametype.time) / (float)(gt_bombTimer.integer * 1000);
				addTime = 6.0f - 6.0f * addTime ;
			}
		}

		if ( gametype.bombPlantTime && gametype.time > gametype.bombPlantTime )
		{
			//static vec3_t up = {0,0,1};
			
			//trap_Cmd_TextMessage ( -1, va("Origin: %f %f %f", gametype.bombPlantOrigin[0], gametype.bombPlantOrigin[1], gametype.bombPlantOrigin[2] ));
			//trap_Cmd_TextMessage ( -999, gametype.bombPlantTarget);
			//trap_Cmd_PlayEffect ( gametype.bombExplodeEffect, gametype.bombPlantOrigin, up );
			// GodOrDevil - use radio message hack since its not in 1.00
			//trap_Cmd_UseTargets ( -999, gametype.bombPlantTarget );
			trap_Cmd_RadioMessage ( -999, gametype.bombPlantTarget );
			trap_Cmd_RadioMessage ( -777, va("%i", gametype.bombPlantClient) );
			//END
			trap_Cmd_ResetItem ( ITEM_PLANTED_BOMB );
			trap_Cmd_ResetItem ( ITEM_BOMB );

			if ( !gametype.roundOver )
			{
				trap_Cmd_AddTeamScore ( TEAM_BLUE, 1 );
				trap_Cmd_TextMessage ( -1, "Blue team has destroyed the target!" );
				trap_Cmd_RadioMessage ( -888, va("%i",TEAM_BLUE) );
				trap_Cmd_StartGlobalSound ( gametype.bombExplodedSound );
				trap_Cmd_Restart ( 5 );
			}

			// Give the guy who planted it some props
			if ( !gt_simpleScoring.integer )
			{
				trap_Cmd_AddClientScore ( gametype.bombPlantClient, 10 );
			}

			gametype.bombPlantTime = 0;
			gametype.roundOver = qtrue;
		}
	}
	GT_UpdateCvars ( );
}


int GT_Event_DEM ( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 )
{
	switch ( cmd )
	{
		// GodOrDevil - arrrgggg
		//case GTEV_SETORIGIN:
		//{
		//	VectorCopy((float*)arg0, gametype.bombPlantOrigin);
		//	break;
		//}
		//END
		//case GTEV_RESET_BOMB:
		//{
		//	trap_Cmd_ResetItem ( ITEM_BOMB );
		//	trap_Cmd_ResetItem ( ITEM_PLANTED_BOMB );	
		//	gametype.bombPlantTime = 0;
		//	gametype.bombPlantedTeam = -1;
		//}
		case GTEV_START_PAUSE:
 			gametype.paused = qtrue;
			return 0;
		case GTEV_END_PAUSE:
			gametype.paused = qfalse;
			return 0;

		case GTEV_ITEM_DEFEND:
			if ( !gt_simpleScoring.integer )
			{
				trap_Cmd_AddClientScore ( arg1, 5 );
			}
			return 0;

		case GTEV_ITEM_STUCK:
			//trap_Cmd_ResetItem ( ITEM_BOMB );
			trap_Cmd_TextMessage ( -1, "^3The bomb has been lost!" );
			trap_Cmd_StartGlobalSound ( gametype.caseReturnSound );
			return 0;

		case GTEV_ITEM_DROPPED:
		{
			char clientname[MAX_QPATH];
			trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
			trap_Cmd_TextMessage ( -1, va("%s has ^$dropped the bomb!", clientname ) );
			break;
		}

		case GTEV_ITEM_TOUCHED:			
			if ( arg0 == ITEM_BOMB && arg2 == TEAM_BLUE && !gametype.bombPlantTime && !gametype.roundOver)
			{
				char clientname[MAX_QPATH];
				trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
				trap_Cmd_TextMessage ( -1, va("%s ^4has taken the bomb!", clientname ) );
				trap_Cmd_StartGlobalSound ( gametype.bombTakenSound );
				trap_Cmd_RadioMessage ( arg1, "got_it" );
				return 1;
			}

			return 0;
		case GTEV_TIME_EXPIRED:
			// GodOrDevil
			if(gametype.bombPlantTime)
				return 1;
			// End
			trap_Cmd_TextMessage ( -1, "Red team has defended the bomb site!" );
			trap_Cmd_AddTeamScore ( TEAM_RED, 1 );
			trap_Cmd_RadioMessage ( -888, va("%i",TEAM_RED ));
			gametype.bombPlantTime = 0;
			gametype.bombBeepTime = 0;
			trap_Cmd_Restart ( 5 );
			break;

		case GTEV_TEAM_ELIMINATED:
			switch ( arg0 )
			{
				case TEAM_RED:
					trap_Cmd_TextMessage ( -1, "Red team eliminated!" );
					trap_Cmd_AddTeamScore ( TEAM_BLUE, 1 );
					trap_Cmd_RadioMessage ( -888, va("%i",TEAM_BLUE) );
					trap_Cmd_ResetItem ( ITEM_PLANTED_BOMB );
					trap_Cmd_ResetItem ( ITEM_BOMB );
					gametype.bombPlantTime = 0;
					gametype.bombBeepTime = 0;
					trap_Cmd_Restart ( 5 );
					gametype.roundOver = qtrue;
					break;

				case TEAM_BLUE:

					// If the bomb is planted the defending team MUST defuse it.
					if ( !gametype.bombPlantTime )
					{
						trap_Cmd_TextMessage ( -1, "Blue team eliminated!" );
						trap_Cmd_AddTeamScore ( TEAM_RED, 1 );
						trap_Cmd_RadioMessage ( -888, va("%i",TEAM_RED) );
						trap_Cmd_ResetItem ( ITEM_PLANTED_BOMB );
						trap_Cmd_ResetItem ( ITEM_BOMB );
						gametype.bombPlantTime = 0;
						gametype.bombBeepTime = 0;
						trap_Cmd_Restart ( 5 );
						gametype.roundOver = qtrue;
					}
					break;
			}
			break;

		case GTEV_ITEM_CANBEUSED:
			//if ( arg0 == ITEM_PLANTED_BOMB /*&& arg2 == TEAM_RED*/ )
			if ( arg0 == ITEM_BOMB && gametype.bombPlantTime && arg2 == TEAM_RED )
			{
				return 1;
			}
			return 0;


		case GTEV_TRIGGER_CANBEUSED:
			if ( trap_Cmd_DoesClientHaveItem ( arg1, ITEM_BOMB ) )
			{
				return 1;
			}
			return 0;			

		case GTEV_ITEM_USED:
		{
			char	name[128];
			trap_Cmd_ResetItem ( ITEM_PLANTED_BOMB );
			trap_Cmd_AddTeamScore ( TEAM_RED, 1 );
			trap_Cmd_RadioMessage ( -888, va("%i", TEAM_RED ));
			trap_Cmd_GetClientName ( arg1, name, 128 );
			trap_Cmd_TextMessage ( -1, va("%s ^$has defused the bomb!", name ) );
			trap_Cmd_StartGlobalSound ( gametype.bombExplodedSound );
			trap_Cmd_RadioMessage ( -555, va("%i", gametype.bombPlantClient) );
			trap_Cmd_Restart ( 5 );
			gametype.roundOver = qtrue;
			gametype.bombPlantTime = 0;
			gametype.bombBeepTime = 0;


			// Give the guy who defused it some props
			if ( !gt_simpleScoring.integer )
			{
				trap_Cmd_AddClientScore ( arg1, 5 );
			}

			return 1;
		}


		case GTEV_TRIGGER_USED:
		{
			char	name[128];			
			gametype.bombPlantTime = time + gt_bombTimer.integer * 1000;
			gametype.bombPlantClient = arg1;
			trap_Cmd_GetClientName ( arg1, name, 128 );
			trap_Cmd_TextMessage ( -1, va("%s ^4has planted the bomb!", name ) );
			// this is not in 1.00, so gonna reuse GetClientName
			//trap_Cmd_GetTriggerTarget ( arg0, gametype.bombPlantTarget, sizeof(gametype.bombPlantTarget) );
			trap_Cmd_GetClientName( arg0, gametype.bombPlantTarget, sizeof(gametype.bombPlantTarget) );
			//
			trap_Cmd_StartGlobalSound ( gametype.bombPlantedSound );
			return 0;
		}

	}

	return 0;
}

