// Archivo del lado servidor para la práctica RPC.
// Aquí se implementa la lógica real de las operaciones remotas.

#include <stdio.h>          // Para printf y mensajes de depuración si hicieran falta.
#include <stdlib.h>         // Para NULL.
#include <string.h>         // Para memset, strcpy y otras operaciones con memoria.

#include "claves.h"         // Para usar destroy, set_value, get_value, modify_value, delete_key y exist.
#include "clavesRPC.h"      // Cabecera generada por rpcgen a partir de clavesRPC.x.

// Tamaño máximo de texto permitido en key y value1.
#define MAX_TEXTO 256

// Número máximo de floats permitidos en value2.
#define MAX_VALUE2 32


// ==========================================================
// OPERACIÓN REMOTA: destroy
// ==========================================================

// Esta función implementa la operación RPC equivalente a destroy().
// No recibe argumentos útiles, por eso el parámetro se llama argp pero no se usa.
int *destroy_rpc_1_svc(void *argp, struct svc_req *rqstp) {
    static int resultado;   // Variable estática para devolver el resultado por puntero.

    (void)argp;             // Se marca como no usado para evitar warnings del compilador.
    (void)rqstp;            // También marcamos como no usado el contexto RPC.

    resultado = destroy();  // Llamamos a la función real del servicio local.
    return &resultado;      // Devolvemos la dirección del resultado.
}


// ==========================================================
// OPERACIÓN REMOTA: set_value
// ==========================================================

// Esta función implementa la operación RPC equivalente a set_value().
int *set_value_rpc_1_svc(set_modify_args *argp, struct svc_req *rqstp) {
    static int resultado;   // Variable estática para devolver el estado final.

    (void)rqstp;            // El contexto RPC no se usa directamente aquí.

    // Comprobamos que el puntero recibido no sea NULL.
    if (argp == NULL) {
        resultado = -1;     // Si no hay argumentos válidos, devolvemos error.
        return &resultado;
    }

    // Comprobamos que key y value1 existan.
    if (argp->key == NULL || argp->value1 == NULL) {
        resultado = -1;     // Si faltan datos básicos, devolvemos error.
        return &resultado;
    }

    // Comprobamos que el número de floats esté dentro del rango permitido.
    if (argp->N_value2 < 1 || argp->N_value2 > MAX_VALUE2) {
        resultado = -1;     // Si el tamaño es inválido, devolvemos error.
        return &resultado;
    }

    // Comprobamos que la longitud real del vector RPC coincida con N_value2.
    if ((int)argp->V_value2.V_value2_len != argp->N_value2) {
        resultado = -1;     // Si no coincide, consideramos que la petición es incorrecta.
        return &resultado;
    }

    // Comprobamos que el puntero al vector exista.
    if (argp->V_value2.V_value2_val == NULL) {
        resultado = -1;     // Si no hay vector, devolvemos error.
        return &resultado;
    }

    // Construimos la estructura Paquete local a partir de la estructura RPC.
    struct Paquete valor3;                  // Estructura local equivalente a PaqueteRPC.
    valor3.x = argp->value3.x;             // Copiamos x.
    valor3.y = argp->value3.y;             // Copiamos y.
    valor3.z = argp->value3.z;             // Copiamos z.

    // Llamamos a la función real del servicio local.
    resultado = set_value(
        argp->key,                         // Clave recibida por RPC.
        argp->value1,                      // value1 recibido por RPC.
        argp->N_value2,                    // Número de floats.
        argp->V_value2.V_value2_val,       // Puntero al array de floats.
        valor3                             // Estructura Paquete reconstruida.
    );

    return &resultado;                     // Devolvemos el estado de la operación.
}


// ==========================================================
// OPERACIÓN REMOTA: get_value
// ==========================================================

// Esta función implementa la operación RPC equivalente a get_value().
get_value_res *get_value_rpc_1_svc(key_args *argp, struct svc_req *rqstp) {
    static get_value_res resultado;        // Estructura estática para devolver estado y datos.
    static char value1_buffer[MAX_TEXTO];  // Buffer estático para guardar value1.
    static float v2_buffer[MAX_VALUE2];    // Buffer estático para guardar el vector de floats.
    struct Paquete valor3_local;           // Estructura local para recibir x, y y z.

    (void)rqstp;                           // No usamos el contexto RPC directamente.

    // Ponemos toda la estructura de resultado a cero antes de reutilizarla.
    memset(&resultado, 0, sizeof(resultado));

    // Hacemos que el campo string del resultado apunte a nuestro buffer estático.
    resultado.value1 = value1_buffer;

    // Hacemos que el vector RPC del resultado apunte al buffer de floats.
    resultado.V_value2.V_value2_val = v2_buffer;

    // Inicialmente decimos que la longitud del vector es 0.
    resultado.V_value2.V_value2_len = 0;

    // Comprobamos que el argumento y la clave existan.
    if (argp == NULL || argp->key == NULL) {
        resultado.estado = -1;            // Si no hay clave válida, devolvemos error.
        return &resultado;
    }

    // Llamamos a la función real del servicio local.
    resultado.estado = get_value(
        argp->key,                        // Clave que se quiere consultar.
        value1_buffer,                    // Aquí se escribirá value1.
        &resultado.N_value2,              // Aquí se escribirá el número de floats.
        v2_buffer,                        // Aquí se escribirán los floats.
        &valor3_local                     // Aquí se escribirá la estructura Paquete.
    );

    // Si ha fallado get_value, devolvemos solo el estado.
    if (resultado.estado != 0) {
        return &resultado;
    }

    // Si ha ido bien, copiamos los tres enteros al resultado RPC.
    resultado.value3.x = valor3_local.x;  // Copiamos x.
    resultado.value3.y = valor3_local.y;  // Copiamos y.
    resultado.value3.z = valor3_local.z;  // Copiamos z.

    // Guardamos también la longitud real del vector RPC.
    resultado.V_value2.V_value2_len = resultado.N_value2;

    // Devolvemos la estructura completa con estado y datos.
    return &resultado;
}


// ==========================================================
// OPERACIÓN REMOTA: modify_value
// ==========================================================

// Esta función implementa la operación RPC equivalente a modify_value().
int *modify_value_rpc_1_svc(set_modify_args *argp, struct svc_req *rqstp) {
    static int resultado;   // Variable estática para devolver el estado.

    (void)rqstp;            // El contexto RPC no se usa directamente aquí.

    // Comprobamos que el puntero recibido no sea NULL.
    if (argp == NULL) {
        resultado = -1;     // Si no hay argumentos válidos, devolvemos error.
        return &resultado;
    }

    // Comprobamos que key y value1 existan.
    if (argp->key == NULL || argp->value1 == NULL) {
        resultado = -1;     // Si faltan datos básicos, devolvemos error.
        return &resultado;
    }

    // Comprobamos que el número de floats esté dentro del rango permitido.
    if (argp->N_value2 < 1 || argp->N_value2 > MAX_VALUE2) {
        resultado = -1;     // Si el tamaño no es válido, devolvemos error.
        return &resultado;
    }

    // Comprobamos que la longitud real del vector coincida con N_value2.
    if ((int)argp->V_value2.V_value2_len != argp->N_value2) {
        resultado = -1;     // Si no coincide, consideramos la petición incorrecta.
        return &resultado;
    }

    // Comprobamos que exista el puntero al vector.
    if (argp->V_value2.V_value2_val == NULL) {
        resultado = -1;     // Si no hay vector, devolvemos error.
        return &resultado;
    }

    // Reconstruimos la estructura Paquete local.
    struct Paquete valor3;                  // Estructura local.
    valor3.x = argp->value3.x;             // Copiamos x.
    valor3.y = argp->value3.y;             // Copiamos y.
    valor3.z = argp->value3.z;             // Copiamos z.

    // Llamamos a la función real del servicio local.
    resultado = modify_value(
        argp->key,                         // Clave recibida.
        argp->value1,                      // Nuevo value1.
        argp->N_value2,                    // Número de floats.
        argp->V_value2.V_value2_val,       // Vector recibido.
        valor3                             // Nueva estructura Paquete.
    );

    return &resultado;                     // Devolvemos el estado.
}


// ==========================================================
// OPERACIÓN REMOTA: delete_key
// ==========================================================

// Esta función implementa la operación RPC equivalente a delete_key().
int *delete_key_rpc_1_svc(key_args *argp, struct svc_req *rqstp) {
    static int resultado;   // Variable estática para devolver el estado.

    (void)rqstp;            // No usamos el contexto RPC aquí.

    // Comprobamos que el argumento y la clave existan.
    if (argp == NULL || argp->key == NULL) {
        resultado = -1;     // Si no hay clave válida, devolvemos error.
        return &resultado;
    }

    resultado = delete_key(argp->key); // Llamamos a la función real del servicio.
    return &resultado;                 // Devolvemos el estado de la operación.
}


// ==========================================================
// OPERACIÓN REMOTA: exist
// ==========================================================

// Esta función implementa la operación RPC equivalente a exist().
int *exist_rpc_1_svc(key_args *argp, struct svc_req *rqstp) {
    static int resultado;   // Variable estática para devolver el estado.

    (void)rqstp;            // El contexto RPC no se usa directamente.

    // Comprobamos que el argumento y la clave existan.
    if (argp == NULL || argp->key == NULL) {
        resultado = -1;     // Si no hay clave válida, devolvemos error.
        return &resultado;
    }

    resultado = exist(argp->key);      // Llamamos a la función real del servicio.
    return &resultado;                 // Devolvemos 1, 0 o -1 según corresponda.
}