#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/* You will to add includes here */
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>



// Enable if you want debugging to be printed, see examble below.
// Alternative, pass CFLAGS=-DDEBUG to make, make CFLAGS=-DDEBUG

#define DEBUG
// INCLUDED MAX RECV  BUF SIZE

#define MAXDATASIZE 1401 //MAX NUM OF BYTES WE CAN GET AT ONCE
// Included to get the support library

#include <calcLib.h>
// get sockaddr, IPv4 or IPv6:



void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	if (sa->sa_family == AF_INET6) {
	  	return &(((struct sockaddr_in6*)sa)->sin6_addr);
	}
	printf("Unknown FAMILY!!!!\n");
	return(0); 
}



int main(int argc, char *argv[]){

  /*
    Read first input, assumes <ip>:<port> syntax, convert into one string (Desthost) and one integer (port). 
     Atm, works only on dotted notation, i.e. IPv4 and DNS. IPv6 does not work if its using ':'.
     SAMPLE USAGE 13.53.76.30:5000 
  */
  if (argc != 2) {
     fprintf(stderr,"usage: client hostname:port\n");
     exit(1);
    }
  char delim[]=":";
  char calcDelim[] = " ";
  char *Desthost=strtok(argv[1],delim);
  char *Destport=strtok(NULL,delim);
  int  sockfd, numbytes;
  char buf[MAXDATASIZE];
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];


  /* Initialize the library, this is needed for this library. */
  //initCalcLib();
  double f1,f2,fresult;
  int i1,i2,iresult;
  // *Desthost now points to a sting holding whatever came before the delimiter, ':'.
  // *Dstport points to whatever string came after the delimiter. 

  /* Do magic */
  //int  port=atoi(Destport);
  #ifdef DEBUG 
  printf("Host %s, and port %s.\n",Desthost,Destport);
  #endif
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;


 if ((rv = getaddrinfo(Desthost, Destport, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }    
 
 //Desthost
 
 // loop through all the results and connect to the first we can
 for(p = servinfo; p != NULL; p = p->ai_next) {
	 if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }
	printf("Connection successfull.\n");
        break;
    }
    /* Why are we here??? p == NULL, Looped no match (socket &| connect).. 
       Success?? */
    if (p == NULL) {
        fprintf(stderr, "client: failed to socket||connect\n");
	/* Clear servinfo */
	freeaddrinfo(servinfo); // all done with this structure
        return 2;
    }
    printf("client: connected to %s:%s\n", s,Destport);
    freeaddrinfo(servinfo); // all done with this structure
  
    while(1){

      numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0);
      if(numbytes == -1 ){
        perror("recv");
        exit(1);
      }
      if (numbytes == 0) {
	    printf("Server closed.\n");
	    break;
      }
      printf("\n\nclient (%d bytes) : receive complete : %s\n",numbytes, buf);
      bzero(buf,MAXDATASIZE);
      //Server Message
      buf[0] = 'O';
      buf[1] = 'K';
      buf[2] = '\n';
      
      numbytes = send(sockfd, buf,strlen(buf),0);

      if(numbytes == -1 ){
         perror("send");
         exit(1);
      }

      printf("client (%d bytes) : send complete : %s\n",numbytes,buf);
      
      bzero(buf,MAXDATASIZE);
      
      numbytes = recv(sockfd, buf, MAXDATASIZE-1,0);
	    if(numbytes == -1) {
		    perror("recv");
		    exit(1);

      }

      if(numbytes == 0) {
		    printf("server closed.\n");
		    exit(1);
	    }
             
      if(buf[0] == 'f'){	   
        printf("client (%d bytes) : receive complete : %s\n",numbytes, buf);
		    char *cmd = strtok(buf,calcDelim);
		    //printf("cmd operation is : '%s'\n", cmd);
		    char *remStr1 = strtok(NULL,calcDelim);
		    f1 = atof(remStr1);
		    //printf("1st value is : '%f'\n", f1);
		    char *remStr2 = strtok(NULL,calcDelim);
		    f2 = atof(remStr2);
		    //printf("2nd value is : '%f'\n", f2);
        
        //##########fmul###########
		    if (strcmp(cmd,"fmul")==0){
		      fresult = f1*f2;
          //printf("fresult is : '%f'\n",fresult);

          bzero(buf,MAXDATASIZE);
          sprintf(buf,"%f\n",fresult);
          numbytes = send(sockfd, buf,strlen(buf),0);

          if(numbytes == -1 ){
            perror("send");
	          /* close socket */
            exit(1);
          }

          printf("client (%d bytes) : send complete : %s\n",numbytes,buf);
      
          bzero(buf,MAXDATASIZE);

          numbytes = recv(sockfd, buf, MAXDATASIZE-1,0);
	        if(numbytes == -1) {
		       perror("recv");
		       exit(1);
          }

          if(numbytes == 0) {
		        printf("server closed.\n");
		        exit(1);
	        }
          printf("client (%d bytes) : receive complete : %s\n",numbytes,buf);

          bzero(buf,MAXDATASIZE);
          break;
          }

        //##########fdiv###########
		    if (strcmp(cmd,"fdiv")==0){
		      fresult = f1/f2;
          //printf("fresult is : '%f'\n",fresult);

          bzero(buf,MAXDATASIZE);
          sprintf(buf,"%f\n",fresult);
          numbytes = send(sockfd, buf,strlen(buf),0);

          if(numbytes == -1 ){
            perror("send");
	          /* close socket */
            exit(1);
          }

          printf("client (%d bytes) : send complete : %s\n",numbytes,buf);
      
          bzero(buf,MAXDATASIZE);

          numbytes = recv(sockfd, buf, MAXDATASIZE-1,0);
	        if(numbytes == -1) {
		       perror("recv");
		       exit(1);
          }

          if(numbytes == 0) {
		        printf("server closed.\n");
		        exit(1);
	        }
          printf("client (%d bytes) : receive complete : %s\n",numbytes,buf);

          bzero(buf,MAXDATASIZE);
          break;
          }

        //##########fadd###########
		    if (strcmp(cmd,"fadd")==0){
		      fresult = f1+f2;
          //printf("fresult is : '%f'\n",fresult);

          bzero(buf,MAXDATASIZE);
          sprintf(buf,"%f\n",fresult);
          numbytes = send(sockfd, buf,strlen(buf),0);

          if(numbytes == -1 ){
            perror("send");
	          /* close socket */
            exit(1);
          }

          printf("client (%d bytes) : send complete : %s\n",numbytes,buf);
      
          bzero(buf,MAXDATASIZE);

          numbytes = recv(sockfd, buf, MAXDATASIZE-1,0);
	        if(numbytes == -1) {
		       perror("recv");
		       exit(1);
          }

          if(numbytes == 0) {
		        printf("server closed.\n");
		        exit(1);
	        }
          printf("client (%d bytes) : receive complete : %s\n",numbytes,buf);

          bzero(buf,MAXDATASIZE);
          break;
          }
        //##########fsub###########
		    if (strcmp(cmd,"fsub")==0){
		      fresult = f1-f2;
          //printf("fresult is : '%f'\n",fresult);

          bzero(buf,MAXDATASIZE);
          sprintf(buf,"%f\n",fresult);
          numbytes = send(sockfd, buf,strlen(buf),0);

          if(numbytes == -1 ){
            perror("send");
	          /* close socket */
            exit(1);
          }

          printf("client (%d bytes) : send complete : %s\n",numbytes,buf);
      
          bzero(buf,MAXDATASIZE);

          numbytes = recv(sockfd, buf, MAXDATASIZE-1,0);
	        if(numbytes == -1) {
		       perror("recv");
		       exit(1);
          }

          if(numbytes == 0) {
		        printf("server closed.\n");
		        exit(1);
	        }
          printf("client (%d bytes) : receive complete : %s\n",numbytes,buf);

          bzero(buf,MAXDATASIZE);
          break;
          }

      }
      
      else {

        printf("client (%d bytes) : receive complete : %s\n",numbytes, buf);
		    char *cmd = strtok(buf,calcDelim);
		    //printf("cmd operation is : '%s'\n", cmd);
		    char *remStr1 = strtok(NULL,calcDelim);
		    i1 = atoi(remStr1);
		    //printf("1st value is : '%f'\n", f1);
		    char *remStr2 = strtok(NULL,calcDelim);
		    i2 = atoi(remStr2);
		    //printf("2nd value is : '%f'\n", f2);
        
        //##########mul###########
		    if (strcmp(cmd,"mul")==0){
		      iresult = i1*i2;

          bzero(buf,MAXDATASIZE);
          sprintf(buf,"%d\n",iresult);
          numbytes = send(sockfd, buf,strlen(buf),0);

          if(numbytes == -1 ){
            perror("send");
	          /* close socket */
            exit(1);
          }

          printf("client (%d bytes) : send complete : %s\n",numbytes,buf);
          bzero(buf,MAXDATASIZE);

          numbytes = recv(sockfd, buf, MAXDATASIZE-1,0);
	        if(numbytes == -1) {
		       perror("recv");
		       exit(1);
          }

          if(numbytes == 0) {
		        printf("server closed.\n");
		        exit(1);
	        }
          printf("client (%d bytes) : receive complete : %s\n",numbytes,buf);

          bzero(buf,MAXDATASIZE);
          break;
          }

        //##########div###########
		      if (strcmp(cmd,"div")==0){
		         iresult = i1/i2;
             //printf("fresult is : '%f'\n",fresult);

          bzero(buf,MAXDATASIZE);
          sprintf(buf,"%d\n",iresult);
          numbytes = send(sockfd, buf,strlen(buf),0);

          if(numbytes == -1 ){
            perror("send");
	          /* close socket */
            exit(1);
          }

          printf("client (%d bytes) : send complete : %s\n",numbytes,buf);
      
          bzero(buf,MAXDATASIZE);

          numbytes = recv(sockfd, buf, MAXDATASIZE-1,0);
	        if(numbytes == -1) {
		       perror("recv");
		       exit(1);
          }

          if(numbytes == 0) {
		        printf("server closed.\n");
		        exit(1);
	        }
          printf("client (%d bytes) : receive complete : %s\n",numbytes,buf);

          bzero(buf,MAXDATASIZE);
          break;
          }

        //##########add###########
		    if (strcmp(cmd,"add")==0){
		      iresult = i1+i2;

          bzero(buf,MAXDATASIZE);
          sprintf(buf,"%d\n",iresult);
          numbytes = send(sockfd, buf,strlen(buf),0);

          if(numbytes == -1 ){
            perror("send");
	          /* close socket */
            exit(1);
          }

          printf("client (%d bytes) : send complete : %s\n",numbytes,buf);
      
          bzero(buf,MAXDATASIZE);

          numbytes = recv(sockfd, buf, MAXDATASIZE-1,0);
	        if(numbytes == -1) {
		       perror("recv");
		       exit(1);
          }

          if(numbytes == 0) {
		        printf("server closed.\n");
		        exit(1);
	        }
          printf("client (%d bytes) : receive complete : %s\n",numbytes,buf);

          bzero(buf,MAXDATASIZE);
          break;
          }
        //##########sub###########
		    if (strcmp(cmd,"sub")==0){
		      iresult = i1-i2;
          

          bzero(buf,MAXDATASIZE);
          sprintf(buf,"%d\n",iresult);
          numbytes = send(sockfd, buf,strlen(buf),0);

          if(numbytes == -1 ){
            perror("send");
	          /* close socket */
            exit(1);
          }

          printf("client (%d bytes) : send complete : %s\n",numbytes,buf);
      
          bzero(buf,MAXDATASIZE);

          numbytes = recv(sockfd, buf, MAXDATASIZE-1,0);
	        if(numbytes == -1) {
		       perror("recv");
		       exit(1);
          }

          if(numbytes == 0) {
		        printf("server closed.\n");
		        exit(1);
	        }
          printf("client (%d bytes) : receive complete : %s\n",numbytes,buf);

          bzero(buf,MAXDATASIZE);
          break;
          }

    }

  }
    close(sockfd);
    return 0;
}




