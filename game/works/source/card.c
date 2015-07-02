/** 
 * Create by myl 
 * in 2015-5-20 
 */

#include "game.h"
#include "card.h"

int 
analyse_cur_cards(struct card all_cards[], int cards_num) 
{
	int point_array[15] = {0};
	int color_array[4]  = {0};

	if (is_royal_straight_flush(all_cards, cards_num))
		return royal_straight_flush;
	if (is_straight_flush(all_cards, cards_num, point_array, color_array))
		return straight_flush;
	if (is_four_of_a_kind(all_cards, cards_num, point_array))
		return four_of_a_kind;
	if (is_full_house(all_cards, cards_num, point_array))
		return full_house;
}

int 
big_cards(struct card all_cards[], int cards_num) 
{
	int point_array[15] = {0};
	int color_array[4]  = {0};

	if (is_royal_straight_flush(all_cards, cards_num))
		return royal_straight_flush;
	if (is_straight_flush(all_cards, cards_num, point_array, color_array))
		return straight_flush;
	if (is_four_of_a_kind(all_cards, cards_num, point_array))
		return four_of_a_kind;
	if (is_full_house(all_cards, cards_num, point_array))
		return full_house;
	if (is_flush(all_cards, cards_num, color_array))
		return flush;
	if (is_straight(all_cards, cards_num, point_array))
		return straight;
	if (is_three_of_a_kind(all_cards, cards_num, point_array))
		return three_of_a_kind;
	if (is_two_pair(all_cards, cards_num, point_array))
		return two_pair;
	return normal_cards;
}

int  
pre_flop_bet(int inquire_rounds) 
{
	int i, j, retval;
	int betted_player_num 	= 0;
	int inquire_player_num 	= cur_inquire.cur_player_num;
	int total_pot 			= cur_inquire.total_pot;
	int my_jetton 			= 0;
	int my_money 			= 0;
	int raise_num 			= 0;

	int all_fold 			= 1;
	int one_call 			= 0;
	int more_call 			= 0;
	int one_raise_no_call 	= 0;
	int one_raise_more_call = 0;
	int has_raise			= 0;
	int has_allin			= 0;
	int only_check			= 1;

	int check_raise			= 0;

	/** get my own bet info */
	if (inquire_rounds == 1) {
		betted_player_num = cur_seat.cur_player_num;
		for (i = 0; i < betted_player_num; ++i) {
			if (cur_seat.players[i].player_id == my_player_id) {
				my_jetton = cur_seat.players[i].jetton;
				my_money = cur_seat.players[i].money;
			}
		}
	} else {
		betted_player_num = cur_inquire.cur_player_num;
		for (i = 0; i < betted_player_num; ++i) {
			if (cur_inquire.cur_states[i].m_player_info.player_id == my_player_id) {
				my_jetton = cur_inquire.cur_states[i].m_player_info.jetton;
				my_money = cur_inquire.cur_states[i].m_player_info.money;
			}
		}
	}

	/** get other players' action */
	for (i = 0; i < inquire_player_num; ++i) {
		if (cur_inquire.cur_states[i].m_player_info.player_id != my_player_id) {
			if (cur_inquire.cur_states[i].action_num != FOLD_ACTION
				&& cur_inquire.cur_states[i].action_num != BLIND_ACTION) {
					all_fold = 0;
			}

			if (cur_inquire.cur_states[i].action_num == CALL_ACTION) {
				++more_call;
			}

			if (cur_inquire.cur_states[i].action_num == RAISE_ACTION) {
				one_raise_more_call = 1;
				has_raise = 1;
			}

			if (cur_inquire.cur_states[i].action_num != CHECK_ACTION 
				&& cur_inquire.cur_states[i].action_num != FOLD_ACTION
				&& cur_inquire.cur_states[i].action_num != BLIND_ACTION) {
				only_check = 0;
			}

			if (cur_inquire.cur_states[i].action_num == ALLIN_ACTION) {
				has_allin = 1;
			}
		}
	}

	if (more_call == 1)
		one_call = 1;
	if (!more_call && one_raise_more_call)
		one_raise_no_call = 1;
	if (one_raise_no_call)
		one_raise_more_call = 0;

	/**  find current bet */
	if ((raise_num = find_bet()) <= 0) {
		fprintf(stderr, "no inquire bet msg.\n");
		return FOLD_ACTION;
	}

	if (great_hole_cards(all_cards)) {
		switch (my_seat) {			
			case FRONT_SEAT:
			case MIDDLE_SEAT:
			case BACK_SEAT:
			case SMALL_BLIND_SEAT:
			case BIG_BLIND_SEAT:
			{
				if (inquire_rounds == 1) {
					if (cur_inquire.total_pot == 60) {
						return CALL_ACTION;
					} else {
						if (cur_inquire.total_pot >= 5 * 60) {
							if (all_cards[0].m_point == all_cards[1].m_point) {
								return raise_num;
							} else {
								return FOLD_ACTION;
							}
						} else {
							return CALL_ACTION;
						}
					}
				} else if (inquire_rounds == 2) {
					if (cur_inquire.total_pot >= 5 * 60) {
						if (all_cards[0].m_point == all_cards[1].m_point) {
							return raise_num;
						} else {
							return CHECK_ACTION;
						}
					} else {
						return CALL_ACTION;
					}
				} else {
					if (all_cards[0].m_point == all_cards[1].m_point) {
						return raise_num;
					} else {
						if (all_fold) {
							return raise_num;
						}
						for (i = 0; i < cur_inquire.cur_player_num; ++i) {
							if (cur_inquire.cur_states[i].m_player_info.player_id != my_player_id 
								&& (cur_inquire.cur_states[i].action_num == ALLIN_ACTION 
								|| cur_inquire.cur_states[i].action_num == RAISE_ACTION))
							{
								for (j = 0; j < 8; ++j) {
									if (cur_inquire.cur_states[i].m_player_info.player_id == cur_features[j].player_id) {
										if (cur_features[j].like_character == CONSERVATIVE) {
											if (all_cards[0].m_point == all_cards[1].m_point) {
												return raise_num;
											} else {
												if (cur_inquire.total_pot < 5 * 60) {
													return FOLD_ACTION;
												} else {
													return CHECK_ACTION;
												}
											}
										}
									}
								}
							}
						}
						return CHECK_ACTION;
					}
				}
				break;
			}
		}
	} else if (big_hole_cards(all_cards)) {
		switch (my_seat) {
			case SMALL_BLIND_SEAT:
			{
				if (all_fold) {
					if (inquire_rounds == 1) {
						return CALL_ACTION;
					} else {
						return raise_num * 2;
					}
				} else if (one_call || more_call || !has_raise) {
					if (inquire_rounds == 1) {
						return CALL_ACTION;
					} else {
						if (all_cards[0].m_point == all_cards[1].m_point) {
							return raise_num;
						} else {
							return CHECK_ACTION;
						}
					}
				} else if (one_raise_no_call) {
					if (all_cards[0].m_point == all_cards[1].m_point) {
						return raise_num;
					} else {
						if (cur_inquire.total_pot <= 2 * 60) {
							return FOLD_ACTION;
						} else if (cur_inquire.total_pot > 5 * 60) {
							return CHECK_ACTION;
						} else {
							return raise_num;
						}
					}
				} else if (one_raise_more_call) {
					return CALL_ACTION;
				} else if (only_check) {
					if (all_cards[0].m_point == all_cards[1].m_point && all_cards[0].m_point >= POINT_10) {
						return raise_num * 2;
					} else {
						return CHECK_ACTION;
					}
				} else {
					return CHECK_ACTION;
				}
				break;
			}
			case BIG_BLIND_SEAT:
			{
				if (inquire_rounds == 1) {
					if (all_fold) {
						return raise_num * 2;
					} else if (cur_inquire.total_pot > 5 * 60) {
						if (all_cards[0].m_point == all_cards[1].m_point && all_cards[0].m_point == POINT_J) {
							return raise_num;
						} else {
							for (i = 0; i < cur_inquire.cur_player_num; ++i) {
								if (cur_inquire.cur_states[i].m_player_info.player_id != my_player_id 
									&& (cur_inquire.cur_states[i].action_num == ALLIN_ACTION 
									|| cur_inquire.cur_states[i].action_num == RAISE_ACTION))
								{
									for (j = 0; j < 8; ++j) {
										if (cur_inquire.cur_states[i].m_player_info.player_id == cur_features[j].player_id) {
											if (cur_features[j].like_character == CONSERVATIVE) {
												if (cur_inquire.total_pot < 5 * 60) {
													return FOLD_ACTION;
												} else {
													return CHECK_ACTION;
												}
											}
										}
									}
								}
							}
							return CHECK_ACTION;
						}
					} else {
						if (all_cards[0].m_point == all_cards[1].m_point) {
							return raise_num;
						} else {
							return CHECK_ACTION;
						}
					}
				} else if (inquire_rounds == 2) {
					if (cur_inquire.total_pot >= 5 * 60) {
						if (all_cards[0].m_point == all_cards[1].m_point && all_cards[0].m_point == POINT_J) {
							return raise_num;
						} else {
							return FOLD_ACTION;
						}
					} else {
						if (all_cards[0].m_point == all_cards[1].m_point) {
							return raise_num;
						} else {
							return CHECK_ACTION;
						}
					}
				} else {
					if (all_fold) {
						return raise_num;
					}
					for (i = 0; i < cur_inquire.cur_player_num; ++i) {
						if (cur_inquire.cur_states[i].m_player_info.player_id != my_player_id 
							&& (cur_inquire.cur_states[i].action_num == ALLIN_ACTION 
							|| cur_inquire.cur_states[i].action_num == RAISE_ACTION))
						{
							for (j = 0; j < 8; ++j) {
								if (cur_inquire.cur_states[i].m_player_info.player_id == cur_features[j].player_id) {
									if (cur_features[j].like_character == CONSERVATIVE) {
										if (cur_inquire.total_pot < 5 * 60) {
											return FOLD_ACTION;
										} else {
											if (all_cards[0].m_point == all_cards[1].m_point && all_cards[0].m_point == POINT_J) {
												return CALL_ACTION;
											} else {
												return FOLD_ACTION;
											}
										}
									}
								}
							}
						}
					}
					if (all_cards[0].m_point == all_cards[1].m_point && all_cards[0].m_point == POINT_J) {
						return raise_num;
					} else {
						return CHECK_ACTION;
					}
				}
				break;
			}
			case FRONT_SEAT:
			case MIDDLE_SEAT:
			case BACK_SEAT:
			{
				if (inquire_rounds == 1) {
					return CHECK_ACTION;
				} else if (inquire_rounds == 2) {
					if (cur_inquire.total_pot >= 5 * 60) {
						if (all_cards[0].m_point == all_cards[1].m_point && all_cards[0].m_point == POINT_J) {
							return raise_num;
						} else {
							return FOLD_ACTION;
						}
					} else {
						if (all_cards[0].m_point == all_cards[1].m_point) {
							return raise_num;
						} else {
							return CHECK_ACTION;
						}
					}
				} else {
					if (all_fold) {
						return raise_num;
					}
					for (i = 0; i < cur_inquire.cur_player_num; ++i) {
						if (cur_inquire.cur_states[i].m_player_info.player_id != my_player_id 
							&& (cur_inquire.cur_states[i].action_num == ALLIN_ACTION 
							|| cur_inquire.cur_states[i].action_num == RAISE_ACTION))
						{
							for (j = 0; j < 8; ++j) {
								if (cur_inquire.cur_states[i].m_player_info.player_id == cur_features[j].player_id) {
									if (cur_features[j].like_character == CONSERVATIVE) {
										if (cur_inquire.total_pot < 5 * 60) {
											return FOLD_ACTION;
										} else {
											if (all_cards[0].m_point == all_cards[1].m_point && all_cards[0].m_point == POINT_J) {
												return CALL_ACTION;
											} else {
												return FOLD_ACTION;
											}
										}
									}
								}
							}
						}
					}
					if (all_cards[0].m_point == all_cards[1].m_point && all_cards[0].m_point == POINT_J) {
						return raise_num;
					} else {
						return CHECK_ACTION;
					}
				}
				break;
			}
		}
	} else if (middle_hole_cards(all_cards)) {
		switch (my_seat) {
			case FRONT_SEAT:
			case MIDDLE_SEAT:
			{
				if (inquire_rounds == 1) {
					return CHECK_ACTION;
				} else if (inquire_rounds == 2) {
					if (cur_inquire.total_pot >= 5 * 60) {
						return FOLD_ACTION;
					} else {
						if (all_fold) {
							return raise_num;
						} else if (has_allin) {
							return FOLD_ACTION;
						} else {
							return CHECK_ACTION;
						}
					}
				} else {
					if (all_fold) {
						return raise_num;
					} else if (cur_inquire.total_pot >= 5 * 60) {
						return FOLD_ACTION;
					} else {
						return CHECK_ACTION;
					}
				}
				break;
			}
			case BACK_SEAT:
			case SMALL_BLIND_SEAT:
			{
				if (inquire_rounds == 1) {
					return CHECK_ACTION;
				} else if (inquire_rounds == 2) {
					if (cur_inquire.total_pot >= 5 * 60) {
						return FOLD_ACTION;
					} else {
						if (all_fold) {
							return raise_num;
						} else if (has_allin) {
							return FOLD_ACTION;
						} else {
							return CHECK_ACTION;
						}
					}
				} else {
					if (all_fold) {
						return raise_num;
					} else if (one_raise_no_call) {
						return FOLD_ACTION;
					}
					for (i = 0; i < cur_inquire.cur_player_num; ++i) {
						if (cur_inquire.cur_states[i].m_player_info.player_id != my_player_id 
							&& (cur_inquire.cur_states[i].action_num == ALLIN_ACTION 
							|| cur_inquire.cur_states[i].action_num == RAISE_ACTION))
						{
							for (j = 0; j < 8; ++j) {
								if (cur_inquire.cur_states[i].m_player_info.player_id == cur_features[j].player_id) {
									if (cur_features[j].like_character == CONSERVATIVE) {
										if (cur_inquire.total_pot < 5 * 60) {
											return FOLD_ACTION;
										} else {
											if (all_cards[0].m_point == POINT_K && all_cards[1].m_point == POINT_Q
							 					|| all_cards[0].m_point == POINT_Q && all_cards[1].m_point == POINT_K) {
							 					return CHECK_ACTION;
											} else {
												return FOLD_ACTION;
											}
										}
									}
								}
							}
						}
					}
					if (cur_inquire.total_pot < 5 * 60) {
						if (!has_raise && !has_allin) {
							return CHECK_ACTION;
						} else {
							return FOLD_ACTION;
						}
					} else {
						if (all_cards[0].m_point == POINT_K && all_cards[1].m_point == POINT_Q
							|| all_cards[0].m_point == POINT_Q && all_cards[1].m_point == POINT_K) {
							return CHECK_ACTION;
						} else {
							return FOLD_ACTION;
						}
					}
				}	
				break;
			}
			case BIG_BLIND_SEAT:
			{
				if (inquire_rounds == 1) {
					if (all_fold) {
						return raise_num * 2;
					} else if (cur_inquire.total_pot >= 5 * 60) {
						return FOLD_ACTION;
					} else {
						return CHECK_ACTION;
					}
				} else if (inquire_rounds == 2) {
					if (cur_inquire.total_pot >= 5 * 60) {
						return FOLD_ACTION;
					} else {
						return CHECK_ACTION;
					}
				} else {
					if (all_fold) {
						return raise_num;
					}
					for (i = 0; i < cur_inquire.cur_player_num; ++i) {
						if (cur_inquire.cur_states[i].m_player_info.player_id != my_player_id 
							&& (cur_inquire.cur_states[i].action_num == ALLIN_ACTION 
							|| cur_inquire.cur_states[i].action_num == RAISE_ACTION))
						{
							for (j = 0; j < 8; ++j) {
								if (cur_inquire.cur_states[i].m_player_info.player_id == cur_features[j].player_id) {
									if (cur_features[j].like_character == CONSERVATIVE) {
										if (cur_inquire.total_pot < 5 * 60) {
											return FOLD_ACTION;
										} else {
											if (all_cards[0].m_point == POINT_K && all_cards[1].m_point == POINT_Q
												|| all_cards[0].m_point == POINT_Q && all_cards[1].m_point == POINT_K) {
												return CHECK_ACTION;
											} else {
												return FOLD_ACTION;
											}
										}
									}
								}
							}
						}
					}
					if (cur_inquire.total_pot < 5 * 60) {
						if (!has_raise && !has_allin) {
							return CHECK_ACTION;
						}
					} else {
						if (all_cards[0].m_point == POINT_K && all_cards[1].m_point == POINT_Q
							|| all_cards[0].m_point == POINT_Q && all_cards[1].m_point == POINT_K) {
							return CHECK_ACTION;
						} else {
							return FOLD_ACTION;
						}
					}
				}
				break;
			}
		}
	} else if (speculative_hole_cards(all_cards)) {
		switch (my_seat) {			
			case FRONT_SEAT:
			case MIDDLE_SEAT:
			case BACK_SEAT:
			{
				if (inquire_rounds == 1) {
					if (cur_inquire.total_pot >= 5 * 60) {
						return FOLD_ACTION;
					} else if (has_raise || has_allin) {
						return FOLD_ACTION;
					} else {
						return CHECK_ACTION;
					}
				}
			}
			case SMALL_BLIND_SEAT:
			{
				if (inquire_rounds == 1) {
					if (cur_inquire.total_pot >= 5 * 60) {
						return FOLD_ACTION;
					} else if (has_raise || has_allin) {
						return FOLD_ACTION;
					} else {
						return CHECK_ACTION;
					}
				} else if (inquire_rounds == 2) {
					if (cur_inquire.total_pot >= 5 * 60) {
						return FOLD_ACTION;
					} else {
						if (all_fold) {
							return raise_num;
						} else if (has_allin) {
							return FOLD_ACTION;
						} else {
							if (all_cards[0].m_point == all_cards[1].m_point && all_cards[0].m_point >= POINT_5) {
								return CALL_ACTION;
							} else if (all_cards[0].m_point != all_cards[1].m_point) {
								return CHECK_ACTION;
							} else {
								return FOLD_ACTION;
							}
						}
					}
				} else {
					if (all_fold) {
						return raise_num;
					} else if (one_raise_no_call) {
						return FOLD_ACTION;
					}
					for (i = 0; i < cur_inquire.cur_player_num; ++i) {
						if (cur_inquire.cur_states[i].m_player_info.player_id != my_player_id 
							&& (cur_inquire.cur_states[i].action_num == ALLIN_ACTION 
							|| cur_inquire.cur_states[i].action_num == RAISE_ACTION))
						{
							for (j = 0; j < 8; ++j) {
								if (cur_inquire.cur_states[i].m_player_info.player_id == cur_features[j].player_id) {
									if (cur_features[j].like_character == CONSERVATIVE) {
										return FOLD_ACTION;
									}
								}
							}
						}
					}
					if (cur_inquire.total_pot < 5 * 60) {
						if (!has_raise && !has_allin) {
							return CHECK_ACTION;
						} else {
							return FOLD_ACTION;
						}
					} else {
						return FOLD_ACTION;
					}
				}	
				break;
			}
			case BIG_BLIND_SEAT:
			{
				if (inquire_rounds == 1) {
					if (all_fold) {
						return raise_num * 2;
					} else if (cur_inquire.total_pot >= 5 * 60) {
						return FOLD_ACTION;
					} else {
						return CHECK_ACTION;
					}
				} else if (inquire_rounds == 2) {
					if (cur_inquire.total_pot >= 5 * 60) {
						return FOLD_ACTION;
					} else {
						return CHECK_ACTION;
					}
				} else {
					if (all_fold) {
						return raise_num;
					}
					for (i = 0; i < cur_inquire.cur_player_num; ++i) {
						if (cur_inquire.cur_states[i].m_player_info.player_id != my_player_id 
							&& (cur_inquire.cur_states[i].action_num == ALLIN_ACTION 
							|| cur_inquire.cur_states[i].action_num == RAISE_ACTION))
						{
							for (j = 0; j < 8; ++j) {
								if (cur_inquire.cur_states[i].m_player_info.player_id == cur_features[j].player_id) {
									if (cur_features[j].like_character == CONSERVATIVE) {
										return FOLD_ACTION;
									}
								}
							}
						}
					}
					if (cur_inquire.total_pot < 5 * 60) {
						if (!has_raise && !has_allin) {
							return CHECK_ACTION;
						} else {
							return FOLD_ACTION;
						}
					} else {
						return FOLD_ACTION;
					}
				}
				break;
			}
		}
	} else if (mixed_hole_cards(all_cards)) {
		switch (my_seat) {			
			case FRONT_SEAT:
			case MIDDLE_SEAT:
			case BACK_SEAT:
			{
				if (inquire_rounds == 1) {
					if (cur_inquire.total_pot >= 5 * 60) {
						return FOLD_ACTION;
					} else if (has_raise || has_allin) {
						return FOLD_ACTION;
					} else {
						return CHECK_ACTION;
					}
				}
			}
			case SMALL_BLIND_SEAT:
			{
				if (inquire_rounds == 1) {
					if (cur_inquire.total_pot >= 5 * 60) {
						return FOLD_ACTION;
					} else if (has_raise || has_allin) {
						return FOLD_ACTION;
					} else {
						return CHECK_ACTION;
					}
				} else if (inquire_rounds == 2) {
					if (cur_inquire.total_pot >= 5 * 60) {
						return FOLD_ACTION;
					} else {
						if (all_fold) {
							return raise_num;
						} else if (has_allin && has_raise) {
							return FOLD_ACTION;
						} else {
							if (all_cards[0].m_point == POINT_A || all_cards[1].m_point == POINT_A) {
								return CALL_ACTION;
							} else {
								return CHECK_ACTION;
							}
						}
					}
				} else {
					if (all_fold) {
						return raise_num;
					} else if (one_raise_no_call) {
						return FOLD_ACTION;
					}
					for (i = 0; i < cur_inquire.cur_player_num; ++i) {
						if (cur_inquire.cur_states[i].m_player_info.player_id != my_player_id 
							&& (cur_inquire.cur_states[i].action_num == ALLIN_ACTION 
							|| cur_inquire.cur_states[i].action_num == RAISE_ACTION))
						{
							for (j = 0; j < 8; ++j) {
								if (cur_inquire.cur_states[i].m_player_info.player_id == cur_features[j].player_id) {
									if (cur_features[j].like_character == CONSERVATIVE) {
										return FOLD_ACTION;
									}
								}
							}
						}
					}
					if (cur_inquire.total_pot < 5 * 60) {
						if (!has_raise && !has_allin) {
							if (all_cards[0].m_point == POINT_A || all_cards[1].m_point == POINT_A) {
								return CHECK_ACTION;
							} else {
								return FOLD_ACTION;
							}
						} else {
							return FOLD_ACTION;
						}
					} else {
						return FOLD_ACTION;
					}
				}	
				break;
			}
			case BIG_BLIND_SEAT:
			{
				if (inquire_rounds == 1) {
					if (all_fold) {
						return raise_num * 2;
					} else if (cur_inquire.total_pot >= 5 * 60) {
						return FOLD_ACTION;
					} else {
						if (has_raise || has_allin) {
							return FOLD_ACTION;
						} else {
							return CHECK_ACTION;
						}
					}
				} else if (inquire_rounds == 2) {
					if (cur_inquire.total_pot >= 5 * 60) {
						return FOLD_ACTION;
					} else {
						if (has_raise || has_allin) {
							return FOLD_ACTION;
						} else {
							return CHECK_ACTION;
						}
					}
				} else {
					if (all_fold) {
						return raise_num;
					}
					for (i = 0; i < cur_inquire.cur_player_num; ++i) {
						if (cur_inquire.cur_states[i].m_player_info.player_id != my_player_id 
							&& (cur_inquire.cur_states[i].action_num == ALLIN_ACTION 
							|| cur_inquire.cur_states[i].action_num == RAISE_ACTION))
						{
							for (j = 0; j < 8; ++j) {
								if (cur_inquire.cur_states[i].m_player_info.player_id == cur_features[j].player_id) {
									if (cur_features[j].like_character == CONSERVATIVE) {
										return FOLD_ACTION;
									}
								}
							}
						}
					}
					if (cur_inquire.total_pot < 5 * 60) {
						if (!has_raise && !has_allin) {
							if (all_cards[0].m_point == POINT_A || all_cards[1].m_point == POINT_A) {
								return CHECK_ACTION;
							} else {
								return FOLD_ACTION;
							}
						} else {
							return FOLD_ACTION;
						}
					} else {
						return FOLD_ACTION;
					}
				}
				break;
			}
		}
	}

	if (inquire_rounds == 1) {
		if (my_seat == BIG_BLIND_SEAT) {
			for (i = 0; i < cur_inquire.cur_player_num; ++i) {
				if (cur_inquire.cur_states[i].action_num != FOLD_ACTION) {
					if (cur_inquire.cur_states[i].action_num != BLIND_ACTION) {
						return FOLD_ACTION;
					}
				}
			}
			return ALLIN_ACTION;
		} else if (my_seat == SMALL_BLIND_SEAT) {
			if (cur_inquire.total_pot == 60)
				return CHECK_ACTION;
			return FOLD_ACTION;
		}
	}
	return FOLD_ACTION;	
}

int  
flop_bet(int inquire_rounds) 
{
	int point_array[15] 	= {0};
	int color_array[4]		= {0};
	int i, j, cur_bet, how_big = 0;
	int inquire_player_num 	= cur_inquire.cur_player_num;
	int pos1 				= 0;
	int pos2 				= 0;
	int my_jetton 			= 0;
	int my_money 			= 0;
	int betted_player_num   = 0;

	int all_fold 			= 1;
	int one_call 			= 0;
	int more_call 			= 0;
	int one_raise_no_call 	= 0;
	int one_raise_more_call = 0;
	int has_raise			= 0;
	int has_allin			= 0;
	int only_check			= 1;

	int check_raise			= 0;
	int has_a_pair			= 0;
	int has_three_of_a_kind	= 0;
	int has_two_flush		= 0;
	int has_three_flush		= 0;

	// get some info from inquire or seat msg
	if (inquire_rounds == 1) {
		betted_player_num = cur_seat.cur_player_num;
		for (i = 0; i < betted_player_num; ++i) {
			if (cur_seat.players[i].player_id == my_player_id) {
				my_jetton = cur_seat.players[i].jetton;
				my_money = cur_seat.players[i].money;
			}
		}
	} else {
		betted_player_num = cur_inquire.cur_player_num;
		for (i = 0; i < betted_player_num; ++i) {
			if (cur_inquire.cur_states[i].m_player_info.player_id == my_player_id) {
				my_jetton = cur_inquire.cur_states[i].m_player_info.jetton;
				my_money = cur_inquire.cur_states[i].m_player_info.money;
			}
		}
	}

	/** get other players' action */
	for (i = 0; i < inquire_player_num; ++i) {
		if (cur_inquire.cur_states[i].m_player_info.player_id != my_player_id) {
			if (cur_inquire.cur_states[i].action_num != FOLD_ACTION
				&& cur_inquire.cur_states[i].action_num != BLIND_ACTION) {
					all_fold = 0;
			}

			if (cur_inquire.cur_states[i].action_num == CALL_ACTION) {
				++more_call;
			}

			if (cur_inquire.cur_states[i].action_num == RAISE_ACTION) {
				one_raise_more_call = 1;
				has_raise = 1;
			}

			if (cur_inquire.cur_states[i].action_num != CHECK_ACTION 
				&& cur_inquire.cur_states[i].action_num != FOLD_ACTION
				&& cur_inquire.cur_states[i].action_num != BLIND_ACTION) {
				only_check = 0;
			}

			if (cur_inquire.cur_states[i].action_num == ALLIN_ACTION) {
				has_allin = 1;
			}
		}
	}

	if (more_call == 1)
		one_call = 1;
	if (!more_call && one_raise_more_call)
		one_raise_no_call = 1;
	if (one_raise_no_call)
		one_raise_more_call = 0;

	// find current bet
	if ((cur_bet = find_bet()) <= 0) {
		fprintf(stderr, "no inquire bet msg.\n");
		return FOLD_ACTION;
	}

	/** find same flash in three flop cards */
	for (i = 2; i < 5; ++i) {
		j = all_cards[i].m_color;
		if (j >= 0 && j <= 3) {
			++color_array[j];
		}
	}
	for (i = 0; i < 4; ++i) {
		if (color_array[i] == 3) {
			has_three_flush = 1;
			break;
		} else if (color_array[i] == 2) {
			has_two_flush = 1;
			break;
		}
	}

	/** find same kind in three flop cards */
	for (i = 2; i < 5; ++i) {
		j = all_cards[i].m_point;
		if (j >= 2 && j <= 14) {
			++point_array[j];
		}
	}
	for (i = 2; i < 15; ++i) {
		if (point_array[i] == 3) {
			has_three_of_a_kind = 1;
			break;
		} else if (point_array[i] == 2) {
			has_a_pair = 1;
			break;
		}
	}

	// big cards, check one time, then raise forever
	how_big = big_cards(all_cards, 5);

	if (how_big == royal_straight_flush || how_big == straight_flush) {
		if (inquire_rounds == 1) {
			return CALL_ACTION;
		} else if (inquire_rounds <= 3) {
			if (has_raise || has_allin) {
				return CALL_ACTION;
			} else if (one_call || more_call) {
				return CALL_ACTION;
			} else {
				return CHECK_ACTION;
			}
		} else {
			return cur_bet;
		}
	} else if (how_big == four_of_a_kind) {
		if (has_three_of_a_kind) {
			if (all_fold) {
				return cur_bet;
			} else if (only_check) {
				return CHECK_ACTION;
			} else {
				return FOLD_ACTION;
			}
		} else {
			if (inquire_rounds == 1) {
				return CALL_ACTION;
			} else if (inquire_rounds <= 3) {
				if (has_raise || has_allin) {
					return CALL_ACTION;
				} else if (one_call || more_call) {
					return CALL_ACTION;
				} else {
					return CHECK_ACTION;
				}
			} else {
				return cur_bet;
			}
		}
	} else if (how_big == full_house) {
		if (has_three_of_a_kind) {
			if (all_fold) {
				return CALL_ACTION;
			} else if (only_check) {
				return CHECK_ACTION;
			} else {
				return FOLD_ACTION;
			}
		} else {
			if (inquire_rounds == 1) {
				return CALL_ACTION;
			} else if (inquire_rounds <= 3) {
				if (has_raise || has_allin) {
					return CALL_ACTION;
				} else if (one_call || more_call) {
					return CALL_ACTION;
				} else {
					return CHECK_ACTION;
				}
			} else {
				return cur_bet;
			}
		}
	} else if (how_big == flush) {
		if (has_three_flush || has_three_of_a_kind) {
			if (all_fold) {
				return CALL_ACTION;
			} else if (only_check) {
				return CHECK_ACTION;
			} else {
				return FOLD_ACTION;
			}
		} else {
			if (inquire_rounds == 1) {
				return CALL_ACTION;
			} else if (inquire_rounds <= 3) {
				if (has_raise || has_allin) {
					return CALL_ACTION;
				} else if (one_call || more_call) {
					return CALL_ACTION;
				} else {
					return CHECK_ACTION;
				}
			} else {
				return cur_bet;
			}
		}
	} else {
		/** one check and raise again seems that one has big cards */
		if ((inquire_rounds == 1 && how_big == two_pair && (has_a_pair || has_two_flush || has_three_flush || has_three_of_a_kind))
			|| (inquire_rounds == 1 && how_big > two_pair)) {			
			for (i = 0; i < inquire_player_num; ++i) {
				if (cur_inquire.cur_states[i].action_num == CHECK_ACTION) {
					check_raise_man.player_id[i] = cur_inquire.cur_states[i].m_player_info.player_id;
				} else {
					check_raise_man.player_id[i] = 0;
				}
			}
		} else if ((inquire_rounds == 2 && how_big == two_pair && (has_a_pair || has_two_flush || has_three_flush || has_three_of_a_kind))
				|| (inquire_rounds == 2 && how_big > two_pair)) {
			for (i = 0; i < inquire_player_num; ++i) {
				if (cur_inquire.cur_states[i].action_num == RAISE_ACTION) {
					for (j = 0; j < 8; ++j) {
						if (check_raise_man.player_id[j] == cur_inquire.cur_states[i].m_player_info.player_id) {
							return FOLD_ACTION;
						}
					}
				}
			}
		}

		if (how_big == straight) {
			for (i = 0; i < 5; ++i) {
				if (all_cards[i].m_point >= POINT_Q) {
					if (inquire_rounds == 1) {
						return CALL_ACTION;
					} else if (inquire_rounds == 2) {
						if (all_fold) {
							return cur_bet;
						} else if ((has_raise || has_allin) && (has_three_flush || has_three_of_a_kind)) {
							if (cur_inquire.total_pot <= (my_jetton / 5)) {
								return FOLD_ACTION;
							} else {
								for (i = 0; i < cur_inquire.cur_player_num; ++i) {
									if (cur_inquire.cur_states[i].m_player_info.player_id != my_player_id 
										&& (cur_inquire.cur_states[i].action_num == ALLIN_ACTION 
										|| cur_inquire.cur_states[i].action_num == RAISE_ACTION))
									{
										for (j = 0; j < 8; ++j) {
											if (cur_inquire.cur_states[i].m_player_info.player_id == cur_features[j].player_id) {
												if (cur_features[j].like_character == CONSERVATIVE) {
													return FOLD_ACTION;
												}
											}
										}
									}
								}
								return CALL_ACTION;
							}
						} else {
							return cur_bet;	
						}
					} else {
						if (all_fold) {
							return cur_bet;
						} else if (has_allin) {
							if (cur_inquire.total_pot >= (my_jetton / 5)) {
								return FOLD_ACTION;
							} else {
								return CALL_ACTION;
							}
						} else if (has_raise) {
							if (has_three_flush || has_three_of_a_kind) {
								return FOLD_ACTION;
							} else {
								return CALL_ACTION;
							}
						} else {
							if (has_three_flush || has_three_of_a_kind) {
								for (i = 0; i < cur_inquire.cur_player_num; ++i) {
									if (cur_inquire.cur_states[i].m_player_info.player_id != my_player_id 
										&& (cur_inquire.cur_states[i].action_num == ALLIN_ACTION 
										|| cur_inquire.cur_states[i].action_num == RAISE_ACTION))
									{
										for (j = 0; j < 8; ++j) {
											if (cur_inquire.cur_states[i].m_player_info.player_id == cur_features[j].player_id) {
												if (cur_features[j].like_character == CONSERVATIVE) {
													if (cur_inquire.total_pot >= (my_jetton / 5)) {
														return FOLD_ACTION;
													}
												}
											}
										}
									}
								}
								return CALL_ACTION;
							} else {
								return CALL_ACTION;
							}
						}
					}
					return CALL_ACTION;
				} else {
					if (inquire_rounds == 1) {
						return CALL_ACTION;
					} else if (inquire_rounds == 2) {
						if (all_fold) {
							return cur_bet;
						} else if ((has_raise || has_allin) && (has_three_flush || has_three_of_a_kind)) {
							return FOLD_ACTION;
						} else {
							return cur_bet;	
						}
					} else {
						if (all_fold) {
							return cur_bet;
						} else if (has_allin) {
							if ((has_three_flush || has_three_of_a_kind)) {
								return FOLD_ACTION;
							} else {
								return CALL_ACTION;
							}
						} else if (has_raise) {
							if (has_three_flush || has_three_of_a_kind) {
								return FOLD_ACTION;
							} else {
								return CALL_ACTION;
							}
						} else {
							if (has_three_flush || has_three_of_a_kind) {
								for (i = 0; i < cur_inquire.cur_player_num; ++i) {
									if (cur_inquire.cur_states[i].m_player_info.player_id != my_player_id 
										&& (cur_inquire.cur_states[i].action_num == ALLIN_ACTION 
										|| cur_inquire.cur_states[i].action_num == RAISE_ACTION))
									{
										for (j = 0; j < 8; ++j) {
											if (cur_inquire.cur_states[i].m_player_info.player_id == cur_features[j].player_id) {
												if (cur_features[j].like_character == CONSERVATIVE) {
													return FOLD_ACTION;
												}
											}
										}
									}
								}
								return CHECK_ACTION;
							} else {
								return CALL_ACTION;
							}
						}
					}
					return CALL_ACTION;
				}
			}
		} else if (how_big == three_of_a_kind) {
			if (inquire_rounds == 1) {
				if (has_three_of_a_kind) {
					return FOLD_ACTION;
				} else if (has_three_flush) {
					if (all_fold) {
						return CALL_ACTION;
					} else if (only_check) {
						return CHECK_ACTION;
					} else {
						return FOLD_ACTION;
					}
				} else if (has_a_pair) {
					if (all_fold) {
						return CALL_ACTION;
					} else if (has_allin || has_raise) {
						return FOLD_ACTION;
					} else {
						return CALL_ACTION;
					}
				} else {
					return CALL_ACTION;
				}
			} else if (inquire_rounds == 2) {
				if (all_fold) {
					return cur_bet;
				} else if ((has_raise || has_allin) && (has_three_flush || has_three_of_a_kind || has_a_pair)) {
					return FOLD_ACTION;
				} else {
					return CALL_ACTION;	
				}
			} else {
				memset(point_array, 0, sizeof(point_array));
				save_point(all_cards, 5, point_array);
				for (i = 2; i < 15; ++i) {
					if (point_array[i] == 3 && i >= POINT_Q) {
						if (all_fold) {
							return CALL_ACTION;
						} else if (has_raise || has_allin) {
							if (has_three_flush || has_three_of_a_kind || has_a_pair) {
								return FOLD_ACTION;
							} else {
								for (i = 0; i < cur_inquire.cur_player_num; ++i) {
									if (cur_inquire.cur_states[i].m_player_info.player_id != my_player_id 
										&& (cur_inquire.cur_states[i].action_num == ALLIN_ACTION 
										|| cur_inquire.cur_states[i].action_num == RAISE_ACTION))
									{
										for (j = 0; j < 8; ++j) {
											if (cur_inquire.cur_states[i].m_player_info.player_id == cur_features[j].player_id) {
												if (cur_features[j].like_character == CONSERVATIVE) {
													return FOLD_ACTION;
												}
											}
										}
									}
								}
								return CALL_ACTION;
							}
						} else {
							if (has_three_flush || has_three_of_a_kind || has_a_pair) {
								return FOLD_ACTION;
							} else {
								return CALL_ACTION;
							}
						}
					} else {
						if (all_fold) {
							return CALL_ACTION;
						} else if (has_raise || has_allin) {
							if (has_three_flush || has_three_of_a_kind || has_a_pair) {
								return FOLD_ACTION;
							} else {
								for (i = 0; i < cur_inquire.cur_player_num; ++i) {
									if (cur_inquire.cur_states[i].m_player_info.player_id != my_player_id 
										&& (cur_inquire.cur_states[i].action_num == ALLIN_ACTION 
										|| cur_inquire.cur_states[i].action_num == RAISE_ACTION))
									{
										for (j = 0; j < 8; ++j) {
											if (cur_inquire.cur_states[i].m_player_info.player_id == cur_features[j].player_id) {
												if (cur_features[j].like_character == CONSERVATIVE) {
													return FOLD_ACTION;
												}
											}
										}
									}
								}
								return CALL_ACTION;
							}
						} else {
							if (has_three_flush || has_three_of_a_kind || has_a_pair) {
								return FOLD_ACTION;
							} else {
								return CALL_ACTION;
							}
						}
					}
				}
			}
		} else if (how_big == two_pair) {
			if (has_a_pair || has_three_of_a_kind) {
				goto flop_one_pair;
			}

			if (inquire_rounds == 1) {
				if (has_three_of_a_kind) {
					return FOLD_ACTION;
				} else {
					return CALL_ACTION;
				}
			} else if (inquire_rounds == 2) {
				if (all_fold) {
					return cur_bet;
				} else if ((has_raise || has_allin) && (has_three_flush || has_three_of_a_kind || has_a_pair)) {
					return FOLD_ACTION;
				} else {
					return CALL_ACTION;	
				}
			} else {
				memset(point_array, 0, sizeof(point_array));
				save_point(all_cards, 5, point_array);
				for (i = 2; i < 15; ++i) {
					if (point_array[i] == 2) {					
						if (pos1 == 0)
							pos1 = i;
						else {
							pos2 = i;
							if (pos1 > POINT_Q || pos2 > POINT_Q) {
								if (all_fold) {
									return CALL_ACTION;
								} else if (has_raise || has_allin) {
									if (has_three_flush || has_three_of_a_kind || has_a_pair) {
										return FOLD_ACTION;
									} else {
										for (i = 0; i < cur_inquire.cur_player_num; ++i) {
											if (cur_inquire.cur_states[i].m_player_info.player_id != my_player_id 
												&& (cur_inquire.cur_states[i].action_num == ALLIN_ACTION 
												|| cur_inquire.cur_states[i].action_num == RAISE_ACTION))
											{
												for (j = 0; j < 8; ++j) {
													if (cur_inquire.cur_states[i].m_player_info.player_id == cur_features[j].player_id) {
														if (cur_features[j].like_character == CONSERVATIVE) {
															return FOLD_ACTION;
														}
													}
												}
											}
										}
										if (cur_inquire.total_pot > (my_jetton / 5)) {
											return FOLD_ACTION;
										} else {
											return CALL_ACTION;
										}
									}
								} else {
									if (has_three_flush || has_three_of_a_kind || has_a_pair) {
										return FOLD_ACTION;
									} else {
										CHECK_ACTION;
									}
								}
							}
						}
					}
				}
			}
		} else if (is_one_pair(all_cards, 5, point_array)) {
flop_one_pair:
			if (has_a_pair || has_three_flush || has_three_of_a_kind) {
				return FOLD_ACTION;
			}
	
			if (inquire_rounds == 1) {
				return CHECK_ACTION;
			} else if (inquire_rounds == 2) {
				if (all_fold) {
					return CALL_ACTION;
				} else if (only_check) {
					return CHECK_ACTION;
				} else if (has_allin || has_raise) {
					return FOLD_ACTION;
				} else {
					for (i = 0; i < cur_inquire.cur_player_num; ++i) {
						if (cur_inquire.cur_states[i].m_player_info.player_id != my_player_id 
							&& (cur_inquire.cur_states[i].action_num == CALL_ACTION 
							|| cur_inquire.cur_states[i].action_num == RAISE_ACTION))
						{
							for (j = 0; j < 8; ++j) {
								if (cur_inquire.cur_states[i].m_player_info.player_id == cur_features[j].player_id) {
									if (cur_features[j].like_character == CONSERVATIVE) {
										if (cur_inquire.total_pot > (my_jetton / 5)) {
											if (only_check) {
												return CHECK_ACTION;
											}
											return FOLD_ACTION;
										} else {
											return CHECK_ACTION;
										}
									}
								}
							}
						}
					}
					return CHECK_ACTION;
				}
			} else {
				if (all_fold) {
					return CALL_ACTION;
				} else if (has_allin || has_raise) {
					return FOLD_ACTION;
				} else {
					if (cur_inquire.total_pot > (my_jetton / 5)) {
						return FOLD_ACTION;
					} else {
						memset(point_array, 0, sizeof(point_array));
						save_point(all_cards, 5, point_array);
						if (point_array[i] == 2 && i > POINT_Q) {
							return CHECK_ACTION;
						}
						return FOLD_ACTION;
					}
				}
			}
		} else {
			goto flop_other_cards;
		}
	}

	goto flop_other_cards;

flop_other_cards:
	if (cur_inquire.total_pot < (my_jetton / 10)) {
		if (has_raise || has_allin || one_call || more_call) {
			return FOLD_ACTION;
		} else {
			return CHECK_ACTION;
		}
	} else {
		return FOLD_ACTION;
	}
	return FOLD_ACTION;
}

int  
turn_bet(int inquire_rounds) 
{
	int point_array[15] 	= {0};
	int color_array[4]		= {0};
	int i, j, cur_bet, how_big = 0;
	int inquire_player_num 	= cur_inquire.cur_player_num;
	int pos1 				= 0;
	int pos2 				= 0;
	int my_jetton 			= 0;
	int my_money 			= 0;
	int betted_player_num   = 0;

	int all_fold 			= 1;
	int one_call 			= 0;
	int more_call 			= 0;
	int one_raise_no_call 	= 0;
	int one_raise_more_call = 0;
	int has_raise			= 0;
	int has_allin			= 0;
	int only_check			= 1;

	int check_raise			= 0;
	int has_a_pair			= 0;
	int has_three_of_a_kind	= 0;
	int has_four_of_a_kind  = 0;
	int has_two_pair		= 0;
	int has_two_flush		= 0;
	int has_three_flush		= 0;
	int has_four_flush		= 0;

	// get some info from inquire or seat msg
	if (inquire_rounds == 1) {
		betted_player_num = cur_seat.cur_player_num;
		for (i = 0; i < betted_player_num; ++i) {
			if (cur_seat.players[i].player_id == my_player_id) {
				my_jetton = cur_seat.players[i].jetton;
				my_money = cur_seat.players[i].money;
			}
		}
	} else {
		betted_player_num = cur_inquire.cur_player_num;
		for (i = 0; i < betted_player_num; ++i) {
			if (cur_inquire.cur_states[i].m_player_info.player_id == my_player_id) {
				my_jetton = cur_inquire.cur_states[i].m_player_info.jetton;
				my_money = cur_inquire.cur_states[i].m_player_info.money;
			}
		}
	}

	/** get other players' action */
	for (i = 0; i < inquire_player_num; ++i) {
		if (cur_inquire.cur_states[i].m_player_info.player_id != my_player_id) {
			if (cur_inquire.cur_states[i].action_num != FOLD_ACTION
				&& cur_inquire.cur_states[i].action_num != BLIND_ACTION) {
					all_fold = 0;
			}

			if (cur_inquire.cur_states[i].action_num == CALL_ACTION) {
				++more_call;
			}

			if (cur_inquire.cur_states[i].action_num == RAISE_ACTION) {
				one_raise_more_call = 1;
				has_raise = 1;
			}

			if (cur_inquire.cur_states[i].action_num != CHECK_ACTION 
				&& cur_inquire.cur_states[i].action_num != FOLD_ACTION
				&& cur_inquire.cur_states[i].action_num != BLIND_ACTION) {
				only_check = 0;
			}

			if (cur_inquire.cur_states[i].action_num == ALLIN_ACTION) {
				has_allin = 1;
			}
		}
	}

	if (more_call == 1)
		one_call = 1;
	if (!more_call && one_raise_more_call)
		one_raise_no_call = 1;
	if (one_raise_no_call)
		one_raise_more_call = 0;

	// find current bet
	if ((cur_bet = find_bet()) <= 0) {
		fprintf(stderr, "no inquire bet msg.\n");
		return FOLD_ACTION;
	}

	/** find same flash in three flop cards */
	for (i = 2; i < 6; ++i) {
		j = all_cards[i].m_color;
		if (j >= 0 && j <= 3) {
			++color_array[j];
		}
	}
	for (i = 0; i < 4; ++i) {
		if (color_array[i] == 4) {
			has_four_flush = 1;
			break;
		} else if (color_array[i] == 3) {
			has_three_flush = 1;
			break;
		} else if (color_array[i] == 2) {
			has_two_flush = 1;
			break;
		}
	}

	/** find same kind in three flop cards */
	for (i = 2; i < 6; ++i) {
		j = all_cards[i].m_point;
		if (j >= 2 && j <= 14) {
			++point_array[j];
		}
	}
	for (i = 2; i < 15; ++i) {
		if (point_array[i] == 4) {
			has_four_of_a_kind = 1;
		} else if (point_array[i] == 3) {
			has_three_of_a_kind = 1;
			break;
		} else if (point_array[i] == 2) {
			has_a_pair = 1;
			break;
		}
	}

	// big cards, check one time, then raise forever
	how_big = big_cards(all_cards, 6);

	if (how_big == royal_straight_flush || how_big == straight_flush) {
		if (inquire_rounds == 1) {
			return CALL_ACTION;
		} else if (inquire_rounds <= 3) {
			if (has_raise || has_allin) {
				return CALL_ACTION;
			} else if (one_call || more_call) {
				return CALL_ACTION;
			} else {
				return CHECK_ACTION;
			}
		} else {
			return cur_bet;
		}
	} else if (how_big == four_of_a_kind) {
		if (has_four_of_a_kind) {
			return CHECK_ACTION;
		} else if (has_three_of_a_kind) {
			if (all_fold) {
				return cur_bet;
			} else if (only_check) {
				return CHECK_ACTION;
			} else {
				return FOLD_ACTION;
			}
		} else {
			if (inquire_rounds == 1) {
				return CALL_ACTION;
			} else if (inquire_rounds <= 3) {
				if (has_raise || has_allin) {
					return CALL_ACTION;
				} else if (one_call || more_call) {
					return CALL_ACTION;
				} else {
					return CHECK_ACTION;
				}
			} else {
				return cur_bet;
			}
		}
	} else if (how_big == full_house) {
		if (has_four_of_a_kind) {
			return FOLD_ACTION;
		} else if (has_three_of_a_kind) {
			if (all_fold) {
				return CALL_ACTION;
			} else if (only_check) {
				return CHECK_ACTION;
			} else {
				return FOLD_ACTION;
			}
		} else {
			if (inquire_rounds == 1) {
				return CALL_ACTION;
			} else if (inquire_rounds <= 3) {
				if (has_raise || has_allin) {
					return CALL_ACTION;
				} else if (one_call || more_call) {
					return CALL_ACTION;
				} else {
					return CHECK_ACTION;
				}
			} else {
				return cur_bet;
			}
		}
	} else if (how_big == flush) {
		if (has_four_of_a_kind || has_four_flush) {
			return FOLD_ACTION;
		} else if (has_three_flush || has_three_of_a_kind) {
			if (all_fold) {
				return CALL_ACTION;
			} else if (only_check) {
				return CHECK_ACTION;
			} else {
				return FOLD_ACTION;
			}
		} else {
			if (inquire_rounds == 1) {
				return CALL_ACTION;
			} else if (inquire_rounds <= 3) {
				if (has_raise || has_allin) {
					return CALL_ACTION;
				} else if (one_call || more_call) {
					return CALL_ACTION;
				} else {
					return CHECK_ACTION;
				}
			} else {
				return cur_bet;
			}
		}
	} else {
		/** one check and raise again seems that one has big cards */
		if ((inquire_rounds == 1 && how_big == two_pair 
			&& (has_a_pair || has_two_flush || has_three_flush || has_three_of_a_kind || has_four_flush || has_four_of_a_kind))
			|| (inquire_rounds == 1 && how_big > two_pair)) {			
			for (i = 0; i < inquire_player_num; ++i) {
				if (cur_inquire.cur_states[i].action_num == CHECK_ACTION) {
					check_raise_man.player_id[i] = cur_inquire.cur_states[i].m_player_info.player_id;
				} else {
					check_raise_man.player_id[i] = 0;
				}
			}
		} else if ((inquire_rounds == 2 && how_big == two_pair 
			&& (has_a_pair || has_two_flush || has_three_flush || has_three_of_a_kind || has_four_flush || has_four_of_a_kind))
				|| (inquire_rounds == 2 && how_big > two_pair)) {
			for (i = 0; i < inquire_player_num; ++i) {
				if (cur_inquire.cur_states[i].action_num == RAISE_ACTION) {
					for (j = 0; j < 8; ++j) {
						if (check_raise_man.player_id[j] == cur_inquire.cur_states[i].m_player_info.player_id) {
							return FOLD_ACTION;
						}
					}
				}
			}
		}

		if (how_big == straight) {
			for (i = 0; i < 6; ++i) {
				if (all_cards[i].m_point >= POINT_Q) {
					if (inquire_rounds == 1) {
						return CALL_ACTION;
					} else if (inquire_rounds == 2) {
						if (all_fold) {
							return cur_bet;
						} else if ((has_raise || has_allin) && (has_three_flush || has_three_of_a_kind)) {
							if (cur_inquire.total_pot <= (my_jetton / 5)) {
								return FOLD_ACTION;
							} else {
								for (i = 0; i < cur_inquire.cur_player_num; ++i) {
									if (cur_inquire.cur_states[i].m_player_info.player_id != my_player_id 
										&& (cur_inquire.cur_states[i].action_num == ALLIN_ACTION 
										|| cur_inquire.cur_states[i].action_num == RAISE_ACTION))
									{
										for (j = 0; j < 8; ++j) {
											if (cur_inquire.cur_states[i].m_player_info.player_id == cur_features[j].player_id) {
												if (cur_features[j].like_character == CONSERVATIVE) {
													return FOLD_ACTION;
												}
											}
										}
									}
								}
								return CALL_ACTION;
							}
						} else {
							return cur_bet;	
						}
					} else {
						if (all_fold) {
							return cur_bet;
						} else if (has_allin) {
							if (cur_inquire.total_pot >= (my_jetton / 5)) {
								return FOLD_ACTION;
							} else {
								return CALL_ACTION;
							}
						} else if (has_raise) {
							if (has_three_flush || has_three_of_a_kind || has_four_flush || has_four_of_a_kind) {
								return FOLD_ACTION;
							} else {
								return CALL_ACTION;
							}
						} else {
							if (has_three_flush || has_three_of_a_kind || has_four_flush || has_four_of_a_kind) {
								for (i = 0; i < cur_inquire.cur_player_num; ++i) {
									if (cur_inquire.cur_states[i].m_player_info.player_id != my_player_id 
										&& (cur_inquire.cur_states[i].action_num == ALLIN_ACTION 
										|| cur_inquire.cur_states[i].action_num == RAISE_ACTION))
									{
										for (j = 0; j < 8; ++j) {
											if (cur_inquire.cur_states[i].m_player_info.player_id == cur_features[j].player_id) {
												if (cur_features[j].like_character == CONSERVATIVE) {
													if (cur_inquire.total_pot >= (my_jetton / 5)) {
														return FOLD_ACTION;
													}
												}
											}
										}
									}
								}
								return CALL_ACTION;
							} else {
								return CALL_ACTION;
							}
						}
					}
					return CALL_ACTION;
				} else {
					if (inquire_rounds == 1) {
						return CALL_ACTION;
					} else if (inquire_rounds == 2) {
						if (all_fold) {
							return cur_bet;
						} else if ((has_raise || has_allin) && (has_three_flush || has_three_of_a_kind || has_four_flush || has_four_of_a_kind)) {
							return FOLD_ACTION;
						} else {
							return cur_bet;	
						}
					} else {
						if (all_fold) {
							return cur_bet;
						} else if (has_allin) {
							if ((has_three_flush || has_three_of_a_kind || has_four_flush || has_four_of_a_kind)) {
								return FOLD_ACTION;
							} else {
								return CALL_ACTION;
							}
						} else if (has_raise) {
							if (has_three_flush || has_three_of_a_kind || has_four_flush || has_four_of_a_kind) {
								return FOLD_ACTION;
							} else {
								return CALL_ACTION;
							}
						} else {
							if (has_three_flush || has_three_of_a_kind || has_four_flush || has_four_of_a_kind) {
								for (i = 0; i < cur_inquire.cur_player_num; ++i) {
									if (cur_inquire.cur_states[i].m_player_info.player_id != my_player_id 
										&& (cur_inquire.cur_states[i].action_num == ALLIN_ACTION 
										|| cur_inquire.cur_states[i].action_num == RAISE_ACTION))
									{
										for (j = 0; j < 8; ++j) {
											if (cur_inquire.cur_states[i].m_player_info.player_id == cur_features[j].player_id) {
												if (cur_features[j].like_character == CONSERVATIVE) {
													return FOLD_ACTION;
												}
											}
										}
									}
								}
								return CHECK_ACTION;
							} else {
								return CALL_ACTION;
							}
						}
					}
					return CALL_ACTION;
				}
			}
		} else if (how_big == three_of_a_kind) {
			if (inquire_rounds == 1) {
				if (has_three_of_a_kind) {
					return FOLD_ACTION;
				} else if (has_three_flush) {
					if (all_fold) {
						return CALL_ACTION;
					} else if (only_check) {
						return CHECK_ACTION;
					} else {
						return FOLD_ACTION;
					}
				} else if (has_a_pair) {
					if (all_fold) {
						return CALL_ACTION;
					} else if (has_allin || has_raise) {
						return FOLD_ACTION;
					} else {
						return CALL_ACTION;
					}
				} else {
					return CALL_ACTION;
				}
			} else if (inquire_rounds == 2) {
				if (all_fold) {
					return cur_bet;
				} else if ((has_raise || has_allin) && (has_three_flush || has_three_of_a_kind || has_a_pair || has_four_flush || has_four_of_a_kind)) {
					return FOLD_ACTION;
				} else {
					return CALL_ACTION;	
				}
			} else {
				memset(point_array, 0, sizeof(point_array));
				save_point(all_cards, 6, point_array);
				for (i = 2; i < 15; ++i) {
					if (point_array[i] == 3 && i >= POINT_Q) {
						if (all_fold) {
							return CALL_ACTION;
						} else if (has_raise || has_allin) {
							if (has_three_flush || has_three_of_a_kind || has_a_pair) {
								return FOLD_ACTION;
							} else {
								for (i = 0; i < cur_inquire.cur_player_num; ++i) {
									if (cur_inquire.cur_states[i].m_player_info.player_id != my_player_id 
										&& (cur_inquire.cur_states[i].action_num == ALLIN_ACTION 
										|| cur_inquire.cur_states[i].action_num == RAISE_ACTION))
									{
										for (j = 0; j < 8; ++j) {
											if (cur_inquire.cur_states[i].m_player_info.player_id == cur_features[j].player_id) {
												if (cur_features[j].like_character == CONSERVATIVE) {
													return FOLD_ACTION;
												}
											}
										}
									}
								}
								return CALL_ACTION;
							}
						} else {
							if (has_three_flush || has_three_of_a_kind || has_a_pair || has_four_flush || has_four_of_a_kind) {
								return FOLD_ACTION;
							} else {
								return CALL_ACTION;
							}
						}
					} else {
						if (all_fold) {
							return CALL_ACTION;
						} else if (has_raise || has_allin) {
							if (has_three_flush || has_three_of_a_kind || has_a_pair || has_four_flush || has_four_of_a_kind) {
								return FOLD_ACTION;
							} else {
								for (i = 0; i < cur_inquire.cur_player_num; ++i) {
									if (cur_inquire.cur_states[i].m_player_info.player_id != my_player_id 
										&& (cur_inquire.cur_states[i].action_num == ALLIN_ACTION 
										|| cur_inquire.cur_states[i].action_num == RAISE_ACTION))
									{
										for (j = 0; j < 8; ++j) {
											if (cur_inquire.cur_states[i].m_player_info.player_id == cur_features[j].player_id) {
												if (cur_features[j].like_character == CONSERVATIVE) {
													return FOLD_ACTION;
												}
											}
										}
									}
								}
								return CALL_ACTION;
							}
						} else {
							if (has_three_flush || has_three_of_a_kind || has_a_pair) {
								return FOLD_ACTION;
							} else {
								return CALL_ACTION;
							}
						}
					}
				}
			}
		} else if (how_big == two_pair) {
			if (has_a_pair || has_three_of_a_kind || has_four_of_a_kind) {
				goto turn_one_pair;
			}

			if (inquire_rounds == 1) {
				if (has_three_of_a_kind || has_four_of_a_kind) {
					return FOLD_ACTION;
				} else {
					return CALL_ACTION;
				}
			} else if (inquire_rounds == 2) {
				if (all_fold) {
					return cur_bet;
				} else if ((has_raise || has_allin) && (has_three_flush || has_three_of_a_kind || has_a_pair || has_four_flush || has_four_of_a_kind)) {
					return FOLD_ACTION;
				} else {
					return CALL_ACTION;	
				}
			} else {
				memset(point_array, 0, sizeof(point_array));
				save_point(all_cards, 6, point_array);
				for (i = 2; i < 15; ++i) {
					if (point_array[i] == 2) {					
						if (pos1 == 0)
							pos1 = i;
						else {
							pos2 = i;
							if (pos1 > POINT_Q || pos2 > POINT_Q) {
								if (all_fold) {
									return CALL_ACTION;
								} else if (has_raise || has_allin) {
									if (has_three_flush || has_three_of_a_kind || has_a_pair) {
										return FOLD_ACTION;
									} else {
										for (i = 0; i < cur_inquire.cur_player_num; ++i) {
											if (cur_inquire.cur_states[i].m_player_info.player_id != my_player_id 
												&& (cur_inquire.cur_states[i].action_num == ALLIN_ACTION 
												|| cur_inquire.cur_states[i].action_num == RAISE_ACTION))
											{
												for (j = 0; j < 8; ++j) {
													if (cur_inquire.cur_states[i].m_player_info.player_id == cur_features[j].player_id) {
														if (cur_features[j].like_character == CONSERVATIVE) {
															return FOLD_ACTION;
														}
													}
												}
											}
										}
										if (cur_inquire.total_pot > (my_jetton / 5)) {
											return FOLD_ACTION;
										} else {
											return CALL_ACTION;
										}
									}
								} else {
									if (has_three_flush || has_three_of_a_kind || has_a_pair || has_four_flush || has_four_of_a_kind) {
										return FOLD_ACTION;
									} else {
										CHECK_ACTION;
									}
								}
							}
						}
					}
				}
			}
		} else if (is_one_pair(all_cards, 6, point_array)) {
turn_one_pair:
			if (has_a_pair || has_three_flush || has_three_of_a_kind || has_four_flush || has_four_of_a_kind) {
				return FOLD_ACTION;
			}
	
			if (inquire_rounds == 1) {
				return CHECK_ACTION;
			} else if (inquire_rounds == 2) {
				if (all_fold) {
					return CALL_ACTION;
				} else if (has_allin || has_raise) {
					return FOLD_ACTION;
				} else {
					return CHECK_ACTION;
				}
			} else {
				if (all_fold) {
					return CALL_ACTION;
				} else if (has_allin || has_raise) {
					return FOLD_ACTION;
				} else {
					if (cur_inquire.total_pot > (my_jetton / 5)) {
						return FOLD_ACTION;
					} else {
						memset(point_array, 0, sizeof(point_array));
						save_point(all_cards, 6, point_array);
						if (point_array[i] == 2 && i > POINT_Q) {
							return CHECK_ACTION;
						}
						return FOLD_ACTION;
					}
				}
			}
		} else {
			goto turn_other_cards;
		}
	}
	goto turn_other_cards;

turn_other_cards:
	if (inquire_rounds == 1) {
		if (cur_inquire.total_pot < (my_jetton / 10)) {
			return CHECK_ACTION;
		} else {
			return FOLD_ACTION;
		}
	} else if (inquire_rounds == 2) {
		if (has_raise || has_allin) {
			return FOLD_ACTION;
		} else {
			if (cur_inquire.total_pot < (my_jetton / 10)) {
				return CHECK_ACTION;
			} else {
				return FOLD_ACTION;
			}
		}
	} else {
		return FOLD_ACTION;
	}
}

int  
river_bet(int inquire_rounds) 
{
	int point_array[15] 	= {0};
	int color_array[4]		= {0};
	int i, j, cur_bet, how_big = 0;
	int inquire_player_num 	= cur_inquire.cur_player_num;
	int pos1 				= 0;
	int pos2 				= 0;
	int my_jetton 			= 0;
	int my_money 			= 0;
	int betted_player_num   = 0;

	int all_fold 			= 1;
	int one_call 			= 0;
	int more_call 			= 0;
	int one_raise_no_call 	= 0;
	int one_raise_more_call = 0;
	int has_raise			= 0;
	int has_allin			= 0;
	int only_check			= 1;

	int check_raise			= 0;
	int has_a_pair			= 0;
	int has_three_of_a_kind	= 0;
	int has_four_of_a_kind  = 0;
	int has_two_pair		= 0;
	int has_two_flush		= 0;
	int has_three_flush		= 0;
	int has_four_flush		= 0;

	// get some info from inquire or seat msg
	if (inquire_rounds == 1) {
		betted_player_num = cur_seat.cur_player_num;
		for (i = 0; i < betted_player_num; ++i) {
			if (cur_seat.players[i].player_id == my_player_id) {
				my_jetton = cur_seat.players[i].jetton;
				my_money = cur_seat.players[i].money;
			}
		}
	} else {
		betted_player_num = cur_inquire.cur_player_num;
		for (i = 0; i < betted_player_num; ++i) {
			if (cur_inquire.cur_states[i].m_player_info.player_id == my_player_id) {
				my_jetton = cur_inquire.cur_states[i].m_player_info.jetton;
				my_money = cur_inquire.cur_states[i].m_player_info.money;
			}
		}
	}

	/** get other players' action */
	for (i = 0; i < inquire_player_num; ++i) {
		if (cur_inquire.cur_states[i].m_player_info.player_id != my_player_id) {
			if (cur_inquire.cur_states[i].action_num != FOLD_ACTION
				&& cur_inquire.cur_states[i].action_num != BLIND_ACTION) {
					all_fold = 0;
			}

			if (cur_inquire.cur_states[i].action_num == CALL_ACTION) {
				++more_call;
			}

			if (cur_inquire.cur_states[i].action_num == RAISE_ACTION) {
				one_raise_more_call = 1;
				has_raise = 1;
			}

			if (cur_inquire.cur_states[i].action_num != CHECK_ACTION 
				&& cur_inquire.cur_states[i].action_num != FOLD_ACTION
				&& cur_inquire.cur_states[i].action_num != BLIND_ACTION) {
				only_check = 0;
			}

			if (cur_inquire.cur_states[i].action_num == ALLIN_ACTION) {
				has_allin = 1;
			}
		}
	}

	if (more_call == 1)
		one_call = 1;
	if (!more_call && one_raise_more_call)
		one_raise_no_call = 1;
	if (one_raise_no_call)
		one_raise_more_call = 0;

	// find current bet
	if ((cur_bet = find_bet()) <= 0) {
		fprintf(stderr, "no inquire bet msg.\n");
		return FOLD_ACTION;
	}

	/** find same flash in three flop cards */
	for (i = 2; i < 7; ++i) {
		j = all_cards[i].m_color;
		if (j >= 0 && j <= 3) {
			++color_array[j];
		}
	}
	for (i = 0; i < 4; ++i) {
		if (color_array[i] == 4) {
			has_four_flush = 1;
			break;
		} else if (color_array[i] == 3) {
			has_three_flush = 1;
			break;
		} else if (color_array[i] == 2) {
			has_two_flush = 1;
			break;
		}
	}

	/** find same kind in three flop cards */
	for (i = 2; i < 7; ++i) {
		j = all_cards[i].m_point;
		if (j >= 2 && j <= 14) {
			++point_array[j];
		}
	}
	for (i = 2; i < 15; ++i) {
		if (point_array[i] == 4) {
			has_four_of_a_kind = 1;
		} else if (point_array[i] == 3) {
			has_three_of_a_kind = 1;
			break;
		} else if (point_array[i] == 2) {
			has_a_pair = 1;
			break;
		}
	}

	// big cards, check one time, then raise forever
	how_big = big_cards(all_cards, 6);

	if (how_big == royal_straight_flush || how_big == straight_flush) {
		if (inquire_rounds == 1) {
			return CALL_ACTION;
		} else if (inquire_rounds <= 3) {
			if (has_raise || has_allin) {
				return CALL_ACTION;
			} else if (one_call || more_call) {
				return CALL_ACTION;
			} else {
				return CHECK_ACTION;
			}
		} else {
			return cur_bet;
		}
	} else if (how_big == four_of_a_kind) {
		if (has_four_of_a_kind) {
			return CHECK_ACTION;
		} else if (has_three_of_a_kind) {
			if (all_fold) {
				return cur_bet;
			} else if (only_check) {
				return CHECK_ACTION;
			} else {
				return CHECK_ACTION;
			}
		} else {
			if (inquire_rounds == 1) {
				return CALL_ACTION;
			} else if (inquire_rounds <= 3) {
				if (has_raise || has_allin) {
					return CALL_ACTION;
				} else if (one_call || more_call) {
					return CALL_ACTION;
				} else {
					return CHECK_ACTION;
				}
			} else {
				return cur_bet;
			}
		}
	} else if (how_big == full_house) {
		if (has_four_of_a_kind) {
			return FOLD_ACTION;
		} else if (has_three_of_a_kind) {
			if (all_fold) {
				return CALL_ACTION;
			} else if (only_check) {
				return CHECK_ACTION;
			} else {
				return CHECK_ACTION;
			}
		} else {
			if (inquire_rounds == 1) {
				return CALL_ACTION;
			} else if (inquire_rounds <= 3) {
				if (has_raise || has_allin) {
					return CALL_ACTION;
				} else if (one_call || more_call) {
					return CALL_ACTION;
				} else {
					return CHECK_ACTION;
				}
			} else {
				return cur_bet;
			}
		}
	} else if (how_big == flush) {
		if (has_four_of_a_kind) {
			return FOLD_ACTION;
		} else if (has_four_flush || has_three_of_a_kind) {
			if (all_fold) {
				return CALL_ACTION;
			} else {
				if (cur_inquire.total_pot < (my_jetton / 5)) {
					return FOLD_ACTION;
				} else {
					return CHECK_ACTION;
				}
			}
		} else {
			if (inquire_rounds == 1) {
				return CALL_ACTION;
			} else if (inquire_rounds <= 3) {
				if (has_raise || has_allin) {
					return CALL_ACTION;
				} else if (one_call || more_call) {
					return CALL_ACTION;
				} else {
					return CHECK_ACTION;
				}
			} else {
				return cur_bet;
			}
		}
	} else {
		/** one check and raise again seems that one has big cards */
		if ((inquire_rounds == 1 && how_big == two_pair 
			&& (has_a_pair || has_two_flush || has_three_flush || has_three_of_a_kind || has_four_flush || has_four_of_a_kind))
			|| (inquire_rounds == 1 && how_big > two_pair)) {			
			for (i = 0; i < inquire_player_num; ++i) {
				if (cur_inquire.cur_states[i].action_num == CHECK_ACTION) {
					check_raise_man.player_id[i] = cur_inquire.cur_states[i].m_player_info.player_id;
				} else {
					check_raise_man.player_id[i] = 0;
				}
			}
		} else if ((inquire_rounds == 2 && how_big == two_pair 
			&& (has_a_pair || has_two_flush || has_three_flush || has_three_of_a_kind || has_four_flush || has_four_of_a_kind))
				|| (inquire_rounds == 2 && how_big > two_pair)) {
			for (i = 0; i < inquire_player_num; ++i) {
				if (cur_inquire.cur_states[i].action_num == RAISE_ACTION) {
					for (j = 0; j < 8; ++j) {
						if (check_raise_man.player_id[j] == cur_inquire.cur_states[i].m_player_info.player_id) {
							return FOLD_ACTION;
						}
					}
				}
			}
		}

		if (how_big == straight) {
			for (i = 0; i < 7; ++i) {
				if (all_cards[i].m_point >= POINT_Q) {
					if (inquire_rounds == 1) {
						return CALL_ACTION;
					} else if (inquire_rounds == 2) {
						if (all_fold) {
							return cur_bet;
						} else if ((has_raise || has_allin) && (has_three_flush || has_three_of_a_kind)) {
							if (cur_inquire.total_pot <= (my_jetton / 5)) {
								return FOLD_ACTION;
							} else {
								for (i = 0; i < cur_inquire.cur_player_num; ++i) {
									if (cur_inquire.cur_states[i].m_player_info.player_id != my_player_id 
										&& (cur_inquire.cur_states[i].action_num == ALLIN_ACTION 
										|| cur_inquire.cur_states[i].action_num == RAISE_ACTION))
									{
										for (j = 0; j < 8; ++j) {
											if (cur_inquire.cur_states[i].m_player_info.player_id == cur_features[j].player_id) {
												if (cur_features[j].like_character == CONSERVATIVE) {
													return FOLD_ACTION;
												}
											}
										}
									}
								}
								return CALL_ACTION;
							}
						} else {
							return cur_bet;	
						}
					} else {
						if (all_fold) {
							return cur_bet;
						} else if (has_allin) {
							if (cur_inquire.total_pot >= (my_jetton / 5)) {
								return FOLD_ACTION;
							} else {
								return CALL_ACTION;
							}
						} else if (has_raise) {
							if (has_three_flush || has_three_of_a_kind || has_four_flush || has_four_of_a_kind) {
								return FOLD_ACTION;
							} else {
								return CALL_ACTION;
							}
						} else {
							if (has_three_flush || has_three_of_a_kind || has_four_flush || has_four_of_a_kind) {
								for (i = 0; i < cur_inquire.cur_player_num; ++i) {
									if (cur_inquire.cur_states[i].m_player_info.player_id != my_player_id 
										&& (cur_inquire.cur_states[i].action_num == ALLIN_ACTION 
										|| cur_inquire.cur_states[i].action_num == RAISE_ACTION))
									{
										for (j = 0; j < 8; ++j) {
											if (cur_inquire.cur_states[i].m_player_info.player_id == cur_features[j].player_id) {
												if (cur_features[j].like_character == CONSERVATIVE) {
													if (cur_inquire.total_pot >= (my_jetton / 5)) {
														return FOLD_ACTION;
													}
												}
											}
										}
									}
								}
								return CALL_ACTION;
							} else {
								return CALL_ACTION;
							}
						}
					}
					return CALL_ACTION;
				} else {
					if (inquire_rounds == 1) {
						return CALL_ACTION;
					} else if (inquire_rounds == 2) {
						if (all_fold) {
							return cur_bet;
						} else if ((has_raise || has_allin) && (has_three_flush || has_three_of_a_kind || has_four_flush || has_four_of_a_kind)) {
							return FOLD_ACTION;
						} else {
							return cur_bet;	
						}
					} else {
						if (all_fold) {
							return cur_bet;
						} else if (has_allin) {
							if ((has_three_flush || has_three_of_a_kind || has_four_flush || has_four_of_a_kind)) {
								return FOLD_ACTION;
							} else {
								return CALL_ACTION;
							}
						} else if (has_raise) {
							if (has_three_flush || has_three_of_a_kind || has_four_flush || has_four_of_a_kind) {
								return FOLD_ACTION;
							} else {
								return CALL_ACTION;
							}
						} else {
							if (has_three_flush || has_three_of_a_kind || has_four_flush || has_four_of_a_kind) {
								for (i = 0; i < cur_inquire.cur_player_num; ++i) {
									if (cur_inquire.cur_states[i].m_player_info.player_id != my_player_id 
										&& (cur_inquire.cur_states[i].action_num == ALLIN_ACTION 
										|| cur_inquire.cur_states[i].action_num == RAISE_ACTION))
									{
										for (j = 0; j < 8; ++j) {
											if (cur_inquire.cur_states[i].m_player_info.player_id == cur_features[j].player_id) {
												if (cur_features[j].like_character == CONSERVATIVE) {
													return FOLD_ACTION;
												}
											}
										}
									}
								}
								return CHECK_ACTION;
							} else {
								return CALL_ACTION;
							}
						}
					}
					return CALL_ACTION;
				}
			}
		} else if (how_big == three_of_a_kind) {
			if (inquire_rounds == 1) {
				if (has_three_of_a_kind) {
					return FOLD_ACTION;
				} else if (has_three_flush) {
					if (all_fold) {
						return CALL_ACTION;
					} else if (only_check) {
						return CHECK_ACTION;
					} else {
						return FOLD_ACTION;
					}
				} else if (has_a_pair) {
					if (all_fold) {
						return CALL_ACTION;
					} else if (has_allin || has_raise) {
						return FOLD_ACTION;
					} else {
						return CALL_ACTION;
					}
				} else {
					return CALL_ACTION;
				}
			} else if (inquire_rounds == 2) {
				if (all_fold) {
					return cur_bet;
				} else if ((has_raise || has_allin) && (has_three_flush || has_three_of_a_kind || has_a_pair || has_four_flush || has_four_of_a_kind)) {
					return FOLD_ACTION;
				} else {
					return CALL_ACTION;	
				}
			} else {
				memset(point_array, 0, sizeof(point_array));
				save_point(all_cards, 7, point_array);
				for (i = 2; i < 15; ++i) {
					if (point_array[i] == 3 && i >= POINT_Q) {
						if (all_fold) {
							return CALL_ACTION;
						} else if (has_raise || has_allin) {
							if (has_three_flush || has_three_of_a_kind || has_a_pair) {
								return FOLD_ACTION;
							} else {
								for (i = 0; i < cur_inquire.cur_player_num; ++i) {
									if (cur_inquire.cur_states[i].m_player_info.player_id != my_player_id 
										&& (cur_inquire.cur_states[i].action_num == ALLIN_ACTION 
										|| cur_inquire.cur_states[i].action_num == RAISE_ACTION))
									{
										for (j = 0; j < 8; ++j) {
											if (cur_inquire.cur_states[i].m_player_info.player_id == cur_features[j].player_id) {
												if (cur_features[j].like_character == CONSERVATIVE) {
													return FOLD_ACTION;
												}
											}
										}
									}
								}
								return CALL_ACTION;
							}
						} else {
							if (has_three_flush || has_three_of_a_kind || has_a_pair || has_four_flush || has_four_of_a_kind) {
								return FOLD_ACTION;
							} else {
								return CALL_ACTION;
							}
						}
					} else {
						if (all_fold) {
							return CALL_ACTION;
						} else if (has_raise || has_allin) {
							if (has_three_flush || has_three_of_a_kind || has_a_pair || has_four_flush || has_four_of_a_kind) {
								return FOLD_ACTION;
							} else {
								for (i = 0; i < cur_inquire.cur_player_num; ++i) {
									if (cur_inquire.cur_states[i].m_player_info.player_id != my_player_id 
										&& (cur_inquire.cur_states[i].action_num == ALLIN_ACTION 
										|| cur_inquire.cur_states[i].action_num == RAISE_ACTION))
									{
										for (j = 0; j < 8; ++j) {
											if (cur_inquire.cur_states[i].m_player_info.player_id == cur_features[j].player_id) {
												if (cur_features[j].like_character == CONSERVATIVE) {
													return FOLD_ACTION;
												}
											}
										}
									}
								}
								return CALL_ACTION;
							}
						} else {
							if (has_three_flush || has_three_of_a_kind || has_a_pair) {
								return FOLD_ACTION;
							} else {
								return CALL_ACTION;
							}
						}
					}
				}
			}
		} else if (how_big == two_pair) {
			if (has_a_pair || has_three_of_a_kind || has_four_of_a_kind) {
				goto turn_one_pair;
			}

			if (inquire_rounds == 1) {
				if (has_three_of_a_kind || has_four_of_a_kind) {
					return FOLD_ACTION;
				} else {
					return CALL_ACTION;
				}
			} else if (inquire_rounds == 2) {
				if (all_fold) {
					return cur_bet;
				} else if ((has_raise || has_allin) && (has_three_flush || has_three_of_a_kind || has_a_pair || has_four_flush || has_four_of_a_kind)) {
					return FOLD_ACTION;
				} else {
					return CALL_ACTION;	
				}
			} else {
				memset(point_array, 0, sizeof(point_array));
				save_point(all_cards, 7, point_array);
				for (i = 2; i < 15; ++i) {
					if (point_array[i] == 2) {					
						if (pos1 == 0)
							pos1 = i;
						else {
							pos2 = i;
							if (pos1 > POINT_Q || pos2 > POINT_Q) {
								if (all_fold) {
									return CALL_ACTION;
								} else if (has_raise || has_allin) {
									if (has_three_flush || has_three_of_a_kind || has_a_pair) {
										return FOLD_ACTION;
									} else {
										for (i = 0; i < cur_inquire.cur_player_num; ++i) {
											if (cur_inquire.cur_states[i].m_player_info.player_id != my_player_id 
												&& (cur_inquire.cur_states[i].action_num == ALLIN_ACTION 
												|| cur_inquire.cur_states[i].action_num == RAISE_ACTION))
											{
												for (j = 0; j < 8; ++j) {
													if (cur_inquire.cur_states[i].m_player_info.player_id == cur_features[j].player_id) {
														if (cur_features[j].like_character == CONSERVATIVE) {
															return FOLD_ACTION;
														}
													}
												}
											}
										}
										if (cur_inquire.total_pot > (my_jetton / 5)) {
											return FOLD_ACTION;
										} else {
											return CALL_ACTION;
										}
									}
								} else {
									if (has_three_flush || has_three_of_a_kind || has_a_pair || has_four_flush || has_four_of_a_kind) {
										return FOLD_ACTION;
									} else {
										CHECK_ACTION;
									}
								}
							}
						}
					}
				}
			}
		} else if (is_one_pair(all_cards, 7, point_array)) {
turn_one_pair:
			if (has_a_pair || has_three_flush || has_three_of_a_kind || has_four_flush || has_four_of_a_kind) {
				return FOLD_ACTION;
			}
	
			if (inquire_rounds == 1) {
				return CHECK_ACTION;
			} else if (inquire_rounds == 2) {
				if (all_fold) {
					return CALL_ACTION;
				} else if (has_allin || has_raise) {
					return FOLD_ACTION;
				} else {
					return CHECK_ACTION;
				}
			} else {
				if (all_fold) {
					return CALL_ACTION;
				} else if (has_allin || has_raise) {
					return FOLD_ACTION;
				} else {
					if (cur_inquire.total_pot > (my_jetton / 5)) {
						return FOLD_ACTION;
					} else {
						memset(point_array, 0, sizeof(point_array));
						save_point(all_cards, 7, point_array);
						if (point_array[i] == 2 && i > POINT_Q) {
							return CHECK_ACTION;
						}
						return FOLD_ACTION;
					}
				}
			}
		} else {
			goto turn_other_cards;
		}
	}
	goto turn_other_cards;

turn_other_cards:
	if (inquire_rounds == 1) {
		if (cur_inquire.total_pot < (my_jetton / 10)) {
			return CHECK_ACTION;
		} else if (only_check) {
			return CHECK_ACTION;
		} else {
			return FOLD_ACTION;
		}
	} else if (inquire_rounds == 2) {
		if (all_fold) {
			return CALL_ACTION;
		} else if (only_check) {
			return CHECK_ACTION;
		} else if (has_raise || has_allin) {
			return FOLD_ACTION;
		} else {
			if (cur_inquire.total_pot < (my_jetton / 10)) {
				return CHECK_ACTION;
			} else {
				return FOLD_ACTION;
			}
		}
	} else {
		if (all_fold) {
			return CALL_ACTION;
		} else if (only_check) {
			return CHECK_ACTION;
		} else {
			return FOLD_ACTION;
		}
	}
}
