#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "FUNCTIONS_C.h"

#define DEF_SERVER_IP    "127.0.0.1"
#define DEF_SERVER_PORT  "9099"

int main( int argc, char **argv )
{
	bold_off();
	color_def();
	fill_table();
	printf("Witaj, podaj Twoj nick: ");
	scanf("%s",nickname);


  if ( argc > 1 ) srv_addr = argv[1];
  else srv_addr = DEF_SERVER_IP;
  if ( argc > 2 ) srv_port = argv[2];
  else srv_port = DEF_SERVER_PORT;

  // Tworzymy adres docelowego serwera z rodziny INET:
  memset( &adr_srv, 0, sizeof( adr_srv ) );
  adr_srv.sin_family = AF_INET;
  adr_srv.sin_port = htons( atoi( srv_port ) );
  if ( !inet_aton( srv_addr, &adr_srv.sin_addr ) )
    error_handler( "inet_aton()" );
  adr_srv_len = sizeof( adr_srv );
polacz_sie();
read_command();
strcpy(opponent,command);
out_code = write(s_inet_srv,nickname,strlen(nickname));
printf("\nTwoim przeciwnikiem jest: ");
printf("%s \n",command);
	
	do{
		fprintf(stdout,"\033[2J");
		print_table();
		read_command();
		if(!strcmp(command,"S_WIN")){
			printf("Koniec gry. Wygral %s \n",opponent);
			break;
		}
		else if(!strcmp(command,"END")){
			printf("Koniec giery. Remis!\n");
			break;
		}
			
		if(check_stat())strcpy(command,"C_WIN");
		else if((end_of_game())==1){
			strcpy(command,"END");
		}
		else strcpy(command,"NOT");
		send_command(command);
		if(!strcmp(command,"C_WIN")){
			printf("WYGRALES! Brawo!\n");
			break;
		}
		else if(!strcmp(command,"END")){
			printf("Koniec gry. Remis");
			break;
		}
		
		out_code = read(s_inet_srv, who, sizeof(who));
		who[out_code]='\0';
		move();
		if(!strcmp(who,"server")){
			out_code = read(s_inet_srv,&rcv_str,sizeof(rcv_str));
			modify_table(rcv_str);
			print_table();
		} 
		else if(!strcmp(who,"client")){
			rcv_str = type_coordinates();
			send_struct(rcv_str);
			modify_table(rcv_str);
			print_table();
		}
	}
	while(1);
	close( s_inet_srv );
	
}
