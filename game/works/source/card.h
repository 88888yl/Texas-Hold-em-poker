/** 
 * Create by myl 
 * in 2015-5-20 
 */

#ifndef _CARD_H_
#define _CARD_H_

/** check the card point */
static inline int 
has_point(struct card all_cards[], int cards_num, enum point p) 
{
	int i;
	for (i = 0; i < cards_num; ++i)
		if (all_cards[i].m_point == p)
			return 1;
	return 0;
}

/** In order to count color numbers, save colors in an array. */
static inline int 
save_color(struct card all_cards[], int cards_num, int* color_array) 
{
	int i, n;
	for (i = 0; i < cards_num; ++i) {
		n = all_cards[i].m_color;
		if (n < 0 || n > 3)
			return FAILED;
		++color_array[n];
	}
	return SUCCESS;
}

/** In order to count point numbers, save points in an array. */
static inline int 
save_point(struct card all_cards[], int cards_num, int* point_array) 
{
	int i, n;
	for (i = 0; i < cards_num; ++i) {
		n = all_cards[i].m_point;
		if (n < 2 || n > 14)
			return FAILED;
		++point_array[n];
	}
	return SUCCESS;
}

/** check if cards is flash */
static inline int 
has_same_color(struct card all_cards[], int cards_num, int* color_array) 
{
	int i;
	if (!save_color(all_cards, cards_num, color_array)) {
		fprintf(stderr, "error color\n");
		return FAILED;
	} else {
		for (i = 0; i < 4; ++i) {
			if (color_array[i] >= 5)
				return i;
		}
		return FAILED;
	}
}

/** check if cards has straight */
static inline int 
has_straight(struct card all_cards[], int cards_num, int* point_array) 
{
	int i, j;
	int same = 0;
	if (!save_point(all_cards, cards_num, point_array)) {
		fprintf(stderr, "error point\n");
		return FAILED;
	} else {
		for (i = 2; i < 15; ++i) {
			if (point_array[i]) {
				for (j = 0; j < 15 - i; ++j) {
					if (point_array[i + j])
						++same;
					else {
						if (same >= 5)
							break;
						else
							goto next;
					}
				}
				if (same == 5)
					return i;
				if (same > 5)
					return i + same - 5;
			}
next:
			same = 0;
		}
		return FAILED;
	}
}

/**  those below functions can be used only in Flop round, Turn round and River round.
 *   @all_cards: hole cards and board, max cards is 7.
 *   @cards_num: can be 5, 6 and 7 witch in Flop round, Turn round and River round.
 */
// if cards is Royal Straight Flush
static inline int 
is_royal_straight_flush(struct card all_cards[], int cards_num) 
{
	int i, j, k;
	int pos1 = -1, pos2 = -1;
	int repeat = 1, derepeat = 0;
	enum color common_color;

	switch (cards_num) {
		case 5: {
			common_color = all_cards[0].m_color;
			for (i = 1; i < cards_num; ++i)
				if (all_cards[i].m_color != common_color)
					return FAILED;		
			for (i = 14; i > 9; --i)
				if (!has_point(all_cards, cards_num, i))
					return FAILED;
			return SUCCESS;
		}
		case 6: {
			for (i = 0; i < cards_num; ++i) {
				common_color = all_cards[i].m_color;
				for (j = 0; j < cards_num; ++j) {
					if (i != j) {
						if (all_cards[j].m_color == common_color)
							++repeat;
						else {
							++derepeat;
							if (derepeat == 2)
								break;
							else
								pos1 = j;
						}
					}
				}
				if (repeat >= 5) {
					for (k = 14; k > 9; --k) {
						for (j = 0; j < cards_num; ++j) {
							if (j != pos1 && all_cards[j].m_point == k)
								break;
							else if (j == cards_num - 1)
								return FAILED;
						}
					}
					return SUCCESS;
				}
				if (i == cards_num - 1)
					return FAILED;
				repeat = 1;
				derepeat = 0;
			}
		}
		case 7: {
			for (i = 0; i < cards_num; ++i) {
				common_color = all_cards[i].m_color;
				for (j = 0; j < cards_num; ++j) {
					if (i != j) {
						if (all_cards[j].m_color == common_color)
							++repeat;
						else {
							++derepeat;
							if (derepeat == 3)
								break;
							else {
								if (derepeat == 1)
									pos1 = j;
								if (derepeat == 2)
									pos2 = j;
							}
						}
					}
				}
				if (repeat >= 5) {
					for (k = 14; k > 9; --k) {
						for (j = 0; j < cards_num; ++j) {
							if (j != pos1 && j != pos2 && all_cards[j].m_point == k)
								break;
							else if (j == cards_num - 1)
								return FAILED;
						}
					}
					return SUCCESS;
				}
				if (i == cards_num - 1)
					return FAILED;
				repeat = 1;
				derepeat = 0;
			}
		}
	}

	return 0;
}

// if cards is Straight Flush
static inline int 
is_straight_flush(struct card all_cards[], int cards_num, int* point_array, int* color_array) 
{
	int i, j;
	int color;
	int start_point;
	int checked;
	memset(point_array, 0, 15);
	memset(color_array, 0, 4);
	switch (cards_num) {
		case 5: {
			if (has_same_color(all_cards, cards_num, color_array) && has_straight(all_cards, cards_num, point_array))
				return SUCCESS;
		}
		case 6:
		case 7: {
			if ((color = has_same_color(all_cards, cards_num, color_array)) == FAILED)
				return FAILED;
			if ((start_point = has_straight(all_cards, cards_num, point_array)) == FAILED)
				return FAILED;
			checked = 0;
			for (i = start_point; i < start_point + 5; ++i) {
				for (j = 0; j < cards_num; ++j) {
					if (all_cards[j].m_point == i && all_cards[j].m_color == color) {
						++checked;
						break;
					}
				}
			}
			if (checked == 5)
				return SUCCESS;
			return FAILED;
		}
	}
}

// if cards is Four of a kind
static inline int 
is_four_of_a_kind(struct card all_cards[], int cards_num, int* point_array) 
{
	int i;
	memset(point_array, 0, 15);
	if (!save_point(all_cards, cards_num, point_array)) {
		fprintf(stderr, "error point\n");
		return FAILED;
	} else {
		for (i = 2; i < 15; ++i) {
			if (point_array[i] == 4)
				return SUCCESS;
		}
	}
	return FAILED;
}

// if cards is Full house
static inline int 
is_full_house(struct card all_cards[], int cards_num, int* point_array) 
{
	int i, checked = 0;
	memset(point_array, 0, 15);
	if (!save_point(all_cards, cards_num, point_array)) {
		fprintf(stderr, "error point\n");
		return FAILED;
	} else {
		for (i = 2; i < 15; ++i) {
			if (point_array[i] == 3)
				++checked;
		}
		for (i = 2; i < 15; ++i) {
			if (point_array[i] == 2)
				++checked;
		}
	}
	if (checked == 2)
		return SUCCESS;
	return FAILED;	
}

// if cards is Flush
static inline int 
is_flush(struct card all_cards[], int cards_num, int* color_array) 
{
	memset(color_array, 0, 4);
	if (has_same_color(all_cards, cards_num, color_array) == FAILED)
		return FAILED;
	else
		return SUCCESS;
}

// if cards is Straight
static inline int 
is_straight(struct card all_cards[], int cards_num, int* point_array) 
{
	memset(point_array, 0, 15);
	if (has_straight(all_cards, cards_num, point_array) == FAILED)
		return FAILED;
	else
		return SUCCESS;
}

// if cards is Three of a kind
static inline int 
is_three_of_a_kind(struct card all_cards[], int cards_num, int* point_array) 
{
	int i;
	memset(point_array, 0, 15);
	if (!save_point(all_cards, cards_num, point_array)) {
		fprintf(stderr, "error point\n");
		return FAILED;
	} else {
		for (i = 2; i < 15; ++i) {
			if (point_array[i] == 3)
				return SUCCESS;
		}
	}
	return FAILED;
}

// if cards is Two pair
static inline int 
is_two_pair(struct card all_cards[], int cards_num, int* point_array) 
{
	int i, repeat = 0;
	memset(point_array, 0, 15);
	if (!save_point(all_cards, cards_num, point_array)) {
		fprintf(stderr, "error point\n");
		return FAILED;
	} else {
		for (i = 2; i < 15; ++i) {
			if (point_array[i] == 2) {
				++repeat;
				if (repeat == 2)
					return SUCCESS;
			}
		}
	}
	return FAILED;
}

// if cards is One pair
static inline int 
is_one_pair(struct card all_cards[], int cards_num, int* point_array) 
{
	int i;
	memset(point_array, 0, 15);
	if (!save_point(all_cards, cards_num, point_array)) {
		fprintf(stderr, "error point\n");
		return FAILED;
	} else {
		for (i = 2; i < 15; ++i) {
			if (point_array[i] == 2) {
				return SUCCESS;
			}
		}
	}
	return FAILED;
}

static inline int 
big_hands(int first_hand, int second_hand, int my_jetton, int total_pot, int betted_player_num)
{
	int i, has_all_in = 0;
	if ((all_cards[0].m_point == first_hand || all_cards[1].m_point == first_hand)
		&& (all_cards[0].m_point == second_hand || all_cards[1].m_point == second_hand)) {

		for (i = 0; i < betted_player_num; ++i) {
			if (cur_inquire.cur_states[i].action_num == ALLIN_ACTION)
				has_all_in = 1;
		}

		if (inquire_rounds == 1 && !has_all_in)
			return CALL_ACTION;
		if (inquire_rounds == 2 && !has_all_in)			
			return CHECK_ACTION;
		if (inquire_rounds > 2 && inquire_rounds <= 4) {
			if (betted_player_num < 6) {
				if (has_all_in)
					return FOLD_ACTION;
				return CALL_ACTION;
			} else {
				return FOLD_ACTION;
			}
		}
		if (inquire_rounds > 5)
			return CHECK_ACTION;

		return FOLD_ACTION;
	} else
		return -1;
}

static inline int 
find_bet()
{
	int i;
	if (cur_inquire.total_pot == 60)
		return 40;
	if (cur_inquire.total_pot > 60) {
		if (cur_inquire.cur_player_num == 0)
			return 40;
		else {
			for (i = 0; i < cur_inquire.cur_player_num; ++i) {
				if (cur_inquire.cur_states[i].bet != 0)
					return cur_inquire.cur_states[i].bet;
			}
		}
	}
	return 80;
}

// AA, KK, QQ / AKs / AKo
static inline int 
great_hole_cards(struct card all_cards[])
{
	if (all_cards[0].m_point == POINT_A && all_cards[1].m_point == POINT_A
	 || all_cards[0].m_point == POINT_K && all_cards[1].m_point == POINT_K
	 || all_cards[0].m_point == POINT_Q && all_cards[1].m_point == POINT_Q)
		return SUCCESS;
	if (all_cards[0].m_point == POINT_A && all_cards[1].m_point == POINT_K
	 || all_cards[0].m_point == POINT_K && all_cards[1].m_point == POINT_A)
		return SUCCESS;
	return FAILED;
}

// JJ, 10-10, 99 / AQs, AQo, AJs
static inline int 
big_hole_cards(struct card all_cards[])
{
	if (all_cards[0].m_point == POINT_J && all_cards[1].m_point == POINT_J
	 || all_cards[0].m_point == POINT_10 && all_cards[1].m_point == POINT_10
	 || all_cards[0].m_point == POINT_9 && all_cards[1].m_point == POINT_9)
		return SUCCESS;
	if (all_cards[0].m_point == POINT_A && all_cards[1].m_point == POINT_Q
	 || all_cards[0].m_point == POINT_Q && all_cards[1].m_point == POINT_A)
		return SUCCESS;
	if (all_cards[0].m_point == POINT_A && all_cards[1].m_point == POINT_J
	 || all_cards[0].m_point == POINT_J && all_cards[1].m_point == POINT_A) {
		if (all_cards[0].m_color == all_cards[1].m_color)
			return SUCCESS;
	}
	return FAILED;
}

// AJo, A-10s, A-10o, KQs, KQo
static inline int 
middle_hole_cards(struct card all_cards[])
{
	if (all_cards[0].m_point == POINT_A && all_cards[1].m_point == POINT_J
	 || all_cards[0].m_point == POINT_J && all_cards[1].m_point == POINT_A) {
		if (all_cards[0].m_color != all_cards[1].m_color)
			return SUCCESS;
	}
	if (all_cards[0].m_point == POINT_A && all_cards[1].m_point == POINT_10
	 || all_cards[0].m_point == POINT_10 && all_cards[1].m_point == POINT_A)
		return SUCCESS;
	if (all_cards[0].m_point == POINT_K && all_cards[1].m_point == POINT_Q
	 || all_cards[0].m_point == POINT_Q && all_cards[1].m_point == POINT_K)
		return SUCCESS;
	return FAILED;
}

// 88 to 22 /KJs, K-10s, QJs, Q-10s, J-10s, 10-9s
static inline int 
speculative_hole_cards(struct card all_cards[])
{
	if (all_cards[0].m_point == all_cards[1].m_point) {
		if (all_cards[0].m_point >= POINT_2 && all_cards[0].m_point <= POINT_8)
			return SUCCESS;
	}
	if (all_cards[0].m_color == all_cards[1].m_color) {
		if (all_cards[0].m_point == POINT_K && all_cards[1].m_point == POINT_J
	 	 || all_cards[0].m_point == POINT_J && all_cards[1].m_point == POINT_K)
			return SUCCESS;
		if (all_cards[0].m_point == POINT_K && all_cards[1].m_point == POINT_10
	 	 || all_cards[0].m_point == POINT_10 && all_cards[1].m_point == POINT_K)
			return SUCCESS;
		if (all_cards[0].m_point == POINT_Q && all_cards[1].m_point == POINT_J
	 	 || all_cards[0].m_point == POINT_J && all_cards[1].m_point == POINT_Q)
			return SUCCESS;
		if (all_cards[0].m_point == POINT_Q && all_cards[1].m_point == POINT_10
	 	 || all_cards[0].m_point == POINT_10 && all_cards[1].m_point == POINT_Q)
			return SUCCESS;
		if (all_cards[0].m_point == POINT_J && all_cards[1].m_point == POINT_10
	 	 || all_cards[0].m_point == POINT_10 && all_cards[1].m_point == POINT_J)
			return SUCCESS;
		if (all_cards[0].m_point == POINT_J && all_cards[1].m_point == POINT_9
	 	 || all_cards[0].m_point == POINT_9 && all_cards[1].m_point == POINT_J)
			return SUCCESS;
	}
	return FAILED;
}

//  KJo, K-10o, QJo, Q-10o, J-10o / A9s to A2s, K9s, 98s, 87s
static inline int 
mixed_hole_cards(struct card all_cards[])
{
	if (all_cards[0].m_color != all_cards[1].m_color) {
		if (all_cards[0].m_point == POINT_K && all_cards[1].m_point == POINT_J
	 	 || all_cards[0].m_point == POINT_J && all_cards[1].m_point == POINT_K)
			return SUCCESS;
		if (all_cards[0].m_point == POINT_K && all_cards[1].m_point == POINT_10
	 	 || all_cards[0].m_point == POINT_10 && all_cards[1].m_point == POINT_K)
			return SUCCESS;
		if (all_cards[0].m_point == POINT_Q && all_cards[1].m_point == POINT_J
	 	 || all_cards[0].m_point == POINT_J && all_cards[1].m_point == POINT_Q)
			return SUCCESS;
		if (all_cards[0].m_point == POINT_Q && all_cards[1].m_point == POINT_10
	 	 || all_cards[0].m_point == POINT_10 && all_cards[1].m_point == POINT_Q)
			return SUCCESS;
		if (all_cards[0].m_point == POINT_J && all_cards[1].m_point == POINT_10
	 	 || all_cards[0].m_point == POINT_10 && all_cards[1].m_point == POINT_J)
			return SUCCESS;
	} else {
		if (all_cards[0].m_point == POINT_A) {
			if (all_cards[1].m_point >= 2 && all_cards[1].m_point <= 9)
				return SUCCESS;
		}
		if (all_cards[1].m_point == POINT_A) {
			if (all_cards[0].m_point >= 2 && all_cards[0].m_point <= 9)
				return SUCCESS;
		}
		if (all_cards[0].m_point == POINT_K && all_cards[1].m_point == POINT_9
	 	 || all_cards[0].m_point == POINT_9 && all_cards[1].m_point == POINT_K)
			return SUCCESS;
		if (all_cards[0].m_point == POINT_8 && all_cards[1].m_point == POINT_9
	 	 || all_cards[0].m_point == POINT_9 && all_cards[1].m_point == POINT_8)
			return SUCCESS;
		if (all_cards[0].m_point == POINT_8 && all_cards[1].m_point == POINT_7
	 	 || all_cards[0].m_point == POINT_7 && all_cards[1].m_point == POINT_8)
			return SUCCESS;
	}
	return FAILED;
}

int big_cards(struct card all_cards[], int cards_num);
int analyse_cur_cards(struct card all_cards[], int cards_num);

#endif	// _CARD_H_
