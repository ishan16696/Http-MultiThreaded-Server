#include <bits/stdc++.h>
#include "server.h"
using namespace std;


void dispatch_to_me(int arg){
	getConnection(arg);
}

//************************************* main driver function ***********************************************
int main(int argc,char *argv[])
{
    int default_port=9923;
    if(argc==2)
    {
    	if(argv[1][0] >= '0' && argv[1][0] <= '9') {
            default_port = atoi(argv[1]);
        } 
    }

    dispatch_to_me(default_port);
	

	return 0;
}
