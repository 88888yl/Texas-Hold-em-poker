/** 
 * Create by myl 
 * in 2015-5-12 
 */

#include "game.h"
#include "card.h"

int
main(int argc, char* argv[]) 
{
    if (argc != 6) {
        fprintf(stderr, "Usage: %s server_ip server_port client_ip client_port player_id\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /**  test server ip is 127.0.0.1 port is 6000 */
    /**  test client ip is 127.0.0.1 port is 600x */
    /**  test player id is xxxx */
    /**  x is one of 1 2 3 4 5 6 7 8 */
    char* server_ip 		= argv[1];
    char* server_port_str 	= argv[2];
    char* client_ip 		= argv[3];
    char* client_port_str 	= argv[4];
    char* player_id_str 	= argv[5];

    int   server_port = 0;
    int   client_port = 0;

    char *check_port_endptr_1, *check_port_endptr_2, *check_player_id;

    /**  check server ip and port */
    if (!check_ip(server_ip)) {
    	fprintf(stderr, "The server_ip is in illegal format\n");
        exit(EXIT_FAILURE);
    }
    errno = 0;
    server_port = (int)strtol(server_port_str, &check_port_endptr_1, 10);
    if ((errno == ERANGE && (server_port == LONG_MAX || server_port ==LONG_MIN)) 
            || (errno != 0 && server_port == 0)) {
        perror("strtol");
        exit(EXIT_FAILURE);
    }
    if (*check_port_endptr_1 != '\0') {
        fprintf(stderr, "The server_port is in illegal format\n");
        exit(EXIT_FAILURE);
    }

	/**  check client ip and port */
    if (!check_ip(client_ip)) {
    	fprintf(stderr, "The client_ip is in illegal format\n");
        exit(EXIT_FAILURE);
    }
    errno = 0;
    client_port = (int)strtol(client_port_str, &check_port_endptr_2, 10);
    if ((errno == ERANGE && (client_port == LONG_MAX || client_port ==LONG_MIN)) 
            || (errno != 0 && client_port == 0)) {
        perror("strtol");
        exit(EXIT_FAILURE);
    }
    if (*check_port_endptr_2 != '\0') {
        fprintf(stderr, "The client_port is in illegal format\n");
        exit(EXIT_FAILURE);
    }

    /**  check player id */
    errno = 0;
    my_player_id = (int)strtol(player_id_str, &check_player_id, 10);
    if ((errno == ERANGE && (my_player_id == LONG_MAX || my_player_id ==LONG_MIN)) 
            || (errno != 0 && my_player_id == 0)) {
        perror("strtol");
        exit(EXIT_FAILURE);
    }
    if (*check_player_id != '\0') {
        fprintf(stderr, "The player_id is in illegal format\n");
        exit(EXIT_FAILURE);
    }
 
    do_main(server_ip, server_port, client_ip, client_port);
 
    return 0;
}

void 
do_main(char* server_ip, int server_port, char* client_ip, int client_port) 
{
	int client_fd, connect_flag = 0, bind_flag = 0;
	int sock_option = 1;
	char player_name[] = "CUIT6312";	// team name
    struct sockaddr_in server_addr, client_addr;
    
    /**  Try to connect gameserver using TCP socket. */
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(server_port);

    /** Client must bind client ip and port to avoid DOS attact. */
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = inet_addr(client_ip);
    client_addr.sin_port = htons(client_port);

	if (setsockopt(client_fd, SOL_SOCKET, SO_REUSEADDR, &sock_option, sizeof(sock_option)) < 0) {
	   perror("setsockopt");
	   exit(EXIT_FAILURE);
	}

    while (bind(client_fd, (struct sockaddr* )&client_addr, sizeof(client_addr)) == -1) {
    	perror("bind");
        usleep(10000);
        fprintf(stderr, "try to rebind...\n");
        ++bind_flag;
        if (bind_flag == 20) {
        	close(client_fd);
        	exit(EXIT_FAILURE);
        }
    }

	socklen_t server_addr_length = sizeof(server_addr);
	// If error, reconnect 20 times. if error again, then exit.
    while (connect(client_fd, (struct sockaddr *)&server_addr, server_addr_length) == -1) {
		perror("connect");
		usleep(10000);
        fprintf(stderr, "try to reconnct...\n");
        ++connect_flag;
        if (connect_flag == 20) {
        	close(client_fd);
        	exit(EXIT_FAILURE);
        }
    }

    /** Format is "reg: pid pname \n" */
    register_game(player_name, client_fd);

    /** main function */
    play_game(client_fd);
	
    close(client_fd);
}

void 
register_game(char* player_name, int client_fd) 
{
	int  register_flag = 0;		// used for resend when send error.
	char register_buffer[32];	// "reg: pid pname \n"
	
	memset(register_buffer, '\0', 32);
	// need_notify when fold or allin, server will still send inquire msg.
	sprintf(register_buffer, "reg: %d %s need_notify \n", my_player_id, player_name);

	// If error, resend 20 times. if error again, then exit.
    while (send(client_fd, &register_buffer, strlen(register_buffer), 0) == -1) {
        perror("send");
        usleep(5000);	// 5ms
        fprintf(stderr, "try to resend...\n");
        ++register_flag;
        if (register_flag == 20) {
        	close(client_fd);
        	exit(EXIT_FAILURE);
        }
    }

    usleep(5000);	// 5ms
}

int 
play_game(int client_fd) 
{
	int  choose_num = -1; 		// choose a num to identify how to bet
	int  i = 0, j; 				// just for loop
	int  recv_bytes = 0;
	char *start = NULL; 		// locale message begin
	char *tmp_buffer[128];
	char *scan = NULL;			// scan the message to choose useful info
	char *player_ptr = NULL;	// just for strtok_r()
	
	/** In evey round, to empty the info struct and set flag zero. */
	init_gamble_data();
	memset(cur_features, 0, sizeof(cur_features));

	/** start Texas Hold’em poker */
	while(1) {
		memset(recv_buffer, '\0', BUFFER_MAX_LEN);
        if ((recv_bytes = recv(client_fd, recv_buffer, sizeof(recv_buffer), 0)) == -1) {
         	perror("recv");
         	close(client_fd);
         	exit(EXIT_FAILURE);
    	}

    	if (!showdown_checked && (start = strstr(recv_buffer, showdown_tag)) != NULL) {
			cur_showdown.is_showdown = 1;
			memset(msg_buffer, '\0', BUFFER_MAX_LEN);
    		scan = msg_buffer;
			if (!extract_msg(msg_buffer, start, showdown_begin_tag, showdown_tag)) {
				next_round("showdown message");
				++error_showdown_times;
			} else {
				showdown_checked = 1;
				memset(tmp_buffer, '\0', sizeof(tmp_buffer));
				player_ptr = NULL;

				// printf("========================\n");
				// printf("msg:\n%s\n", scan);

				// if (strstr(scan, "down/")) {
				// 	printf("-----------------\n");
				// 	printf("receive:\n%s\n", recv_buffer);
				// 	printf("-----------------\n");
				// 	printf("start:\n%s\n", start);
				// }
				i = 0;
				while ((tmp_buffer[i] = strtok_r(scan, "\n", &player_ptr)) != NULL) {
					++i;
					scan = NULL;
				}

				for (j = 7; j < i; ++j) {
					// printf("total %d row, and tmp_buffer[%d]: %s\n", i, j, tmp_buffer[j]);
					if (!_extract_msg(tmp_buffer[j], SHOWDOWN, NULL, NULL, NULL))
						fprintf(stderr, "extract showdown error\n"); 
					++cur_showdown.cur_player_num;
				}
			}
			// printf("===============showdown==============\n");
			// for (i = 0; i < cur_showdown.cur_player_num; ++i) {
			// 	printf("%d\n", cur_showdown.cur_rests[i].player_id);
			// 	printf("%d\n", cur_showdown.cur_rests[i].hand_cards[0].m_color);
			// 	printf("%d\n", cur_showdown.cur_rests[i].hand_cards[0].m_point);
			// 	printf("%d\n", cur_showdown.cur_rests[i].hand_cards[1].m_color);
			// 	printf("%d\n", cur_showdown.cur_rests[i].hand_cards[1].m_point);
			// 	printf("%d\n", cur_showdown.cur_rests[i].nut_hand);
			// }
			update_players_character();

			cur_showdown.is_showdown = 0;
		}

    	/**  when recv inquire(betting_rounds), this function will choose what bet to send */
    	if ((start = strstr(recv_buffer, potwin_tag)) != NULL) {
    		//printf("\n==========next round=========\n");
    		init_gamble_data();
    		win_checked = 1;
    	} 

    	if (!seat_checked && (start = strstr(recv_buffer, seat_tag)) != NULL) {
    		++poker_rounds;

    		memset(msg_buffer, '\0', BUFFER_MAX_LEN);
    		scan = msg_buffer;
			if (!extract_msg(msg_buffer, start, seat_begin_tag, seat_tag)) {
				next_round("seat message");
				++error_seat_times;
			} else {
				seat_checked = 1;
				memset(tmp_buffer, '\0', sizeof(tmp_buffer));
				player_ptr = NULL;

				i = 0;
				while ((tmp_buffer[i] = strtok_r(scan, "\n", &player_ptr)) != NULL) {
					++i;
					scan = NULL;
				}

				cur_seat.cur_player_num = i;
				for (j = 0; j < i; ++j) {
					if (strstr(tmp_buffer[j], "button") != NULL) {
						if (!_extract_msg(tmp_buffer[j], BUTTON, &cur_seat.players[j], NULL, NULL))
							fprintf(stderr, "button player info error\n");
						cur_seat.button = cur_seat.players[j].player_id;
						
					} else if (strstr(tmp_buffer[j], "small blind") != NULL) {
						if (!_extract_msg(tmp_buffer[j], SM_BLIND, &cur_seat.players[j], NULL, NULL))
							fprintf(stderr, "small blind player info error\n");
						cur_seat.small_blind_id = cur_seat.players[j].player_id;
	
					} else if (strstr(tmp_buffer[j], "big blind") != NULL) {
						if (!_extract_msg(tmp_buffer[j], BG_BLIND, &cur_seat.players[j], NULL, NULL))
							fprintf(stderr, "big blind player info error\n");
						cur_seat.big_blind_id = cur_seat.players[j].player_id;
	
					} else {
						if (!_extract_msg(tmp_buffer[j], NORMAL, &cur_seat.players[j], NULL, NULL))
							fprintf(stderr, "other player info error\n");
					}
				}
				i = 0;
				j = 0;

				location();

				if (poker_rounds == 1)
					init_player_feature();

				//print_debug_info(SEAT);	// debug info
			}			
    	}

    	if (!blind_checked && (start = strstr(recv_buffer, blind_tag)) != NULL) {
    		memset(msg_buffer, '\0', BUFFER_MAX_LEN);
    		scan = msg_buffer;
			if (!extract_msg(msg_buffer, start, blind_begin_tag, blind_tag)) {
				next_round("blind message");
				++error_blind_times;
			} else {
				blind_checked = 1;
				memset(tmp_buffer, '\0', sizeof(tmp_buffer));
				player_ptr = NULL;

				i = 0;
				while ((tmp_buffer[i] = strtok_r(scan, "\n", &player_ptr)) != NULL) {
					++i;
					scan = NULL;
				}
				
				if (!_extract_msg(tmp_buffer[0], SMALL, NULL, NULL, NULL))
					fprintf(stderr, "extract small blind error\n"); 
				if (cur_seat.cur_player_num != 2) {
					if (!_extract_msg(tmp_buffer[1], BIG, NULL, NULL, NULL))
						fprintf(stderr, "extract big blind error\n");
				}

				i = 0;
				j = 0;

				//print_debug_info(BLIND);	// debug info
			}

		} 

		if (!hold_checked && (start = strstr(recv_buffer, hold_tag)) != NULL) {
			memset(msg_buffer, '\0', BUFFER_MAX_LEN);
			scan = msg_buffer;

			if (!extract_msg(msg_buffer, start, hold_begin_tag, hold_tag)) {
				next_round("hold message");
				++error_hold_times;
			} else {
				win_checked  = 0;
				hold_checked = 1;
				memset(tmp_buffer, '\0', sizeof(tmp_buffer));
				player_ptr = NULL;

				i = 0;
				while ((tmp_buffer[i] = strtok_r(scan, "\n", &player_ptr)) != NULL) {
					++i;
					scan = NULL;
				}

				if (!_extract_msg(tmp_buffer[0], HOLD_1, NULL, NULL, NULL))
					fprintf(stderr, "extract hold error\n");
				if (!_extract_msg(tmp_buffer[1], HOLD_2, NULL, NULL, NULL))
					fprintf(stderr, "extract hold error\n");

				i = 0;
				j = 0;

				//print_debug_info(HOLD);	// debug info
			}
			++betting_rounds;
			inquire_rounds = 0;
			memset(&check_raise_man, 0, sizeof(check_raise_man));

			if (strstr(recv_buffer, flop_tag) || strstr(recv_buffer, turn_tag) || strstr(recv_buffer, river_tag)) {
				memset(recv_buffer, '\0', BUFFER_MAX_LEN);
			}
		} 

		if (!flop_checked && hold_checked && (start = strstr(recv_buffer, flop_tag)) != NULL) {
			memset(msg_buffer, '\0', BUFFER_MAX_LEN);
			scan = msg_buffer;

			if (!extract_msg(msg_buffer, start, flop_begin_tag, flop_tag)) {
				next_round("flop message");
				++error_flop_times;
			} else {
				flop_checked = 1;
				memset(tmp_buffer, '\0', sizeof(tmp_buffer));
				player_ptr = NULL;

				i = 0;
				while ((tmp_buffer[i] = strtok_r(scan, "\n", &player_ptr)) != NULL) {
					++i;
					scan = NULL;
				}

				if (!_extract_msg(tmp_buffer[0], FLOP_1, NULL, NULL, NULL))
					fprintf(stderr, "extract flop error\n");
				if (!_extract_msg(tmp_buffer[1], FLOP_2, NULL, NULL, NULL))
					fprintf(stderr, "extract flop error\n");
				if (!_extract_msg(tmp_buffer[2], FLOP_3, NULL, NULL, NULL))
					fprintf(stderr, "extract flop error\n");

				i = 0;
				j = 0;

				//print_debug_info(FLOP);	// debug info
			}
			++betting_rounds;
			inquire_rounds = 0;
			memset(&check_raise_man, 0, sizeof(check_raise_man));
		} 

		if (!turn_checked && hold_checked && (start = strstr(recv_buffer, turn_tag)) != NULL) {
			memset(msg_buffer, '\0', BUFFER_MAX_LEN);
			scan = msg_buffer;

			if (!extract_msg(msg_buffer, start, turn_begin_tag, turn_tag)) {
				next_round("turn message");
				++error_turn_times;
			} else {
				turn_checked = 1;
				memset(tmp_buffer, '\0', sizeof(tmp_buffer));
				player_ptr = NULL;

				i = 0;
				while ((tmp_buffer[i] = strtok_r(scan, "\n", &player_ptr)) != NULL) {
					++i;
					scan = NULL;
				}

				if (!_extract_msg(tmp_buffer[0], TURN, NULL, NULL, NULL))
					fprintf(stderr, "extract turn error\n");

				i = 0;
				j = 0;

				//print_debug_info(TURN);	// debug info
			}
			++betting_rounds;
			inquire_rounds = 0;
			memset(&check_raise_man, 0, sizeof(check_raise_man));
		} 

		if (!river_checked && hold_checked && (start = strstr(recv_buffer, river_tag)) != NULL) {
			memset(msg_buffer, '\0', BUFFER_MAX_LEN);
			scan = msg_buffer;

			if (!extract_msg(msg_buffer, start, river_begin_tag, river_tag)) {
				next_round("river message");
				++error_river_times;
			} else {
				turn_checked = 1;
				memset(tmp_buffer, '\0', sizeof(tmp_buffer));
				player_ptr = NULL;

				i = 0;
				while ((tmp_buffer[i] = strtok_r(scan, "\n", &player_ptr)) != NULL) {
					++i;
					scan = NULL;
				}

				if (!_extract_msg(tmp_buffer[0], RIVER, NULL, NULL, NULL))
					fprintf(stderr, "extract river error\n");

				i = 0;
				j = 0;

				//print_debug_info(RIVER);	// debug info
			}
			++betting_rounds;
			inquire_rounds = 0;
			memset(&check_raise_man, 0, sizeof(check_raise_man));
		} 

		if (strstr(recv_buffer, gameover) != NULL) {
    		fprintf(stdout, "game over!!!\n");
    		fprintf(stderr, "error seat round: %d\n", error_seat_times);
    		fprintf(stderr, "error blind round: %d\n", error_blind_times);
    		fprintf(stderr, "error hold round: %d\n", error_hold_times);
    		fprintf(stderr, "error flop round: %d\n", error_flop_times);
    		fprintf(stderr, "error turn round: %d\n", error_turn_times);
    		fprintf(stderr, "error river round: %d\n", error_river_times);
    		fprintf(stderr, "error notify round: %d\n", error_notify_times);
    		fprintf(stderr, "error showdown round: %d\n", error_showdown_times);
    		fprintf(stdout, "all rounds: %d\n", poker_rounds);
    		return 0;
    	} 

    	if (!win_checked && (start = strstr(recv_buffer, inquire_tag)) != NULL) { 
    		++inquire_rounds;
			memset(&cur_inquire, 0, sizeof(cur_inquire));
    		memset(msg_buffer, '\0', BUFFER_MAX_LEN);
    		scan = msg_buffer;
			if (!extract_msg(msg_buffer, start, inquire_begin_tag, inquire_tag)) {
				next_round("inquire message");
			} else {
				memset(tmp_buffer, '\0', sizeof(tmp_buffer));
				player_ptr = NULL;

				i = 0;
				while ((tmp_buffer[i] = strtok_r(scan, "\n", &player_ptr)) != NULL) {
					++i;
					scan = NULL;
				}

				for (j = 0; j < i - 1; ++j) {
					if (!_extract_msg(tmp_buffer[j], INQUIRE, NULL, NULL, NULL))
						fprintf(stderr, "extract inquire error\n");
					++cur_inquire.cur_player_num;
				}
				if (!_extract_msg(tmp_buffer[i - 1], INQUIRE, NULL, NULL, NULL))
					fprintf(stderr, "extract inquire error\n");

				i = 0;
				j = 0;

				update_players_feature(INQUIRE);
				//print_debug_info(INQUIRE);	// debug info
			}

    		choose_num = -1;
    		if (inquire_checked == -1) {
    			choose_num = 3;
    		} else {
    			switch (betting_rounds) {
    				case PRE_FLOP: { 					
    					choose_num = pre_flop_bet(inquire_rounds);
    					break;
    				}
    				case FLOP_ROUND: {
    					choose_num = flop_bet(inquire_rounds);
    					break;
    				}
    				case TURN_ROUND: {
    					choose_num = turn_bet(inquire_rounds);
    					break;
    				}
    				case RIVER_ROUND: {
    					choose_num = river_bet(inquire_rounds);
    					break;
    				}
    			}
    		}

    		if (choose_num == -1) {
    			fprintf(stderr, "error: nothing to choose\n");
    			exit(EXIT_FAILURE);
    		} else if (choose_num >= 0 && choose_num <= 3) {
    			if (send(client_fd, action[choose_num], strlen(action[choose_num]), 0) == -1) {
         			next_round("send message");
         		}
    		} else if (choose_num > 3) {
    			memset(send_buffer, '\0', BUFFER_MAX_LEN);
    			sprintf(send_buffer, "raise %d \n", choose_num);
    			if (send(client_fd, send_buffer, strlen(send_buffer), 0) == -1) {
         			next_round("send message");
    			}
    		}
    	}

    	if ((start = strstr(recv_buffer, notify_tag)) != NULL) {
    		memset(&cur_notify, 0, sizeof(cur_notify));
			memset(msg_buffer, '\0', BUFFER_MAX_LEN);
			scan = msg_buffer;

			if (!extract_msg(msg_buffer, start, notify_begin_tag, notify_tag)) {
				next_round("notify message");
				++error_notify_times;
			} else {
				memset(tmp_buffer, '\0', sizeof(tmp_buffer));
				player_ptr = NULL;

				i = 0;
				while ((tmp_buffer[i] = strtok_r(scan, "\n", &player_ptr)) != NULL) {
					++i;
					scan = NULL;
				}

				for (j = 0; j < i - 1; ++j) {
					if (!_extract_msg(tmp_buffer[j], NOTIFY, NULL, NULL, NULL))
						fprintf(stderr, "extract inquire error\n");
					++cur_notify.cur_player_num;
				}
				if (!_extract_msg(tmp_buffer[i - 1], NOTIFY, NULL, NULL, NULL))
					fprintf(stderr, "extract inquire error\n");

				i = 0;
				j = 0;

				update_players_feature(NOTIFY);
				//print_debug_info(NOTIFY);	// debug info
			}
		}
    }
    return 0;
}

void
update_players_feature(int player_flag) 
{
	int i, j, cur_player_num;
	int action_num;
	int like_action = 0, action_times = 0;

	for (i = 0; i < 8; ++i) {
		cur_features[i].like_action = 0;
		for (j = 0; j < 5; ++j)
			cur_features[i].actions[j] = 0;
	}
	

	if (IS_INQUIRE(player_flag)) {
		cur_player_num = cur_inquire.cur_player_num;
		for (i = 0; i < 8; ++i) {
			for (j = 0; j < cur_player_num; ++j) {
				if (cur_inquire.cur_states[j].m_player_info.player_id == cur_features[i].player_id) {
					action_num = cur_inquire.cur_states[j].action_num;
					++cur_features[i].actions[action_num];
					break;
				}
			}
		}

	} else if (IS_NOTIFY(player_flag)) {
		cur_player_num = cur_notify.cur_player_num;
		for (i = 0; i < 8; ++i) {
			for (j = 0; j < cur_player_num; ++j) {
				if (cur_notify.cur_states[j].m_player_info.player_id == cur_features[i].player_id) {
					action_num = cur_notify.cur_states[j].action_num;
					++cur_features[i].actions[action_num];
					break;
				}
			}
		}
	}

	for (i = 0; i < 8; ++i) {
		for (j = 0; j < 5; ++j) {
			if (cur_features[i].actions[j] > action_times) {
				action_times = cur_features[i].actions[j];
				like_action = j;
		 	}			
		}		
		cur_features[i].like_action = like_action;
		if (like_action == FOLD_ACTION || like_action == FOLD_ACTION)
			++cur_features[i].characters[1];
		if (like_action == RAISE_ACTION  || like_action == CHECK_ACTION)
			++cur_features[i].characters[0];
		if (like_action == ALLIN_ACTION)
			++cur_features[i].characters[2];

		like_action = 0;
		action_times = 0;
	}

	// fprintf(stdout, "================feature===============\n");
	// for (i = 0; i < 7; ++i) {	
 //    	fprintf(stdout, "player %d like %s", cur_features[i].player_id, action[cur_features[i].like_action]);
 //    	fprintf(stdout, "player %d check %d times\n", cur_features[i].player_id, cur_features[i].actions[0]);
 //    	fprintf(stdout, "player %d call %d times\n", cur_features[i].player_id, cur_features[i].actions[1]);
 //    	fprintf(stdout, "player %d all_in %d times\n", cur_features[i].player_id, cur_features[i].actions[2]);
 //    	fprintf(stdout, "player %d fold %d times\n", cur_features[i].player_id, cur_features[i].actions[3]);
 //    	fprintf(stdout, "player %d raise %d times\n\n", cur_features[i].player_id, cur_features[i].actions[4]);
 //    }
 //    fprintf(stdout, "================feature===============\n");
}

void 
update_players_character()
{
	int i, j, cur_player_num;
	int character_num;
	int like_character = 0, character_times = 0;

	cur_player_num = cur_showdown.cur_player_num;
	for (i = 0; i < cur_player_num; ++i) {
		for (j = 0; j < 8; ++j) {
			if (cur_showdown.cur_rests[i].player_id == cur_features[j].player_id) {
				if (great_hole_cards(cur_showdown.cur_rests[i].hand_cards)) {
					if (cur_features[j].like_action == CHECK_ACTION || cur_features[j].like_action == CALL_ACTION) {
						cur_features[j].characters[1] += 100;
					} else if (cur_features[j].like_action == RAISE_ACTION) {
						cur_features[j].characters[0] += 5;
					} else if (cur_features[j].like_action == ALLIN_ACTION) {
						cur_features[j].characters[0] += 5;
					}
				} else if (big_hole_cards(cur_showdown.cur_rests[i].hand_cards)) {
					if (cur_features[j].like_action == CHECK_ACTION || cur_features[j].like_action == CALL_ACTION) {
						cur_features[j].characters[1] += 80;
					} else if (cur_features[j].like_action == RAISE_ACTION) {
						cur_features[j].characters[0] += 5;
					} else if (cur_features[j].like_action == ALLIN_ACTION) {
						cur_features[j].characters[0] += 5;
					}
				} else if (middle_hole_cards(cur_showdown.cur_rests[i].hand_cards)) {
					if (cur_features[j].like_action == CHECK_ACTION || cur_features[j].like_action == CALL_ACTION) {
						cur_features[j].characters[1] += 20;
					} else if (cur_features[j].like_action == RAISE_ACTION) {
						cur_features[j].characters[0] += 5;
					} else if (cur_features[j].like_action == ALLIN_ACTION) {
						cur_features[j].characters[2] += 5;
					}
				} else if (speculative_hole_cards(cur_showdown.cur_rests[i].hand_cards)) {
					if (cur_features[j].like_action == CHECK_ACTION || cur_features[j].like_action == CALL_ACTION) {
						cur_features[j].characters[1] += 10;
					} else if (cur_features[j].like_action == RAISE_ACTION) {
						cur_features[j].characters[0] += 5;
					} else if (cur_features[j].like_action == ALLIN_ACTION) {
						cur_features[j].characters[2] += 10;
					}
				} else if (mixed_hole_cards(cur_showdown.cur_rests[i].hand_cards)) {
					if (cur_features[j].like_action == CHECK_ACTION || cur_features[j].like_action == CALL_ACTION) {
						cur_features[j].characters[0] += 5;
					} else if (cur_features[j].like_action == RAISE_ACTION) {
						cur_features[j].characters[0] += 5;
					} else if (cur_features[j].like_action == ALLIN_ACTION) {
						cur_features[j].characters[2] += 20;
					}
				} else {
					if (cur_features[j].like_action == CHECK_ACTION || cur_features[j].like_action == CALL_ACTION) {
						cur_features[j].characters[0] += 10;
					} else if (cur_features[j].like_action == RAISE_ACTION) {
						cur_features[j].characters[2] += 30;
					} else if (cur_features[j].like_action == ALLIN_ACTION) {
						cur_features[j].characters[2] += 50;
					}
				}
			}
		}
	}

	for (i = 0; i < 8; ++i) {
		for (j = 0; j < 3; ++j) {
			if (cur_features[i].characters[j] > character_times) {
				character_times = cur_features[i].characters[j];
				like_character = j;
			}			
		}		
		cur_features[i].like_character = like_character;
		like_character = 0;
		character_times = 0;
	}

	// for (i = 0; i < 8; ++i) {
	// 	if (cur_features[i].like_character == RADICAL || cur_features[i].like_character == CONSERVATIVE || cur_features[i].player_id == ) {	
	// 		printf("---round:%d---\n", poker_rounds);
 //    		fprintf(stdout, "player %d looks like %d\n", cur_features[i].player_id, cur_features[i].like_character);
 //    		for (j = 0; j < 3; ++j)
 //    			fprintf(stdout, "\tplayer %d character[%d] = %d\n", cur_features[i].player_id, j, cur_features[i].characters[j]);
 //    	}
 //    }
}
