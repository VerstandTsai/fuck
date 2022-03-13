#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printhelp() {
    printf(
            "Usage: fuck [options] file\n"
            "Options:\n"
            "  -h, --help       Display this information.\n"
            "  -v, --version    Display compiler version infomation.\n"
    );
}

void printver() {
    printf("fuck 1.0.0\n");
}

void compile(char filename[]) {
    char outname[32];
    strcpy(outname, filename);
    char *outext = strrchr(outname, '.');
    *outext = '\0';
    char asmname[32];
    strcpy(asmname, outname);
    strcat(asmname, ".s");

    FILE *dest = fopen(asmname, "w");
    fprintf(dest,
            "\t.intel_syntax noprefix\n"
            "\t.set memsize, 30000\n"
            "\t.text\n"
            "\t.global main\n"
            "main:\n"
            "\tpush rbp\n"
            "\tmov rbp, rsp\n"
            "\tsub rsp, memsize\n"
            "\txor rbx, rbx\n"
            "init:\n"
            "\tcmp rbx, memsize\n"
            "\tjge endinit\n"
            "\tmov BYTE PTR [rbp+rbx-memsize], 0\n"
            "\tinc rbx\n"
            "endinit:\n"
            "\txor rbx, rbx\n"
    );

    FILE *src = fopen(filename, "r");
    int loopcount = 0;
    int currentloop = -1;
    while (!feof(src)) {
        char c = fgetc(src);
        switch (c) {
            case '>':
                fprintf(dest, "\tinc rbx\n");
                break;
            case '<':
                fprintf(dest, "\tdec rbx\n");
                break;
            case '+':
                fprintf(dest, "\tinc BYTE PTR [rbp+rbx-memsize]\n");
                break;
            case '-':
                fprintf(dest, "\tdec BYTE PTR [rbp+rbx-memsize]\n");
                break;
            case '.':
                fprintf(dest,
                        "\tmov dil, [rbp+rbx-memsize]\n"
                        "\txor eax, eax\n"
                        "\tcall putchar\n"
                );
                break;
            case ',':
                fprintf(dest,
                        "\txor eax, eax\n"
                        "\tcall getchar\n"
                        "\tmov [rbp+rbx-memsize], al\n"
                );
                break;
            case '[':
                fprintf(dest,
                        "L%d:\n"
                        "\tcmp BYTE PTR [rbp+rbx-memsize], 0\n"
                        "\tje  ENDL%d\n",
                        loopcount, loopcount
                );
                loopcount++;
                currentloop++;
                break;
            case ']':
                fprintf(dest,
                        "\tjmp L%d\n"
                        "ENDL%d:\n",
                        currentloop, currentloop
                );
                currentloop--;
                break;
        }
    }
    fclose(src);

    fprintf(dest,
            "\txor eax, eax\n"
            "\tleave\n"
            "\tret\n"
    );
    fclose(dest);

    char compcmd[128];
    sprintf(compcmd, "gcc -o %s %s", outname, asmname);
    system(compcmd);
    char rmcmd[64];
    sprintf(rmcmd, "rm -f %s", asmname);
    system(rmcmd);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printhelp();
    } else {
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            printhelp();
        } else if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0) {
            printver();
        } else {
            compile(argv[1]);
        }
    }
    return 0;
}
