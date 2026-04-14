# ==========================================================
# MAKEFILE - PRÁCTICA 3 SISTEMAS DISTRIBUIDOS (RPC)
# ==========================================================

# Guardamos en la variable CC el compilador que vamos a usar.
CC = gcc

# Detectamos el sistema operativo para adaptar la compilación.
UNAME_S := $(shell uname -s)

# Guardamos en CFLAGS las opciones básicas de compilación.
# -g añade información de depuración.
# -Wall activa avisos útiles del compilador.
# -fPIC permite crear bibliotecas compartidas.
CFLAGS = -g -Wall -fPIC

# Flags especiales para archivos generados por rpcgen (que usan sintaxis K&R antigua)
RPC_CFLAGS = $(CFLAGS) -Wno-incompatible-function-pointer-types -Wno-implicit-function-declaration

# Guardamos en RPCGEN el comando que genera los archivos RPC.
RPCGEN = rpcgen

# Guardamos en RPCFLAGS la opción -N para que rpcgen use estilo ANSI C.
RPCFLAGS = -N

# Inicializamos variables relacionadas con RPC y bibliotecas compartidas.
RPC_INC =
RPC_LIBS =
SHARED_FLAGS = -shared
RPATH_FLAG = -Wl,-rpath=.

# Si estamos en macOS, no usamos tirpc y ajustamos la biblioteca compartida.
ifeq ($(UNAME_S),Darwin)
CFLAGS += -Wno-deprecated-non-prototype
SHARED_FLAGS = -dynamiclib -Wl,-install_name,@rpath/libclaves.so
RPATH_FLAG = -Wl,-rpath,@executable_path
endif

# Si estamos en Linux, usamos tirpc y sus cabeceras.
ifeq ($(UNAME_S),Linux)
RPC_INC = -I/usr/include/tirpc
RPC_LIBS = -ltirpc
SHARED_FLAGS = -shared
RPATH_FLAG = -Wl,-rpath=.
endif

# Guardamos el nombre base de la interfaz RPC.
RPC_BASE = clavesRPC

# Guardamos el nombre del archivo .x de la interfaz RPC.
RPC_X = $(RPC_BASE).x

# Guardamos el nombre de la cabecera generada por rpcgen.
RPC_H = $(RPC_BASE).h

# Guardamos el nombre del archivo cliente generado por rpcgen.
RPC_CLNT_C = $(RPC_BASE)_clnt.c

# Guardamos el nombre del archivo servidor generado por rpcgen.
RPC_SVC_C = $(RPC_BASE)_svc.c

# Guardamos el nombre del archivo XDR generado por rpcgen.
RPC_XDR_C = $(RPC_BASE)_xdr.c

# Guardamos el nombre de la cabecera original que reutilizamos.
LOCAL_HEADER = claves.h

# Guardamos el nombre del archivo con la lógica local del servicio.
LOCAL_SERVICE = claves.c

# Guardamos el nombre del cliente de pruebas.
APP_CLIENT = app-cliente.c

# Guardamos el nombre de tu archivo manual del servidor RPC.
SERVER_IMPL = clavesRPC_server.c

# Guardamos el nombre del archivo manual del proxy RPC.
PROXY_IMPL = proxy-rpc.c

# Agrupamos en una variable los objetos del lado cliente.
CLIENT_OBJS = $(PROXY_IMPL:.c=.o) $(RPC_BASE)_clnt.o $(RPC_BASE)_xdr.o

# Agrupamos en una variable los objetos del lado servidor.
SERVER_OBJS = $(SERVER_IMPL:.c=.o) $(RPC_BASE)_svc.o $(RPC_BASE)_xdr.o claves.o

# Indicamos que estas reglas no representan archivos reales.
.PHONY: all clean rpc

# La regla principal compila todo lo necesario del proyecto.
# Ojo: para que funcione del todo también tiene que existir proxy-rpc.c.
all: rpc libclaves.so clavesRPC_server app-cliente

# ==========================================================
# GENERACIÓN DE ARCHIVOS RPC
# ==========================================================

# La regla rpc obliga a generar todos los archivos básicos de rpcgen.
# NOTA: No regeneramos clavesRPC_clnt.c ya que contiene correcciones manuales para compatibilidad con C99
rpc: $(RPC_H) $(RPC_SVC_C) $(RPC_XDR_C)

# Esta regla genera la cabecera .h a partir del archivo .x.
$(RPC_H): $(RPC_X)
	$(RPCGEN) $(RPCFLAGS) -h -o $(RPC_H) $(RPC_X)

# Esta regla genera el stub cliente a partir del archivo .x.
# Se genera manualmente, no se regenera automáticamente
# $(RPC_CLNT_C): $(RPC_X)
#	$(RPCGEN) $(RPCFLAGS) -l -o $(RPC_CLNT_C) $(RPC_X)

# Esta regla genera el stub servidor a partir del archivo .x.
# Usamos -m para generar solo el servidor y no machacar tu archivo manual.
$(RPC_SVC_C): $(RPC_X)
	$(RPCGEN) $(RPCFLAGS) -m -o $(RPC_SVC_C) $(RPC_X)

# Esta regla genera el archivo XDR a partir del archivo .x.
$(RPC_XDR_C): $(RPC_X)
	$(RPCGEN) $(RPCFLAGS) -c -o $(RPC_XDR_C) $(RPC_X)

# ==========================================================
# BIBLIOTECA DEL LADO CLIENTE
# ==========================================================

# Esta regla construye libclaves.so con el proxy RPC, el stub cliente y el XDR.
libclaves.so: $(CLIENT_OBJS)
	$(CC) $(SHARED_FLAGS) -o libclaves.so $(CLIENT_OBJS) $(RPC_LIBS)

# Esta regla compila proxy-rpc.c y genera su objeto.
$(PROXY_IMPL:.c=.o): $(PROXY_IMPL) $(LOCAL_HEADER) $(RPC_H)
	$(CC) $(CFLAGS) $(RPC_INC) -c $(PROXY_IMPL)

# Esta regla compila el archivo cliente generado por rpcgen.
$(RPC_BASE)_clnt.o: $(RPC_CLNT_C) $(RPC_H)
	$(CC) $(RPC_CFLAGS) $(RPC_INC) -c $(RPC_CLNT_C)

# Esta regla compila el archivo XDR generado por rpcgen.
$(RPC_BASE)_xdr.o: $(RPC_XDR_C) $(RPC_H)
	$(CC) $(RPC_CFLAGS) $(RPC_INC) -c $(RPC_XDR_C)

# ==========================================================
# SERVIDOR RPC
# ==========================================================

# Esta regla construye el ejecutable del servidor RPC.
# Junta tu implementación manual, el stub servidor, el XDR y claves.c.
clavesRPC_server: $(SERVER_OBJS)
	$(CC) -o clavesRPC_server $(SERVER_OBJS) $(RPC_LIBS)

# Esta regla compila tu archivo manual del servidor RPC.
$(SERVER_IMPL:.c=.o): $(SERVER_IMPL) $(LOCAL_HEADER) $(RPC_H)
	$(CC) $(CFLAGS) $(RPC_INC) -c $(SERVER_IMPL)

# Esta regla compila el stub servidor generado por rpcgen.
$(RPC_BASE)_svc.o: $(RPC_SVC_C) $(RPC_H)
	$(CC) $(RPC_CFLAGS) $(RPC_INC) -c $(RPC_SVC_C)

# Esta regla compila claves.c, que contiene la lógica real del servicio.
claves.o: $(LOCAL_SERVICE) $(LOCAL_HEADER)
	$(CC) $(CFLAGS) -c $(LOCAL_SERVICE)

# ==========================================================
# CLIENTE DE PRUEBAS
# ==========================================================

# Esta regla construye el ejecutable app-cliente.
# El cliente se enlaza con libclaves.so, que en esta práctica implementa RPC.
app-cliente: $(APP_CLIENT) libclaves.so $(LOCAL_HEADER)
	$(CC) $(CFLAGS) -o app-cliente $(APP_CLIENT) -L. $(RPATH_FLAG) -lclaves

# ==========================================================
# LIMPIEZA
# ==========================================================

# Esta regla borra objetos, biblioteca, ejecutables y archivos generados por rpcgen.
# NOTA: No limpiamos clavesRPC_clnt.c ya que contiene correcciones manuales para compatibilidad C99
clean:
	rm -f *.o *.so app-cliente clavesRPC_server
	rm -f $(RPC_H) $(RPC_SVC_C) $(RPC_XDR_C)