#include <errno.h>


struct pole{						// Struktura przechowywująca dane: położenie x,y, oraz znak X lub O
	int x;
	int y;
	char znak;
};

char 		tablica[3][3];			// Glowna tablica do grania, 3x3, wypelniana *
char 		nickname[12];
char 		opponent[12];
char   		command[10];			// Zmienna do wysylania informajci do servera
char  		who[10];				// Zmienna do odczytywania od kogo jest ruch
char   		*srv_addr = NULL;		// Lancuch z adresem IP serwera
char  	   	*srv_port = NULL;		// Lancuch z numerem portu serwera
struct 		sockaddr_in adr_srv;	// Adres serwera
struct		servent *service;		// Opis uslugi
struct	    pole rcv_str;			// RCV_STR przechowuje strukture x,y,znak
int    		adr_srv_len;			// Dlugosc adresu
int   		i, n;					// Zmienne pomocnicze
int    		out_code;				// Rezultat wykonania funkcji typu int
int    		s_inet_srv;				// Nr gniazda


void 		fill_table(void);			// Wypełnia całą TABLICE gwiazdkami(*) 
void 		print_table(void);			// Wyświetla tablicę bez gwiazdek, ale z X i O
void 		bold_on(void);				// Włączenie pogrubienia czcionki
void 		bold_off(void);				// Wyłączenie pogrubienia czcionki
void 		color(char kolor[10]);			// Zmiana koloru na podany w nawiasie
void 		color_def(void);			// Zmiana koloru na kolor podstawowy
void 		modify_table(struct pole rcv_str);	// Wprowadza to TABLICY podane w strukturze wartości
int 		send_struct(struct pole); 		// Wysyła strukture do serwera 
int 		send_command(char command[10]);  	// Wysyła polecenie do serwera
static void 	error_handler( const char *on_what ); 	//Obsługa błędów
void 		polacz_sie(void);			// Nawiązanie połączenia z serwerem
struct pole 	type_coordinates(void);			// Mechanizm wprowadzania z klawiatury wsp. X i Y
int 		check_stat(void);			// Sprawdzanie czy Client nie wygrał gry
void 		read_command(void);
void 		move(void);
int 		end_of_game(void);

void fill_table(void){
	int i,j;
	for(i=0; i<3; i++){
		for(j=0; j<3; j++){
			tablica[i][j]='*';
		}
	}
}

void print_table(void){
	int i;
	    printf("Twoim przeciwnikiem jest: %s",opponent);
	    printf("\n\n       Jestes: \"");
	    color("\x1B[32m");
	    printf("O");
	    color_def();
	    printf("\"\n");
	    color("\x1B[36m");
		bold_on();
		printf("\n     1     2     3   \n");
		color_def();
		bold_off();
		printf("  |-----------------|\n");
	for(i=0; i<3; i++){
	    color("\x1B[35m");
		bold_on();
		printf("%d",(i+1));
		bold_off();
		color_def();
		printf(" |");
		if(tablica[0][i]!='*'){
			if(tablica[0][i]=='O') color("\x1B[32m");
			printf("  %c  ",tablica[0][i]); 
			color_def();
			printf("|");
		}	
		else printf("     |");
		
		if(tablica[1][i]!='*'){
			if(tablica[1][i]=='O') color("\x1B[32m");
			printf("  %c  ",tablica[1][i]);
			color_def();
			printf("|");
		}
		else printf("     |");
		
		if(tablica[2][i]!='*'){
			if(tablica[2][i]=='O') color("\x1B[32m");
			printf("  %c  ",tablica[2][i]);
			color_def();
			printf("|");
		}
		else printf("     |");
		printf("\n  |-----------------|\n");
	}
		printf("\n");
}

void bold_on(void){
	fprintf(stdout,"\x1B[1m");
}

void bold_off(void){
	fprintf(stdout,"\x1B[22m");
}

void color(char kolor[10]){
	fprintf(stdout,kolor);
}

void color_def(void){
	fprintf(stdout,"\x1B[39m");
}

void modify_table(struct pole rcv_str){
   tablica[rcv_str.x][rcv_str.y]=rcv_str.znak;
}

int send_struct(struct pole rcv_str){
	out_code=write(s_inet_srv, &rcv_str, sizeof(rcv_str) );
		if(out_code==-1) {
			printf("write()");
			return(0);
		}
		else return(1);
}

int send_command(char command[10]){
	out_code=write(s_inet_srv,command,strlen(command));
	if(out_code==-1) {
	    error_handler("write()");
		return(0);
	}
	else return(1);
}

static void error_handler( const char *on_what ){
  fputs( strerror(errno), stderr );
  fputs( ": ", stderr ); fputs( on_what, stderr );
  fputc( '\n', stderr ); exit( 1 );

}

void polacz_sie(void){
	s_inet_srv = socket( AF_INET, SOCK_STREAM, 0 );
           if ( s_inet_srv == -1 ) error_handler( "socket()" );
                       // Laczymy sie z serwerem:
                        out_code = connect( s_inet_srv, (struct sockaddr *)&adr_srv, adr_srv_len );
						if( out_code == -1 ) error_handler( "connect()" );
}

struct pole type_coordinates(void){
	int i=0,x,y,j=0;
	struct pole temporary;
	do{
		do{
		printf("Podaj wspolrzedna ");
		color("\x1B[36m");
		bold_on();
		printf("x: ");
		scanf("%d",&x);
		bold_off();
		color_def();
		printf("Podaj wspolrzedna ");
		color("\x1B[35m");
		bold_on();
		printf("y: ");
		scanf("%d",&y);
		bold_off();
		color_def();
		if(((x>=1) || (x<=3)) && ((y>=1) || (y<=3))) j=1;
		else printf("Wprowadzono zle dane,sprobuj ponownie");
		}
		while(j!=1);
	
	if(tablica[x-1][y-1]=='*'){
			temporary.x = x-1;
			temporary.y = y-1;
			temporary.znak = 'O';
			i=1;
			return(temporary);
		}
		else{
			printf("Nie mozna wprowadzic w to miejsce! Sprobuj ponownie!\n");
	
	}
	}
	while(i==0);
}

int check_stat(void){
	int i,w;
	w=0;
	if((tablica[0][0]&tablica[1][1]&tablica[2][2])=='O') w=1; 
	if((tablica[2][0]&tablica[1][1]&tablica[0][2])=='O') w=1; 
	for(i=0; i<3; i++){
		if((tablica[i][0]&tablica[i][1]&tablica[i][2])=='O'){ w=1; break;}
		if((tablica[0][i]&tablica[1][i]&tablica[2][i])=='O'){ w=1; break;}
	}
	return(w);
}

void read_command(void){
	out_code = read(s_inet_srv,command,sizeof(command));
	command[out_code]='\0';
}

void move(void){
	if(!strcmp(who,"client")){
		color("\x1B[32m");
		printf("%s",nickname);
		color_def();
		printf(", Twoj ruch!\n");
	}
	if (!strcmp(who,"server")){
		printf("Czekaj, %s posiada teraz ruch.\n",opponent);
	}
	color_def();
}

int end_of_game(void){
	int i,j,k=1;
		for(i=0; i<3; i++){
			for(j=0; j<3; j++){
				if(tablica[i][j]=='*') {
				k=0;
				break;
				}
			}
		}
	if(k){
		return(k);
	}
}
