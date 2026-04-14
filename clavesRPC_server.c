// clavesRPC_server.c
// Este archivo implementa la parte servidor de las operaciones RPC.
// Aquí conectamos las llamadas remotas con las funciones reales de claves.c.

#include <stdio.h>      // Para funciones básicas de entrada/salida.
#include <stdlib.h>     // Para utilidades generales.
#include <string.h>     // Para memset, strlen y manejo de cadenas.

#include "claves.h"     // Aquí están destroy, set_value, get_value, modify_value, delete_key y exist.
#include "clavesRPC.h"  // Cabecera generada automáticamente por rpcgen.

// Tamaño máximo real de texto en C: 255 caracteres + '\0'.
#define MAX_TEXTO 256

// Número máximo de floats permitidos en value2.
#define MAX_VALUE2 32


// ==========================================================
// OPERACIÓN RPC: destroy
// ==========================================================

// Esta función atiende la llamada remota destroy().
int *destroy_rpc_1_svc(void *argp, struct svc_req *rqstp) {
    static int resultado;      // Variable estática para poder devolver un puntero válido.

    (void)argp;                // No usamos argumentos en destroy(), así que evitamos warning.
    (void)rqstp;               // Tampoco usamos directamente la información de la petición RPC.

    resultado = destroy();     // Llamamos a la función real del servicio.
    return &resultado;         // Devolvemos la dirección del resultado.
}


// ==========================================================
// OPERACIÓN RPC: set_value
// ==========================================================

// Esta función atiende la llamada remota set_value().
int *set_value_rpc_1_svc(set_modify_args *argp, struct svc_req *rqstp) {
    static int resultado;      // Variable estática para devolver el estado final.
    struct Paquete valor3;     // Estructura local equivalente a PaqueteRPC.

    (void)rqstp;               // No usamos directamente la información RPC.

    // Comprobamos que el puntero principal no sea NULL.
    if (argp == NULL) {
        resultado = -1;
        return &resultado;
    }

    // Comprobamos que key y value1 existan.
    if (argp->key == NULL || argp->value1 == NULL) {
        resultado = -1;
        return &resultado;
    }

    // Comprobamos que las cadenas no superen el tamaño permitido.
    if (strlen(argp->key) >= MAX_TEXTO || strlen(argp->value1) >= MAX_TEXTO) {
        resultado = -1;
        return &resultado;
    }

    // Comprobamos que N_value2 esté en el rango correcto.
    if (argp->N_value2 < 1 || argp->N_value2 > MAX_VALUE2) {
        resultado = -1;
        return &resultado;
    }

    // Comprobamos que la longitud real del vector RPC coincida con N_value2.
    if ((int)argp->V_value2.vector_floats_len != argp->N_value2) {
        resultado = -1;
        return &resultado;
    }

    // Comprobamos que el puntero al array de floats exista.
    if (argp->V_value2.vector_floats_val == NULL) {
        resultado = -1;
        return &resultado;
    }

    // Reconstruimos la estructura local Paquete a partir de la estructura RPC.
    valor3.x = argp->value3.x;
    valor3.y = argp->value3.y;
    valor3.z = argp->value3.z;

    // Llamamos a la función real del servicio con los datos recibidos por RPC.
    resultado = set_value(
        argp->key,
        argp->value1,
        argp->N_value2,
        argp->V_value2.vector_floats_val,
        valor3
    );

    // Devolvemos el estado de la operación.
    return &resultado;
}


// ==========================================================
// OPERACIÓN RPC: get_value
// ==========================================================

// Esta función atiende la llamada remota get_value().
get_value_res *get_value_rpc_1_svc(key_args *argp, struct svc_req *rqstp) {
    static get_value_res resultado;           // Estructura estática para devolver estado y datos.
    static char value1_buffer[MAX_TEXTO];     // Buffer estático para value1.
    static float value2_buffer[MAX_VALUE2];   // Buffer estático para el vector de floats.
    struct Paquete valor3_local;              // Estructura local para recibir x, y y z.

    (void)rqstp;                              // No usamos directamente la información RPC.

    // Limpiamos toda la estructura de salida antes de reutilizarla.
    memset(&resultado, 0, sizeof(resultado));

    // Dejamos también los buffers en un estado limpio.
    memset(value1_buffer, 0, sizeof(value1_buffer));
    memset(value2_buffer, 0, sizeof(value2_buffer));

    // Hacemos que el campo value1 del resultado apunte al buffer estático.
    resultado.value1 = value1_buffer;

    // Hacemos que el vector RPC del resultado apunte al buffer de floats.
    resultado.V_value2.vector_floats_val = value2_buffer;

    // Inicialmente ponemos la longitud del vector a 0.
    resultado.V_value2.vector_floats_len = 0;

    // Comprobamos que el argumento y la clave existan.
    if (argp == NULL || argp->key == NULL) {
        resultado.estado = -1;
        return &resultado;
    }

    // Comprobamos que la clave no supere el tamaño permitido.
    if (strlen(argp->key) >= MAX_TEXTO) {
        resultado.estado = -1;
        return &resultado;
    }

    // Llamamos a la función real del servicio.
    resultado.estado = get_value(
        argp->key,
        value1_buffer,
        &resultado.N_value2,
        value2_buffer,
        &valor3_local
    );

    // Si hubo error, devolvemos solo el estado.
    if (resultado.estado != 0) {
        return &resultado;
    }

    // Comprobamos que el número de floats recibido sea válido.
    if (resultado.N_value2 < 1 || resultado.N_value2 > MAX_VALUE2) {
        resultado.estado = -1;
        resultado.V_value2.vector_floats_len = 0;
        return &resultado;
    }

    // Copiamos la estructura Paquete local al resultado RPC.
    resultado.value3.x = valor3_local.x;
    resultado.value3.y = valor3_local.y;
    resultado.value3.z = valor3_local.z;

    // Indicamos cuántos floats lleva realmente el vector.
    resultado.V_value2.vector_floats_len = resultado.N_value2;

    // Devolvemos el resultado completo.
    return &resultado;
}


// ==========================================================
// OPERACIÓN RPC: modify_value
// ==========================================================

// Esta función atiende la llamada remota modify_value().
int *modify_value_rpc_1_svc(set_modify_args *argp, struct svc_req *rqstp) {
    static int resultado;      // Variable estática para devolver el estado final.
    struct Paquete valor3;     // Estructura local equivalente a PaqueteRPC.

    (void)rqstp;               // No usamos directamente la información RPC.

    // Comprobamos que el puntero principal no sea NULL.
    if (argp == NULL) {
        resultado = -1;
        return &resultado;
    }

    // Comprobamos que key y value1 existan.
    if (argp->key == NULL || argp->value1 == NULL) {
        resultado = -1;
        return &resultado;
    }

    // Comprobamos que las cadenas no superen el tamaño permitido.
    if (strlen(argp->key) >= MAX_TEXTO || strlen(argp->value1) >= MAX_TEXTO) {
        resultado = -1;
        return &resultado;
    }

    // Comprobamos que N_value2 esté en el rango correcto.
    if (argp->N_value2 < 1 || argp->N_value2 > MAX_VALUE2) {
        resultado = -1;
        return &resultado;
    }

    // Comprobamos que la longitud real del vector RPC coincida con N_value2.
    if ((int)argp->V_value2.vector_floats_len != argp->N_value2) {
        resultado = -1;
        return &resultado;
    }

    // Comprobamos que el puntero al array de floats exista.
    if (argp->V_value2.vector_floats_val == NULL) {
        resultado = -1;
        return &resultado;
    }

    // Reconstruimos la estructura local Paquete.
    valor3.x = argp->value3.x;
    valor3.y = argp->value3.y;
    valor3.z = argp->value3.z;

    // Llamamos a la función real del servicio.
    resultado = modify_value(
        argp->key,
        argp->value1,
        argp->N_value2,
        argp->V_value2.vector_floats_val,
        valor3
    );

    // Devolvemos el estado.
    return &resultado;
}


// ==========================================================
// OPERACIÓN RPC: delete_key
// ==========================================================

// Esta función atiende la llamada remota delete_key().
int *delete_key_rpc_1_svc(key_args *argp, struct svc_req *rqstp) {
    static int resultado;      // Variable estática para devolver el estado.

    (void)rqstp;               // No usamos directamente la información RPC.

    // Comprobamos que el argumento y la clave existan.
    if (argp == NULL || argp->key == NULL) {
        resultado = -1;
        return &resultado;
    }

    // Comprobamos que la clave no supere el tamaño permitido.
    if (strlen(argp->key) >= MAX_TEXTO) {
        resultado = -1;
        return &resultado;
    }

    // Llamamos a la función real del servicio.
    resultado = delete_key(argp->key);

    // Devolvemos el estado.
    return &resultado;
}


// ==========================================================
// OPERACIÓN RPC: exist
// ==========================================================

// Esta función atiende la llamada remota exist().
int *exist_rpc_1_svc(key_args *argp, struct svc_req *rqstp) {
    static int resultado;      // Variable estática para devolver el estado.

    (void)rqstp;               // No usamos directamente la información RPC.

    // Comprobamos que el argumento y la clave existan.
    if (argp == NULL || argp->key == NULL) {
        resultado = -1;
        return &resultado;
    }

    // Comprobamos que la clave no supere el tamaño permitido.
    if (strlen(argp->key) >= MAX_TEXTO) {
        resultado = -1;
        return &resultado;
    }

    // Llamamos a la función real del servicio.
    resultado = exist(argp->key);

    // Devolvemos 1, 0 o -1 según corresponda.
    return &resultado;
}