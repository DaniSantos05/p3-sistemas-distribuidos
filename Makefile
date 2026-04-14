# Compilador que vamos a usar para C
CC = gcc

# Flags de compilación:
# -g     -> mete información para depurar
# -Wall  -> activa avisos útiles del compilador
# -fPIC  -> hace falta para crear bibliotecas compartidas
CFLAGS = -g -Wall -fPIC

# En muchos Linux actuales, ONC RPC va con tirpc
# Por eso añadimos la ruta de cabeceras y la librería al enlazar
RPC_INC = -I/usr/include/tirpc
RPC_LIBS = -ltirpc

# Nombre base del archivo RPC
RPC_NAME = clavesRPC

# Archivo .x que define la interfaz RPC
RPC_X = $(RPC_NAME).x

# Archivos generados por rpcgen que realmente vamos a usar
RPC_H = $(RPC_NAME).h
RPC_CLNT = $(RPC_NAME)_clnt.c
RPC_SVC = $(RPC_NAME)_svc.c
RPC_XDR = $(RPC_NAME)_xdr.c

# Archivo manual del servidor que haces tú
SERVER_IMPL = rpc_server_impl.c

# Archivo manual del proxy RPC que hará Sergio
PROXY_IMPL = proxy-rpc.c

# Cliente de pruebas que reutilizáis
APP_CLIENT = app-cliente.c

# Lógica real del servicio que reutilizáis del ejercicio 1
LOCAL_SERVICE = claves.c

# Cabecera original del servicio
LOCAL_HEADER = claves.h

# Objetos del lado cliente para construir libclaves.so
CLIENT_OBJS = proxy-rpc.o $(RPC_NAME)_clnt.o $(RPC_NAME)_xdr.o

# Objetos del lado servidor para construir el ejecutable servidor
SERVER_OBJS = $(SERVER_IMPL:.c=.o) $(RPC_NAME)_svc.o $(RPC_NAME)_xdr.o claves.o

# Marcamos reglas que no representan archivos reales
.PHONY: all clean rpc

# Regla principal: compila todo
all: rpc libclaves.so clavesRPC_server app-cliente

# ==========================================================
# REGLA PARA GENERAR LOS ARCHIVOS RPC
# ==========================================================

# Esta regla invoca rpcgen sobre clavesRPC.x
# Usamos -aNM porque es la forma que aparece en el enunciado
# y así se generan los archivos necesarios de soporte RPC
rpc: $(RPC_X)
	rpcgen -aNM $(RPC_X)

# ==========================================================
# BIBLIOTECA DEL LADO CLIENTE: libclaves.so
# ==========================================================

# En esta práctica, la biblioteca que usa app-cliente.c
# ya no sale de claves.c, sino de la parte cliente RPC
# Es decir: proxy-rpc.c + archivos cliente generados por rpcgen
libclaves.so: $(CLIENT_OBJS)
	$(CC) -shared -o libclaves.so $(CLIENT_OBJS) $(RPC_LIBS)

# ==========================================================
# EJECUTABLE DEL SERVIDOR RPC
# ==========================================================

# El servidor se construye juntando:
# - tu implementación manual del servidor RPC
# - el stub servidor generado por rpcgen
# - la serialización XDR generada por rpcgen
# - claves.c, que contiene la lógica real del servicio
clavesRPC_server: $(SERVER_OBJS)
	$(CC) -o clavesRPC_server $(SERVER_OBJS) $(RPC_LIBS)

# ==========================================================
# EJECUTABLE DEL CLIENTE DE PRUEBA
# ==========================================================

# app-cliente.c sigue siendo el cliente de pruebas
# y se enlaza con la biblioteca libclaves.so
app-cliente: $(APP_CLIENT) libclaves.so $(LOCAL_HEADER)
	$(CC) $(CFLAGS) -o app-cliente $(APP_CLIENT) -L. -Wl,-rpath=. -lclaves

# ==========================================================
# REGLAS DE COMPILACIÓN DE OBJETOS
# ==========================================================

# Compila tu implementación manual del servidor RPC
rpc_server_impl.o: $(SERVER_IMPL) $(RPC_H) $(LOCAL_HEADER)
	$(CC) $(CFLAGS) $(RPC_INC) -c $(SERVER_IMPL)

# Compila el proxy RPC del lado cliente
proxy-rpc.o: $(PROXY_IMPL) $(RPC_H) $(LOCAL_HEADER)
	$(CC) $(CFLAGS) $(RPC_INC) -c $(PROXY_IMPL)

# Compila el stub cliente generado por rpcgen
$(RPC_NAME)_clnt.o: $(RPC_CLNT) $(RPC_H)
	$(CC) $(CFLAGS) $(RPC_INC) -c $(RPC_CLNT)

# Compila el stub servidor generado por rpcgen
$(RPC_NAME)_svc.o: $(RPC_SVC) $(RPC_H)
	$(CC) $(CFLAGS) $(RPC_INC) -c $(RPC_SVC)

# Compila el archivo XDR generado por rpcgen
$(RPC_NAME)_xdr.o: $(RPC_XDR) $(RPC_H)
	$(CC) $(CFLAGS) $(RPC_INC) -c $(RPC_XDR)

# Compila claves.c, que contiene la lógica real del servicio
claves.o: $(LOCAL_SERVICE) $(LOCAL_HEADER)
	$(CC) $(CFLAGS) -c $(LOCAL_SERVICE)

# ==========================================================
# LIMPIEZA
# ==========================================================

# Borra objetos, biblioteca, ejecutables y archivos generados por rpcgen
clean:
	rm -f *.o *.so app-cliente clavesRPC_server
	rm -f $(RPC_H) $(RPC_CLNT) $(RPC_SVC) $(RPC_XDR)
	rm -f $(RPC_NAME)_server.c $(RPC_NAME)_client.c Makefile.$(RPC_NAME)