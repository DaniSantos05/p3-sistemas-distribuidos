#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "claves.h"

int main(int argc, char *argv[]) {
    char key[64];
    char value1_out[256];
    int n_out = 0;
    float v2_out[32];
    struct Paquete v3_out;

    float v2_in[3] = {1.1f, 2.2f, 3.3f};
    float v2_mod[2] = {4.4f, 5.5f};
    struct Paquete v3_in = {1, 2, 3};
    struct Paquete v3_mod = {7, 8, 9};

    if (argc > 1) {
        snprintf(key, sizeof(key), "%s", argv[1]);
    } else {
        snprintf(key, sizeof(key), "KEY_%d", getpid());
    }

    printf("[START] key=%s\n", key);

    if (set_value(key, "valor1", 3, v2_in, v3_in) != 0) {
        printf("[FAIL] set_value\n");
        return 1;
    }

    if (get_value(key, value1_out, &n_out, v2_out, &v3_out) != 0) {
        printf("[FAIL] get_value inicial\n");
        return 1;
    }

    if (strcmp(value1_out, "valor1") != 0 || n_out != 3 || v3_out.x != 1 || v3_out.y != 2 || v3_out.z != 3) {
        printf("[FAIL] validacion get_value inicial\n");
        return 1;
    }

    if (modify_value(key, "nuevo_valor", 2, v2_mod, v3_mod) != 0) {
        printf("[FAIL] modify_value\n");
        return 1;
    }

    if (get_value(key, value1_out, &n_out, v2_out, &v3_out) != 0) {
        printf("[FAIL] get_value modificado\n");
        return 1;
    }

    if (strcmp(value1_out, "nuevo_valor") != 0 || n_out != 2 || v3_out.x != 7 || v3_out.y != 8 || v3_out.z != 9) {
        printf("[FAIL] validacion get_value modificado\n");
        return 1;
    }

    if (delete_key(key) != 0) {
        printf("[FAIL] delete_key\n");
        return 1;
    }

    if (exist(key) != 0) {
        printf("[FAIL] exist tras delete\n");
        return 1;
    }

    printf("[OK] key=%s\n", key);
    return 0;
}
