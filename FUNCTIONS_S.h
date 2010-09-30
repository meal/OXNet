#include <errno.h>


struct pole{						// Struktura przechowywująca dane: położenie x,y, oraz znak X lub O
	int x;
	int y;
	char znak;
};

char 		tablica[3][3];			// Glowna tablica do grania, 3x3, wypelniana *
char   		command[10];			// Zmienna do wysylania informajci do servera
char  		who[10];				// Zmienna do odczytywania od kogo jest ruch
char   		*srv_addr;		// Lancuch z adresem IP serwera
char  	        *srv_port;		// Lancuch z numerem portu serwera
char 		nickname[12];		// Nick graczja
char 		opponent[12];
struct 		sockaddr_in adr_srv;	// Adres serwera
struct		servent *service;		// Opis uslugi
struct	        pole rcv_str;			// RCV_STR przechowuje strukture x,y,znak
int    		adr_srv_len;			// Dlugosc adresu
int   		i, n;					// Zmienne pomocnicze
int    		out_code;				// Rezultat wykonania funkcji typu int
int    		s_cln_inet;				// Nr gniazda klienta
int			s_srv_inet;				// Nr gniazda serwera
int 		adr_srv_len, adr_cln_len;
struct 		sockaddr_in adr_srv,adr_cln;


void fill_table(void);							// Wypełnia całą TABLICE gwiazdkami(*) 
void print_table(void);							// Wyświetla tablicę bez gwiazdek, ale z X i O
void bold_on(void);								// Włączenie pogrubienia czcionki
void bold_off(void);							// Wyłączenie pogrubienia czcionki
void color(char kolor[10]);						// Zmiana koloru na podany w nawiasie
void color_def(void);							// Zmiana koloru na kolor podstawowy
void modify_table(struct pole rcv_str);			// Wprowadza to TABLICY podane w strukturze wartości
int send_struct(struct pole); 					// Wysyła strukture do serwera 
int send_command(char command[10]);  			// Wysyła polecenie do serwera
static void error_handler( const char *on_what ); //Obsługa błędów
void acceptance(void);							// Nawiązanie połączenia z serwerem
struct pole type_coordinates(void);				// Mechanizm wprowadzania z klawiatury wsp. X i Y
int check_stat(void);							// Sprawdzanie czy Client nie wygrał gry
void losowanie(void);
void read_command(void);
void move(void);
int end_of_game(void);

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
	color("\x1B[33m");
	printf("X");
	color_def();
	printf("\"\n\n");
	color("\x1B[36m");
		bold_on();
		printf("     1     2     3   \n");
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
			if(tablica[0][i]=='X') color("\x1B[33m");
		       	printf("  %c  ",tablica[0][i]);
			color_def();
			printf("|");
		}
		else printf("     |");
		
		if(tablica[1][i]!='*'){
			if(tablica[1][i]=='X') color("\x1B[33m");
			printf("  %c  ",tablica[1][i]);
			color_def();
			printf("|");
		}
		else printf("     |");

		if(tablica[2][i]!='*'){
		 	if(tablica[2][i]=='X') color("\x1B[33m");      
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
	fprintf(stdout,"\x1B[37m");
}

void modify_table(struct pole rcv_str){
   tablica[rcv_str.x][rcv_str.y]=rcv_str.znak;
}

int send_struct(struct pole rcv_str){
	out_code=write(s_cln_inet, &rcv_str, sizeof(rcv_str) );
		if(out_code==-1) {
			printf("write()");
			return(0);
		}
		else return(1);
}

int send_command(char command[10]){
	out_code=write(s_cln_inet,command,strlen(command));
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

void acceptance(void){
	adr_cln_len = sizeof( adr_cln );
	s_cln_inet = accept( s_srv_inet, (struct sockaddr *)&adr_cln,
	                            &adr_cln_len );
	if ( s_cln_inet == -1 ) error_handler( "accept()" );
}

struct pole type_coordinates(void){
	int i=0,x,y,w=0;
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
		if(((x>=1) || (x<=3)) && ((y>=1) || (y<=3))) w=1;
		else printf("Wprowadzono zle dane. Wpisz ponownie!");
	   }
	   while(w!=1);
		if(tablica[x-1][y-1]=='*'){
			temporary.x = x-1;
			temporary.y = y-1;
			temporary.znak = 'X';
			i=1;
			return(temporary);
		}
		else{
			printf("\nNie mozna wprowadzic w to miejsce! Sprobuj ponownie!\n");
	
	}
	}
	while(i==0);
	color_def();
}

int check_stat(void){
	int i,w;
	w=0;
	if((tablica[0][0]&tablica[1][1]&tablica[2][2])=='X') w=1; 
	if((tablica[2][0]&tablica[1][1]&tablica[0][2])=='X') w=1; 
	for(i=0; i<3; i++){
		if((tablica[i][0]&tablica[i][1]&tablica[i][2])=='X'){ w=1; break;}
		if((tablica[0][i]&tablica[1][i]&tablica[2][i])=='X'){ w=1; break;}
	}
	return(w);
}

void losowanie(void){
	srand(time(0));
	if(((rand()%20)%2) == 0){
		strcpy(who,"server");	
	}
	else{
		strcpy(who,"client");
	}

}

void read_command(void){
	out_code = read(s_cln_inet,command,sizeof(command));
	command[out_code]='\0';
}

void move(void){
     if(!strcmp(who,"server")) {
	color("\x1B[33m");
	printf("%s",nickname);
	color_def();
	printf(", Twoj ruch!\n",nickname);
     }
     else if(!strcmp(who,"client")){
	printf("Czekaj, %s posiada teraz ruch\n",opponent);
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
		return(1);
	}
}
