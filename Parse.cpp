#include <unistd.h>
#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <dirent.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "Parse.h"
#include "send.h"
using namespace std;



char buf[512];

// This function have the current client structure, this function will parse the request and fill the fields
void  Parse::parseRequest(clientIdentity c_id)
{	

	getcwd(buf,256);
	string rootdir(buf);
	
	clientInfo c_info_obj;
	char buffer[4096];
	int recvbytes=0;

	char *p,*pbuffer;

	fcntl( c_id.acceptId, O_NONBLOCK, 0 );//manipulate file descriptor

	//Fetching header as string
	if((recvbytes=(recv(c_id.acceptId,buffer,sizeof(buffer),0)))==-1)// recv(newSockfd,buffer,bufferlen,0)
		perror("Receive:");
	buffer[recvbytes] = '\0';
	string header(buffer);
	//cout<<header<<endl; 

	int current = 0;
	int end = header.find_first_of("\r\n",current);

	if(end >=0){
		c_info_obj.r_firstline = header.substr(current,end-current);
		string first_line_data[3];//array of string
		pbuffer=new char[c_info_obj.r_firstline.size()+1];
		copy(c_info_obj.r_firstline.begin(),c_info_obj.r_firstline.end(),pbuffer);
		pbuffer[c_info_obj.r_firstline.size()]='\0';

		p=strtok(pbuffer," /");// split string at delimiter and return char* to first token 

		first_line_data[0].assign(p);
		int i=1;
		while(i<3){
			p=strtok(NULL," ");
			first_line_data[i].assign(p);
			i++;
		}
		i=0;
		// Setting values into c_info_obj from client
		c_info_obj.r_acceptid = c_id.acceptId;
		c_info_obj.r_portno = c_id.portNo;
		c_info_obj.r_ip = c_id.ip;
		c_info_obj.r_time = c_id.requestTime;
		c_info_obj.r_type = first_line_data[0];// GET is assigned
		c_info_obj.r_filename = first_line_data[1];
		

		int k=c_info_obj.r_filename.find_first_of("/",0);
		int z=c_info_obj.r_filename.find_first_of("/",k+1);

		if(z >=0)
			c_info_obj.rootcheck = false;
		else
			c_info_obj.rootcheck = true;
		
		c_info_obj.r_filename = rootdir + c_info_obj.r_filename;
		c_info_obj.r_method = first_line_data[2];
		
		delete [] pbuffer;  // only memory block is deleted not pointer
		pbuffer = NULL;
		checkRequest(c_info_obj); 

	}


	else{
		// write(newsockfd,buffer,no_of_bytes);
		write(c_info_obj.r_acceptid,"Error: Bad Request, Retry",25);
		close(c_info_obj.r_acceptid);
	}


}


bool Parse::fileExists(char *filename)
{
	struct stat filenamecheck;
    if (stat(filename, &filenamecheck) != -1)
       	   	return true;
    
	return false;
}


void Parse::checkRequest(clientInfo c_info_obj)
{
	//printf("tyagi\n");
	int pos=c_info_obj.r_filename.find_last_of("/");
	int next=c_info_obj.r_filename.find_first_of(".",pos);
	if(next>0){

		char *fname=new char[c_info_obj.r_filename.size()+1];
		copy(c_info_obj.r_filename.begin(),c_info_obj.r_filename.end(),fname);
		fname[c_info_obj.r_filename.size()]='\0';
		ifstream::pos_type size=0;

		// Knowing file status and size
		ifstream file;
		file.open(fname);
		if(file.is_open()){
			file.seekg(0,ios::end);
			size=file.tellg();
		}
		file.close();


		if(fileExists(fname)&& c_info_obj.r_type=="GET"){
			c_info_obj.status_file = true;
			c_info_obj.r_filesize = (int)size;
			delete [] fname;
			fname = NULL;
				sendData s;
				s.transferData(c_info_obj);
		
		}
		
		else if(fileExists(fname) && c_info_obj.r_type=="HEAD"){
			c_info_obj.status_file = true;
			c_info_obj.r_filesize = 0;
			delete [] fname;
			fname = NULL;
				sendData s;
				s.transferData(c_info_obj);
		
		}

		else{
			delete [] fname;
			fname = NULL;
			c_info_obj.r_filesize = 0;
			c_info_obj.status_file = false;
				sendData s;
				s.transferData(c_info_obj);
		}

	}

	else{
		c_info_obj.r_filesize=0;
		c_info_obj.status_file=false;
		sendData s;
		s.transferData(c_info_obj);
	}

}