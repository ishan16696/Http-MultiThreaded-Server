#ifndef INC_SERVER_H
#define INC_SERVER_H



#define MAX_LISTEN  100  /* second argument to listen() */



void dispatch_to_here(void *arg);
u_int16_t get_port_number(struct sockaddr *s);

int getConnection(int);

#endif