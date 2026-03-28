#include "achievement.h"
#include "data.h"
#include <stdio.h>

void append_conditions(struct CONDITION *condition_head, struct GROUP *group)
{
  if (condition_head == NULL) return;

  group->condition_tail->next = condition_head;
  condition_head->prev = group->condition_tail;

  struct CONDITION *tail = condition_head;
  while (tail->next)
  {
    tail = tail->next;
  }
  group->condition_tail = tail;
}

void append_groups(struct GROUP *group_head, struct ACHIEVEMENT_LOGIC *logic)
{
  if (group_head == NULL) return;

  logic->group_tail->next = group_head;
  group_head->prev = logic->group_tail;

  struct GROUP *tail = group_head;
  while (tail->next)
  {
    tail = tail->next;
  }
  logic->group_tail = tail;
}

void append_achievements(struct ACHIEVEMENT *achievement_head, struct ACHIEVEMENT_SET *set)
{
  if (achievement_head == NULL) return;

  set->achievement_tail->next = achievement_head;
  achievement_head->prev = set->achievement_tail;

  struct ACHIEVEMENT *tail = achievement_head;
  while (tail->next)
  {
    tail = tail->next;
  }
  set->achievement_tail = tail;
}

void append_leaderboards(struct LEADERBOARD *leaderboard_head, struct ACHIEVEMENT_SET *set)
{
  if (leaderboard_head == NULL) return;

  set->leaderboard_tail->next = leaderboard_head;
  leaderboard_head->prev = set->leaderboard_tail;

  struct LEADERBOARD *tail = leaderboard_head;
  while (tail->next)
  {
    tail = tail->next;
  }
  set->leaderboard_tail = tail;
}

void append_set(struct ACHIEVEMENT_SET *set,struct GAME *game)
{
  int new_count = game->set_count + 1;
  struct ACHIEVEMENT_SET **temp = realloc(game->sets, new_count * sizeof(struct ACHIEVEMENT_SET *));
  if (!temp)
  {
    printf("REALLOCATION FAILED");
    return;
  }
  game->sets = temp;
  game->sets[game->set_count - 1] = set;
}

void free_condition(struct CONDITION *condition)
{
  if (!condition) return;
  free(condition);
}

void free_group(struct GROUP *group)
{
  if (!group) return;

  struct CONDITION *condition;
  for_each_condition(condition, group)
    free_condition(condition->prev);
  free_condition(group->condition_tail);

  free(group);
}

void free_achievement_logic(struct ACHIEVEMENT_LOGIC *logic)
{
  if (!logic) return;

  struct GROUP *group;
  for_each_group(group, logic)
    free_group(group->prev);
  free_group(logic->group_tail);

  free(logic);
}

void free_achievement(struct ACHIEVEMENT *achievement)
{
  if (!achievement) return;

  free(achievement->title);

  free(achievement->description);
  free_achievement_logic(achievement->logic);

  free(achievement);
}

void free_leaderboard(struct LEADERBOARD *leaderboard)
{
  if (!leaderboard) return;

  free(leaderboard->title);
  free(leaderboard->description);

  free_achievement_logic(leaderboard->start);
  free_achievement_logic(leaderboard->cancel);
  free_achievement_logic(leaderboard->submit);
  free_achievement_logic(leaderboard->value);

  free(leaderboard);
}

void free_set(struct ACHIEVEMENT_SET *set)
{
  if (!set) return;

  struct ACHIEVEMENT *achievement;
  for_each_achievement(achievement, set)
    free_achievement(achievement->prev);
  free_achievement(set->achievement_tail);

  struct LEADERBOARD *leaderboard;
  for_each_leaderboard(leaderboard, set)
    free_leaderboard(leaderboard->prev);
  free_leaderboard(set->leaderboard_tail);

  free(set);
}

void free_game(struct GAME *game)
{
  if (!game) return;

  free(game->title);

  struct ACHIEVEMENT_SET *set;
  for_each_set(set, game)
    free_set(set);

  free(game);
}
