#include <unistd.h>
#include <bits/stdc++.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sstream>
#include <dirent.h>
#include <sys/stat.h>
#include "send.h"
#include "Parse.h"
using namespace std;

bool daam=false;


void sendHead(clientInfo c){
	char currtime[50];
	char lastmodify[50];
	struct stat buf;
	time_t tim=time(NULL);
		tm *now=gmtime(&tim);
		
	if (strftime(currtime, 50,"%c", now) == 0)
				perror("Date Error");
	char *fname=new char[c.r_filename.size()+1];
	copy(c.r_filename.begin(),c.r_filename.end(),fname);
	fname[c.r_filename.size()]='\0';
	stat(fname,&buf);
	strcpy(lastmodify,ctime(&buf.st_mtime));
	delete[] fname;
	fname=NULL;

	int pos=c.r_filename.find_last_of(".");
	string c_type=c.r_filename.substr(pos+1,c.r_filename.size());

	// Type of file requested
	if (c_type == "txt" || c_type == "html" || c_type == "htm")
			c.r_ctype ="text/html";
	else if(c_type == "gif" || c_type == "jpeg")
			c.r_ctype = "image/"+c_type;
	else
			c.r_ctype = " ";

	stringstream st;
	st << c.r_filesize;
	string fileSize=st.str();
	string header= c.r_type +" "+c.r_method +" 200 OK\r\nDate: ";
	string time(currtime);
	header = header + time;
	header = header +"\r\n" +"Server: myhttpd 1.0\r\n" + "Last-Modified:";
	string lmodify(lastmodify);
	header = header + lmodify ;
		// cout<<"thread_id of current thread is: ";
		// cout<<pthread_self()<<endl;
	header = header + "Content-Type:" + c.r_ctype +"\r\n" + "Content-Length:" + fileSize +"\r\n\r\n";
	if (send(c.r_acceptid, header.c_str(), strlen(header.c_str()), 0) == -1)//send(sockfd,buffer,bufferlen,0);
		perror("send");


	c.status_code = 200;
}

void sendGET(clientInfo c){
	char currtime[50];
	char lastmodify[50];
	struct stat buf;
	time_t tim=time(NULL);
		tm *now=gmtime(&tim);
		
	if (strftime(currtime, 50,"%c", now) == 0)
				perror("Date Error");
	char *fname=new char[c.r_filename.size()+1];
	copy(c.r_filename.begin(),c.r_filename.end(),fname);
	fname[c.r_filename.size()]='\0';
	stat(fname,&buf);
	strcpy(lastmodify,ctime(&buf.st_mtime));
	delete[] fname;
	fname=NULL;

	int pos=c.r_filename.find_last_of(".");
	string c_type=c.r_filename.substr(pos+1,c.r_filename.size());

	// Type of file requested
	if (c_type == "txt" || c_type == "html" || c_type == "htm")
			c.r_ctype ="text/html";
	else if(c_type == "gif" || c_type == "jpeg")
			c.r_ctype = "image/"+c_type;
	else
			c.r_ctype = " ";

	stringstream st;
	st << c.r_filesize;
	string fileSize=st.str();
	string header= c.r_type +" "+c.r_method +" 200 OK\r\nDate: ";
	string time(currtime);
	header = header + time;
	header = header +"\r\n" +"Server: myhttpd 1.0\r\n" + "Last-Modified:";
	string lmodify(lastmodify);
	header = header + lmodify ;
		
	header = header + "Content-Type:" + c.r_ctype +"\r\n" + "Content-Length:" + fileSize +"\r\n\r\n";
	if (send(c.r_acceptid, header.c_str(), strlen(header.c_str()), 0) == -1)//send(sockfd,buffer,bufferlen,0);
		perror("send");

	
	ifstream file;
	char *readBlock;
	size_t size;
	file.open(c.r_filename.c_str());
	if(file.is_open()){
		string read;
		file.seekg(0,ios::end);
		size=file.tellg();
		readBlock=new char[size];
		file.seekg(0,ios::beg);
		file.read(readBlock,size);
	}
	else{
		cout<<"Never went Inside"<<endl;
	}
		
	if (send(c.r_acceptid, readBlock, size, 0) == -1)
       perror("send");
	file.close();
	c.status_code = 200;
	delete [] readBlock;

}



void sendData::transferData(clientInfo c)
{
 
	if(c.status_file ==true && c.r_type =="GET") 
	{
		sendGET(c);

	} 
	else if(c.status_file ==true && c.r_type =="HEAD") 
	{
		sendHead(c);

	} 
	else 
	{
		if (c.rootcheck == true) 
		{

			write(c.r_acceptid,"Error 404: File Not Found",25);
			c.status_code = 404;
		} 
		else 
		{
			c.status_code = 404;
		}
		
	}



	display_log(c);
	
	close(c.r_acceptid);

}

void sendData::display_log(clientInfo c)
{

	time_t tim=time(NULL);
	tm *now=gmtime(&tim);
	char currtime[50];
	if (strftime(currtime, 50,"%x:%X", now) == 0)
		perror("Date Panga");
	string servetime(currtime);
	c.r_servetime = servetime;
	if(!daam)
	{
		usleep(0);
		 cout<<c.r_firstline<<" "<<"thread_id:"<<pthread_self()<< " time : "<< clock()<<endl;
	}
	else
	cout<<c.r_ip<<"  ["<<c.r_time<<"]  ["<<c.r_servetime<<"]  "<<c.r_firstline<<" "<<c.status_code<<" "<<c.r_filesize<<" "<<"thread_id:"<<pthread_self()<<endl;

}
