/*
 * Proxy RPC - Implementación del cliente RPC que actúa como proxy
 * 
 * Este archivo implementa las funciones definidas en claves.h
 * realizando llamadas RPC al servidor remoto.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rpc/rpc.h>
#include "clavesRPC.h"
#include "claves.h"

/* Dirección IP del servidor RPC (por defecto localhost) */
#define DEFAULT_HOST "localhost"

/* Variable global para guardar el cliente RPC */
static CLIENT *clnt = NULL;

/* Función auxiliar para obtener el cliente RPC */
static CLIENT *get_rpc_client(void) {
    if (clnt == NULL) {
        /* Si no hay cliente, intentamos conectar al servidor */
        char *host_ip = getenv("IP_TUPLAS");
        if (host_ip == NULL) {
            host_ip = DEFAULT_HOST;
        }
        
        clnt = clnt_create(host_ip, CLAVES_RPC_PROG, CLAVES_RPC_VERS, "tcp");
        if (clnt == NULL) {
            clnt_pcreateerror(host_ip);
            return NULL;
        }
    }
    return clnt;
}

/**
 * @brief Initialize or reset the key-value service
 */
int destroy(void) {
    CLIENT *client = get_rpc_client();
    if (client == NULL) {
        return -1;
    }
    
    int *result = destroy_rpc_1(client);
    if (result == NULL) {
        clnt_perror(client, "call failed");
        return -1;
    }
    
    return *result;
}

/**
 * @brief Insert a key-value pair in the service
 */
int set_value(char *key, char *value1, int N_value2, float *V_value2, struct Paquete value3) {
    CLIENT *client = get_rpc_client();
    if (client == NULL) {
        return -1;
    }
    
    /* Preparamos los argumentos */
    set_modify_args args;
    args.key = key;
    args.value1 = value1;
    args.N_value2 = N_value2;
    args.V_value2.vector_floats_val = V_value2;
    args.V_value2.vector_floats_len = N_value2;
    args.value3.x = value3.x;
    args.value3.y = value3.y;
    args.value3.z = value3.z;
    
    int *result = set_value_rpc_1(args, client);
    if (result == NULL) {
        clnt_perror(client, "call failed");
        return -1;
    }
    
    return *result;
}

/**
 * @brief Get value by key
 */
int get_value(char *key, char *value1, int *N_value2, float *V_value2, struct Paquete *value3) {
    CLIENT *client = get_rpc_client();
    if (client == NULL) {
        return -1;
    }
    
    /* Preparamos los argumentos */
    key_args args;
    args.key = key;
    
    get_value_res *result = get_value_rpc_1(args, client);
    if (result == NULL) {
        clnt_perror(client, "call failed");
        return -1;
    }
    
    /* Copiamos los resultados */
    if (result->estado != 0) {
        return result->estado;
    }
    
    if (strlen(result->value1) >= 256) {
        return -1;
    }
    strcpy(value1, result->value1);
    
    *N_value2 = result->N_value2;
    
    if (result->N_value2 > 0 && result->N_value2 <= 32) {
        memcpy(V_value2, result->V_value2.vector_floats_val, 
               result->N_value2 * sizeof(float));
    }
    
    value3->x = result->value3.x;
    value3->y = result->value3.y;
    value3->z = result->value3.z;
    
    return result->estado;
}

/**
 * @brief Modify value by key
 */
int modify_value(char *key, char *value1, int N_value2, float *V_value2, struct Paquete value3) {
    CLIENT *client = get_rpc_client();
    if (client == NULL) {
        return -1;
    }
    
    /* Preparamos los argumentos */
    set_modify_args args;
    args.key = key;
    args.value1 = value1;
    args.N_value2 = N_value2;
    args.V_value2.vector_floats_val = V_value2;
    args.V_value2.vector_floats_len = N_value2;
    args.value3.x = value3.x;
    args.value3.y = value3.y;
    args.value3.z = value3.z;
    
    int *result = modify_value_rpc_1(args, client);
    if (result == NULL) {
        clnt_perror(client, "call failed");
        return -1;
    }
    
    return *result;
}

/**
 * @brief Delete key
 */
int delete_key(char *key) {
    CLIENT *client = get_rpc_client();
    if (client == NULL) {
        return -1;
    }
    
    /* Preparamos los argumentos */
    key_args args;
    args.key = key;
    
    int *result = delete_key_rpc_1(args, client);
    if (result == NULL) {
        clnt_perror(client, "call failed");
        return -1;
    }
    
    return *result;
}

/**
 * @brief Check if key exists
 */
int exist(char *key) {
    CLIENT *client = get_rpc_client();
    if (client == NULL) {
        return -1;
    }
    
    /* Preparamos los argumentos */
    key_args args;
    args.key = key;
    
    int *result = exist_rpc_1(args, client);
    if (result == NULL) {
        clnt_perror(client, "call failed");
        return -1;
    }
    
    return *result;
}
