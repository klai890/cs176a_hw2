all: compile_project
compile_project:
	gcc client_c_udp.c -o client_c_udp
	gcc server_c_udp.c -o server_c_udp
	gcc client_c_tcp.c -o client_c_tcp
	gcc server_c_tcp.c -o server_c_tcp