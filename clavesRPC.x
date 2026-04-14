/* clavesRPC.x */
/* Interfaz RPC del servicio de tuplas */

const MAX_TEXTO = 255;
const MAX_VALUE2 = 32;

/* Tipo RPC para cadenas de hasta 255 caracteres */
typedef string texto<MAX_TEXTO>;

/* Tipo RPC para un vector de floats de hasta 32 elementos */
typedef float vector_floats<MAX_VALUE2>;

/* Estructura equivalente a Paquete */
struct PaqueteRPC {
    int x;
    int y;
    int z;
};

/* Argumentos para operaciones que solo usan key */
struct key_args {
    texto key;
};

/* Argumentos para set_value y modify_value */
struct set_modify_args {
    texto key;
    texto value1;
    int N_value2;
    vector_floats V_value2;
    PaqueteRPC value3;
};

/* Respuesta de get_value */
struct get_value_res {
    int estado;
    texto value1;
    int N_value2;
    vector_floats V_value2;
    PaqueteRPC value3;
};

/* Programa RPC */
program CLAVES_RPC_PROG {
    version CLAVES_RPC_VERS {

        /* Equivale a destroy() */
        int DESTROY_RPC(void) = 1;

        /* Equivale a set_value() */
        int SET_VALUE_RPC(set_modify_args) = 2;

        /* Equivale a get_value() */
        get_value_res GET_VALUE_RPC(key_args) = 3;

        /* Equivale a modify_value() */
        int MODIFY_VALUE_RPC(set_modify_args) = 4;

        /* Equivale a delete_key() */
        int DELETE_KEY_RPC(key_args) = 5;

        /* Equivale a exist() */
        int EXIST_RPC(key_args) = 6;

    } = 1;
} = 0x20000001;