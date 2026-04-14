// Archivo de definición RPC del servicio de tuplas.
// A partir de este archivo, rpcgen genera parte del código cliente/servidor.

const MAX_TEXTO = 255;      // Longitud máxima permitida para key y value1.
const MAX_VALUE2 = 32;      // Número máximo de floats permitidos en value2.

// Tipo RPC para cadenas de texto de hasta 255 caracteres.
typedef string texto<MAX_TEXTO>;

// Tipo RPC para un vector dinámico de floats de hasta 32 elementos.
typedef float vector_floats<MAX_VALUE2>;

// Estructura RPC equivalente a la estructura Paquete de vuestra práctica.
struct PaqueteRPC {
    int x;                  // Primer campo entero.
    int y;                  // Segundo campo entero.
    int z;                  // Tercer campo entero.
};

// Estructura para las operaciones que solo necesitan una clave.
struct key_args {
    texto key;              // Clave sobre la que se quiere trabajar.
};

// Estructura para set_value y modify_value.
// Agrupa todos los datos de entrada de esas operaciones.
struct set_modify_args {
    texto key;              // Clave de la tupla.
    texto value1;           // Primer valor asociado a la clave.
    int N_value2;           // Número real de elementos del vector.
    vector_floats V_value2; // Vector de floats asociado a la clave.
    PaqueteRPC value3;      // Estructura con x, y y z.
};

// Estructura de respuesta para get_value.
// Hace falta porque get_value devuelve estado y también datos.
struct get_value_res {
    int estado;             // 0 si todo va bien, -1 si hay error.
    texto value1;           // value1 recuperado.
    int N_value2;           // Número de floats devueltos.
    vector_floats V_value2; // Vector de floats devuelto.
    PaqueteRPC value3;      // Estructura Paquete devuelta.
};

// Definición del programa RPC.
program CLAVES_RPC_PROG {

    // Definición de la versión 1 del servicio.
    version CLAVES_RPC_VERS {

        // Operación RPC equivalente a destroy().
        // No recibe argumentos y devuelve un entero de estado.
        int DESTROY_RPC(void) = 1;

        // Operación RPC equivalente a set_value().
        // Recibe todos los datos agrupados en set_modify_args.
        int SET_VALUE_RPC(set_modify_args) = 2;

        // Operación RPC equivalente a get_value().
        // Recibe una clave y devuelve estado + datos.
        get_value_res GET_VALUE_RPC(key_args) = 3;

        // Operación RPC equivalente a modify_value().
        // Recibe todos los datos necesarios para modificar una tupla.
        int MODIFY_VALUE_RPC(set_modify_args) = 4;

        // Operación RPC equivalente a delete_key().
        // Recibe una clave y devuelve el estado de la operación.
        int DELETE_KEY_RPC(key_args) = 5;

        // Operación RPC equivalente a exist().
        // Devuelve 1 si existe, 0 si no existe y -1 si hay error.
        int EXIST_RPC(key_args) = 6;

    } = 1;                  // Número de versión del servicio RPC.

} = 0x20000001;             // Identificador del programa RPC.