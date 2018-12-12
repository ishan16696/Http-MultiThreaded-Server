#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

#include "server.h"
#include "threadPool.h"
#include "Parse.h"
using namespace std;


void dispatch_to_here(void *arg)
{
	struct clientIdentity *cid_t = (struct clientIdentity*)arg ; 
    struct clientIdentity cid;  
    cid.acceptId = cid_t->acceptId ; 
    cid.ip = cid_t->ip ; 
    cid.portNo = cid_t->portNo ; 
    cid.requestTime  = cid_t->requestTime ; 
    // cout << pthread_self()<<endl;
    Parse *p=new Parse();
    p->parseRequest(cid) ; 
}



u_int16_t get_port_number(struct sockaddr *s)
{
    return (((struct sockaddr_in  *)s)->sin_port);
}

//Return Ip address
void * get_ip_address(sockaddr *s)
{
    return &((sockaddr_in *)s)->sin_addr;
}


int getConnection(int portNo)
{
	int sockId=0;
	string port=to_string(portNo);

	struct addrinfo inValue,*serverInfo,*validInfo;

/*
   struct addrinfo {
	    int              ai_flags;     // AI_PASSIVE, AI_CANONNAME, etc.
	    int              ai_family;    // AF_INET, AF_INET6, AF_UNSPEC
	    int              ai_socktype;  // SOCK_STREAM, SOCK_DGRAM
	    int              ai_protocol;  // use 0 for "any"
	    size_t           ai_addrlen;   // size of ai_addr in bytes
	    struct sockaddr *ai_addr;      // struct sockaddr_in or _in6
	    char            *ai_canonname; // full canonical hostname

	    struct addrinfo *ai_next;      // linked list, next node
};
*/	

  	struct sockaddr_storage clientAddr;
    int acceptId, address,yes;
    char ip1[INET6_ADDRSTRLEN];


    memset(&inValue, 0, sizeof(inValue));
    inValue.ai_family = AF_INET;         //  AF_INET is for IPv4  , AF_UNSPEC for both IPv4 and IPv6
    inValue.ai_socktype = SOCK_STREAM;   // SOCK_STREAM is for TCP(if UDP then SOCK_DGRAM)
    inValue.ai_flags = AI_PASSIVE;      //this is used by applications that intend to communicate with peers running on the same host.
    yes=1;

/*     
    int getaddrinfo(const char *node, const char *service , const struct addrinfo *hints,struct addrinfo **res);
    getaddrinfo() returns one or more addrinfo structures, each of which contains an Internet address that can be specified in a call 
    to bind(2) or connect(2).

    getaddrinfo() returns a list of address structures.
    Try each address until we successfully bind.
*/
    if (getaddrinfo(NULL, port.c_str(), &inValue, &serverInfo) != 0)
            perror("Get Address:");

    for(validInfo = serverInfo; validInfo != NULL; validInfo = validInfo->ai_next) {
        if((sockId = (socket(validInfo->ai_family, validInfo->ai_socktype,0))) == -1)
                continue;
                

        if (setsockopt(sockId, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1){
            perror("setsockopt");
            break;
        }
        
        if(bind(sockId,validInfo->ai_addr, validInfo->ai_addrlen) == 0)
             break;// successfully done with bind
            
       close(sockId);//close the Connection if Unable to bind
        
    }

    
    freeaddrinfo(serverInfo);
    if(listen(sockId, MAX_LISTEN) == -1)
        perror("Listen:");

    
    threadpool tp;
    tp = create_threadpool(10);
    printf("ThreadPool created with 10 threads\n");


    while(true) {  
        // Main thread will listen continously 
        address = sizeof(clientAddr);
       
        if((acceptId = accept(sockId,(struct sockaddr*)&clientAddr,(socklen_t *)&address)) == -1)
                perror("Accept:");
        cout << "accept time " << clock() << endl;

        inet_ntop(clientAddr.ss_family,get_ip_address((struct sockaddr *)&clientAddr),ip1, sizeof(ip1));
        u_int16_t clientport = get_port_number((struct sockaddr *)&(validInfo));
        time_t tim=time(NULL);
        tm *now=gmtime(&tim);
        char currtime[50];
        
        if (strftime(currtime, 50,"%x:%X", now) == 0)
                perror("Date Error");
        
        string requesttime(currtime);


        clientIdentity cid;
        cid.acceptId = acceptId;
        string str(ip1);
        cid.ip=str;
        cid.portNo = clientport;
        cid.requestTime = requesttime;
 
        dispatch(tp, dispatch_to_here, (void *) &cid);     

    }
}