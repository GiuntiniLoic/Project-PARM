#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_SIZE 256
#define MAX_INSTR 10000
#define INVALID_INSTR 0xFFFF

typedef struct {
    unsigned short code;
} Instruction;

Instruction instructions[MAX_INSTR];
int instr_count = 0;

//Lecture du fichier
void trim(char *s) {
    int i = (int)strlen(s) - 1;
    while (i >= 0 && isspace((unsigned char)s[i])) {
        s[i] = '\0';
        i--;
    }
    char *start = s;
    while (isspace((unsigned char)*start)) start++;
    if (start != s) memmove(s, start, strlen(start) + 1);
}

int s2e(char *inst) {
    if (strcmp(inst, "movs") == 0) return 0;
    if (strcmp(inst, "adds") == 0) return 1;
    if (strcmp(inst, "subs") == 0) return 2;
    return -1;
}

// Traduction des instruction en binaire
void assembleur(char *ASSEMBLY_FILE, char *HEXA_FILE) {
    FILE *in, *out;

    if ((in = fopen(ASSEMBLY_FILE, "r")) == NULL) {
        exit(EXIT_FAILURE);
    }

    if ((out = fopen(HEXA_FILE, "w")) == NULL) {
        fclose(in);
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_SIZE];
    char debut[MAX_LINE_SIZE];
    int addr = 0;
    int rd, rn, imm8;

    while (fgets(line, sizeof(line), in)) {
        trim(line);
        if (line[0] == '\0' || line[0] == '@') continue;

        sscanf(line, "%s", debut);
        int inst_line = s2e(debut);

        if (inst_line == -1) continue;

        switch (inst_line) {
            case 0:
                if (sscanf(line, "%*s r%d, #%d", &rd, &imm8) == 2) {
                    unsigned short opcode = (0b00100 << 11) | (rd << 8) | (imm8 & 0xFF);
                    instructions[instr_count++].code = opcode;
                    addr++;
                }
                break;

            case 1:
                if (sscanf(line, "%*s r%d, r%d, #%d", &rd, &rn, &imm8) == 3) {
                    unsigned short opcode = (0b0001100 << 9) | (rn << 6) | (rd << 3) | (imm8 & 0x7);
                    instructions[instr_count++].code = opcode;
                    addr++;
                }
                break;
        }
    }

    fclose(in);

    //Traduction en hexadécimale
    fprintf(out, "v2.0 raw\n");
    for (int i = 0; i < instr_count - 1; i++) {
        fprintf(out, "%04X ", instructions[i].code);
    }
    fprintf(out, "%04X\n ", instructions[instr_count-1].code);

    fclose(out);

}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        return 1;
    }

    assembleur(argv[1], argv[2]);
    return 0;
}

