// Incluimos stdio por si queremos mostrar mensajes o usar funciones básicas.
#include <stdio.h>

// Incluimos stdlib para utilidades generales.
#include <stdlib.h>

// Incluimos string.h para poder usar memset y strlen.
#include <string.h>

// Incluimos la cabecera original del servicio.
#include "claves.h"

// Incluimos la cabecera generada por rpcgen.
#include "clavesRPC.h"

// Declaraciones extern necesarias para RPC
extern void claves_rpc_prog_1(struct svc_req *rqstp, SVCXPRT *transp);

// Definimos el tamaño máximo real de texto en C.
// En RPC MAX_TEXTO vale 255, pero en C necesitamos 256 para incluir '\0'.
#define MAX_TEXTO_C 256

// Definimos el número máximo de floats permitidos.
#define MAX_VALUE2_C 32


// ==========================================================
// FUNCIÓN RPC: DESTROY
// ==========================================================

// Esta función atiende la operación remota DESTROY_RPC.
int *destroy_rpc_1_svc(struct svc_req *rqstp) {
    // Variable estática para devolver el resultado por puntero.
    static int resultado;

    // No usamos directamente la información de la petición RPC.
    (void)rqstp;

    // Llamamos a la función real del servicio.
    resultado = destroy();

    // Devolvemos el resultado.
    return &resultado;
}


// ==========================================================
// FUNCIÓN RPC: SET_VALUE
// ==========================================================

// Esta función atiende la operación remota SET_VALUE_RPC.
int *set_value_rpc_1_svc(set_modify_args argp, struct svc_req *rqstp) {
    // Variable estática para devolver el estado final.
    static int resultado;

    // Estructura local equivalente a PaqueteRPC.
    struct Paquete valor3;

    // No usamos directamente la información de la petición RPC.
    (void)rqstp;

    // Si la clave o value1 son NULL, devolvemos error.
    if (argp.key == NULL || argp.value1 == NULL) {
        resultado = -1;
        return &resultado;
    }

    // Si la clave o value1 son demasiado largos, devolvemos error.
    if (strlen(argp.key) >= MAX_TEXTO_C || strlen(argp.value1) >= MAX_TEXTO_C) {
        resultado = -1;
        return &resultado;
    }

    // Si N_value2 no está en el rango permitido, devolvemos error.
    if (argp.N_value2 < 1 || argp.N_value2 > MAX_VALUE2_C) {
        resultado = -1;
        return &resultado;
    }

    // Comprobamos que la longitud real del vector RPC coincida con N_value2.
    if ((int)argp.V_value2.vector_floats_len != argp.N_value2) {
        resultado = -1;
        return &resultado;
    }

    // Si el puntero al vector es NULL, devolvemos error.
    if (argp.V_value2.vector_floats_val == NULL) {
        resultado = -1;
        return &resultado;
    }

    // Copiamos la estructura RPC a la estructura local.
    valor3.x = argp.value3.x;
    valor3.y = argp.value3.y;
    valor3.z = argp.value3.z;

    // Llamamos a la función real del servicio.
    resultado = set_value(
        argp.key,
        argp.value1,
        argp.N_value2,
        argp.V_value2.vector_floats_val,
        valor3
    );

    // Devolvemos el resultado.
    return &resultado;
}


// ==========================================================
// FUNCIÓN RPC: GET_VALUE
// ==========================================================

// Esta función atiende la operación remota GET_VALUE_RPC.
get_value_res *get_value_rpc_1_svc(key_args argp, struct svc_req *rqstp) {
    // Estructura estática para devolver estado y datos.
    static get_value_res resultado;

    // Buffer estático para guardar value1.
    static char value1_buffer[MAX_TEXTO_C];

    // Buffer estático para guardar los floats.
    static float value2_buffer[MAX_VALUE2_C];

    // Estructura local para recibir x, y y z.
    struct Paquete valor3_local;

    // No usamos directamente la información RPC.
    (void)rqstp;

    // Limpiamos la estructura resultado.
    memset(&resultado, 0, sizeof(resultado));

    // Limpiamos los buffers.
    memset(value1_buffer, 0, sizeof(value1_buffer));
    memset(value2_buffer, 0, sizeof(value2_buffer));

    // Hacemos que value1 apunte al buffer de texto.
    resultado.value1 = value1_buffer;

    // Hacemos que el vector RPC apunte al buffer de floats.
    resultado.V_value2.vector_floats_val = value2_buffer;

    // Inicialmente la longitud del vector es 0.
    resultado.V_value2.vector_floats_len = 0;

    // Si la clave es NULL, devolvemos error.
    if (argp.key == NULL) {
        resultado.estado = -1;
        return &resultado;
    }

    // Si la clave es demasiado larga, devolvemos error.
    if (strlen(argp.key) >= MAX_TEXTO_C) {
        resultado.estado = -1;
        return &resultado;
    }

    // Llamamos a la función real del servicio.
    resultado.estado = get_value(
        argp.key,
        value1_buffer,
        &resultado.N_value2,
        value2_buffer,
        &valor3_local
    );

    // Si hubo error, devolvemos solo el estado.
    if (resultado.estado != 0) {
        return &resultado;
    }

    // Comprobamos que el número de floats sea válido.
    if (resultado.N_value2 < 1 || resultado.N_value2 > MAX_VALUE2_C) {
        resultado.estado = -1;
        resultado.V_value2.vector_floats_len = 0;
        return &resultado;
    }

    // Copiamos los tres enteros al resultado RPC.
    resultado.value3.x = valor3_local.x;
    resultado.value3.y = valor3_local.y;
    resultado.value3.z = valor3_local.z;

    // Guardamos la longitud real del vector.
    resultado.V_value2.vector_floats_len = resultado.N_value2;

    // Devolvemos la respuesta completa.
    return &resultado;
}


// ==========================================================
// FUNCIÓN RPC: MODIFY_VALUE
// ==========================================================

// Esta función atiende la operación remota MODIFY_VALUE_RPC.
int *modify_value_rpc_1_svc(set_modify_args argp, struct svc_req *rqstp) {
    // Variable estática para devolver el estado final.
    static int resultado;

    // Estructura local equivalente a PaqueteRPC.
    struct Paquete valor3;

    // No usamos directamente la información RPC.
    (void)rqstp;

    // Si la clave o value1 son NULL, devolvemos error.
    if (argp.key == NULL || argp.value1 == NULL) {
        resultado = -1;
        return &resultado;
    }

    // Si la clave o value1 son demasiado largos, devolvemos error.
    if (strlen(argp.key) >= MAX_TEXTO_C || strlen(argp.value1) >= MAX_TEXTO_C) {
        resultado = -1;
        return &resultado;
    }

    // Si N_value2 no está en rango, devolvemos error.
    if (argp.N_value2 < 1 || argp.N_value2 > MAX_VALUE2_C) {
        resultado = -1;
        return &resultado;
    }

    // Comprobamos que la longitud real del vector coincida con N_value2.
    if ((int)argp.V_value2.vector_floats_len != argp.N_value2) {
        resultado = -1;
        return &resultado;
    }

    // Si el vector no existe, devolvemos error.
    if (argp.V_value2.vector_floats_val == NULL) {
        resultado = -1;
        return &resultado;
    }

    // Reconstruimos la estructura local.
    valor3.x = argp.value3.x;
    valor3.y = argp.value3.y;
    valor3.z = argp.value3.z;

    // Llamamos a la función real del servicio.
    resultado = modify_value(
        argp.key,
        argp.value1,
        argp.N_value2,
        argp.V_value2.vector_floats_val,
        valor3
    );

    // Devolvemos el resultado.
    return &resultado;
}


// ==========================================================
// FUNCIÓN RPC: DELETE_KEY
// ==========================================================

// Esta función atiende la operación remota DELETE_KEY_RPC.
int *delete_key_rpc_1_svc(key_args argp, struct svc_req *rqstp) {
    // Variable estática para devolver el resultado.
    static int resultado;

    // No usamos directamente la información RPC.
    (void)rqstp;

    // Si la clave es NULL, devolvemos error.
    if (argp.key == NULL) {
        resultado = -1;
        return &resultado;
    }

    // Si la clave es demasiado larga, devolvemos error.
    if (strlen(argp.key) >= MAX_TEXTO_C) {
        resultado = -1;
        return &resultado;
    }

    // Llamamos a la función real del servicio.
    resultado = delete_key(argp.key);

    // Devolvemos el estado.
    return &resultado;
}


// ==========================================================
// FUNCIÓN RPC: EXIST
// ==========================================================

// Esta función atiende la operación remota EXIST_RPC.
int *exist_rpc_1_svc(key_args argp, struct svc_req *rqstp) {
    // Variable estática para devolver el resultado.
    static int resultado;

    // No usamos directamente la información RPC.
    (void)rqstp;

    // Si la clave es NULL, devolvemos error.
    if (argp.key == NULL) {
        resultado = -1;
        return &resultado;
    }

    // Si la clave es demasiado larga, devolvemos error.
    if (strlen(argp.key) >= MAX_TEXTO_C) {
        resultado = -1;
        return &resultado;
    }

    // Llamamos a la función real del servicio.
    resultado = exist(argp.key);

    // Devolvemos 1, 0 o -1.
    return &resultado;
}


// ==========================================================
// VARIABLES GLOBALES NECESARIAS PARA RPC
// ==========================================================

// Estas variables son referencias externas utilizadas por el código generado por rpcgen.
// Deben estar definidas en este archivo de implementación del servidor.

int _rpcpmstart;   /* Started by a port monitor ? */
int _rpcfdtype;    /* Whether Stream or Datagram ? */
int _rpcsvcdirty;  /* Still serving ? */


// ==========================================================
// FUNCIÓN PRINCIPAL
// ==========================================================

int main(int argc, char *argv[]) {
    SVCXPRT *transp;

    (void)argc;  /* Prevenimos avisos de parámetro no usado */
    (void)argv;  /* Prevenimos avisos de parámetro no usado */

    /* Creamos un transporte TCP en el puerto por defecto (0 = puerto asignado automáticamente) */
    transp = svctcp_create(RPC_ANYSOCK, 0, 0);
    if (transp == NULL) {
        fprintf(stderr, "%s", "cannot create tcp service.\n");
        exit(1);
    }

    /* Registramos el programa RPC */
    if (!svc_register(transp, CLAVES_RPC_PROG, CLAVES_RPC_VERS, claves_rpc_prog_1, IPPROTO_TCP)) {
        fprintf(stderr, "%s", "unable to register (CLAVES_RPC_PROG, CLAVES_RPC_VERS, tcp).\n");
        exit(1);
    }

    /* El servidor entra en el bucle de atención de llamadas RPC */
    svc_run();

    /* Si svc_run() termina, algo fue mal */
    fprintf(stderr, "%s", "svc_run returned\n");
    exit(1);

    return 0;
}