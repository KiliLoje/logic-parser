#include "print.h"
#include "achievement.h"

#include <wchar.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <stdbool.h>

struct TEXT_BLOC
{
  size_t line_count;
  wchar_t **lines;
};


static const struct PRINT_CONFIG PRESETS[] = {
  {
    .flag_style = DETAIL_NORMAL,
    .type_style = DETAIL_NORMAL,
    .size_style = DETAIL_NORMAL,
    .op_style = OP_STYLE_NORMAL,
    .val_style = NUMBER_PREFIX_HEX_LOWERCASE,
    .mem_style = NUMBER_PREFIX_HEX_LOWERCASE,
    .border_style = BORDER_THIN_ROUNDED,
    .table_style = TABLE_NO_LINE,
    .show_ach_description = true,
    .show_logic_header = true,
    .collapse_add_address_chain = false,
    .print_to_file = false
  },
  {
    .flag_style = DETAIL_COMPACT,
    .type_style = DETAIL_COMPACT,
    .size_style = DETAIL_COMPACT,
    .op_style = OP_STYLE_NORMAL,
    .val_style = NUMBER_HEX_UPPERCASE,
    .mem_style = NUMBER_HEX_UPPERCASE,
    .border_style = BORDER_THIN_ROUNDED,
    .table_style = TABLE_NO_LINE,
    .show_ach_description = true,
    .show_logic_header = true,
    .collapse_add_address_chain = false,
    .print_to_file = false
  },
  {
    .flag_style = DETAIL_VERY_COMPACT,
    .type_style = DETAIL_VERY_COMPACT,
    .size_style = DETAIL_VERY_COMPACT,
    .op_style = OP_STYLE_NORMAL,
    .val_style = NUMBER_HEX_UPPERCASE,
    .mem_style = NUMBER_HEX_UPPERCASE,
    .border_style = BORDER_THIN_ROUNDED,
    .table_style = TABLE_NO_LINE,
    .show_ach_description = false,
    .show_logic_header = false,
    .collapse_add_address_chain = true,
    .print_to_file = false
  },
};

static struct PRINT_CONFIG global_config = PRESETS[PRESET_NORMAL];

#define HORIZ_BAR 0
#define VERT_BAR 1
#define DOWN_RIGHT 2
#define DOWN_LEFT 3
#define UP_RIGHT 4
#define UP_LEFT 5
#define UP_DOWN_RIGHT 6
#define UP_DOWN_LEFT 7
#define DOWN_RIGHT_LEFT 8
#define UP_RIGHT_LEFT 9
#define UP_DOWN_RIGHT_LEFT 10

static const wchar_t table_chars[][11] =
{
  {
    L'─',L'│',L'┌',L'┐',L'└',L'┘',L'├',L'┤',L'┬',L'┴',L'┼'
  },
  {
    L'─',L'│',L'╭',L'╮',L'╰',L'╯',L'├',L'┤',L'┬',L'┴',L'┼'
  },
  {
    L'━',L'┃',L'┏',L'┓',L'┗',L'┛',L'┣',L'┫',L'┳',L'┻',L'╋'
  }
};

static const wchar_t * const flags[][15] =
{
  { /* DETAIL_NORMAL */
    L"",
    L"Pause If",
    L"Reset If",
    L"ResetNextIf",
    L"Add Source",
    L"Sub Source",
    L"Add Hits",
    L"Sub Hits",
    L"Add Address",
    L"And Next",
    L"Or Next",
    L"Measured",
    L"Measured If",
    L"Trigger",
    L"Remember"
  },
  { /* DETAIL_COMPACT */
    L"",
    L"Paus",
    L"Rese",
    L"RNIf",
    L"AddS",
    L"SubS",
    L"AddH",
    L"SubH",
    L"AddA",
    L"AndN",
    L"Or N",
    L"Meas",
    L"MeIf",
    L"Trig",
    L"Remb"
  },
  { /* DETAIL_VERY_COMPACT */
    L"",
    L"P",
    L"R",
    L"Z",
    L"A",
    L"B",
    L"C",
    L"D",
    L"I",
    L"N",
    L"O",
    L"M",
    L"Q",
    L"T",
    L"K"
  }
};

static const wchar_t *types[][8] =
{
  {
    L"Mem",        // DETAIL_NORMAL
    L"Value",
    L"Delta",
    L"Prior",
    L"BCD",
    L"Float",
    L"Invert",
    L"Recall",
  },
  {
    L"Mem",        // DETAIL_COMPACT
    L"Val",
    L"Del",
    L"Pri",
    L"BCD",
    L"Flt",
    L"Inv",
    L"Rec",
  },
  {
    L"m",          // DETAIL_VERY_COMPACT
    L"v",
    L"d",
    L"p",
    L"b",
    L"f",
    L"~"
  }
};

static const wchar_t * const sizes[][25] =
{
  {
   L"",               // DETAIL_NORMAL
   L"Bit0",
   L"Bit1",
   L"Bit2",
   L"Bit3",
   L"Bit4",
   L"Bit5",
   L"Bit6",
   L"Bit7",
   L"Lower4",
   L"Upper4",
   L"8-bit",
   L"16-bit",
   L"24-bit",
   L"32-bit",
   L"16-bit BE",
   L"24-bit BE",
   L"32-bit BE",
   L"BitCount",
   L"Float",
   L"Float BE",
   L"Double32",
   L"Double32 BE",
   L"MBF32",
   L"MBF32 LE",
  },
  {
    L"",            // DETAIL_COMPACT
    L"Bit0",
    L"Bit1",
    L"Bit2",
    L"Bit3",
    L"Bit4",
    L"Bit5",
    L"Bit6",
    L"Bit7",
    L"Lower 4",
    L"Upper 4",
    L"8bit",
    L"16bit",
    L"24bit",
    L"32bit",
    L"16bitBE",
    L"24bitBE",
    L"32bitBE",
    L"BitCnt",
    L"Float",
    L"FloatBE",
    L"db32",
    L"db32BE",
    L"MBF32",
    L"MBF32BE",
  },
  {
    L"",           // DETAIL_VERY_COMPACT
    L"M",
    L"N",
    L"O",
    L"P",
    L"Q",
    L"R",
    L"S",
    L"T",
    L"L",
    L"U",
    L"H",
    L" ",
    L"W",
    L"X",
    L"I",
    L"J",
    L"G",
    L"K",
    L"fF",
    L"fB",
    L"fH",
    L"fI",
    L"fM",
    L"fL"
  },
};

static const wchar_t * const operator[][14] =
{
  {
   L"",
   L"=",
   L"<",
   L"<=",
   L">",
   L">=",
   L"!=",
   L"*",
   L"/",
   L"%",
   L"+",
   L"-",
   L"&",
   L"^",
  },
  {
   L"",
   L"=",
   L"<",
   L"≤",
   L">",
   L"≥",
   L"≠",
   L"×",
   L"÷",
   L"%",
   L"+",
   L"-",
   L"∧",
   L"⊕"
  }
};

static const wchar_t * const ach_type[][4] =
{
  {
    L"",
    L"Progression",
    L"Win Condition",
    L"Missable",
  },
  {
    L"",
    L"prog",
    L"win con",
    L"miss",
  },
  {
    L"",
    L"(#)",
    L"(*)",
    L"(!)"
  }
};

static const char *lb_formats[] =
{
  "Score",
  "Frames",
  "Centiseconds",
  "Seconds",
  "Minutes",
  "Seconds as minutes",
  "Value",
  "Unsigned",
  "Tens",
  "Hundreds",
  "Thousands",
  "Fixed1",
  "Fixed2",
  "Fixed3"
};

static const char *set_types[] =
{
  "core",
  "subset"
};

static const char *consoles[] =
{
  "",
    "Mega Drive",
    "Nintendo 64",
    "SNES",
    "Game Boy",
    "Game Boy Advance",
    "Game Boy Color",
    "NES",
    "PC Engine",
    "Sega CD",
    "Sega 32X",
    "Master System",
    "Playstation",
    "Lynx",
    "NeoGeo Pocket",
    "GameGear",
    "GameCube",
    "Jaguar",
    "DS",
    "Wii",
    "Wii-U",
    "PS2",
    "XBOX",
    "Magnavox Odyssey",
    "Pokemon Mini",
    "Atatri 2600",
    "MSDOS",
    "Arcade",
    "Virtual Boy",
    "MSX",
    "C64",
    "ZX81",
    "Oric",
    "SG1000",
    "VIC20",
    "Amiga",
    "Atari ST",
    "Amstrad CPC",
    "Apple II",
    "Saturn",
    "DreamCast",
    "PSP",
    "CDi",
    "3DO",
    "Colecovision",
    "Intellivision",
    "Vectrex",
    "PC 8800",
    "PC 9800",
    "PCFX",
    "Atari 5200",
    "Atari 7800",
    "X68K",
    "WonderSwan",
    "Cassette Vision",
    "Super Cassette Vision",
    "NeoGeo CD",
    "Channel F",
    "FMTowns",
    "ZX Spectrum",
    "Game n Watch",
    "NokiaNGage",
    "Nintendo 3DS",
    "Supervision",
    "Sharp X1",
    "TIC-80",
    "Thomson TO8",
    "PC 6000",
    "PICO-8",
    "Mega Duck",
    "Zeebo",
    "Arduboy",
    "WASM 4",
    "Arcadia 2001",
    "Interton VC4000",
    "Elektor TV Game Computer",
    "PC Engine CD",
    "Jaguar CD",
    "DSi",
    "TI-83",
    "Uzebox",
    "Famicom Disk System"
};

void init_printing()
{
  setlocale(LC_ALL, "en_US.UTF-8");
}

void use_preset(enum Preset preset)
{
  global_config = PRESETS[preset];
}

void set_flag_detail(Detail_level detail)
{
  global_config.flag_style = detail;
}

void set_type_detail(Detail_level detail)
{
  global_config.type_style = detail;
}

void set_size_detail(Detail_level detail)
{
  global_config.size_style = detail;
}

void set_op_style(Operator_style op_style)
{
  global_config.op_style = op_style;
}

void set_value_style(Number_style num_style)
{
  global_config.val_style = num_style;
}

void set_mem_style(Number_style num_style)
{
  global_config.mem_style = num_style;
}

void set_border_style(Border_style border_style)
{
  global_config.border_style = border_style;
}

void set_table_style(Table_style table_style)
{
  global_config.table_style = table_style;
}

void set_show_ach_descriptiont(bool b)
{
  global_config.show_ach_description = b;
}

void set_show_logic_header(bool b)
{
  global_config.show_logic_header = b;
}

void set_collapse_add_address_chain(bool b)
{
  global_config.collapse_add_address_chain = b;
}

void set_print_to_file(bool b)
{
  global_config.print_to_file = b;
}

static const int collumn_widths[][3] = {
  {11, 6, 11},
  {4, 3, 7},
  {1, 1, 1}
};

#define COND_ID_WIDTH 4
#define HIT_TARGET_WIDTH 8

static inline wchar_t *get_container_left(int depth, struct PRINT_CONFIG *config)
{
  int len_per_unit = 2; // VERT_BAR + ' '
  size_t total_len = depth * len_per_unit + 1;
  wchar_t *output = malloc(sizeof(wchar_t) * total_len);
  if (!output) return NULL;

  wchar_t *ptr = output;
  for (int i = 0; i < depth; i ++)
  {
    *ptr = table_chars[config->border_style][VERT_BAR];
    ptr ++;
    *ptr = L' ';
    ptr ++;
  }
  *ptr = L'\0';

  return output;
}

static inline wchar_t *get_container_right(int depth, struct PRINT_CONFIG *config)
{
  int len_per_unit = 2; // ' ' + VERT_BAR
  size_t total_len = depth * len_per_unit + 1;
  wchar_t *output = malloc(sizeof(wchar_t) * total_len);
  if (!output) return NULL;

  wchar_t *ptr = output;
  for (int i = 0; i < depth; i ++)
  {
    *ptr = L' ';
    ptr ++;
    *ptr = table_chars[config->border_style][VERT_BAR];
    ptr ++;
  }
  *ptr = L'\0';

  return output;
}

static wchar_t *contain_line(wchar_t *line, int depth, struct PRINT_CONFIG *config)
{
  const wchar_t *left = get_container_left(depth, config);
  const wchar_t *right = get_container_right(depth, config);
  if (!left || !right) return NULL;

  size_t total_len = wcslen(left) + wcslen(right) + wcslen(line);
  wchar_t *output = malloc(sizeof(wchar_t) * (total_len + 1));
  if (!output)
  {
    free((void *)left);
    free((void *)right);
    return NULL;
  }
  output[0] = '\0';


  wcscat(output, left);
  wcscat(output, line);
  wcscat(output, right);

  free((void *)left); free((void *)right);

  return output;
}

#define FLAG 0
#define TYPE 1
#define SIZE 2
#define VALUE 3
#define OP 4
#define HIT_TARGET 5

static const wchar_t * const logic_header_txt[][6] = {
  {L"Flag", L"Type", L"Size", L"Value", L"op", L"Hits"},
  {L"Flag", L"Typ", L"Size", L"Value", L"op", L"Hits"},
  {L"F", L"T", L"S", L"Value", L"op", L"Hits"}
};

static int get_logic_line_length(struct PRINT_CONFIG *config)
{
  int id_len = COND_ID_WIDTH;
  int op_len = 2;
  int hit_target_len = HIT_TARGET_WIDTH;
  int value_len = 8;
  int separator_len = 1;

  int hex_prefix_len = 0;
  if (
    config->mem_style == NUMBER_PREFIX_HEX_LOWERCASE ||
    config->mem_style == NUMBER_PREFIX_HEX_UPPERCASE ||
    config->val_style == NUMBER_PREFIX_HEX_LOWERCASE ||
    config->val_style == NUMBER_PREFIX_HEX_UPPERCASE
  ) hex_prefix_len = 2;

  int output = 0;
  output += separator_len + id_len; // |ID
  output += separator_len + collumn_widths[config->flag_style][FLAG]; // |FLAG
  output += (
    separator_len + collumn_widths[config->type_style][TYPE] // |TYPE
  + separator_len + collumn_widths[config->size_style][SIZE] // |SIZE
  + separator_len + hex_prefix_len + value_len                    // |0xVALUE
  ) * 2; // for both operand
  output += separator_len + op_len; // |OP
  output += separator_len + hit_target_len + separator_len; // |HIT TARGET|

  return output;
}

static inline wchar_t *pwchar(wchar_t *ptr, wchar_t c)
{
  *ptr = c;
  return ptr + 1;
}

static wchar_t *put_titled_section(wchar_t *ptr, const wchar_t * const title, int section_len, struct PRINT_CONFIG *config)
{
  int title_len = wcslen(title);
  int start_index = (section_len - title_len) / 2;
  if (title_len > section_len) title_len = 0;
  for (int i = 0; i < section_len; i ++)
  {
    int title_pos = i - start_index;
    if (title_pos >= 0 && title_pos < title_len && title[title_pos] != L' ')
      ptr = pwchar(ptr, title[title_pos]);
    else
      ptr = pwchar(ptr, table_chars[config->border_style][HORIZ_BAR]);
  }
  return ptr;
}

static wchar_t *get_group_title(int id)
{
  if (id == 0) return L"CORE";

  int id_len = 0;
  int tmp = id;
  while (tmp)
  {
    tmp = tmp / 10;
    id_len += 1;
  }

  wchar_t *output = malloc(sizeof(wchar_t) * (id_len + 5));
  if(!output) return NULL;
  swprintf(output, id_len + 5, L"ALT %d", id);
  return output;
}

static wchar_t *get_logic_header(bool is_header, struct PRINT_CONFIG *config)
{
  wchar_t collumn_separator;
  if (config->table_style == TABLE_LINE) collumn_separator = table_chars[config->border_style][DOWN_RIGHT_LEFT];
  else collumn_separator = table_chars[config->border_style][HORIZ_BAR];

  wchar_t start_char;
  wchar_t end_char;
  if (!is_header)
  {
    start_char = table_chars[config->border_style][UP_DOWN_RIGHT];
    end_char = table_chars[config->border_style][UP_DOWN_LEFT];
  }
  else
  {
    start_char = table_chars[config->border_style][DOWN_RIGHT];
    end_char = table_chars[config->border_style][DOWN_LEFT];
  }

  int operand = 0;

  size_t line_len = get_logic_line_length(config);

  const int id_len = 4;
  const int op_len = 2;
  const int hex_prefix  = 2 *
  (
    config->mem_style == NUMBER_PREFIX_HEX_LOWERCASE ||
    config->mem_style == NUMBER_PREFIX_HEX_UPPERCASE ||
    config->val_style == NUMBER_PREFIX_HEX_LOWERCASE ||
    config->val_style == NUMBER_PREFIX_HEX_UPPERCASE
  );
  const int value_len = 8 + hex_prefix;
  const int hit_target_len = 8;

  wchar_t *output = malloc(sizeof(wchar_t) * (line_len + 4));
  if (!output) return NULL;

  wchar_t *ptr = output;

  ptr = pwchar(ptr, start_char);
  ptr = pwchar(ptr, L'#');
  for (int i = 1; i < id_len; i ++) ptr = pwchar(ptr, table_chars[config->border_style][HORIZ_BAR]);
  ptr = pwchar(ptr, table_chars[config->border_style][DOWN_RIGHT_LEFT]);

  ptr = put_titled_section(
    ptr,
    logic_header_txt[config->flag_style][FLAG],
    collumn_widths[config->flag_style][FLAG],
    config);
  ptr = pwchar(ptr, collumn_separator);

operand:
  {
    operand ++;

    ptr = put_titled_section(
      ptr,
      logic_header_txt[config->type_style][TYPE],
      collumn_widths[config->type_style][TYPE],
      config);
    ptr = pwchar(ptr, collumn_separator);

    ptr = put_titled_section(
      ptr,
      logic_header_txt[config->size_style][SIZE],
      collumn_widths[config->size_style][SIZE],
      config);
    ptr = pwchar(ptr, collumn_separator);

    ptr = put_titled_section(
      ptr,
      logic_header_txt[0][VALUE],
      value_len,
      config);
    ptr = pwchar(ptr, collumn_separator);
  }
  if (operand == 2) goto hit_target;

  ptr = put_titled_section(
    ptr,
    logic_header_txt[0][OP],
    op_len,
    config);
  ptr = pwchar(ptr, collumn_separator);

  goto operand;
hit_target:
  ptr = put_titled_section(
    ptr,
    logic_header_txt[0][HIT_TARGET],
    hit_target_len,
    config);
  ptr = pwchar(ptr, end_char);
  *ptr = L'\0';

  size_t written = ptr - output;
  return output;
}

static wchar_t *get_header_line(wchar_t *title, int line_len, struct PRINT_CONFIG *config)
{
  wchar_t *output = malloc(sizeof(wchar_t) * (line_len + 1));
  if (!output) return NULL;

  wchar_t start_char = table_chars[config->border_style][DOWN_RIGHT];
  wchar_t end_char = table_chars[config->border_style][DOWN_LEFT];
  wchar_t middle_char = table_chars[config->border_style][HORIZ_BAR];
  wchar_t *ptr = output;
  int title_len = wcslen(title);
  int inner_len = line_len - 2;

  int start_index = (inner_len - title_len) / 2;

  if (title_len > inner_len) title_len = 0; // do not print the title if it's bigger than the line

  ptr = pwchar(ptr, start_char);
  ptr = put_titled_section(
    ptr,
    title,
    inner_len,
  config);
  ptr = pwchar(ptr, end_char);
  *ptr = L'\0';

  return output;
}

static wchar_t *get_logic_footer(struct PRINT_CONFIG *config)
{
  int line_len = get_logic_line_length(config);
  wchar_t *output = malloc(sizeof(wchar_t) * (line_len + 1));
  if (!output) return NULL;
  wchar_t *ptr = output;

  wchar_t middle_char = table_chars[config->border_style][HORIZ_BAR];
  wchar_t separator = table_chars[config->border_style][HORIZ_BAR];
  if (config->table_style == TABLE_LINE) separator = table_chars[config->border_style][UP_RIGHT_LEFT];

  const int hex_prefix  = 2 *
  (
    config->mem_style == NUMBER_PREFIX_HEX_LOWERCASE ||
    config->mem_style == NUMBER_PREFIX_HEX_UPPERCASE ||
    config->val_style == NUMBER_PREFIX_HEX_LOWERCASE ||
    config->val_style == NUMBER_PREFIX_HEX_UPPERCASE
  );
  const int value_len = 8 + hex_prefix;

  ptr = pwchar(ptr, table_chars[config->border_style][UP_RIGHT]);
  for (int i = 0; i < COND_ID_WIDTH; i ++) ptr = pwchar(ptr, middle_char);
  ptr = pwchar(ptr, table_chars[config->border_style][UP_RIGHT_LEFT]);
  for (int i = 0; i < collumn_widths[config->flag_style][FLAG]; i ++) ptr = pwchar(ptr, middle_char);
  ptr = pwchar(ptr, separator);
  for (int i = 0; i < collumn_widths[config->type_style][TYPE]; i ++) ptr = pwchar(ptr, middle_char);
  ptr = pwchar(ptr, separator);
  for (int i = 0; i < collumn_widths[config->size_style][SIZE]; i ++) ptr = pwchar(ptr, middle_char);
  ptr = pwchar(ptr, separator);
  for (int i = 0; i < value_len; i ++) ptr = pwchar(ptr, middle_char);
  ptr = pwchar(ptr, separator);
  for (int i = 0; i < 2; i ++) ptr = pwchar(ptr, middle_char);
  ptr = pwchar(ptr, separator);
  for (int i = 0; i < collumn_widths[config->type_style][TYPE]; i ++) ptr = pwchar(ptr, middle_char);
  ptr = pwchar(ptr, separator);
  for (int i = 0; i < collumn_widths[config->size_style][SIZE]; i ++) ptr = pwchar(ptr, middle_char);
  ptr = pwchar(ptr, separator);
  for (int i = 0; i < value_len; i ++) ptr = pwchar(ptr, middle_char);
  ptr = pwchar(ptr, separator);
  for (int i = 0; i < HIT_TARGET_WIDTH; i ++) ptr = pwchar(ptr, middle_char);
  ptr = pwchar(ptr, table_chars[config->border_style][UP_LEFT]);

  *ptr = L'\0';

  return output;
}

static wchar_t *get_condition_line(struct CONDITION *condition, struct PRINT_CONFIG *config)
{
  int line_len = get_logic_line_length(config);
  wchar_t *output = malloc(sizeof(wchar_t) * (line_len + 1));
  if (!output) return NULL;

  const int hex_prefix  = 2 *
  (
    config->mem_style == NUMBER_PREFIX_HEX_LOWERCASE ||
    config->mem_style == NUMBER_PREFIX_HEX_UPPERCASE ||
    config->val_style == NUMBER_PREFIX_HEX_LOWERCASE ||
    config->val_style == NUMBER_PREFIX_HEX_UPPERCASE
  );
  const int value_len = 8 + hex_prefix;

  wchar_t line_char = table_chars[config->border_style][VERT_BAR];
  wchar_t separator = L' ';
  if (config->table_style == TABLE_LINE) separator = line_char;

  wchar_t *ptr = output;
  ptr = pwchar(ptr, line_char);

  int written;
  int remaining = line_len;

  written = swprintf(ptr, remaining, L"%-*d", COND_ID_WIDTH, condition->id);
  if (written < 0 || written > remaining) goto deallocate;
  ptr += written;
  remaining -= written + 1;
  ptr = pwchar(ptr, line_char);

  written = swprintf(ptr, remaining,
                     L"%-*ls", collumn_widths[config->flag_style][FLAG],
                     flags[config->flag_style][condition->flag]);
  if (written < 0 || written > remaining) goto deallocate;
  ptr += written;
  remaining -= written + 1;
  ptr = pwchar(ptr, separator);

  struct NUMERAL *num = &condition->lhs;
operand :
  written = swprintf(ptr, remaining,
                     L"%-*ls", collumn_widths[config->type_style][TYPE],
                     types[config->type_style][num->type]);
  if (written < 0 || written > remaining) goto deallocate;
  ptr += written;
  remaining -= written + 1;
  ptr = pwchar(ptr, separator);

  written = swprintf(ptr, remaining,
                     L"%-*ls", collumn_widths[config->size_style][SIZE],
                     sizes[config->size_style][num->size]);
  if (written < 0 || written > remaining) goto deallocate;
  ptr += written;
  remaining -= written + 1;
  ptr = pwchar(ptr, separator);

  switch (num->type)
  {
    case TYPE_FLOAT:
      {
        float f;
        memcpy(&f, &num->value, sizeof(float));
        written = swprintf(ptr, remaining, L"%-*f", value_len, f);
      }
      break;
    case TYPE_RECALL:
      written = swprintf(ptr, remaining, L"%*s", value_len, " ");
      break;
    case TYPE_VALUE:
      switch (config->val_style)
      {
        case NUMBER_DECIMAL:
          written = swprintf(ptr, remaining, L"%-*d", value_len, num->value);
          break;
        case NUMBER_HEX_UPPERCASE:
          written = swprintf(ptr, remaining, L"%-*X", value_len, num->value);
          break;
        case NUMBER_HEX_LOWERCASE:
          written = swprintf(ptr, remaining, L"%-*x", value_len, num->value);
          break;
        case NUMBER_PREFIX_HEX_UPPERCASE:
          written = swprintf(ptr, remaining, L"%#-*X", value_len, num->value);
          break;
        case NUMBER_PREFIX_HEX_LOWERCASE:
          written = swprintf(ptr, remaining, L"%#-*x", value_len, num->value);
          break;
      }
      break;
    default:
      switch (config->mem_style)
      {
        case NUMBER_DECIMAL:
          written = swprintf(ptr, remaining, L"%-*d", value_len, num->value);
          break;
        case NUMBER_HEX_UPPERCASE:
          written = swprintf(ptr, remaining, L"%-*X", value_len, num->value);
          break;
        case NUMBER_HEX_LOWERCASE:
          written = swprintf(ptr, remaining, L"%-*x", value_len, num->value);
          break;
        case NUMBER_PREFIX_HEX_UPPERCASE:
          written = swprintf(ptr, remaining, L"%#-*X", value_len, num->value);
          break;
        case NUMBER_PREFIX_HEX_LOWERCASE:
          written = swprintf(ptr, remaining, L"%#-*x", value_len, num->value);
          break;
      }
      break;
  }
  if (written < 0 || written > remaining) goto deallocate;
  ptr += written;
  remaining -= written + 1;
  ptr = pwchar(ptr, separator);

  if (num == &condition->rhs) goto hit_target;


  if (condition->op == OP_NONE)
  {
    written = swprintf(ptr, remaining, L"%*s", 2, "");
    if (written < 0 || written > remaining) goto deallocate;
    ptr += written;
    remaining -= written + 1;
    ptr = pwchar(ptr, separator);

    written = swprintf(ptr, remaining,
                       L"%*s", collumn_widths[config->type_style][TYPE], "");
    if (written < 0 || written > remaining) goto deallocate;
    ptr += written;
    remaining -= written + 1;
    ptr = pwchar(ptr, separator);

    written = swprintf(ptr, remaining,
                       L"%*s", collumn_widths[config->size_style][SIZE], "");
    if (written < 0 || written > remaining) goto deallocate;
    ptr += written;
    remaining -= written + 1;
    ptr = pwchar(ptr, separator);

    written = swprintf(ptr, remaining, L"%*s", value_len, "");
    if (written < 0 || written > remaining) goto deallocate;
    ptr += written;
    remaining -= written + 1;
    ptr = pwchar(ptr, separator);

    goto hit_target;
  }
  written = swprintf(ptr, remaining, L"%-*ls", 2, operator[config->op_style][condition->op]);
  if (written < 0 || written > remaining) goto deallocate;
  ptr += written;
  remaining -= written + 1;
  ptr = pwchar(ptr, separator);

  num = &condition->rhs;
  goto operand;

hit_target:

  if (condition->flag != FLAG_ADD_SOURCE &&
      condition->flag != FLAG_SUB_SOURCE &&
      condition->flag != FLAG_ADD_ADDRESS &&
      condition->flag != FLAG_REMEMBER)
  {
    ptr = pwchar(ptr, L'(');
    remaining -= 1;
    written = swprintf(ptr, remaining, L"%d", condition->hit_target);
    if (written < 0 || written > remaining) goto deallocate;
    ptr += written;
    remaining -= written + 1;
    ptr = pwchar(ptr, L')');
    written = swprintf(ptr, remaining, L"%*s", remaining - 2, L"");
    if (written < 0 || written > remaining) goto deallocate;
    ptr += written;
    remaining -= written + 1;
  }
  else
  {
    written = swprintf(ptr, remaining, L"%*s", remaining - 2, "");
    if (written < 0 || written > remaining) goto deallocate;
    ptr += written;
    remaining -= written;
  }
  ptr = pwchar(ptr, line_char);
  *ptr = L'\0';
  return output;

deallocate:
  free(output);
  return NULL;
}

void print_condition(struct CONDITION *condition)
{
  wchar_t *header = get_logic_header(true, &global_config);
  if (!header) return;
  wprintf(L"%ls\n", header);
  free(header);

  wchar_t *line = get_condition_line(condition, &global_config);
  if (!line) return;
  wprintf(L"%ls\n", line);
  free(line);

  wchar_t *footer = get_logic_footer(&global_config);
  if (!footer) return;
  wprintf(L"%ls\n", footer);
  free(footer);
}

static struct TEXT_BLOC *get_group_str(struct GROUP *group, struct PRINT_CONFIG *config)
{
  int group_count = 0;
  for (struct CONDITION *condition = group->condition_head; condition != NULL; condition = condition->next)
    group_count ++;

  struct TEXT_BLOC *output = malloc(sizeof(struct TEXT_BLOC));
  output->line_count = group_count + 3; // conditions + group header + logic header + bloc footer
  output->lines = malloc(sizeof(wchar_t *) * (output->line_count));

  wchar_t *group_title = get_group_title(group->id);

  int line_len = get_logic_line_length(config);
  output->lines[0] = get_header_line(group_title, line_len, config);
  //free(group_title);

  wchar_t *logic_header = get_logic_header(false, config);
  if (!logic_header) {free(output->lines[0]); free(output->lines); free(output); return NULL;}
  output->lines[1] = logic_header;

  int copied = 2; // will be used for deallocation in case something goes wrong
  for (struct CONDITION *condition = group->condition_head; condition != NULL; condition = condition->next)
  {
    wchar_t *cond_line = get_condition_line(condition, config);
    if (!cond_line) goto deallocate;
    output->lines[copied] = cond_line;
    copied ++;
  }

  wchar_t *logic_footer = get_logic_footer(config);
  if (!logic_footer) goto deallocate;
  output->lines[copied] = logic_footer;

  return output;

deallocate:
  for (int i = 0; i < copied; i ++)
    free(output->lines[copied]);

  free(output->lines);
  free(output);
  return NULL;
}

void print_group(struct GROUP *group)
{
  struct TEXT_BLOC *group_text = get_group_str(group, &global_config);

  for (int i = 0; i < group_text->line_count; i ++)
    wprintf(L"%ls\n", group_text->lines[i]);
}
