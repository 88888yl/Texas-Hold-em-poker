/** 
 * Create by myl 
 * in 2015-5-13 
 */

#ifndef _GAME_H_
#define _GAME_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

/** Function return flag */
#define SUCCESS 1
#define FAILED  0

/** my own id */
static int my_player_id = 0;

/** Flags, used for check player info */
#define SEAT     8
#define BLIND    16
#define HOLD     32
#define FLOP     64
#define TURN     128
#define RIVER    256
#define INQUIRE  512
#define WIN 	 1024
#define NOTIFY   2048
#define SHOWDOWN 4096

#define NORMAL   (SEAT | 0)
#define BUTTON   (SEAT | 1)
#define SM_BLIND (SEAT | 2)
#define BG_BLIND (SEAT | 4)

#define SMALL    (BLIND | 0)
#define BIG      (BLIND | 1)

#define HOLD_1   (HOLD | 0)
#define HOLD_2   (HOLD | 1)

#define FLOP_1   (FLOP | 0)
#define FLOP_2   (FLOP | 1)
#define FLOP_3   (FLOP | 3)

#define IS_SEAT(player_flag) 		which_msg(player_flag, 3)
#define IS_BLIND(player_flag) 		which_msg(player_flag, 4)
#define IS_HOLD(player_flag) 		which_msg(player_flag, 5)
#define IS_FLOP(player_flag) 		which_msg(player_flag, 6)
#define IS_TURN(player_flag) 		which_msg(player_flag, 7)
#define IS_RIVER(player_flag) 		which_msg(player_flag, 8)
#define IS_INQUIRE(player_flag) 	which_msg(player_flag, 9)
#define IS_WIN(player_flag) 		which_msg(player_flag, 10)
#define IS_NOTIFY(player_flag) 		which_msg(player_flag, 11)
#define IS_SHOWDOWN(player_flag) 	which_msg(player_flag, 12)

static inline int which_msg(int player_flag, int num) {
	int temp_flag = player_flag;
	if ((temp_flag >> num) & 1)
		return 1;
	return 0;
}

/**
 * In order to avoid receive the same message more than one time,  
 * every board will restore msg flag to zero to receive message.
 */
static int seat_checked  	= 0;
static int blind_checked 	= 0;
static int hold_checked  	= 0;
static int flop_checked  	= 0;
static int turn_checked  	= 0;
static int river_checked 	= 0;
static int win_checked   	= 0;
static int showdown_checked = 0;
static int inquire_checked  = 0;

/** socket buffers */
#define BUFFER_MAX_LEN 2048
char send_buffer[BUFFER_MAX_LEN];
char recv_buffer[BUFFER_MAX_LEN];
char msg_buffer[BUFFER_MAX_LEN];

/** 
 * Every inquire(betting_rounds) choose a bet, 
 * expect how much money you raise which will be done in another function.
 */
#define CHECK_ACTION 	0
#define CALL_ACTION 	1
#define ALLIN_ACTION 	2
#define FOLD_ACTION 	3
#define RAISE_ACTION 	4
#define BLIND_ACTION	10
static const char *action[] = { "check \n", "call \n", "all_in \n", "fold \n" , "raise \n"};

/** Every hand has 4 times to bet. */
#define PRE_FLOP    1
#define FLOP_ROUND  2
#define TURN_ROUND  3
#define RIVER_ROUND 4
static int betting_rounds = 0;
static int inquire_rounds = 0;

/** Receive message flags */
static const char* seat_tag    			= "/seat \n";
static const char* blind_tag   			= "/blind \n";
static const char* hold_tag   			= "/hold \n";
static const char* inquire_tag 			= "/inquire \n";
static const char* flop_tag    			= "/flop \n";
static const char* turn_tag    			= "/turn \n";
static const char* river_tag   			= "/river \n";
static const char* showdown_tag 		= "/showdown \n";
static const char* notify_tag			= "/notify \n";
static const char* potwin_tag 			= "/pot-win \n";
static const char* seat_begin_tag    	= "taes";
static const char* blind_begin_tag   	= "dnilb";
static const char* hold_begin_tag   	= "dloh";
static const char* inquire_begin_tag 	= "eriuqni";
static const char* flop_begin_tag    	= "polf";
static const char* turn_begin_tag    	= "nrut";
static const char* river_begin_tag   	= "revir";
static const char* showdown_begin_tag   = "nwodwohs";
static const char* notify_begin_tag		= "yfiton";
static const char* potwin_begin_tag 	= "niw-top";
static const char* gameover = "game-over \n";

/** Poker info without red joker and black joker, A is 14. */
enum color { CLUBS = 0, DIAMONDS, HEARTS, SPADES };
enum point { 
	POINT_2 = 2, POINT_3, POINT_4, 
	POINT_5, POINT_6, POINT_7, POINT_8, POINT_9, 
	POINT_10, POINT_J, POINT_Q, POINT_K, POINT_A
};

static const char* color_array[] = { "CLUBS", "DIAMONDS", "HEARTS", "SPADES" };
static const char* point_array[] = {
	"", "", "2", "3", "4", "5", "6", "7", "8","9", "10", "J", "Q", "K", "A"
};
static const char* nut_array[] = {
	"", "ROYAL_STRAIGHT_FLUSH", "STRAIGHT_FLUSH", "FOUR_OF_A_KIND",
	"FULL_HOUSE", "FLUSH", "STRAIGHT", "THREE_OF_A_KIND", "TWO_PAIR",
	"ONE_PAIR", "HIGH_CARD", "NORMAL_CARDS"
};

/** Analyse current cards. */
enum face_of_cards {
	royal_straight_flush = 1,
	straight_flush,
	four_of_a_kind,
	full_house,
	flush,
	straight,
	three_of_a_kind,
	two_pair,
	one_pair,
	high_card,
	normal_cards
};

/** my location */
enum seat_info {
	SMALL_BLIND_SEAT = 0,
	BIG_BLIND_SEAT,
	FRONT_SEAT,
	MIDDLE_SEAT,
	BACK_SEAT
};
enum seat_info my_seat;

/** Receive data struct */
struct player_info {
	int player_id;
	int jetton;
	int money;
};
struct seat {
	int button;
	int small_blind_id;
	int big_blind_id;
	int cur_player_num;
	struct player_info players[8];
};
struct blind {
	int small_blind;
	int big_blind;
};
struct card {
	enum color m_color;
	enum point m_point;
};
struct player_state {
	struct player_info m_player_info;
	int bet;
	int action_num;
};
struct inquire_info {
	struct player_state cur_states[8];
	int total_pot;
	int cur_player_num;
};
struct notify_info {
	struct player_state cur_states[8];
	int total_pot;
	int cur_player_num;
};
struct win_info {
	int player_id;
	int jetton;
};

struct seat  		cur_seat;
struct blind 		cur_blind;
struct card  		all_cards[7];	// current owned cards
struct inquire_info cur_inquire;
struct notify_info  cur_notify;
struct win_info 	cur_win;

/** Collect notify message info and learn other players feature.*/	
static int poker_rounds = 0;

#define REGULAR			0
#define CONSERVATIVE 	1
#define RADICAL 		2

struct check_raise {
	int player_id[8];
};
struct player_feature {
	int player_id;
	int like_action;
	int like_character;
	int actions[5];
	int characters[3];
};
struct rest_player {
	int player_id;
	struct card hand_cards[2];
	enum face_of_cards nut_hand;
};
struct showdown_info {
	int is_showdown;
	int cur_player_num;
	struct rest_player cur_rests[8];	
};

struct check_raise check_raise_man;
struct showdown_info cur_showdown;
struct player_feature cur_features[8];

/** Record receive error message times */
static int error_seat_times 	= 0;	
static int error_blind_times 	= 0;
static int error_hold_times		= 0;
static int error_flop_times 	= 0;
static int error_turn_times 	= 0;
static int error_river_times 	= 0;
static int error_inquire_times 	= 0;
static int error_notify_times   = 0;
static int error_showdown_times = 0;

static inline int has_point(struct card all_cards[], int cards_num, enum point p);
static inline int is_royal_straight_flush(struct card all_cards[], int cards_num);
static inline int is_straight_flush(struct card all_cards[], int cards_num, int* point_array, int* color_array);
static inline int is_four_of_a_kind(struct card all_cards[], int cards_num, int* point_array);
static inline int is_full_house(struct card all_cards[], int cards_num, int* point_array);
static inline int is_flush(struct card all_cards[], int cards_num, int* color_array);
static inline int is_straight(struct card all_cards[], int cards_num, int* point_array);
static inline int is_three_of_a_kind(struct card all_cards[], int cards_num, int* point_array);
static inline int is_two_pair(struct card all_cards[], int cards_num, int* point_array);
static inline int is_one_pair(struct card all_cards[], int cards_num, int* point_array);
static inline int is_high_card(struct card all_cards[], int cards_num);

void do_main(char* server_ip, int server_port, char* client_ip, int client_port);

void register_game(char* player_name, int client_fd);
int  play_game(int client_fd);
void update_players_feature(int player_flag);
void update_players_character();

int  extract_msg(char* buffer, char* start, const char* begin_tag, const char* end_tag);
int  _extract_msg(char* new_player, int player_flag, struct player_info* cur_player_info, struct card* cur_card, struct player_state* cur_state);
void print_debug_info(int player_flag);

int pre_flop_bet(int inquire_rounds);
int flop_bet(int inquire_rounds);
int turn_bet(int inquire_rounds);
int river_bet(int inquire_rounds);

static inline int 
check_ip(const char* ip) 
{
    int n[4];
    char c[4];

    if (sscanf(ip, "%d%c%d%c%d%c%d%c",
        &n[0], &c[0], &n[1], &c[1],
        &n[2], &c[2], &n[3], &c[3]) == 7) {
      	int i;
      	for(i = 0; i < 3; ++i)
        	if (c[i] != '.')
          		return FAILED;
      	for(i = 0; i < 4; ++i)
        	if (n[i] > 255 || n[i] < 0)
          		return FAILED;
      	return SUCCESS;
    } else
      	return FAILED;
}

static inline int 
str_to_num(char* str) 
{
	char* endptr;
	int num;
	errno = 0;

	num = (int)strtol(str, &endptr, 10);

	if ((errno == ERANGE && (num == LONG_MAX || num ==LONG_MIN)) 
            || (errno != 0 && num == 0)) {
        perror("strtol");
        exit(EXIT_FAILURE);
    }

    if (*endptr != '\0') {
        fprintf(stderr, "The str is in illegal format\n");
        exit(EXIT_FAILURE);
    }

    return num;
}

static inline void
next_round(char* msg) 
{
	printf("error: %s\n", msg);
	memset(send_buffer, '\0', BUFFER_MAX_LEN);
	memset(recv_buffer, '\0', BUFFER_MAX_LEN);
	memset(msg_buffer, '\0', BUFFER_MAX_LEN);
	inquire_checked = -1;
}

static inline void 
init_gamble_data() 
{
	memset(&cur_seat, 0, sizeof(struct seat));
	memset(&cur_blind, 0, sizeof(struct blind));
	memset(all_cards, 0, sizeof(all_cards));
	memset(&cur_inquire, 0, sizeof(cur_inquire));
	memset(&cur_win, 0, sizeof(cur_win));
	memset(&cur_showdown, 0, sizeof(cur_showdown));
	//memset(cur_features, 0, sizeof(cur_features));

	betting_rounds   = 0;
 
	seat_checked     = 0;
	blind_checked    = 0;
	hold_checked     = 0;
	flop_checked     = 0;
	turn_checked     = 0;
	river_checked    = 0;
	inquire_checked  = 0;
	showdown_checked = 0;
}

static inline void 
location() 
{
	int i, n;
	n = cur_seat.cur_player_num;

	switch (n) {
		case 8:
		case 7: 
		{
			if (cur_seat.button == my_player_id)
				my_seat = BACK_SEAT;
			else if (cur_seat.small_blind_id == my_player_id)
				my_seat = SMALL_BLIND_SEAT;
			else if (cur_seat.big_blind_id == my_player_id)
				my_seat = BIG_BLIND_SEAT;
			else {
				for (i = 0; i < n; ++i) {
					if (cur_seat.players[i].player_id == my_player_id) {
						if (i == 0)
							my_seat = BACK_SEAT;
						else if (i == 3 || i == 4)
							my_seat = FRONT_SEAT;
						else if (i == 5 || i == 6 || i == 7)
							my_seat = MIDDLE_SEAT;
					}
				}
			}
			break;
		}
		case 6:
		{
			if (cur_seat.button == my_player_id)
				my_seat = BACK_SEAT;
			else if (cur_seat.small_blind_id == my_player_id)
				my_seat = SMALL_BLIND_SEAT;
			else if (cur_seat.big_blind_id == my_player_id)
				my_seat = BIG_BLIND_SEAT;
			else {
				for (i = 0; i < n; ++i) {
					if (cur_seat.players[i].player_id == my_player_id) {
						if (i == 3 || i == 4)
							my_seat = FRONT_SEAT;
						else if (i == 5)
							my_seat = MIDDLE_SEAT;
					}
				}
			}	
		}
		case 5:
		{
			if (cur_seat.button == my_player_id)
				my_seat = BACK_SEAT;
			else if (cur_seat.small_blind_id == my_player_id)
				my_seat = SMALL_BLIND_SEAT;
			else if (cur_seat.big_blind_id == my_player_id)
				my_seat = BIG_BLIND_SEAT;
			else {
				for (i = 0; i < n; ++i) {
					if (cur_seat.players[i].player_id == my_player_id) {
						if (i == 3)
							my_seat = FRONT_SEAT;
						else if (i == 4)
							my_seat = MIDDLE_SEAT;
					}
				}
			}		
		}
		case 4:
		case 3:
		{
			if (cur_seat.button == my_player_id)
				my_seat = BACK_SEAT;
			else if (cur_seat.small_blind_id == my_player_id)
				my_seat = SMALL_BLIND_SEAT;
			else if (cur_seat.big_blind_id == my_player_id)
				my_seat = BIG_BLIND_SEAT;
			else 
				my_seat = FRONT_SEAT;		
		}
		case 2:
		{
			if (cur_seat.button == my_player_id)
				my_seat = FRONT_SEAT;
			else if (cur_seat.small_blind_id == my_player_id)
				my_seat = SMALL_BLIND_SEAT;	
		}
	}
}

static inline void 
init_player_feature() 
{
	int i, tmp_id;
	int j = 0;
	for (i = 0; i < 8; ++i) {
		tmp_id = cur_seat.players[i].player_id;
		if (tmp_id != my_player_id) {
			cur_features[j].player_id = tmp_id;
			++j;
		}
	}
}

#endif	// _GAME_H_
