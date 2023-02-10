/*
 * Seven Kingdoms 2: The Fryhtan War
 *
 * Copyright 1999 Enlight Software Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

// Filename    : ODPLAY.H
// Description : Header file of MultiPlayerDP (DirectPlay)
// Owner       : Gilbert

#ifndef __ODPLAY_H
#define __ODPLAY_H

#ifndef IMAGICMP

#include <odynarrb.h>
#include <mptypes.h>

extern GUID GAME_GUID;
extern void* PLAYER_MESSAGE_HANDLE;

#define MP_SERVICE_PROVIDER_NAME_LEN 64
#define MP_SESSION_NAME_LEN 64
#define MP_PASSWORD_LEN 32
#define MP_FRIENDLY_NAME_LEN 20
#define MP_FORMAL_NAME_LEN 64
#define MP_RECV_BUFFER_SIZE 0x2000

typedef struct
{
	GUID guidInstance;
} DPLCONNECTION, DPSESSIONDESC2, *LPDIRECTPLAY4A, *LPDIRECTPLAYLOBBY3A;

struct DPServiceProvider
{
	GUID guid;
	char description[MP_SERVICE_PROVIDER_NAME_LEN+1];
	char long_description[MP_SERVICE_PROVIDER_NAME_LEN+1];
	char connection_ptr[0x100];		// to be passed to DirectPlay4::InitConnection

	char *name_str() { return description; }
	char *name_str_long() { return long_description; }
	GUID service_id() { return guid; }
};

struct DPSessionDesc : public DPSESSIONDESC2
{
	char session_name[MP_SESSION_NAME_LEN+1];
	char pass_word[MP_SESSION_NAME_LEN+1];

	DPSessionDesc() {};
	DPSessionDesc(const DPSESSIONDESC2 &) {};
	DPSessionDesc(const DPSessionDesc &) {};
	DPSessionDesc& operator= (const DPSessionDesc &) {};
	void after_copy() {};
	DPSessionDesc *before_use() { return NULL; }

	char *name_str() { return session_name; };
	GUID session_id() { return guidInstance; }
};


struct DPPlayer
{
	DPID	player_id;
	char	friendly_name[MP_FRIENDLY_NAME_LEN+1];
	char	formal_name[MP_FORMAL_NAME_LEN+1];
	char	connecting;		// initially set to 1, 
	                     // clear after DPSYS_DESTROYPLAYERORGROUP received from DirectPlay

	DPID	pid()			{ return player_id; }
	char *friendly_name_str() { return friendly_name; }
	char *formal_name_str() { return formal_name; }
};

class MultiPlayerDP
{
public:
	int						init_flag;
	int						lobbied_flag;
	DynArrayB				service_providers;		// array of DPServiceProvider
	DynArrayB				current_sessions;			// array of DPSessionDesc
	int						co_inited;
//	LPDIRECTPLAY			direct_play1;
	LPDIRECTPLAY4A			direct_play4;
	LPDIRECTPLAYLOBBY3A	direct_play_lobby;
	DPSessionDesc			joined_session;
	DPLCONNECTION *		connection_string;		// only when lobbied

	DPID						my_player_id;
	int						host_flag;
	DynArrayB				player_pool;

	char *					recv_buffer;
	DWORD						recv_buffer_size;

public:
	MultiPlayerDP() : service_providers(sizeof(DPServiceProvider), 10 ),
			  current_sessions(sizeof(DPSessionDesc), 10 ),
			  player_pool(sizeof(DPPlayer), 8 ),
			  init_flag(0) {};
	~MultiPlayerDP() {};
	void pre_init() {};
	void init(GUID serviceProviderGuid) {};
	void deinit() {};

	// ------- functions on DirectPlayLobby -------- //
	void	init_lobbied(int maxPlayers, char *cmdLine) {};
	int	is_lobbied() { return 0; }		// return 0=not lobbied, 1=auto create, 2=auto join, 4=selectable
	char *get_lobbied_name() { return NULL;	}		// return 0 if not available
	int	send_lobby(LPVOID lpData, DWORD dataSize) { return 0; }
	char *receive_lobby(LPDWORD recvLen) { return NULL; }

	// ------- functions on service provider ------ //
	void	poll_service_providers() {};								// can be called before init
	DPServiceProvider *get_service_provider(int i) { return NULL; }		// can be called before init

	// ------- functions on session --------//
	int	poll_sessions() { return 0; }
	void	sort_sessions(int sortType) {};
	DPSessionDesc *get_session(int i) { return NULL; }
	int	create_session(char *sessionName, int maxPlayers) { return 0; }
	int	join_session(DPSessionDesc* sessionDesc) { return 0; }
	int	join_session(int currentSessionIndex ) { return 0; }
	void	close_session() {};
	void	disable_join_session() {};		// so that new player cannot join

	// -------- functions on player management -------//
	int	create_player(char *friendlyName, char *formalName,
		LPVOID lpData=NULL, DWORD dataSize=0, DWORD flags=0) { return 0; }
	void	destroy_player( DPID playerId ) {};
	void	poll_players() {};
	DPPlayer *get_player(int i) { return NULL; }
	DPPlayer *search_player(DPID player_id) { return NULL; }
	DPPlayer *search_player(char *name) { return NULL; }
	int	is_host(DPID playerId) { return 0; }
	int	am_I_host() { return 0; }
	int	is_player_connecting(DPID playerId) { return 0; }

	// ------- functions on data management ------//
	// remote data (public) : each player has one data to the public
	int	update_public_data(DPID, LPVOID, DWORD ) { return 0; }
	int	retrieve_public_data(DPID, LPVOID, LPDWORD) { return 0; }
	// local data (private) : each player keeps a data on each other player
	int	update_private_data(DPID, LPVOID, DWORD) { return 0; }
	int	retrieve_private_data(DPID, LPVOID, LPDWORD) { return 0; }

	// ------- functions on message passing ------//
	int	send(DPID toId, LPVOID lpData, DWORD dataSize) { return 0; }
	void	begin_stream(DPID toID) {};
	int	send_stream(DPID toId, LPVOID lpData, DWORD dataSize) { return 0; }
	void	end_stream(DPID toID) {};
	int	get_msg_count() { return 0; }
	char *receive(LPDPID from, LPDPID to, LPDWORD recvLen, int *sysMsgCount=0) { return NULL; }

	void	before_receive()		{} // dummy function to compatible with IMMPLAY, call before receive
	void	after_send()			{}	// dummy function to compatible with IMMPLAY, call after send

protected:
	void	handle_system_msg(LPVOID, DWORD ) {};
	void	handle_lobby_system_msg(LPVOID, DWORD) {};
};

extern MultiPlayerDP mp_dp;
#define mp_obj mp_dp

#endif	// IMAGICMP
#endif	// __ODPLAY_H
