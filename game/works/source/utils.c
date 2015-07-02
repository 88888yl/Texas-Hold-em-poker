/** 
 * Create by myl 
 * in 2015-5-20
 */

 #include "game.h"

int 
extract_msg(char* buffer, char* start, const char* begin_tag, const char* end_tag) {
	int is_begin = 1;
	int len = 0;
	int flag = 0;
	char *p = --start;

	if (strcmp(begin_tag, showdown_begin_tag) == 0) {
		for ( ; *p != '\0'; --p, ++len) {
			if (flag == 2 && *p == '/') {
				--p;
				++len;
				for ( ; *begin_tag != '\0'; ++begin_tag, --p, ++len) {
					if (*p != *begin_tag)
						is_begin = 0;
				}
				break;
			} else if (*p == '/') {
				++flag;
			}
		}
	} else {
		for ( ; *p != '\0'; --p, ++len) {
			if (*p == '/') {
				--p;
				++len;
				for ( ; *begin_tag != '\0'; ++begin_tag, --p, ++len) {
					if (*p != *begin_tag)
						is_begin = 0;
				}
				break;
			}
		}
	}

	if (is_begin) {
		++p;
		--len;
		len -= strlen(end_tag);
		p += strlen(end_tag);
		memcpy(buffer, p, len);
		buffer[len] = '\0';

		return SUCCESS;
	}

	return FAILED;
}

int 
_extract_msg(char* new_player, int player_flag, struct player_info* cur_player_info, struct card* cur_card, struct player_state* cur_state) {	
	int n 	    = 0;
	int num     = 0;
	int space   = 0;
	char* begin = NULL;
	char* scan  = NULL;

	if (IS_SEAT(player_flag) && cur_player_info != NULL) {
		char player_id_str[16];
		char jetton_str[16];
		char money_str[16];
	
		begin = new_player;
		switch (player_flag) {
			case NORMAL: {
				scan = begin;
				break;
			}
			case BUTTON:
			case SM_BLIND:
			case BG_BLIND: {
				while (*begin != ':') {
					++begin;
				}
				begin += 2;
				scan = begin;
				break;
			}
		}
	
		while (*scan != '\0') {
			if (*scan != ' ') {
				++num;
				++scan;
			} else {
				++space;
				switch (space) {
					case 1: {
						for (n = 0; n < num; ++n) {
							player_id_str[n] = *(scan - num + n);
						}
						player_id_str[num] = '\0';
						break;
					}
					case 2: {
						for (n = 0; n < num; ++n) {
							jetton_str[n] = *(scan - num + n);
						}
						jetton_str[num] = '\0';
						break;
					}
					case 3: {
						for (n = 0; n < num; ++n) {
							money_str[n] = *(scan - num + n);
						}
						money_str[num] = '\0';
						break;
					}
				}
				++scan;
				num = 0;
			}
		}
	
		if (space != 3)
			return FAILED;
		
		cur_player_info->player_id = str_to_num(player_id_str);
		cur_player_info->jetton = str_to_num(jetton_str);
		cur_player_info->money = str_to_num(money_str);
	
		return SUCCESS;

	} else if (IS_BLIND(player_flag)) {
		char small_blind_str[16];
		char big_blind_str[16];

		begin = new_player;
		while (*begin != ':') {
			++begin;
		}
		begin += 2;
		scan = begin;
		while (*scan != '\0') {
			if (*scan != ' ') {
				++num;
				++scan;
			} else {
				switch (player_flag) {
					case SMALL: {
						for (n = 0; n < num; ++n) {
							small_blind_str[n] = *(scan - num + n);
						}
						small_blind_str[num] = '\0';
						cur_blind.small_blind = str_to_num(small_blind_str);
						break;
					}
					case BIG: {
						for (n = 0; n < num; ++n) {
							big_blind_str[n] = *(scan - num + n);
						}
						big_blind_str[num] = '\0';
						cur_blind.big_blind = str_to_num(big_blind_str);
						break;
					}
				}
				++scan;
				num = 0;
			}
		}

		return SUCCESS;

	} else if (IS_HOLD(player_flag)) {
		char color_str[16];
		char point_str[16];

		begin = new_player;
		scan = begin;

		while (*scan != '\0') {
			if (*scan != ' ') {
				++num;
				++scan;
			} else {
				++space;
				switch (player_flag) {
					case HOLD_1: {
						if (space == 1) {
							for (n = 0; n < num; ++n) {
								color_str[n] = *(scan - num + n);
							}
							color_str[num] = '\0';
							for (n = 0; n < 4; ++n) {
								if (strcmp(color_str, color_array[n]) == 0)
									all_cards[0].m_color = n;
							}
							break;
						} else if (space == 2) {
							for (n = 0; n < num; ++n) {
								point_str[n] = *(scan - num + n);
							}
							point_str[num] = '\0';
							for (n = 2; n < 15; ++n) {
								if (strcmp(point_str, point_array[n]) == 0)
									all_cards[0].m_point = n;
							}
							break;
						}
					}
					case HOLD_2: {
						if (space == 1) {
							for (n = 0; n < num; ++n) {
								color_str[n] = *(scan - num + n);
							}
							color_str[num] = '\0';
							for (n = 0; n < 4; ++n) {
								if (strcmp(color_str, color_array[n]) == 0)
									all_cards[1].m_color = n;
							}
							break;
						} else if (space == 2) {
							for (n = 0; n < num; ++n) {
								point_str[n] = *(scan - num + n);
							}
							point_str[num] = '\0';
							for (n = 2; n < 15; ++n) {
								if (strcmp(point_str, point_array[n]) == 0)
									all_cards[1].m_point = n;
							}
							break;
						}
					}
				}
				++scan;
				num = 0;
			}
		}

		if (space != 2)
			return FAILED;

		return SUCCESS;

	} else if (IS_FLOP(player_flag)) {
		char color_str[16];
		char point_str[16];

		begin = new_player;
		scan = begin;

		while (*scan != '\0') {
			if (*scan != ' ') {
				++num;
				++scan;
			} else {
				++space;
				switch (player_flag) {
					case FLOP_1: {
						if (space == 1) {
							for (n = 0; n < num; ++n) {
								color_str[n] = *(scan - num + n);
							}
							color_str[num] = '\0';
							for (n = 0; n < 4; ++n) {
								if (strcmp(color_str, color_array[n]) == 0)
									all_cards[2].m_color = n;
							}
							break;
						} else if (space == 2) {
							for (n = 0; n < num; ++n) {
								point_str[n] = *(scan - num + n);
							}
							point_str[num] = '\0';
							for (n = 2; n < 15; ++n) {
								if (strcmp(point_str, point_array[n]) == 0)
									all_cards[2].m_point = n;
							}
							break;
						}
					}
					case FLOP_2: {
						if (space == 1) {
							for (n = 0; n < num; ++n) {
								color_str[n] = *(scan - num + n);
							}
							color_str[num] = '\0';
							for (n = 0; n < 4; ++n) {
								if (strcmp(color_str, color_array[n]) == 0)
									all_cards[3].m_color = n;
							}
							break;
						} else if (space == 2) {
							for (n = 0; n < num; ++n) {
								point_str[n] = *(scan - num + n);
							}
							point_str[num] = '\0';
							for (n = 2; n < 15; ++n) {
								if (strcmp(point_str, point_array[n]) == 0)
									all_cards[3].m_point = n;
							}
							break;
						}
					}
					case FLOP_3: {
						if (space == 1) {
							for (n = 0; n < num; ++n) {
								color_str[n] = *(scan - num + n);
							}
							color_str[num] = '\0';
							for (n = 0; n < 4; ++n) {
								if (strcmp(color_str, color_array[n]) == 0)
									all_cards[4].m_color = n;
							}
							break;
						} else if (space == 2) {
							for (n = 0; n < num; ++n) {
								point_str[n] = *(scan - num + n);
							}
							point_str[num] = '\0';
							for (n = 2; n < 15; ++n) {
								if (strcmp(point_str, point_array[n]) == 0)
									all_cards[4].m_point = n;
							}
							break;
						}
					}
				}
				++scan;
				num = 0;
			}
		}

		if (space != 2)
			return FAILED;

		return SUCCESS;

	} else if (IS_TURN(player_flag)) {
		char color_str[16];
		char point_str[16];

		begin = new_player;
		scan = begin;

		while (*scan != '\0') {
			if (*scan != ' ') {
				++num;
				++scan;
			} else {
				++space;
				if (space == 1) {
					for (n = 0; n < num; ++n) {
						color_str[n] = *(scan - num + n);
					}
					color_str[num] = '\0';
					for (n = 0; n < 4; ++n) {
						if (strcmp(color_str, color_array[n]) == 0)
							all_cards[5].m_color = n;
					}
				} else if (space == 2) {
					for (n = 0; n < num; ++n) {
						point_str[n] = *(scan - num + n);
					}
					point_str[num] = '\0';
					for (n = 2; n < 15; ++n) {
						if (strcmp(point_str, point_array[n]) == 0)
							all_cards[5].m_point = n;
					}
				}
				++scan;
				num = 0;
			}
		}

		if (space != 2)
			return FAILED;

		return SUCCESS;
	} else if (IS_RIVER(player_flag)) {
		char color_str[16];
		char point_str[16];

		begin = new_player;
		scan = begin;

		while (*scan != '\0') {
			if (*scan != ' ') {
				++num;
				++scan;
			} else {
				++space;
				if (space == 1) {
					for (n = 0; n < num; ++n) {
						color_str[n] = *(scan - num + n);
					}
					color_str[num] = '\0';
					for (n = 0; n < 4; ++n) {
						if (strcmp(color_str, color_array[n]) == 0)
							all_cards[6].m_color = n;
					}
				} else if (space == 2) {
					for (n = 0; n < num; ++n) {
						point_str[n] = *(scan - num + n);
					}
					point_str[num] = '\0';
					for (n = 2; n < 15; ++n) {
						if (strcmp(point_str, point_array[n]) == 0)
							all_cards[6].m_point = n;
					}
				}
				++scan;
				num = 0;
			}
		}

		if (space != 2)
			return FAILED;

		return SUCCESS;
	} else if (IS_INQUIRE(player_flag)) {
		int player_num = cur_inquire.cur_player_num;
		char temp_str[16];

		begin = new_player;

		if ((scan = strstr(new_player, ":")) != NULL) {
			scan += 2;
			while (*scan != '\0') {
				if (*scan != ' ') {
					++num;
					++scan;
				} else {
					for (n = 0; n < num; ++n) {
						temp_str[n] = *(scan - num + n);
					}
					temp_str[num] = '\0';
					cur_inquire.total_pot = str_to_num(temp_str);

					++scan;
					num = 0;
				}
			}
		} else {
			scan = begin;
			while (*scan != '\0') {
				if (*scan != ' ') {
					++num;
					++scan;
				} else {
					++space;
					switch (space) {
						case 1: {
							for (n = 0; n < num; ++n) {
								temp_str[n] = *(scan - num + n);
							}
							temp_str[num] = '\0';
							cur_inquire.cur_states[player_num].m_player_info.player_id = str_to_num(temp_str);
							break;
						}
						case 2: {
							for (n = 0; n < num; ++n) {
								temp_str[n] = *(scan - num + n);
							}
							temp_str[num] = '\0';
							cur_inquire.cur_states[player_num].m_player_info.jetton = str_to_num(temp_str);
							break;
						}
						case 3: {
							for (n = 0; n < num; ++n) {
								temp_str[n] = *(scan - num + n);
							}
							temp_str[num] = '\0';
							cur_inquire.cur_states[player_num].m_player_info.money = str_to_num(temp_str);
							break;
						}
						case 4: {
							for (n = 0; n < num; ++n) {
								temp_str[n] = *(scan - num + n);
							}
							temp_str[num] = '\0';
							cur_inquire.cur_states[player_num].bet = str_to_num(temp_str);
							break;
						}
						case 5: {
							for (n = 0; n < num; ++n) {
								temp_str[n] = *(scan - num + n);
							}
							temp_str[num] = '\0';
							for (n = 0; n < 5; ++n) {
								if (strstr(action[n], temp_str))
									break;
							}
							cur_inquire.cur_states[player_num].action_num = n;
							break;
						}
					}
					++scan;
					num = 0;
				}
			}
			if (space != 5)
				return FAILED;
		}

		return SUCCESS;

	} else if (IS_WIN(player_flag)) {
		char player_id_str[16];
		char jetton_str[16];

		begin = new_player;
		scan = begin;

		while (*scan != '\0') {
			if (*scan != ' ') {
				++num;
				++scan;
			} else {
				++space;
				if (space == 1) {
					for (n = 0; n < num - 1; ++n) {
						player_id_str[n] = *(scan - num + n);
					}
					player_id_str[num - 1] = '\0';
				} else if (space == 2) {
					for (n = 0; n < num; ++n) {
							jetton_str[n] = *(scan - num + n);
					}
					jetton_str[num] = '\0';
				}
				++scan;
				num = 0;
			}
		}

		if (space != 2)
			return FAILED;

		cur_win.player_id = str_to_num(player_id_str);
		cur_win.jetton = str_to_num(jetton_str);

		return SUCCESS;
	} else if (IS_NOTIFY(player_flag)) {
		int player_num = cur_notify.cur_player_num;
		char temp_str[16];

		begin = new_player;

		if ((scan = strstr(new_player, ":")) != NULL) {
			scan += 2;
			while (*scan != '\0') {
				if (*scan != ' ') {
					++num;
					++scan;
				} else {
					for (n = 0; n < num; ++n) {
						temp_str[n] = *(scan - num + n);
					}
					temp_str[num] = '\0';
					cur_notify.total_pot = str_to_num(temp_str);

					++scan;
					num = 0;
				}
			}
		} else {
			scan = begin;
			while (*scan != '\0') {
				if (*scan != ' ') {
					++num;
					++scan;
				} else {
					++space;
					switch (space) {
						case 1: {
							for (n = 0; n < num; ++n) {
								temp_str[n] = *(scan - num + n);
							}
							temp_str[num] = '\0';
							cur_notify.cur_states[player_num].m_player_info.player_id = str_to_num(temp_str);
							break;
						}
						case 2: {
							for (n = 0; n < num; ++n) {
								temp_str[n] = *(scan - num + n);
							}
							temp_str[num] = '\0';
							cur_notify.cur_states[player_num].m_player_info.jetton = str_to_num(temp_str);
							break;
						}
						case 3: {
							for (n = 0; n < num; ++n) {
								temp_str[n] = *(scan - num + n);
							}
							temp_str[num] = '\0';
							cur_notify.cur_states[player_num].m_player_info.money = str_to_num(temp_str);
							break;
						}
						case 4: {
							for (n = 0; n < num; ++n) {
								temp_str[n] = *(scan - num + n);
							}
							temp_str[num] = '\0';
							cur_notify.cur_states[player_num].bet = str_to_num(temp_str);
							break;
						}
						case 5: {
							for (n = 0; n < num; ++n) {
								temp_str[n] = *(scan - num + n);
							}
							temp_str[num] = '\0';
							for (n = 0; n < 5; ++n) {
								if (strstr(action[n], temp_str))
									break;
							}
							cur_notify.cur_states[player_num].action_num = n;
							break;
						}
					}
					++scan;
					num = 0;
				}
			}
			if (space != 5)
				return FAILED;
		}
		return SUCCESS;
	} else if (IS_SHOWDOWN(player_flag)) {
		char temp_str[16];
		int player_id;

		if ((scan = strstr(new_player, ":")) != NULL) {
			scan += 2;
		}
		while (*scan != '\0') {
			if (*scan != ' ') {
				++num;
				++scan;
			} else {
				++space;
				switch (space) {
					case 1: {
						for (n = 0; n < num; ++n) {
							temp_str[n] = *(scan - num + n);
						}
						temp_str[num] = '\0';
						player_id = str_to_num(temp_str);
						cur_showdown.cur_rests[cur_showdown.cur_player_num].player_id = player_id;
					}
					case 2: {
						for (n = 0; n < num; ++n) {
							temp_str[n] = *(scan - num + n);
						}
						temp_str[num] = '\0';
						for (n = 0; n < 4; ++n) {
							if (strstr(temp_str, color_array[n]))
								break;
						}
						cur_showdown.cur_rests[cur_showdown.cur_player_num].hand_cards[0].m_color = n;
					}
					case 3: {
						for (n = 0; n < num; ++n) {
							temp_str[n] = *(scan - num + n);
						}
						temp_str[num] = '\0';
						for (n = 2; n < 15; ++n) {
							if (strstr(temp_str, point_array[n]))
								break;
						}
						cur_showdown.cur_rests[cur_showdown.cur_player_num].hand_cards[0].m_point = n;
					}
					case 4: {
						for (n = 0; n < num; ++n) {
							temp_str[n] = *(scan - num + n);
						}
						temp_str[num] = '\0';
						for (n = 0; n < 4; ++n) {
							if (strstr(temp_str, color_array[n]))
								break;
						}
						cur_showdown.cur_rests[cur_showdown.cur_player_num].hand_cards[1].m_color = n;
					}
					case 5: {
						for (n = 0; n < num; ++n) {
							temp_str[n] = *(scan - num + n);
						}
						temp_str[num] = '\0';
						for (n = 2; n < 15; ++n) {
							if (strstr(temp_str, point_array[n]))
								break;
						}
						cur_showdown.cur_rests[cur_showdown.cur_player_num].hand_cards[1].m_point = n;
					}
					case 6: {
						for (n = 0; n < num; ++n) {
							temp_str[n] = *(scan - num + n);
						}
						temp_str[num] = '\0';
						for (n = 1; n < 11; ++n) {
							if (strstr(temp_str, nut_array[n]))
								break;
						}
						cur_showdown.cur_rests[cur_showdown.cur_player_num].nut_hand = n;
					}
				}
				++scan;
				num = 0;
			}
		}
		if (space != 6)
			return FAILED;

		return SUCCESS;
	}
}

void 
print_debug_info(int player_flag) {
	if (IS_SEAT(player_flag)) {
		printf("\n\n-------seat--------\n");
		printf("cur_seat button: %d\n", cur_seat.button);
		printf("cur_seat small blind: %d\n", cur_seat.small_blind_id);
		printf("cur_seat big blind: %d\n\n", cur_seat.big_blind_id);
		int a;
		for (a = 0; a <8; ++a) {
			printf("cur_seat player[%d]:\n", a);
			printf("\tplayer id: %d, ", cur_seat.players[a].player_id);
			printf("player jetton: %d, ", cur_seat.players[a].jetton);
			printf("player money: %d\n", cur_seat.players[a].money);
		}
		printf("-------seat--------\n\n");
	} else if (IS_BLIND(player_flag)) {
		printf("\n\n-------blind--------\n");
		printf("small blind: %d\n", cur_blind.small_blind);
		printf("big blind: %d\n", cur_blind.big_blind);
		printf("-------blind--------\n\n");
	} else if (IS_HOLD(player_flag)) {
		printf("\n\n-------hold--------\n");
		printf("hold 1: %d %d\n", all_cards[0].m_color, all_cards[0].m_point);
		printf("hold 2: %d %d\n", all_cards[1].m_color, all_cards[1].m_point);
		printf("-------hold--------\n\n");
	} else if (IS_FLOP(player_flag)) {
		printf("\n\n-------flop--------\n");
		printf("flop 1: %d %d\n", all_cards[2].m_color, all_cards[2].m_point);
		printf("flop 2: %d %d\n", all_cards[3].m_color, all_cards[3].m_point);
		printf("flop 3: %d %d\n", all_cards[4].m_color, all_cards[4].m_point);
		printf("-------flop--------\n\n");
	} else if (IS_TURN(player_flag)) {
		printf("\n\n-------turn--------\n");
		printf("turn: %d %d\n", all_cards[5].m_color, all_cards[5].m_point);
		printf("-------turn--------\n\n");
	} else if (IS_RIVER(player_flag)) {
		printf("\n\n-------river--------\n");
		printf("river: %d %d\n", all_cards[6].m_color, all_cards[5].m_point);
		printf("-------river--------\n\n");
	} else if (IS_INQUIRE(player_flag)) {
		printf("\n\n-------inquire--------\n");
		printf("total pot: %d\n", cur_inquire.total_pot);
		int a, b;
		b = cur_inquire.cur_player_num;
		for (a = 0; a < b; ++a) {
			printf("cur_inquire player[%d]:\n", a);
			printf("\tplayer id: %d, ", cur_inquire.cur_states[a].m_player_info.player_id);
			printf("\tplayer jetton: %d, ", cur_inquire.cur_states[a].m_player_info.jetton);
			printf("\tplayer money: %d\n", cur_inquire.cur_states[a].m_player_info.money);
			printf("\tplayer bet: %d\n", cur_inquire.cur_states[a].bet);
			printf("\tplayer action num: %d\n", cur_inquire.cur_states[a].action_num);
		}
		printf("-------inquire--------\n\n");
	} else if (IS_WIN(player_flag)) {
		printf("\n\n-------win--------\n");
		printf("win: %d %d\n", cur_win.player_id, cur_win.jetton);
		printf("-------win--------\n\n");
	} else if (IS_NOTIFY(player_flag)) {
		printf("\n\n-------notify--------\n");
		printf("total pot: %d\n", cur_notify.total_pot);
		int a, b;
		b = cur_notify.cur_player_num;
		for (a = 0; a < b; ++a) {
			printf("cur_notify player[%d]:\n", a);
			printf("\tplayer id: %d, ", cur_notify.cur_states[a].m_player_info.player_id);
			printf("\tplayer jetton: %d, ", cur_notify.cur_states[a].m_player_info.jetton);
			printf("\tplayer money: %d\n", cur_notify.cur_states[a].m_player_info.money);
			printf("\tplayer bet: %d\n", cur_notify.cur_states[a].bet);
			printf("\tplayer action num: %d\n", cur_notify.cur_states[a].action_num);
		}
		printf("-------notify--------\n\n");
	}
}

