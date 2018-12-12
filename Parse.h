#include <bits/stdc++.h>
using namespace std;

#ifndef INC_PARSE_H
#define INC_PARSE_H


struct clientIdentity
{
	int acceptId;    // socketDescriptor
	string ip;
	int portNo;
	string requestTime;
};


struct clientInfo
{
	string r_method;
	string r_type;
	string r_version;
	string r_firstline;
	string r_filename;
	string r_time;
	string r_servetime;
	int r_acceptid;
	string r_ip;
	u_int16_t r_portno;
	int r_filesize;
	bool status_file;
	string r_ctype;
	bool rootcheck;
	int status_code;

};

class Parse
{
public:
		
		bool fileExists(char *filename);
		void parseRequest(clientIdentity);
	
		void checkRequest(clientInfo);

};

#endif