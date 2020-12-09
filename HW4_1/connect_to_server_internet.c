// run as sudo ./ping <hostname>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h> 
  
#include <stdio.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <netdb.h> 
#include <unistd.h> 
#include <string.h> 
#include <stdlib.h> 
#include <netinet/ip_icmp.h> 
#include <time.h> 
#include <fcntl.h> 
#include <signal.h> 
#include <time.h>
#include <limits.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFFER_SIZE 1024
  
// Define the Packet Constants 
// ping packet size 
#define PING_PKT_S 64 
   
// Automatic port number 
#define PORT_NO 80  
  
// Automatic port number 
#define PING_SLEEP_RATE 10000
  
// Gives the timeout delay for receiving packets 
// in seconds 
#define RECV_TIMEOUT 1  
  
// Define the Ping Loop 
int pingloop=1; 

// Define max number of pings
#define MAX_PING 10

// Define path and name for output file
#define FILE_PATH "server_output.txt"

// Define ping path
#define PING_PATH "google.com"

// ping packet structure 
struct ping_pkt 
{ 
    struct icmphdr hdr; 
    char msg[PING_PKT_S-sizeof(struct icmphdr)]; 
}; 

int socket_connect(char *host, in_port_t port){
	struct hostent *hp;
	struct sockaddr_in addr;
	int on = 1, sock;     

	if((hp = gethostbyname(host)) == NULL){
		herror("gethostbyname");
		exit(1);
	}
	bcopy(hp->h_addr, &addr.sin_addr, hp->h_length);
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));

		if(sock == -1){
		perror("setsockopt");
		exit(1);
	}
	
	if(connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1){
		perror("connect");
		exit(1);

	}
	return sock;
}


int clear_file()
{
	FILE *fp;
	fp = fopen(FILE_PATH, "w");
	fclose(fp);
}


int write_to_file(char* text)
{
	FILE *fp;
	char cwd[PATH_MAX];
	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		// printf("Current working dir: %s\n", cwd);
	}
	else
	{
		perror("getcwd() error");
		return 1;
	}
	fp = fopen(FILE_PATH, "a");
	fprintf(fp, "%s\n", text);
	fclose(fp);
	return 0;
}

  
// Calculating the Check Sum 
unsigned short checksum(void *b, int len) 
{    unsigned short *buf = b; 
    unsigned int sum=0; 
    unsigned short result; 
  
    for ( sum = 0; len > 1; len -= 2 ) 
        sum += *buf++; 
    if ( len == 1 ) 
        sum += *(unsigned char*)buf; 
    sum = (sum >> 16) + (sum & 0xFFFF); 
    sum += (sum >> 16); 
    result = ~sum; 
    return result; 
} 
  
  
// Interrupt handler 
void intHandler(int dummy) 
{ 
    pingloop=0; 
} 
  
// Performs a DNS lookup  
char *dns_lookup(char *addr_host, struct sockaddr_in *addr_con) 
{ 
    printf("\nResolving DNS..\n"); 
    struct hostent *host_entity; 
    char *ip=(char*)malloc(NI_MAXHOST*sizeof(char)); 
    int i; 
  
    if ((host_entity = gethostbyname(addr_host)) == NULL) 
    { 
        // No ip found for hostname 
        return NULL; 
    } 
      
    //filling up address structure 
    strcpy(ip, inet_ntoa(*(struct in_addr *) 
                          host_entity->h_addr)); 
  
    (*addr_con).sin_family = host_entity->h_addrtype; 
    (*addr_con).sin_port = htons (PORT_NO); 
    (*addr_con).sin_addr.s_addr  = *(long*)host_entity->h_addr; 
  
    return ip; 
      
} 

// Resolves the reverse lookup of the hostname 
char* reverse_dns_lookup(char *ip_addr) 
{ 
    struct sockaddr_in temp_addr;     
    socklen_t len; 
    char buf[NI_MAXHOST], *ret_buf; 
  
    temp_addr.sin_family = AF_INET; 
    temp_addr.sin_addr.s_addr = inet_addr(ip_addr); 
    len = sizeof(struct sockaddr_in); 
  
    if (getnameinfo((struct sockaddr *) &temp_addr, len, buf,  
                    sizeof(buf), NULL, 0, NI_NAMEREQD))  
    { 
        printf("Could not resolve reverse lookup of hostname\n"); 
        return NULL; 
    } 
    ret_buf = (char*)malloc((strlen(buf) +1)*sizeof(char) ); 
    strcpy(ret_buf, buf); 
    return ret_buf; 
} 

// make a ping request 
void send_ping(int ping_sockfd, struct sockaddr_in *ping_addr, 
                char *ping_dom, char *ping_ip, char *rev_host) 
{ 
    int ttl_val=64, msg_count=0, i, addr_len, flag=1, msg_received_count=0; 
    struct ping_pkt pckt; 
    struct sockaddr_in r_addr; 
    struct timespec time_start, time_end, tfs, tfe; 
    long double rtt_msec=0, total_msec=0; 
    struct timeval tv_out; 
    tv_out.tv_sec = RECV_TIMEOUT; 
    tv_out.tv_usec = 0; 
    clock_gettime(CLOCK_MONOTONIC, &tfs); 
  
      
    // set socket options at ip to TTL and value to 64, 
    // change to what you want by setting ttl_val 
    if (setsockopt(ping_sockfd, SOL_IP, IP_TTL,  
               &ttl_val, sizeof(ttl_val)) != 0) 
    { 
        printf("\nSetting socket options to TTL failed!\n"); 
        return; 
    } 
  
    else
    { 
        printf("Socket set to TTL..\n"); 
    } 
  
    // setting timeout of recv setting 
    setsockopt(ping_sockfd, SOL_SOCKET, SO_RCVTIMEO, 
                   (const char*)&tv_out, sizeof tv_out); 
  // send icmp packet in an infinite loop 
    while(pingloop && (msg_count < MAX_PING)) 
    { 
        // flag is whether packet was sent or not 
        flag=1; 
       
        //filling packet 
        bzero(&pckt, sizeof(pckt)); 
          
        pckt.hdr.type = ICMP_ECHO; 
        pckt.hdr.un.echo.id = getpid(); 
          
        for ( i = 0; i < sizeof(pckt.msg)-1; i++ ) 
            pckt.msg[i] = i+'0'; 
          
        pckt.msg[i] = 0; 
        pckt.hdr.un.echo.sequence = msg_count++; 
        pckt.hdr.checksum = checksum(&pckt, sizeof(pckt)); 
  
  
        usleep(PING_SLEEP_RATE); 
  
        //send packet 
        clock_gettime(CLOCK_MONOTONIC, &time_start); 
        if ( sendto(ping_sockfd, &pckt, sizeof(pckt), 0,  
           (struct sockaddr*) ping_addr,  
            sizeof(*ping_addr)) <= 0) 
        { 
            printf("\nPacket Sending Failed!\n"); 
            flag=0; 
        } 

        //receive packet 
        addr_len=sizeof(r_addr); 
  
        if ( recvfrom(ping_sockfd, &pckt, sizeof(pckt), 0,  
             (struct sockaddr*)&r_addr, &addr_len) <= 0 
              && msg_count>1)  
        { 
            printf("\nPacket receive failed!\n"); 
        } 
  
        else
        { 
            clock_gettime(CLOCK_MONOTONIC, &time_end); 
              
            double timeElapsed = ((double)(time_end.tv_nsec - time_start.tv_nsec))/1000000.0;
            rtt_msec = (time_end.tv_sec - time_start.tv_sec) * 1000.0 + timeElapsed; 
              
            // if packet was not sent, don't receive 
            if(flag) 
            { 
                if(!(pckt.hdr.type ==69 && pckt.hdr.code==0))  
                { 
                    printf("Error..Packet received with ICMP type %d code %d\n", pckt.hdr.type, pckt.hdr.code); 
                } 
                else
                { 
                	char output_string[256] = { };
                    // printf("%d bytes from %s (h: %s) (%s) msg_seq=%d ttl=%d rtt = %Lf ms.\n"
                    // 	   ,PING_PKT_S, ping_dom
                    // 	   , rev_host, ping_ip, msg_count
                    // 	   , ttl_val, rtt_msec); 
                    sprintf(output_string, "%d bytes from %s (h: %s) (%s) msg_seq=%d ttl=%d rtt = %Lf ms."
                    	   ,PING_PKT_S, ping_dom
                    	   , rev_host, ping_ip, msg_count
                    	   , ttl_val, rtt_msec); 
                    write_to_file(output_string);
  
                    msg_received_count++; 
                } 
            } 
        }     
    } 
	clock_gettime(CLOCK_MONOTONIC, &tfe); 
    double timeElapsed = ((double)(tfe.tv_nsec - tfs.tv_nsec))/1000000.0; 
      
    total_msec = (tfe.tv_sec-tfs.tv_sec)*1000.0 + timeElapsed; 
                     
    printf("===%s ping statistics===\n", ping_ip); 
    printf("%d packets sent, %d packets received, %f percent packet loss. Total time: %Lf ms.\n\n"
    	   , msg_count
    	   , msg_received_count
    	   , ((msg_count - msg_received_count)/msg_count) * 100.0
    	   , total_msec);  
} 

int ping_demo()
{
	printf("\nOpen a connection to ping a server\n");

    int sockfd; 
    char *ip_addr, *reverse_hostname; 
    struct sockaddr_in addr_con; 
    int addrlen = sizeof(addr_con); 
    char net_buf[NI_MAXHOST], server_reply[2000], *message; 

    ip_addr = dns_lookup(PING_PATH, &addr_con); 
    if(ip_addr==NULL) 
    { 
        printf("\nDNS lookup failed! Could not resolve hostname!\n"); 
        return 0; 
    } 
  
    reverse_hostname = reverse_dns_lookup(ip_addr); 
    printf("Trying to connect to '%s' IP: %s\n"
    	   , PING_PATH
    	   , ip_addr); 
    printf("Reverse Lookup domain: %s", reverse_hostname); 
  
    //socket() 
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP); 
    if(sockfd<0) 
    { 
        printf("\nSocket file descriptor not received!!\n"); 
        return 1; 
    } 
    else
    {
        printf("\nSocket file descriptor %d received\n", sockfd); 
    }
  
    signal(SIGINT, intHandler);//catching interrupt 
  
    //send pings continuously 
    send_ping(sockfd, &addr_con, reverse_hostname,  
                                 ip_addr, PING_PATH); 

	close(sockfd);
	printf("Closed the connection to the pinged server!\n\n");

	return 0;
}

int connect_demo()
{
	printf("\nOpen a connection to send and receive some data\n\n");

	int fd;
	char buffer[BUFFER_SIZE];
 
 	printf("Open the server connection\n");
	fd = socket_connect(PING_PATH, PORT_NO); 

	printf("Send a 'GET' message out to the server\n");
	write(fd, "GET /\r\n", strlen("GET /\r\n")); // write(fd, char[]*, len);  
	
	printf("Receive a message from the server (written to server_output.txt)\n");
	bzero(buffer, BUFFER_SIZE);
	while(read(fd, buffer, BUFFER_SIZE - 1) != 0)
	{
		// fprintf(stderr, "%s", buffer);
		write_to_file(buffer);
		bzero(buffer, BUFFER_SIZE);
	}

	shutdown(fd, SHUT_RDWR); 
	close(fd); 

	printf("Closed the connection to the connected server!\n\n");

	return 0;
}

// Driver Code 
int main() 
{ 
    // clear the fie out
    clear_file();

    char header_string[256] = "OUTPUT FROM CONNECT TO SERVER TEST\n";
    write_to_file(header_string);

	if (ping_demo())
	{
		printf("ERROR\n");
		return 1;
	}

	if(connect_demo())
	{
		printf("ERROR\n");
		return 1;
	}
      
    return 0; 
} 



/*
RESOURCES:
https://www.geeksforgeeks.org/ping-in-c/
http://www.strudel.org.uk/itoa/
https://www.binarytides.com/socket-programming-c-linux-tutorial/
*/

