#include <stdbool.h>

#include "achievement.h"

typedef enum
{
  DETAIL_NORMAL,
  DETAIL_COMPACT,
  DETAIL_VERY_COMPACT
} Detail_level;

typedef enum
{
  OP_STYLE_NORMAL,
  OP_STYLE_UTF
} Operator_style;

typedef enum
{
  NUMBER_DECIMAL,
  NUMBER_PREFIX_HEX_LOWERCASE,
  NUMBER_PREFIX_HEX_UPPERCASE,
  NUMBER_HEX_LOWERCASE,
  NUMBER_HEX_UPPERCASE
} Number_style;

typedef enum
{
  BORDER_THIN_SHARP,
  BORDER_THIN_ROUNDED,
  BORDER_THICK_SHARP
} Border_style;

typedef enum
{
  TABLE_NO_LINE,
  TABLE_LINE,
} Table_style;

struct PRINT_CONFIG
{                                              // Default Config :
  Detail_level flag_style;                     // .flag_style = DETAIL_NORMAL
  Detail_level type_style;                     // .type_style = DETAIL_NORMAL
  Detail_level size_style;                     // .size_style = DETAIL_NORMAL
  Operator_style op_style;                     // .op_style = OP_STYLE_NORMAL
  Number_style val_style;                      // .val_style = NUMBER_PREFIX_HEX_LOWERCASE
  Number_style mem_style;                      // .mem_style = NUMBER_PREFIX_HEX_LOWERCASE
  Border_style border_style;                   // .border_style = BORDER_THIN
  Table_style table_style;                     // .table_style = TABLE_NO_LINE

  bool show_ach_description;                   // .show_ach_description = true
  bool show_logic_header;                      // .show_logic_header = true

  /* if set to true, collapse any
   * Add Address chain to a list
   * of encoded operations. e.g. :
   *
   * AddA Mem 0x15
   * AddA Mem 0x4
   * AddA Mem 0x6
   *      Mem 0x0 = 0
   *
   * becomes ->
   *
   * I:0x15_I:0x4_I:0x6
   * ╰─>  Mem 0x0 = 0             */
  bool collapse_add_address_chain;             // .collapse_add_address_chain = false

  // append the output to a output.txt file when set to true
  bool print_to_file;                          // .print_to_file = false
};

enum Preset
{
  PRESET_NORMAL, // <- the default
  PRESET_COMPACT,
  PRESET_VERY_COMPACT
};

void use_preset(enum Preset preset);
void set_flag_detail(Detail_level detail);
void set_type_detail(Detail_level detail);
void set_size_detail(Detail_level detail);
void set_op_style(Operator_style op_style);
void set_value_style(Number_style num_style);
void set_mem_style(Number_style num_style);
void set_border_style(Border_style border_style);
void set_table_style(Table_style table_style);
void set_show_ach_descriptiont(bool b);
void set_show_logic_header(bool b);
void set_collapse_add_address_chain(bool b);
void set_print_to_file(bool b);

// NOTE:IT IS MANDATORY TO USE THIS FUNCTION BEFORE DOING ANY PRINTING
// Or else, nothing will show up.
// initialize the locale to UTF-8
void init_printing();

void print_condition(struct CONDITION *condition);
void print_group(struct GROUP *group);
void print_logic(struct ACHIEVEMENT_LOGIC *logic);
void print_achievement(struct ACHIEVEMENT *achievement);
void print_achievement_minimal(struct ACHIEVEMENT *achievement);
void print_leaderboard(struct LEADERBOARD *leaderboard);
void print_leaderboard_minimal(struct LEADERBOARD *leaderboard);
void print_set(struct ACHIEVEMENT_SET *set);
void print_set_minimal(struct ACHIEVEMENT_SET *set);
void print_game(struct GAME *game);
void print_game_minimal(struct GAME *game);
