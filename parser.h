#ifndef PARSER_H
#define PARSER_H

#include "achievement.h"
#include "cJSON.h"

#define MINIMUM_LEADERBOARD_LENGTH 34 // STA:0=0::CAN:0=0::SUB:0=0::VAL:M:0
#define SIZE_REQUIRE_TYPE_FLOAT 18

extern const char condition_separator;
extern const char group_separator;
extern const char char_flag[];
extern const char char_type[];
extern const char char_size[];

int is_recall(char numeral[], size_t len);

Type parse_type(char numeral[], size_t len);
Size parse_size(char numeral[], size_t len, Type type);

long parse_float(char numeral[]);
long parse_value(char numeral[]);
long parse_address(struct NUMERAL *input, char numeral[]);

struct NUMERAL *parse_numeral(char numeral[], size_t len);


Flag parse_flag(char condition[]);
Operator parse_op(char condition[], size_t len, int *index);
long parse_hit_target(char condition[], size_t len);

struct CONDITION *parse_condition(char condition[], size_t len);

struct GROUP *parse_group(char group[], size_t len);

struct ACHIEVEMENT_LOGIC *parse_achievement(char achievement[], size_t len);

struct LEADERBOARD *parse_leaderboard(char leaderboard[], size_t len);

// json parsing status:
#define SUCCESS 0
#define UNKNOWN_ACHIEVEMENT_TYPE 1
#define UNKNOWN_LEADERBOARD_FORMAT 2
#define UNKNOWN_SET_TYPE 3
#define WRONG_JSON_OBJECT_TYPE 4

struct ACHIEVEMENT *parse_achievement_from_json(const cJSON *json_achievement, int *status);
struct LEADERBOARD *parse_leaderboard_from_json(const cJSON *json_leaderboard, int *status);
struct ACHIEVEMENT_SET *parse_achievement_set_from_json(const cJSON *achievement_set, int *status);
struct GAME *parse_game_from_json(char *path);

#endif // !PARSER_H
