#ifndef DATA_H
#define DATA_H

#include "achievement.h"

#define for_each_condition(condition, group) \
  for \
  ( \
    (condition) = (group)->condition_head; \
    (condition) != NULL; \
    (condition) = (condition)->next\
  )

#define for_each_group(group, logic) \
  for \
  ( \
    (group) = (logic)->group_head; \
    (group) != NULL; \
    (group) = group->next \
  )

#define for_each_achievement(achievement, set) \
  for \
  ( \
    (achievement) = (set)->achievement_head; \
    (achievement) != NULL; \
    (achievement) = (achievement)->next \
  )

#define for_each_leaderboard(leaderboard, set) \
  for \
  ( \
    (leaderboard) = (set)->leaderboard_head; \
    (leaderboard) != NULL; \
    (leaderboard) = (leaderboard)->next \
  )

#define for_each_set(set, game) \
  for \
  ( \
    int i = 0; \
    i < (game)->set_count && ((set) = (game)->sets[i], 1); \
    i ++ \
  )

void append_conditions(struct CONDITION *condition_head, struct GROUP *group);
void append_groups(struct GROUP *group_head, struct ACHIEVEMENT_LOGIC *logic);
void append_achievements(struct ACHIEVEMENT *achievement_head, struct ACHIEVEMENT_SET *set);
void append_leaderboards(struct LEADERBOARD *leaderboard_head, struct ACHIEVEMENT_SET *set);
void append_bonus_set(struct ACHIEVEMENT_SET *set, struct GAME *game);

struct CONDITION *get_condition_by_index(struct GROUP *group, int index);
struct GROUP *get_group_by_index(struct ACHIEVEMENT_LOGIC *logic, int index);
struct ACHIEVEMENT *get_achievement_by_index(struct ACHIEVEMENT_SET *set, int index);
struct ACHIEVEMENT *get_achievement_by_id(struct ACHIEVEMENT_SET *set, int ach_id);
struct LEADERBOARD *get_leaderboard_by_index(struct ACHIEVEMENT_SET *set, int index);
struct LEADERBOARD *get_leaderboard_by_id(struct ACHIEVEMENT_SET *set, int lb_id);
struct ACHIEVEMENT_SET *get_core_set(struct GAME *game);

void free_condition(struct CONDITION *condition);
void free_group(struct GROUP *group);
void free_achievement_logic(struct ACHIEVEMENT_LOGIC *logic);
void free_achievement(struct ACHIEVEMENT *achievement);
void free_leaderboard(struct LEADERBOARD *leaderboard);
void free_set(struct ACHIEVEMENT_SET *set);
void free_game(struct GAME *game);

#endif // !DATA_H
