#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "FUNCTIONS_S.h"

#define DEF_IP_ADDR  "127.0.0.1"
#define DEF_PORT     "9099"
#define QUEUE_LEN    2
int main( int argc, char **argv )
{ 
  bold_off();	
  color_def();
  printf("Witaj, podaj Twoj nick: ");
  scanf("%s",nickname);
  fill_table();

  if ( argc > 1 ) srv_addr = argv[1];
  else srv_addr = DEF_IP_ADDR;
  if ( argc > 2 ) srv_port = argv[2];
  else srv_port = DEF_PORT;

    // Tworzymy pojedyncze gniazdo z rodziny INET typu TCP:
  s_srv_inet = socket( AF_INET, SOCK_STREAM, 0 );
  if ( s_srv_inet == -1 ) error_handler( "socket()" );

  // Tworzymy adres z rodziny INET dla serwera:
  memset( &adr_srv, 0, sizeof( adr_srv ) );
  adr_srv.sin_family = AF_INET;
  adr_srv.sin_port = htons( (unsigned short) atoi( srv_port ) );
  if ( !strcasecmp( srv_addr, "ANY" ) )	// Adres niesprecyzowany
    adr_srv.sin_addr.s_addr = INADDR_ANY;  
  else					// Zwykly adres
    {
      if ( !inet_aton( srv_addr, &adr_srv.sin_addr ) )
        error_handler( "inet_aton()" );
    }
  adr_srv_len = sizeof( adr_srv );

  // Wiazemy adres serwera z gniazdem:
  out_code = bind( s_srv_inet, (struct sockaddr *)&adr_srv, adr_srv_len );
  if ( out_code == -1 ) error_handler( "bind()" );

  // Czynimy gniazdo gniazdem nasluchujacym:
  out_code = listen( s_srv_inet, QUEUE_LEN );
  if ( out_code == -1 ) error_handler( "listen()" );
  printf( "Oczekiwanie na przeciwnika: \n");
  	
	losowanie();
	acceptance(); 			// Akceptacja klienta
	send_command(nickname);
  	out_code = read(s_cln_inet,command,sizeof(command));	
	strcpy(opponent,command);
	do{
		fprintf(stdout,"\033[2J");
		print_table();
		if(check_stat()) strcpy(command,"S_WIN");
		else if((end_of_game())==1){
		       	strcpy(command,"END");
		}
		else strcpy(command,"NOT");
		send_command(command);
		if(!strcmp(command,"S_WIN")){
			printf("WYGRALES! Brawo!\n");
			break;
		}
		else if(!strcmp(command,"END")){
			printf("Koniec gry! Remis.\n");
			break;
		}

		read_command();
		if(!strcmp(command,"C_WIN")){
		       	printf("Koniec gry. Wygral %s \n",opponent);
			break;
		}
		else if(!strcmp(command,"END")){
			printf("Koniec gry! REMIS.\n");
			break;
		}
		
		send_command(who);
		move();
		if(!strcmp(who,"server")) {
			rcv_str = type_coordinates();
			out_code = write(s_cln_inet,&rcv_str,sizeof(rcv_str));
			modify_table(rcv_str);
			print_table();
			strcpy(who,"client");
		}
		else if(!strcmp(who,"client")){
			out_code = read(s_cln_inet,&rcv_str,sizeof(rcv_str));
			modify_table(rcv_str);
			print_table();
			strcpy(who,"server");

		}

   }
   while(1);
	close(s_srv_inet);
	close(s_cln_inet);
		
}
