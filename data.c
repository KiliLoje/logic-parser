#include "data.h"

#include <stdio.h>
#include <string.h>

#include "achievement.h"

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

void append_bonus_set(struct ACHIEVEMENT_SET *set,struct GAME *game)
{
  if (!set || set->type == SET_CORE) return;

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


struct CONDITION *get_condition_by_index(struct GROUP *group, int index)
{
  if (!group) return NULL;

  struct CONDITION *condition;
  int i = 0;
  for_each_condition(condition, group)
  {
    if (i == index) return condition;
    i ++;
  }

  return NULL;
}

struct GROUP *get_group_by_index(struct ACHIEVEMENT_LOGIC *logic, int index)
{
  if (!logic) return NULL;

  struct GROUP *group;
  int i = 0;
  for_each_group(group, logic)
  {
    if (i == index) return group;
    i ++;
  }

  return NULL;
}

struct ACHIEVEMENT *get_achievement_by_index(struct ACHIEVEMENT_SET *set, int index)
{
  if (!set) return NULL;

  struct ACHIEVEMENT *achievement;
  int i = 0;
  for_each_achievement(achievement, set)
  {
    if (i == index) return achievement;
    i ++;
  }

  return NULL;
}

struct ACHIEVEMENT *get_achievement_by_id(struct ACHIEVEMENT_SET *set, int ach_id)
{
  if (!set || !ach_id) return NULL;

  struct ACHIEVEMENT *achievement;
  for_each_achievement(achievement, set)
  {
    if (achievement->id == ach_id) return achievement;
  }

  return NULL;
}

struct LEADERBOARD *get_leaderboard_by_index(struct ACHIEVEMENT_SET *set, int index)
{
  if (!set) return NULL;

  struct LEADERBOARD *leaderboard;
  int i = 0;
  for_each_leaderboard(leaderboard, set)
  {
    if (i == index) return leaderboard;
    i ++;
  }

  return NULL;
}

struct LEADERBOARD *get_leaderboard_by_id(struct ACHIEVEMENT_SET *set, int lb_id)
{
  if (!set) return NULL;

  struct LEADERBOARD *leaderboard;
  for_each_leaderboard(leaderboard, set)
  {
    if (leaderboard->id == lb_id) return leaderboard;
  }

  return NULL;
}

struct ACHIEVEMENT_SET *get_core_set(struct GAME *game)
{
  if (!game || !game->sets) return NULL;

  for (int i = 0; i < game->set_count; i ++)
  {
    if (game->sets[i]->type == SET_CORE) return game->sets[i];
    printf("Set type : %d\n", game->sets[i]->type);
  }

  return NULL;
}


struct CONDITION *copy_conditions(struct CONDITION *head, struct CONDITION *tail)
{
  if (!head || !tail) return NULL;

  struct CONDITION *output = NULL;

  struct CONDITION *output_current = NULL;
  struct CONDITION *output_last = NULL;

  for (struct CONDITION *current = head; current != tail->next; current = current->next)
  {
    if (!current) goto deallocate;

    output_current = malloc(sizeof(struct CONDITION));
    if (!output_current) goto deallocate;
    memcpy(output_current, current, sizeof(struct CONDITION));

    output_current->next = NULL;
    output_current->prev = output_last;

    if (output_last)
      output_last->next = output_current;
    else
      output = output_current;

    output_last = output_current;
  }

  return output;

deallocate:
  {
    struct CONDITION *tmp;
    while (output)
    {
      tmp = output->next;
      free_condition(output);
      output = tmp;
    }
    return NULL;
  }
}

struct GROUP *copy_groups(struct GROUP *head, struct GROUP *tail)
{
  if (!head || !tail) return NULL;

  struct GROUP *output = NULL;

  struct GROUP *output_current = NULL;
  struct GROUP *output_last = NULL;

  for (struct GROUP *current = head; current != tail->next; current = current->next)
  {
    if (!current) goto deallocate;

    output_current = malloc(sizeof(struct GROUP));
    if (!output_current) goto deallocate;

    output_current->id = current->id;
    output_current->condition_head = copy_conditions(current->condition_head, current->condition_tail);
    if (!output_current->condition_head) goto deallocate;

    struct CONDITION *tail = output_current->condition_head;
    while (tail->next)
      tail = tail->next;

    output_current->condition_tail = tail;

    output_current->next = NULL;
    output_current->prev = output_last;

    if (output_last)
      output_last->next = output_current;
    else
      output = output_current;

    output_last = output_current;
  }

  return output;

deallocate:
  {
    struct GROUP *tmp;
    while (output)
    {
      tmp = output->next;
      free_group(output);
      output = tmp;
    }
    return NULL;
  }
}

struct ACHIEVEMENT_LOGIC *copy_logic(struct ACHIEVEMENT_LOGIC *logic)
{
  if (!logic) return NULL;

  struct ACHIEVEMENT_LOGIC *output = malloc(sizeof(struct ACHIEVEMENT_LOGIC));
  if (!output) return NULL;

  output->group_head = copy_groups(logic->group_head, logic->group_tail);

  struct GROUP *tail = output->group_head;
  while (tail->next)
    tail = tail->next;

  output->group_tail = tail;

  return output;
}

struct ACHIEVEMENT *copy_achievements(struct ACHIEVEMENT *head, struct ACHIEVEMENT *tail)
{
  if (!head || !tail) return NULL;;

  struct ACHIEVEMENT *output = NULL;

  struct ACHIEVEMENT *output_current = NULL;
  struct ACHIEVEMENT *output_last = NULL;

  for (struct ACHIEVEMENT *current = head; current != tail->next; current = current->next)
  {
    if (!current) goto deallocate;

    output_current = malloc(sizeof(struct ACHIEVEMENT));
    if (!output_current) goto deallocate;

    output_current->id = current->id;
    output_current->points = current->points;
    output_current->type = current->type;

    output_current->title = malloc(strlen(current->title) + 1);
    strcpy(output_current->title, current->title);

    output_current->description = malloc(strlen(current->description) + 1);
    strcpy(output_current->description, current->description);

    output_current->logic = copy_logic(current->logic);
    if (!output_current) goto deallocate;

    output_current->next = NULL;
    output_current->prev = output_last;

    if (output_last)
      output_last->next = output_current;
    else
      output = output_current;

    output_last = output_current;
  }

  return output;

deallocate:
  {
    struct ACHIEVEMENT *tmp;
    while (output)
    {
      tmp = output->next;
      free_achievement(output);
      output = tmp;
    }
    return NULL;
  }
}

struct LEADERBOARD *copy_leaderboards(struct LEADERBOARD *head, struct LEADERBOARD *tail)
{
  if (!head || !tail) return NULL;;

  struct LEADERBOARD *output = NULL;

  struct LEADERBOARD *output_current = NULL;
  struct LEADERBOARD *output_last = NULL;

  for (struct LEADERBOARD *current = head; current != tail->next; current = current->next)
  {
    if (!current) goto deallocate;

    output_current = malloc(sizeof(struct LEADERBOARD));
    if (!output_current) goto deallocate;

    output_current->id = current->id;
    output_current->format = current->format;
    output_current->lower_is_better = current->lower_is_better;

    strcpy(current->title, output_current->title);
    strcpy(current->description, output_current->description);

    output_current->start = copy_logic(current->start);
    output_current->cancel = copy_logic(current->cancel);
    output_current->submit = copy_logic(current->submit);
    output_current->value = copy_logic(current->value);

    output_current->next = NULL;
    output_current->prev = output_last;

    if (output_last)
      output_last->next = output_current;
    else
      output = output_current;

    output_last = output_current;
  }

  return output;

deallocate:
  {
    struct LEADERBOARD *tmp;
    while (output)
    {
      tmp = output->next;
      free_leaderboard(output);
      output = tmp;
    }
    return NULL;
  }
}


void compute_condition_ids(struct GROUP *group)
{
  struct CONDITION *condition;
  int i = 1; // conditions ID is 1-indexed
  for_each_condition(condition, group)
  {
    condition->id = i;
    i ++;
  }
}

void compute_group_ids(struct ACHIEVEMENT_LOGIC *logic)
{
  struct GROUP *group;
  int i = 0;
  for_each_group(group, logic)
  {
    group->id = i;
    i ++;
  }
}


void free_condition(struct CONDITION *condition)
{
  if (!condition) return;
  free(condition);
}

void free_group(struct GROUP *group)
{
  if (!group) return;

  struct CONDITION *condition = group->condition_head;
  while (condition)
  {
    struct CONDITION *tmp = condition->next;
    free(condition);
    condition = tmp;
  }

  free(group);
}

void free_achievement_logic(struct ACHIEVEMENT_LOGIC *logic)
{
  if (!logic) return;

  struct GROUP *group = logic->group_head;
  while (group)
  {
    struct GROUP *tmp = group->next;
    free(group);
    group = tmp;
  }

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

  struct ACHIEVEMENT *achievement = set->achievement_head;
  while (achievement)
  {
    struct ACHIEVEMENT *tmp = achievement->next;
    free_achievement(achievement);
    achievement = tmp;
  }

  struct LEADERBOARD *leaderboard = set->leaderboard_head;
  while (leaderboard)
  {
    struct LEADERBOARD *tmp = leaderboard->next;
    free_leaderboard(leaderboard);
    leaderboard = tmp;
  }

  free(set);
}

void free_game(struct GAME *game)
{
  if (!game) return;

  free(game->title);

  if (game->sets)
    for(int i = 0; i < game->set_count; i ++)
      free_set(game->sets[i]);

  free(game->sets);
  free(game);
}
