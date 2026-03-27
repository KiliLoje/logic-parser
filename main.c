#include <string.h>
#include <stdio.h>

#include "parser.h"
#include "achievement.h"

const char *FLAGS[] = {
  "",
  "Pause If",
  "Reset If",
  "Reset Next If",
  "Add Source",
  "Sub Source",
  "Add Hits",
  "Sub Hits",
  "Add Address",
  "And Next",
  "Or Next",
  "Measured",
  "Measured If",
  "Trigger",
  "Remember",
};

const char *TYPES[] = {
  "Mem",
  "Value",
  "Delta",
  "Prior",
  "BCD",
  "Float",
  "Invert",
  "Recall"
};

const char *SIZES[] = {
 "",
 "Bit0",
 "Bit1",
 "Bit2",
 "Bit3",
 "Bit4",
 "Bit5",
 "Bit6",
 "Bit7",
 "Lower4",
 "Upper4",
 "8-bit",
 "16-bit",
 "24-bit",
 "32-bit",
 "16-bit BE",
 "24-bit BE",
 "32-bit BE",
 "BitCount",
 "Float",
 "Float BE",
 "Double32",
 "Double32 BE",
 "MBF32",
 "MBF32 LE"
};

const char *OP[] = {
  "",
  "=",
  "<",
  "<=",
  ">",
  ">=",
  "!=",
  "*",
  "/",
  "%",
  "+",
  "-",
  "&",
  "^"
};

const char *CHEEVO_TYPE[] = {
  "",
  "Progression",
  "Win Condition",
  "Missable"
};

const char *LB_FORMAT[] = {
  "Score",
  "Frames",
  "CentiSecs",
  "Seconds",
  "Minutes",
  "Seconds as Minutes",
  "Value",
  "Unsigned",
  "Tens",
  "Hundreds",
  "Thousands",
  "Fixed1",
  "Fixed2",
  "Fixed3"
};

int main(int argc, char *argv[])
{
  struct GAME *game = get_game_from_json(argv[1]);
  struct ACHIEVEMENT_SET *achievement_set = game->sets[0];
  if (achievement_set == NULL) printf("Parsing of FILE failed\n");

  printf("\n\n%d   %s\n", game->id, game->title);
  printf("%d\n\n\n", game->consoleID);
  for (int i = 0; i < achievement_set->achievement_count; i ++)
  {
    struct ACHIEVEMENT *achievement = achievement_set->achievements[i];
    printf("%d|  %s   %d   %s\n", achievement->id, achievement->title, achievement->points, CHEEVO_TYPE[achievement->type]);
    printf("%s\n\n", achievement->description);
    free(achievement);
  }
  printf("\n");
  for (int i = 0; i < achievement_set->leaderboard_count; i ++)
  {
    struct LEADERBOARD *leaderboard = achievement_set->leaderboards[i];
    printf("%d|  %s   %s   %d\n", leaderboard->id, leaderboard->title, LB_FORMAT[leaderboard->format], leaderboard->lower_is_better);
    printf("%s\n\n", leaderboard->description);
    free(leaderboard);
  }

  return EXIT_SUCCESS;
}
