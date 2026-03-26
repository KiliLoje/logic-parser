#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "encoder.h"
#include "parser.h"

// Mapeamento Inverso dos Operadores
const char* OP_STR[] = {
    "", "=", "<", "<=", ">", ">=", "!=", "*", "/", "%", "+", "-", "&", "^"
};

int encode_numeral(char *buffer, size_t max_size, struct NUMERAL *num) {
    if (num->type == TYPE_VALUE) {
        return snprintf(buffer, max_size, "%d", num->value);
    }
    if (num->type == TYPE_RECALL) {
        return snprintf(buffer, max_size, "{recall}");
    }
    if (num->type == TYPE_FLOAT) {
        float f;
        memcpy(&f, &num->value, sizeof(float));
        // O %g omite zeros decimais desnecessários
        return snprintf(buffer, max_size, "f%g", f); 
    }

    int offset = 0;

    // 1. Prefixo de Tipo (Delta 'd', Prior 'p', BCD 'b', Invert '~')
    // Nota: O TYPE_DELTA é o valor 2 no enum, logo o índice é type - 2
    if (num->type >= TYPE_DELTA && num->type <= TYPE_INVERT && num->type != TYPE_FLOAT) {
        offset += snprintf(buffer + offset, max_size - offset, "%c", char_type[num->type - 2]);
    }

    // 2. Prefixo de Memória
    // As memórias do tipo Float começam com 'f'. O resto começa com '0x'
    int is_float_size = (num->size >= SIZE_FLOAT && num->size <= SIZE_MBF32_LE);
    if (is_float_size) {
        offset += snprintf(buffer + offset, max_size - offset, "f");
    } else {
        offset += snprintf(buffer + offset, max_size - offset, "0x");
    }

    // 3. Tamanho (Size)
    // O SIZE_BIT0 é o valor 1 no enum, logo o índice é size - 1
    if (num->size != SIZE_NONE) {
        char s_char = char_size[num->size - 1];
        // Omitimos o espaço para o tamanho SIZE_WORD (' ') para a string ficar mais limpa ("0x1234" vs "0x 1234")
        offset += snprintf(buffer + offset, max_size - offset, "%c", s_char);
    }

    // 4. Endereço / Valor (Formato Hexadecimal comum com mínimo de 4 digitos)
    offset += snprintf(buffer + offset, max_size - offset, "%04x", num->value);

    return offset;
}

int encode_condition(char *buffer, size_t max_size, struct CONDITION *cond) {
    int offset = 0;

    // Adiciona a Flag (ex: "R:")
    if (cond->flag != FLAG_NONE) {
        offset += snprintf(buffer + offset, max_size - offset, "%c:", char_flag[cond->flag - 1]);
    }

    // Lado esquerdo da equação (LHS)
    offset += encode_numeral(buffer + offset, max_size - offset, &cond->lhs);

    // Operador e lado direito (RHS)
    if (cond->op != OP_NONE) {
        offset += snprintf(buffer + offset, max_size - offset, "%s", OP_STR[cond->op]);
        offset += encode_numeral(buffer + offset, max_size - offset, &cond->rhs);

        // Limite de Hits (Hit Target)
        if (cond->hit_target > 0) {
            offset += snprintf(buffer + offset, max_size - offset, ".%d.", cond->hit_target);
        }
    }

    return offset;
}

int encode_group(char *buffer, size_t max_size, struct GROUP *group) {
    int offset = 0;
    for (size_t i = 0; i < group->condition_count; i++) {
        if (i > 0) {
            offset += snprintf(buffer + offset, max_size - offset, "%c", condition_separator);
        }
        offset += encode_condition(buffer + offset, max_size - offset, group->conditions[i]);
    }
    return offset;
}

char* encode_logic(struct ACHIEVEMENT_LOGIC *logic) {
    if (!logic || logic->group_count == 0) return NULL;

    // Alocar 4KB por defeito (suficiente para a esmagadora maioria das conquistas)
    size_t buf_size = 4096;
    char *buffer = malloc(buf_size);
    if (!buffer) return NULL;

    buffer[0] = '\0';
    int offset = 0;

    for (size_t i = 0; i < logic->group_count; i++) {
        if (i > 0) {
            // Separa os Alternate Groups usando o 'S' 
            offset += snprintf(buffer + offset, buf_size - offset, "%c", group_separator);
        }
        offset += encode_group(buffer + offset, buf_size - offset, logic->groups[i]);
    }

    return buffer;
}