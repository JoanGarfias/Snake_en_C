#include <windows.h>
#include <stdio.h>
#include "serpiente.h"
#include <time.h>
#include "estructuras.h" //Ctrl + Click para ver los TDA.

#define TAMSERP 18 //Tamanio recomendado : 15 -> 23

#define CUERPO 	1
#define CABEZA 	2
#define COLA 	3

#define IZQ 	1
#define DER 	2
#define ARRIBA	3
#define ABAJO 	4

#define CRECE	1
#define ACHICA	2
#define NADA	3
#define MATA	4

#define IDT_TIMER1 1

#define TAMNOM 25 		//Cubos necesarios para el nombre
int state = 0;		 	//Esta variable sirve para conocer si el usuario ha muerto.
int levelUPDW = 0;	 	//Saber si el enemigo esta en un estado para subir o bajar de nivel
int windowMinState = 1; //Saber si se maximiza o minimiza la ventana del juego
PEDACITOS *snakeBackup = NULL;

//Variable para personalizacion
PERSZ PERS;
void setColorPers(PERSZ *, int, int, int, int);
void setCheckMenu(HMENU, int);

//Variables contadoras para estadisticas
STATS stats; 
int racha = 0;
int nivelAct  = 1;
int nivelValue = 100;

COMIDA com = {{0,0},NADA};

/*Funciones de Serpiente, Enemigo y Letras*/
PEDACITOS * NuevaSerpiente(int *);
void DibujarSerpiente(HDC, const PEDACITOS *);
void DibujarNombre(HDC, const LETRA *);
void DibujarEnemigo(HDC, const ENEMY *);
LETRA * CrearNombre(RECT);
ENEMY * CrearEnemigo(RECT);
void ModificarNombre(LETRA *, RECT);

/*Funciones de movimiento, modificacion y verificacion de elementos (serpiente, letras, enemigos)*/
int MoverEnemigo(ENEMY *, int, RECT);
int MoverSerpiente(PEDACITOS *, int, RECT, int, LETRA *, ENEMY *);
PEDACITOS * AjustarSerpiente (PEDACITOS *, int *, int, RECT, HWND);
ENEMY * AjustarEnemigo(ENEMY*, RECT);
int Colisionar(PEDACITOS *, int, const LETRA *, ENEMY *, PEDACITOS *);
int Comer(const PEDACITOS *, int);
ENEMY *AjustarEnemigo(ENEMY *, RECT);

/*ScoreBoard*/
char statsBoard[32]; //Racha
char statsBoard2[32]; //Nivel
char nombre[32];

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rect;
	static PEDACITOS *serpiente = NULL;
	static ENEMY *enemigo = NULL;
	static int cuenta = 0;
	static LETRA *letras = NULL;
	static int tams = 5;
	HMENU hMenu = GetMenu(hwnd);
	
	int i=0;
	int wmId = LOWORD(wParam);
	int wmEvent = HIWORD(wParam);	
	
	switch(Message) {
		case WM_CREATE:{
			GetClientRect(hwnd,&rect);
			serpiente = NuevaSerpiente(&tams);
			enemigo = CrearEnemigo(rect);
			letras = CrearNombre(rect);
			SetTimer(hwnd, IDT_TIMER1, 100, NULL);
			FILE *estadisticas = fopen("Data\\stats.txt","r");
				fscanf(estadisticas,"%d %d %d %d %d",&stats.racha, &stats.jugadas, &stats.tamanio, &stats.autocolision, &stats.colision);
			fclose(estadisticas);
			switch(PERS.temaId){
				case IDM_MODO_C:		CheckMenuItem(hMenu, IDM_MODO_C, MF_CHECKED);		break;
				case IDM_MODO_O:		CheckMenuItem(hMenu, IDM_MODO_O, MF_CHECKED);		break;
				case IDM_MODO_A:		CheckMenuItem(hMenu, IDM_MODO_A, MF_CHECKED);		break;
			}
			switch(PERS.cuerpoId){
				case PS_CUERPO_BLANCO:	CheckMenuItem(hMenu, PS_CUERPO_BLANCO, MF_CHECKED);		break;
				case PS_CUERPO_NEGRO:	CheckMenuItem(hMenu, PS_CUERPO_NEGRO, MF_CHECKED);		break;
				case PS_CUERPO_AZUL:	CheckMenuItem(hMenu, PS_CUERPO_AZUL, MF_CHECKED);		break;
				case PS_CUERPO_ROJO:	CheckMenuItem(hMenu, PS_CUERPO_ROJO, MF_CHECKED);		break;
				case PS_CUERPO_VERDE:	CheckMenuItem(hMenu, PS_CUERPO_VERDE, MF_CHECKED);		break;
				case PS_CUERPO_AMARILLO:CheckMenuItem(hMenu, PS_CUERPO_AMARILLO, MF_CHECKED);	break;
				case PS_CUERPO_MORADO:	CheckMenuItem(hMenu, PS_CUERPO_MORADO, MF_CHECKED);		break;
				break; 
			}
			switch(PERS.colaId){
				case PS_COLA_BLANCO:	CheckMenuItem(hMenu, PS_COLA_BLANCO, MF_CHECKED);		break;
				case PS_COLA_NEGRO:		CheckMenuItem(hMenu, PS_COLA_NEGRO, MF_CHECKED);		break;
				case PS_COLA_AZUL:		CheckMenuItem(hMenu, PS_COLA_AZUL, MF_CHECKED);			break;
				case PS_COLA_ROJO:		CheckMenuItem(hMenu, PS_COLA_ROJO, MF_CHECKED);			break;
				case PS_COLA_VERDE:		CheckMenuItem(hMenu, PS_COLA_VERDE, MF_CHECKED);		break;
				case PS_COLA_AMARILLO:	CheckMenuItem(hMenu, PS_COLA_AMARILLO, MF_CHECKED);		break;
				case PS_COLA_MORADO:	CheckMenuItem(hMenu, PS_COLA_MORADO, MF_CHECKED);		break;
				break; 
			}
			switch(PERS.lenguaId){
				case PS_LENGUA_BLANCO:		CheckMenuItem(hMenu, PS_LENGUA_BLANCO, MF_CHECKED);		break;
				case PS_LENGUA_NEGRO:		CheckMenuItem(hMenu, PS_LENGUA_NEGRO, MF_CHECKED);		break;
				case PS_LENGUA_AZUL:		CheckMenuItem(hMenu, PS_LENGUA_AZUL, MF_CHECKED);		break;
				case PS_LENGUA_ROJO:		CheckMenuItem(hMenu, PS_LENGUA_ROJO, MF_CHECKED);		break;
				case PS_LENGUA_VERDE:		CheckMenuItem(hMenu, PS_LENGUA_VERDE, MF_CHECKED);		break;
				case PS_LENGUA_AMARILLO:	CheckMenuItem(hMenu, PS_LENGUA_AMARILLO, MF_CHECKED);	break;
				case PS_LENGUA_MORADO:		CheckMenuItem(hMenu, PS_LENGUA_MORADO, MF_CHECKED);		break;
				break; 
			}
			switch(PERS.enemigoId){
				case PS_ENEMIGO_VERDEBOSQUE:	CheckMenuItem(hMenu, PS_ENEMIGO_VERDEBOSQUE, MF_CHECKED);		break;
				case PS_ENEMIGO_NARANJA:		CheckMenuItem(hMenu, PS_ENEMIGO_NARANJA, MF_CHECKED);			break;
				case PS_ENEMIGO_MARRON:			CheckMenuItem(hMenu, PS_ENEMIGO_MARRON, MF_CHECKED);			break;
				case PS_ENEMIGO_VIOLETA:		CheckMenuItem(hMenu, PS_ENEMIGO_VIOLETA, MF_CHECKED);			break;
				break; 
			}
			switch(PERS.cabezaId){
				case PS_CABEZA_BLANCO:		CheckMenuItem(hMenu, PS_CABEZA_BLANCO, MF_CHECKED);		break;
				case PS_CABEZA_NEGRO:		CheckMenuItem(hMenu, PS_CABEZA_NEGRO, MF_CHECKED);		break;
				case PS_CABEZA_AZUL:		CheckMenuItem(hMenu, PS_CABEZA_AZUL, MF_CHECKED);			break;
				case PS_CABEZA_ROJO:		CheckMenuItem(hMenu, PS_CABEZA_ROJO, MF_CHECKED);			break;
				case PS_CABEZA_VERDE:		CheckMenuItem(hMenu, PS_CABEZA_VERDE, MF_CHECKED);		break;
				case PS_CABEZA_AMARILLO:	CheckMenuItem(hMenu, PS_CABEZA_AMARILLO, MF_CHECKED);		break;
				case PS_CABEZA_MORADO:		CheckMenuItem(hMenu, PS_CABEZA_MORADO, MF_CHECKED);		break;
				break; 
			}
			break;
		}
		
		case WM_SIZE:{
				GetClientRect(hwnd,&rect);
				if(wParam == SIZE_RESTORED){
					SetTimer(hwnd, IDT_TIMER1, nivelValue, NULL);
					InvalidateRect(hwnd,NULL,TRUE);
					windowMinState = 1;
				}
				else{
					if (wParam == SIZE_MINIMIZED){
						windowMinState = 1;
						if(rect.bottom <= 0 || rect.right <= 0){
							KillTimer(hwnd, IDT_TIMER1);
						}
					}
					else if(wParam == SIZE_MAXIMIZED){
						windowMinState=0;
						if(rect.bottom <= 0 || rect.right <= 0){
							KillTimer(hwnd, IDT_TIMER1);
						}
						else{
							ModificarNombre(letras, rect);
							InvalidateRect(hwnd,NULL,TRUE);	
						}
					}	
				}
				ModificarNombre(letras, rect);
			break;
		}
		
		case WM_COMMAND:{
			switch(wmId){
				case IDM_NUEVO:{
					if(serpiente != NULL){
						KillTimer(hwnd,IDT_TIMER1);
						free(serpiente);
						free(enemigo);
						free(letras);
						tams = 5;
						cuenta = 0;
						state = 0;
						nivelAct = 1;
						racha = 0;
						nivelValue = 100;
						FILE *datos = fopen("Data\\datos.txt","w");
						fclose(datos);
						GetClientRect(hwnd,&rect);
						serpiente = NuevaSerpiente(&tams);
						letras = CrearNombre(rect);
						enemigo = CrearEnemigo(rect);
						SetTimer(hwnd, IDT_TIMER1, nivelValue, NULL);
						InvalidateRect(hwnd,NULL,TRUE); /*Funcion para volver a pintar*/
					}
					break;
				}
				case IDM_SALIR:{
					DestroyWindow(hwnd);
					break;
				}
				
				case IDM_INFO:{
					char mensaje[256];
					sprintf(mensaje, "Alumno: Joan Pablo Alvarado Garfias \n Materia: Programacion Estructurada \n Carrera: Computacion \n Profesor: J. Arellano Pimentel");
					KillTimer(hwnd,IDT_TIMER1);
					MessageBox(hwnd,mensaje,"Datos:", MB_OK | MB_ICONINFORMATION);
					SetTimer(hwnd, IDT_TIMER1, nivelValue, NULL);
					break;
				}
				case IDM_MODO_C:{	PERS.temaId = IDM_MODO_C;
					setColorPers(&PERS, 255, 255, 255, IDM_MODO_C);
					setCheckMenu(hMenu, IDM_MODO_C);
					break;
				}
				case IDM_MODO_O:{	PERS.temaId = IDM_MODO_O;
					setColorPers(&PERS, 25, 25, 25, IDM_MODO_C);
					setCheckMenu(hMenu, IDM_MODO_O);
					break;
				}
				case IDM_MODO_A:{	PERS.temaId = IDM_MODO_A;
					setColorPers(&PERS, 0, 96, 100, IDM_MODO_C);
					setCheckMenu(hMenu, IDM_MODO_A);
					break;
				}
				case PS_CUERPO_BLANCO:{
					setColorPers(&PERS, 255, 255, 255, PS_CUERPO_BLANCO);
					setCheckMenu(hMenu, PS_CUERPO_BLANCO);
					break;
				}
				case PS_CUERPO_NEGRO:{
					setColorPers(&PERS, 0, 0, 0, PS_CUERPO_BLANCO);
					setCheckMenu(hMenu, PS_CUERPO_NEGRO);
					break;
				}
				case PS_CUERPO_AZUL:{	PERS.cuerpoId=PS_CUERPO_AZUL;
					setColorPers(&PERS, 0, 0, 255, PS_CUERPO_BLANCO);
					setCheckMenu(hMenu, PS_CUERPO_AZUL);
					break;
				}
				case PS_CUERPO_ROJO:{	PERS.cuerpoId=PS_CUERPO_ROJO;
					setColorPers(&PERS, 162, 40, 42, PS_CUERPO_BLANCO);
					setCheckMenu(hMenu, PS_CUERPO_ROJO);
					break;
				}
				case PS_CUERPO_VERDE:{	PERS.cuerpoId=PS_CUERPO_VERDE;
					setColorPers(&PERS, 34, 139, 34, PS_CUERPO_BLANCO);
					setCheckMenu(hMenu, PS_CUERPO_VERDE);
					break;
				}
				case PS_CUERPO_AMARILLO:{	PERS.cuerpoId=PS_CUERPO_AMARILLO;
					setColorPers(&PERS, 218, 165, 32, PS_CUERPO_BLANCO);
					setCheckMenu(hMenu, PS_CUERPO_AMARILLO);
					break;
				}
				case PS_CUERPO_MORADO:{	PERS.cuerpoId=PS_CUERPO_MORADO;
					setColorPers(&PERS, 153, 50, 204, PS_CUERPO_BLANCO);
					setCheckMenu(hMenu, PS_CUERPO_MORADO);
					break;
				}
				case PS_COLA_BLANCO:{	PERS.colaId=PS_COLA_BLANCO;
					setColorPers(&PERS, 255, 255, 255, PS_COLA_BLANCO);
					setCheckMenu(hMenu, PS_COLA_BLANCO);
					break;
				}
				case PS_COLA_NEGRO:{	PERS.colaId=PS_COLA_NEGRO;
					setColorPers(&PERS, 0, 0, 0, PS_COLA_BLANCO);
					setCheckMenu(hMenu, PS_COLA_NEGRO);
					break;
				}
				case PS_COLA_AZUL:{		PERS.colaId=PS_COLA_AZUL;
					setColorPers(&PERS, 0, 0, 255, PS_COLA_BLANCO);
					setCheckMenu(hMenu, PS_COLA_AZUL);
					break;
				}
				case PS_COLA_ROJO:{		PERS.colaId=PS_COLA_ROJO;
					setColorPers(&PERS, 162, 40, 42, PS_COLA_BLANCO);
					setCheckMenu(hMenu, PS_COLA_ROJO);
					break;
				}
				case PS_COLA_VERDE:{	PERS.colaId=PS_COLA_VERDE;
					setColorPers(&PERS, 34, 139, 34, PS_COLA_BLANCO);
					setCheckMenu(hMenu, PS_COLA_VERDE);
					break;
				}
				case PS_COLA_AMARILLO:{	PERS.colaId=PS_COLA_AMARILLO;
					setColorPers(&PERS, 218, 165, 32, PS_COLA_BLANCO);
					setCheckMenu(hMenu, PS_COLA_AMARILLO);
					break;
				}
				case PS_COLA_MORADO:{	PERS.colaId=PS_COLA_MORADO;
					setColorPers(&PERS, 153, 50, 204, PS_COLA_BLANCO);
					setCheckMenu(hMenu, PS_COLA_MORADO);
					break;
				}
				case PS_LENGUA_BLANCO:{	PERS.lenguaId=PS_LENGUA_BLANCO;
					setColorPers(&PERS, 255, 255, 255, PS_LENGUA_BLANCO);
					setCheckMenu(hMenu, PS_LENGUA_BLANCO);
					break;
				}
				case PS_LENGUA_NEGRO:{	PERS.lenguaId=PS_LENGUA_NEGRO;
					setColorPers(&PERS, 0, 0, 0, PS_LENGUA_BLANCO);
					setCheckMenu(hMenu, PS_LENGUA_NEGRO);
					break;
				}
				case PS_LENGUA_AZUL:{	PERS.lenguaId=PS_LENGUA_AZUL;
					setColorPers(&PERS, 0, 0, 255, PS_LENGUA_BLANCO);
					setCheckMenu(hMenu, PS_LENGUA_AZUL);
					break;
				}
				case PS_LENGUA_ROJO:{	PERS.lenguaId=PS_LENGUA_ROJO;
					setColorPers(&PERS, 162, 40, 42, PS_LENGUA_BLANCO);
					setCheckMenu(hMenu, PS_LENGUA_ROJO);
					break;
				}
				case PS_LENGUA_VERDE:{	PERS.lenguaId=PS_LENGUA_VERDE;
					setColorPers(&PERS, 34, 139, 34, PS_LENGUA_BLANCO);
					setCheckMenu(hMenu, PS_LENGUA_VERDE);
					break;
				}
				case PS_LENGUA_AMARILLO:{	PERS.lenguaId=PS_LENGUA_AMARILLO;
					setColorPers(&PERS, 218, 165, 32, PS_LENGUA_BLANCO);
					setCheckMenu(hMenu, PS_LENGUA_AMARILLO);
					break;
				}
				case PS_LENGUA_MORADO:{	PERS.lenguaId=PS_LENGUA_MORADO;
					setColorPers(&PERS, 153, 50, 204, PS_LENGUA_BLANCO);
					setCheckMenu(hMenu, PS_LENGUA_MORADO);
					break;
				}
				case PS_CABEZA_BLANCO:{	PERS.cabezaId=PS_CABEZA_BLANCO;
					setColorPers(&PERS, 255, 255, 255, PS_CABEZA_BLANCO);
					setCheckMenu(hMenu, PS_CABEZA_BLANCO);
					break;
				}
				case PS_CABEZA_NEGRO:{	PERS.cabezaId=PS_CABEZA_NEGRO;
					setColorPers(&PERS, 0, 0, 0, PS_CABEZA_BLANCO);
					setCheckMenu(hMenu, PS_CABEZA_NEGRO);
					break;
				}
				case PS_CABEZA_AZUL:{	PERS.cabezaId=PS_CABEZA_AZUL;
					setColorPers(&PERS, 0, 0, 255, PS_CABEZA_BLANCO);
					setCheckMenu(hMenu, PS_CABEZA_AZUL);
					break;
				}
				case PS_CABEZA_ROJO:{	PERS.cabezaId=PS_CABEZA_ROJO;
					setColorPers(&PERS, 162, 40, 42, PS_CABEZA_BLANCO);
					setCheckMenu(hMenu, PS_CABEZA_ROJO);
					break;
				}
				case PS_CABEZA_VERDE:{	PERS.cabezaId=PS_CABEZA_VERDE;
					setColorPers(&PERS, 34, 139, 34, PS_CABEZA_BLANCO);
					setCheckMenu(hMenu, PS_CABEZA_VERDE);
					break;
				}
				case PS_CABEZA_AMARILLO:{	PERS.cabezaId=PS_CABEZA_AMARILLO;
					setColorPers(&PERS, 218, 165, 32, PS_CABEZA_BLANCO);
					setCheckMenu(hMenu, PS_CABEZA_AMARILLO);
					break;
				}
				case PS_CABEZA_MORADO:{	PERS.cabezaId=PS_CABEZA_MORADO;
					setColorPers(&PERS, 153, 50, 204, PS_CABEZA_BLANCO);
					setCheckMenu(hMenu, PS_CABEZA_MORADO);
					break;
				}
				
				case PS_ENEMIGO_VERDEBOSQUE:{ PERS.enemigoId=PS_ENEMIGO_VERDEBOSQUE;
					setColorPers(&PERS, 46, 139, 87, PS_ENEMIGO_VERDEBOSQUE);
					setCheckMenu(hMenu, PS_ENEMIGO_VERDEBOSQUE);
					break;
				}
				case PS_ENEMIGO_NARANJA:{	PERS.enemigoId=PS_ENEMIGO_NARANJA;
					setColorPers(&PERS, 255, 140, 0, PS_ENEMIGO_VERDEBOSQUE);
					setCheckMenu(hMenu, PS_ENEMIGO_NARANJA);
					break;
				}
				case PS_ENEMIGO_MARRON:{	PERS.enemigoId=PS_ENEMIGO_MARRON;
					setColorPers(&PERS, 128, 0, 0, PS_ENEMIGO_VERDEBOSQUE);
					setCheckMenu(hMenu, PS_ENEMIGO_MARRON);
					break;
				}
				case PS_ENEMIGO_VIOLETA:{	PERS.enemigoId=PS_ENEMIGO_VIOLETA;
					setColorPers(&PERS, 106, 90, 205, PS_ENEMIGO_VERDEBOSQUE);
					setCheckMenu(hMenu, PS_ENEMIGO_VIOLETA);
					break;
				}
				case IDM_STATS:{
					KillTimer(hwnd,IDT_TIMER1);
					char mensaje[256];
					sprintf(mensaje, "Partidas jugadas: %d\nRacha maxima: %d\nTamanio maximo: %d\nAutocolisiones: %d\nColisiones: %d\nNivel Actual: %d", 
							stats.jugadas, stats.racha, stats.tamanio, stats.autocolision, stats.colision,nivelAct);
					int value = MessageBox(hwnd, mensaje, "Estadisticas del juego", MB_OK | MB_ICONINFORMATION);
					if(value == IDOK || value == IDCANCEL){
						SetTimer(hwnd, IDT_TIMER1, nivelValue, NULL);
					}
					break;
				}
				default:
					return DefWindowProc(hwnd,Message,wParam,lParam);
			}
			break;
		}
		
		case WM_TIMER:{
			switch(wParam){
				case IDT_TIMER1:{
					GetClientRect(hwnd,&rect);
					levelUPDW=0;
					if(!state){
						MoverEnemigo(enemigo, tams, rect);
						if(!MoverSerpiente(serpiente, serpiente[tams-1].dir, rect, tams, letras, enemigo) || state){
							InvalidateRect(hwnd,NULL,TRUE);
							KillTimer(hwnd, IDT_TIMER1);
							int selectionBox = MessageBox(hwnd,"Se murio", "Desea seguir jugando?", MB_YESNO |  MB_ICONQUESTION);
							if(selectionBox == IDYES){
								if(serpiente != NULL){
									//pthread_cancel(music);			pthread_t music;	
									//int status = pthread_create(&music, NULL,MusicaFondo, (void *)"Sounds\\Can_Fondo.wav");
									KillTimer(hwnd,IDT_TIMER1);				free(serpiente);
									tams = 5;				cuenta = 0;		state = 0;
									nivelAct = 1;			racha = 0;		nivelValue = 100;
									FILE *datos = fopen("Data\\datos.txt","w");		fclose(datos);
									serpiente = NuevaSerpiente(&tams);
									enemigo = CrearEnemigo(rect);
									SetTimer(hwnd, IDT_TIMER1, nivelValue, NULL);
									InvalidateRect(hwnd,NULL,TRUE); /*Funcion para volver a pintar*/
								}
							}
							else{
								DestroyWindow(hwnd);
							}
						}
					}
					
					cuenta++; /*Cada 15 unidades de tiempo se aparece una comida de tipo aletorio*/
					int coolDown = (nivelAct==1)? 30: (nivelAct==2)? 35 : (nivelAct==3)? 40: (nivelAct==4)? 50 : (nivelAct==5)? 65 : 70;
					int prob = rand()%100;
					if(cuenta == coolDown){
						if(prob <= 80){
							com.tipo = CRECE;
						}
						else if(prob>=81 && prob<=89){
							com.tipo = ACHICA;
						}
						else if(prob>=90){
							com.tipo = MATA;
						}
						i=0;
						com.pos.x = rand()% rect.right / TAMSERP;
						com.pos.y = rand()% rect.bottom / TAMSERP;
						while(i<tams){
							if(com.pos.x == serpiente[i].pos.x && com.pos.y == serpiente[i].pos.y){
								com.pos.x = rand()% (rect.right-15) / TAMSERP;
								com.pos.y = rand()% (rect.bottom-15) / TAMSERP;
								i=0;
							}
							i++;
						}
						i=0;
						while(i<=TAMNOM){
							if((com.pos.x == letras[i].pos.x && com.pos.y == letras[i].pos.y) || (com.pos.x == 9 && com.pos.y == 4)){
								com.pos.x = rand()% (rect.right-15) / TAMSERP;
								com.pos.y = rand()% (rect.bottom-15) / TAMSERP;
								i=0;
							}
							i++;
						}
						cuenta = 0;
					}
					if(Comer(serpiente, tams)){
						serpiente = AjustarSerpiente(serpiente,&tams,com.tipo, rect, hwnd);
						if(tams<2){
							state=1;
							KillTimer(hwnd, IDT_TIMER1);
							int selectionBox = MessageBox(hwnd,"Se murio", "Desea seguir jugando?", MB_YESNO | MB_ICONQUESTION);
								if(selectionBox == IDYES){
									if(serpiente != NULL){
										//pthread_cancel(music);			pthread_t music;	
										//int status = pthread_create(&music, NULL,MusicaFondo, (void *)"Sounds\\Can_Fondo.wav");
										KillTimer(hwnd,IDT_TIMER1);				free(serpiente);
										tams = 5;				cuenta = 0;		state = 0;
										nivelAct = 1;			racha = 0;		nivelValue = 100;
										FILE *datos = fopen("Data\\datos.txt","w");		fclose(datos);
										serpiente = NuevaSerpiente(&tams);
										enemigo = CrearEnemigo(rect);
										SetTimer(hwnd, IDT_TIMER1, nivelValue, NULL);
										InvalidateRect(hwnd,NULL,TRUE); /*Funcion para volver a pintar*/
									}
								}
								else{
									DestroyWindow(hwnd);
								}
						}
						if(levelUPDW){
							SetTimer(hwnd, IDT_TIMER1, nivelValue, NULL);
							enemigo = AjustarEnemigo(enemigo, rect);
							InvalidateRect(hwnd,NULL,TRUE);
							levelUPDW = 0;
						}
						com.tipo = NADA;
					}
					if(!state) InvalidateRect(hwnd,NULL,TRUE);
					break;
				}
			}
			break;
		}
		case WM_KEYDOWN:{
			GetClientRect(hwnd,&rect);
			int TYPE = (wParam==VK_UP)? ARRIBA : (wParam==VK_DOWN)? ABAJO : (wParam==VK_LEFT)? IZQ : DER;
			if(!state){
				if(!MoverSerpiente(serpiente, TYPE, rect, tams,letras, enemigo)){
					InvalidateRect(hwnd,NULL,TRUE);
					KillTimer(hwnd, IDT_TIMER1);
					int selectionBox = MessageBox(hwnd,"Se murio", "Desea seguir jugando",  MB_YESNO | MB_ICONQUESTION);
						if(selectionBox == IDYES){
							if(serpiente != NULL){
								KillTimer(hwnd,IDT_TIMER1);				free(serpiente);
								tams = 5;				cuenta = 0;		state = 0;
								nivelAct = 1;			racha = 0;		nivelValue = 100;
								FILE *datos = fopen("Data\\datos.txt","w");		fclose(datos);
								serpiente = NuevaSerpiente(&tams);
								enemigo = CrearEnemigo(rect);
								SetTimer(hwnd, IDT_TIMER1, nivelValue, NULL);
								InvalidateRect(hwnd,NULL,TRUE); /*Funcion para volver a pintar*/
							}
						}
						else{
							DestroyWindow(hwnd);
						}
				}
			}
			if(Comer(serpiente, tams)){
				serpiente = AjustarSerpiente(serpiente,&tams,com.tipo, rect, hwnd);
				enemigo = AjustarEnemigo(enemigo, rect);
				if(tams==1){
					KillTimer(hwnd, IDT_TIMER1);
					state = 1;
					int selectionBox = MessageBox(hwnd,"Se murio", "Desea seguir jugando",  MB_YESNO | MB_ICONQUESTION);
						if(selectionBox == IDYES){
							if(serpiente != NULL){
								KillTimer(hwnd,IDT_TIMER1);				free(serpiente);
								tams = 5;				cuenta = 0;		state = 0;
								nivelAct = 1;			racha = 0;		nivelValue = 100;
								FILE *datos = fopen("Data\\datos.txt","w");		fclose(datos);
								serpiente = NuevaSerpiente(&tams);
								enemigo = CrearEnemigo(rect);
								SetTimer(hwnd, IDT_TIMER1, nivelValue, NULL);
								InvalidateRect(hwnd,NULL,TRUE); /*Funcion para volver a pintar*/
							}
						}
						else{
							DestroyWindow(hwnd);
						}
				}
				com.tipo = NADA;
			}
			if(!state) InvalidateRect(hwnd,NULL,TRUE);
			
			break;
		}
		case WM_PAINT:{
			hdc = BeginPaint(hwnd, &ps);
			
			GetClientRect(hwnd, &rect);
			if(PERS.temaId==IDM_MODO_C){
				SetBkColor(hdc, RGB(255, 255, 255));
				SetTextColor(hdc, RGB(0, 0, 0));
			}
			else if (PERS.temaId==IDM_MODO_O){
				SetBkColor(hdc, RGB(47, 77, 77));
				SetTextColor(hdc, RGB(237, 255,33));
			}
			else if(PERS.temaId==IDM_MODO_A){
				SetBkColor(hdc, RGB(25, 25, 112));
				SetTextColor(hdc, RGB(255, 255,33));
			}
			HFONT hfont = CreateFont(28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Comic Sans MS");
			SelectObject(hdc, hfont);
			SetBkMode(hdc, OPAQUE);
			sprintf(statsBoard, "Racha: %d",racha);
			sprintf(statsBoard2,"Nivel %d",nivelAct);
			TextOut(hdc, rect.right-5*TAMSERP, rect.bottom/TAMSERP,statsBoard,strlen(statsBoard));
			TextOut(hdc, rect.right-5*TAMSERP, rect.bottom/TAMSERP+TAMSERP+TAMSERP/2,statsBoard2,strlen(statsBoard2));
			DibujarEnemigo(hdc, enemigo);
			DibujarSerpiente(hdc, serpiente);
			DibujarNombre(hdc, letras);

			if(com.tipo == CRECE){
				SetDCBrushColor(hdc, RGB(0, 255, 0));
				HBRUSH hBrush = CreateSolidBrush(GetDCBrushColor(hdc));
				HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
				RoundRect(hdc, com.pos.x * TAMSERP, com.pos.y * TAMSERP, 
							com.pos.x * TAMSERP + TAMSERP, com.pos.y * TAMSERP + TAMSERP, 7, 7);
				SelectObject(hdc, hOldBrush);
			}
			else if(com.tipo == ACHICA){
				SetDCBrushColor(hdc, RGB(146, 43, 33));
				HBRUSH hBrush = CreateSolidBrush(GetDCBrushColor(hdc));
				HBRUSH hOldBrush;
				hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
				Ellipse(hdc, com.pos.x * TAMSERP, com.pos.y * TAMSERP, 
							 com.pos.x * TAMSERP + TAMSERP, com.pos.y * TAMSERP + TAMSERP);
				SelectObject(hdc, hOldBrush);
			}
			else if(com.tipo == MATA){
				HPEN hPen, hOldPen;
				hPen = CreatePen(PS_SOLID, 3, RGB(139, 0, 0));
				hOldPen = SelectObject(hdc, hPen);
				MoveToEx(hdc, com.pos.x * TAMSERP, com.pos.y * TAMSERP, NULL);
				LineTo(hdc, com.pos.x * TAMSERP + TAMSERP, com.pos.y * TAMSERP + TAMSERP);
				MoveToEx(hdc, com.pos.x * TAMSERP + TAMSERP, com.pos.y * TAMSERP, NULL);
				LineTo(hdc, com.pos.x * TAMSERP, com.pos.y * TAMSERP + TAMSERP);
				SelectObject(hdc, hOldPen);
				DeleteObject(hPen);
			}
        	EndPaint(hwnd, &ps);
			break;
		}
		
		case WM_ERASEBKGND:{
			HDC hdc = (HDC) wParam;
			HBRUSH pincel = CreateSolidBrush(RGB(PERS.tema[0], PERS.tema[1], PERS.tema[2]));
			RECT rect;
        	GetClientRect(hwnd, &rect);
        	FillRect(hdc, &rect, pincel);
        	DeleteObject(pincel);
        	return 1;
			break;
		}
		case WM_DESTROY: { 
			GetClientRect(hwnd, &rect);
			
			FILE *perfil = fopen("Data\\perfil.txt","w"); //Guardando la personalizacion del jugador
				fprintf(perfil,"%d\n%d %d %d",PERS.temaId,PERS.tema[0],PERS.tema[1],PERS.tema[2]);
				fprintf(perfil,"\n%d\n%d %d %d",PERS.cuerpoId,PERS.cuerpo[0],PERS.cuerpo[1],PERS.cuerpo[2]);
				fprintf(perfil,"\n%d\n%d %d %d",PERS.colaId,PERS.cola[0],PERS.cola[1],PERS.cola[2]);
				fprintf(perfil,"\n%d\n%d %d %d",PERS.lenguaId,PERS.lengua[0],PERS.lengua[1],PERS.lengua[2]);
				fprintf(perfil,"\n%d\n%d %d %d",PERS.enemigoId,PERS.enemigo[0],PERS.enemigo[1],PERS.enemigo[2]);
				fprintf(perfil,"\n%d\n%d %d %d",PERS.cabezaId,PERS.cabeza[0],PERS.cabeza[1],PERS.cabeza[2]);
			fclose(perfil);
			FILE* datos = fopen("Data\\datos.txt","w");
			if(state==0){
				fprintf(datos,"%d",tams);
				fprintf(datos,"\n%d %d %d", serpiente[0].pos.x,serpiente[0].pos.y, serpiente[0].dir);	
				fprintf(datos,"\n%d %d %d",serpiente[tams-1].pos.x,serpiente[tams-1].pos.y, serpiente[tams-1].dir);
				for(i=1; i<tams-1; i++){
					fprintf(datos,"\n%d %d %d",serpiente[i].pos.x,serpiente[i].pos.y,serpiente[i].dir);
				}
				fprintf(datos,"\n%d %d",nivelAct,nivelValue);
				fclose(datos);
			}
			else if(state == 1){
				fclose(datos);
			}
			FILE *estadisticas = fopen("Data\\stats.txt","w");
			if(state==1)
				fprintf(estadisticas,"%d %d %d %d %d", stats.racha, (stats.jugadas)+1, stats.tamanio, stats.autocolision, stats.colision);
			else
				fprintf(estadisticas,"%d %d %d %d %d", stats.racha, stats.jugadas, stats.tamanio, stats.autocolision, stats.colision);
			fclose(estadisticas);
			
			free(serpiente);
			free(enemigo);
			free(snakeBackup);
			ReleaseDC(hwnd, hdc);
			PostQuitMessage(0);
			break;
		}
		
		/* All other messages (a lot of them) are processed using default procedures */
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

/* The 'main' function of Win32 GUI programs: this is where execution starts */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wc; /* A properties struct of our window */
	HWND hwnd; /* A 'HANDLE', hence the H, or a pointer to our window */
	MSG msg; /* A temporary location for all messages */
	HICON hIcon;
	memset(&wc,0,sizeof(wc));
	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.lpfnWndProc	 = WndProc; /* This is where we will send messages to */
	wc.hInstance	 = hInstance;
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	/* White, COLOR_WINDOW is just a #define for a system color, try Ctrl+Clicking it */
	FILE *perfil = fopen("Data\\perfil.txt","r");
		fscanf(perfil,"%d \n%d %d %d",&PERS.temaId,&PERS.tema[0],&PERS.tema[1],&PERS.tema[2]);
		fscanf(perfil,"\n%d \n%d %d %d",&PERS.cuerpoId,&PERS.cuerpo[0],&PERS.cuerpo[1],&PERS.cuerpo[2]);
		fscanf(perfil,"\n%d \n%d %d %d",&PERS.colaId,&PERS.cola[0],&PERS.cola[1],&PERS.cola[2]);
		fscanf(perfil,"\n%d \n%d %d %d",&PERS.lenguaId,&PERS.lengua[0],&PERS.lengua[1],&PERS.lengua[2]);
		fscanf(perfil,"\n%d \n%d %d %d",&PERS.enemigoId,&PERS.enemigo[0],&PERS.enemigo[1],&PERS.enemigo[2]);
		fscanf(perfil,"\n%d \n%d %d %d",&PERS.cabezaId,&PERS.cabeza[0],&PERS.cabeza[1],&PERS.cabeza[2]);
	fclose(perfil);
	wc.hbrBackground = CreateSolidBrush(RGB(PERS.tema[0], PERS.tema[1], PERS.tema[2]));
	wc.lpszClassName = "WindowClass";
	wc.hIcon		 = LoadIcon(hInstance, "Images\\snakeT"); /* Load a standard icon */
	wc.hIconSm		 = LoadIcon(hInstance, "Images\\snakeT"); /* use the name "A" to use the project icon */
	wc.lpszMenuName  = "MAINMENU";
	

	if(!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}
	
	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,"WindowClass","Serpiente :>",WS_VISIBLE|WS_CAPTION|WS_MINIMIZEBOX | WS_MAXIMIZEBOX,//WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, /* x */
		CW_USEDEFAULT, /* y */
		690, /* width */
		492, /* height */
		NULL,NULL,hInstance,NULL);
	if(hwnd == NULL) {
		MessageBox(NULL, "Window Creation Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}
	/*
		This is the heart of our program where all input is processed and 
		sent to WndProc. Note that GetMessage blocks code flow until it receives something, so
		this loop will not produce unreasonably high CPU usage
	*/
	while(GetMessage(&msg, NULL, 0, 0) > 0) { /* If no error is received... */
		TranslateMessage(&msg); /* Translate key codes to chars if present */
		DispatchMessage(&msg); /* Send it to WndProc */
	}
	
	return msg.wParam;
}


void setColorPers(PERSZ *pers, int r, int g, int b, int modo){
	switch(modo){
		case IDM_MODO_C:
				pers -> tema[0] = r;
				pers -> tema[1] = g;
				pers -> tema[2] = b;
				break;
		case PS_CUERPO_BLANCO:
				pers -> cuerpo[0] = r;
				pers -> cuerpo[1] = g;
				pers -> cuerpo[2] = b;
				break;
		case PS_COLA_BLANCO:
					pers -> cola[0] = r;
				pers -> cola[1] = g;
				pers -> cola[2] = b;
				break;
		case PS_LENGUA_BLANCO:
				pers -> lengua[0] = r;
				pers -> lengua[1] = g;
				pers -> lengua[2] = b;	
				break;
		case PS_ENEMIGO_VERDEBOSQUE:
				pers -> enemigo[0] = r;
				pers -> enemigo[1] = g;
				pers -> enemigo[2] = b;
				break;
		case PS_CABEZA_BLANCO:
				pers -> cabeza[0] = r;
				pers -> cabeza[1] = g;
				pers -> cabeza[2] = b;
				break;
	}
}

void setCheckMenu(HMENU hMenu, int constante){
	int constante_aux = 0,i;
	if(constante/100==2){
		constante_aux=IDM_MODO_C;	
	}
	else{
		if(constante/100==6)	constante_aux = PS_ENEMIGO_VERDEBOSQUE;
		else	constante_aux = (constante/100==3)?	PS_CUERPO_BLANCO : 
				(constante/100==4)? PS_COLA_BLANCO : (constante/100==5)? PS_LENGUA_BLANCO : PS_CUERPO_BLANCO;		
	}
	int condition = (constante/100==2)?	constante+2 : (constante/100==6) ? constante+3 : constante+6;

	for(i=constante_aux; i<=condition; i++){
		if(i!=constante){
			CheckMenuItem(hMenu, i, MF_UNCHECKED);
		}
		else{
			CheckMenuItem(hMenu, i, MF_CHECKED);
		}
	}
}

ENEMY * CrearEnemigo(RECT rect)
{
	srand(time(0));
	int i=0;
	ENEMY *enemigo = NULL;
	enemigo = (ENEMY *) realloc(enemigo,sizeof(ENEMY) * nivelAct);
	if (enemigo==NULL){
		MessageBox(NULL,"Sin memoria","Error",MB_OK | MB_ICONERROR);
		exit(0);		
	}
	enemigo[0].pos.x = rand()% (rect.right / TAMSERP);
	enemigo[0].pos.y = rect.bottom / TAMSERP / 2;
	for(i=1; i<nivelAct; i++){
		enemigo[i].pos.x = rand()% (rect.right / TAMSERP);
		enemigo[i].pos.y = rect.bottom / TAMSERP /2;
	}
	return enemigo;
}

void DibujarEnemigo(HDC hdc, const ENEMY *enemigo){
	int i=0,j=0;
	SetDCBrushColor(hdc, RGB(PERS.enemigo[0], PERS.enemigo[1], PERS.enemigo[2]));
	HBRUSH hBrush = CreateSolidBrush(GetDCBrushColor(hdc));
	HBRUSH hOldBrush;
	while(j<nivelAct){
		hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
		RoundRect(hdc, enemigo[j].pos.x * TAMSERP, enemigo[j].pos.y * TAMSERP, 
			  		enemigo[j].pos.x * TAMSERP + TAMSERP, enemigo[j].pos.y * TAMSERP + TAMSERP, 5, 5);	
		SelectObject(hdc, hOldBrush);
		j++;
	}
	DeleteObject(hBrush);
	DeleteObject(hOldBrush);
}

LETRA *CrearNombre(RECT rect)
{
	int i=0;
	LETRA *letras = NULL;
	letras = (LETRA *) malloc(sizeof(LETRA) * TAMNOM);
	if(letras == NULL){
		MessageBox(NULL,"Sin memoria","Error",MB_OK | MB_ICONERROR);
		exit(0);
	}
			while(i<4){ //palo de J primeros 4
				letras[i].pos.x = 5 + 7;
				letras[i].pos.y = i+3 + 3;
				i++;
			}
			//ultimos dos bloques de J
			letras[i].pos.x = i + 7;
			letras[i].pos.y = i+2 + 3;
			letras[i+1].pos.x = i-1 + 7;
			letras[i+1].pos.y = i+2 + 3;
			//Letra P
			i=6;
			while(i<10){
				letras[i].pos.x = 8 + 7;
				letras[i].pos.y = i-3 + 3;
				i++;
			}
			while(i<12){
				letras[i].pos.x = i-1 + 7;
				letras[i].pos.y = 3 + 3;
				i++;
			}
			letras[i].pos.x = i-2 + 7;
			letras[i].pos.y = 4 + 3;
			letras[i+1].pos.x = i-2 + 7;
			letras[i+1].pos.y = 5 + 3;
			letras[i+2].pos.x = i-3 + 7;
			letras[i+2].pos.y = 5 + 3;
			i=15;
			while(i<19){
				letras[i].pos.x = 20;
				letras[i].pos.y = i-9;
				i++;
			}
			while(i<21){
				letras[i].pos.x = i+2;
				letras[i].pos.y = 6;
				i++;
			}
			while(i<24){
				letras[i].pos.x = 22;
				letras[i].pos.y = 7+(i-21);
				i++;
			}
			letras[i].pos.x = 21;
			letras[i].pos.y = 8;
	return letras;
}

void ModificarNombre(LETRA *letras, RECT rect){
	int i = 0;
	if(windowMinState){
			while(i<4){ //LETRA J
				letras[i].pos.x = 5 + 7;
				letras[i].pos.y = i+3 + 3;
				i++;
			}
			letras[i].pos.x = i + 7;
			letras[i].pos.y = i+2 + 3;
			letras[i+1].pos.x = i-1 + 7;
			letras[i+1].pos.y = i+2 + 3;
			i=6;
			while(i<10){ //LETRA P
				letras[i].pos.x = 8 + 7;
				letras[i].pos.y = i-3 + 3;
				i++;
			}
			while(i<12){
				letras[i].pos.x = i-1 + 7;
				letras[i].pos.y = 3 + 3;
				i++;
			}
			letras[i].pos.x = i-2 + 7;
			letras[i].pos.y = 4 + 3;
			letras[i+1].pos.x = i-2 + 7;
			letras[i+1].pos.y = 5 + 3;
			letras[i+2].pos.x = i-3 + 7;
			letras[i+2].pos.y = 5 + 3;
			i=15;
			while(i<19){ //LETRA A
				letras[i].pos.x = 20;
				letras[i].pos.y = i-9;
				i++;
			}
			while(i<21){
				letras[i].pos.x = i+2;
				letras[i].pos.y = 6;
				i++;
			}
			while(i<24){
				letras[i].pos.x = 22;
				letras[i].pos.y = 7+(i-21);
				i++;
			}
			letras[i].pos.x = 21;
			letras[i].pos.y = 8;
	}
	else{
		while(i<4){
			letras[i].pos.x = rect.right / TAMSERP / 3 + 4;
			letras[i].pos.y = rect.bottom / TAMSERP / 3 + (i+1);
			i++;
		}
		letras[i].pos.x = rect.right / TAMSERP / 3 + 3;
		letras[i].pos.y = rect.bottom / TAMSERP / 3 + 4;
		letras[i+1].pos.x = rect.right / TAMSERP / 3 + 2;
		letras[i+1].pos.y = rect.bottom / TAMSERP / 3 + 4;
		i = 6;
		while(i<10){
			letras[i].pos.x = rect.right / TAMSERP / 3 + 10;
			letras[i].pos.y = rect.bottom / TAMSERP / 4 + (i-2);
			i++;
		}
		while(i<12){
			letras[i].pos.x = rect.right / TAMSERP / 3 + (i+1);
			letras[i].pos.y = rect.bottom / TAMSERP / 4 + 4;
			i++;
		}
			letras[i].pos.x = rect.right / TAMSERP / 3 + i;
			letras[i].pos.y = rect.bottom / TAMSERP / 4 + 5;
			letras[i+1].pos.x = rect.right / TAMSERP / 3 + i;
			letras[i+1].pos.y = rect.bottom / TAMSERP / 4 + 6;
			letras[i+2].pos.x = rect.right / TAMSERP / 3 + i-1;
			letras[i+2].pos.y = rect.bottom / TAMSERP / 4 + 6;
		i=15;
			i=15;
			while(i<19){
				letras[i].pos.x = rect.right / TAMSERP / 3 + 18;
				letras[i].pos.y = rect.bottom / TAMSERP / 4 + (i-11);				
				i++;
			}
			while(i<21){
			letras[i].pos.x = rect.right / TAMSERP / 3 + i;
			letras[i].pos.y = rect.bottom / TAMSERP / 4 + 4;
				i++;
			}
			while(i<24){
				letras[i].pos.x = rect.right / TAMSERP / 3 + 20;
				letras[i].pos.y = rect.bottom / TAMSERP / 4 + (i-16);
				i++;
			}
			letras[i].pos.x = rect.right / TAMSERP / 2 + 7;
			letras[i].pos.y = rect.right / TAMSERP / 4 - 3;
	}
}

void DibujarNombre(HDC hdc, const LETRA *letras){
	int i=0;
	SetDCBrushColor(hdc, RGB(255, 0, 0));
	HBRUSH hBrush = CreateSolidBrush(GetDCBrushColor(hdc));
	HBRUSH hOldBrush;
	for(i=0; i<TAMNOM; i++){
		hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
		RoundRect(hdc, letras[i].pos.x * TAMSERP, letras[i].pos.y * TAMSERP, 
			    		letras[i].pos.x * TAMSERP + TAMSERP, letras[i].pos.y * TAMSERP + TAMSERP, 5, 5);	
		SelectObject(hdc, hOldBrush);
	}
	DeleteObject(hBrush);
}


PEDACITOS * NuevaSerpiente(int *tams)
{
	int i=0;
	FILE* datos = fopen("Data\\datos.txt", "r");
	fseek(datos, 0, SEEK_END ); //Ir al final del archivo de datos del juego
	PEDACITOS *serpiente = NULL;
	if(*tams < 2)
		*tams = 2;
	if(ftell(datos)!=0){ //Verificar si el archivo esta vacío
		fseek(datos, 0, SEEK_SET);
		int x,y,dir,tam;
		fscanf(datos,"%d",&tam);
		tam = (tam<2)? 2 : tam;
		serpiente = (PEDACITOS *) malloc(sizeof(PEDACITOS) * tam);
		if(serpiente==NULL){
			MessageBox(NULL,"Sin memoria","Error",MB_OK | MB_ICONERROR);
			exit(0);
		}
		serpiente[0].tipo = COLA;
		fscanf(datos,"\n%d %d %d",&serpiente[0].pos.x,&serpiente[0].pos.y,&serpiente[0].dir);
		serpiente[tam-1].tipo = CABEZA;
		fscanf(datos,"\n%d %d %d",&serpiente[tam-1].pos.x,&serpiente[tam-1].pos.y,&serpiente[tam-1].dir);
		for(i=1; i<tam-1; i++){
			serpiente[i].tipo = CUERPO;
			fscanf(datos,"\n%d %d %d",&serpiente[i].pos.x,&serpiente[i].pos.y,&serpiente[i].dir);
		}
		fscanf(datos,"\n%d %d",&nivelAct,&nivelValue);
		*tams = tam;
	}
	else{
		serpiente = (PEDACITOS *) malloc(sizeof(PEDACITOS) * (*tams));
		if(serpiente==NULL){
			MessageBox(NULL,"Sin memoria","Error",MB_OK | MB_ICONERROR);
			exit(0);
		}
		serpiente[0].tipo = COLA;
		serpiente[0].pos.x = 1;
		serpiente[0].pos.y = 1;
		serpiente[0].dir = DER;
		for(i=1; i<*tams-1; i++){
			serpiente[i].tipo = CUERPO;
			serpiente[i].pos.x = i+1;
			serpiente[i].pos.y = 1;
			serpiente[i].dir = DER;
		}
		serpiente[i].tipo = CABEZA;
		serpiente[i].pos.x = *tams;
		serpiente[i].pos.y = 1;
		serpiente[i].dir = DER;
	}	
	fclose(datos);
	return serpiente;
}

void DibujarSerpiente(HDC hdc, const PEDACITOS *serpiente)
{
	int i = 1;
	static int bod = 0;
	HBRUSH Pincel = CreateSolidBrush(RGB(PERS.cola[0], PERS.cola[1], PERS.cola[2]));
	SelectObject(hdc, Pincel);
	POINT points[3];
	POINT lengua[5];
	/*COLA*/
	switch(serpiente[0].dir){
		
		case DER:
			points[0].x = serpiente[0].pos.x * TAMSERP + TAMSERP; points[0].y = serpiente[0].pos.y * TAMSERP;
			points[1].x = serpiente[0].pos.x * TAMSERP;			  points[1].y = serpiente[0].pos.y * TAMSERP + TAMSERP/2;
			points[2].x = serpiente[0].pos.x * TAMSERP + TAMSERP; points[2].y = serpiente[0].pos.y * TAMSERP + TAMSERP;
			break;
		case IZQ:
			points[0].x = serpiente[0].pos.x * TAMSERP;				points[0].y = serpiente[0].pos.y * TAMSERP;
			points[1].x = serpiente[0].pos.x * TAMSERP + TAMSERP;	points[1].y = serpiente[0].pos.y * TAMSERP + TAMSERP/2;
			points[2].x = serpiente[0].pos.x * TAMSERP;				points[2].y = serpiente[0].pos.y * TAMSERP + TAMSERP;
			break;
		case ARRIBA:
			points[0].x = serpiente[0].pos.x * TAMSERP;				points[0].y = serpiente[0].pos.y * TAMSERP;
			points[1].x = serpiente[0].pos.x * TAMSERP + TAMSERP/2; points[1].y = serpiente[0].pos.y * TAMSERP + TAMSERP;
			points[2].x = serpiente[0].pos.x * TAMSERP + TAMSERP;	points[2].y =  serpiente[0].pos.y * TAMSERP;
			break;
		case ABAJO:
			points[0].x = serpiente[0].pos.x * TAMSERP;				points[0].y = serpiente[0].pos.y * TAMSERP + TAMSERP;
			points[1].x = serpiente[0].pos.x * TAMSERP + TAMSERP/2; points[1].y = serpiente[0].pos.y * TAMSERP;
			points[2].x = serpiente[0].pos.x * TAMSERP + TAMSERP;	points[2].y = serpiente[0].pos.y * TAMSERP + TAMSERP;
			break;
	}
	Polygon(hdc, points, 3);
	DeleteObject(Pincel);
	/*CUERPO*/
	HBRUSH pincel = CreateSolidBrush(RGB(PERS.cuerpo[0], PERS.cuerpo[1], PERS.cuerpo[2]));
	SelectObject(hdc, pincel);
	while(serpiente[i].tipo!=CABEZA){
		RoundRect(hdc, serpiente[i].pos.x * TAMSERP, serpiente[i].pos.y * TAMSERP, 
						serpiente[i].pos.x * TAMSERP + TAMSERP, serpiente[i].pos.y * TAMSERP + TAMSERP, 5, 5);
		i++;
	}
	DeleteObject(pincel);

	SetDCBrushColor(hdc, RGB(PERS.cabeza[0], PERS.cabeza[1], PERS.cabeza[2]));
	HBRUSH pincelHead = CreateSolidBrush(GetDCBrushColor(hdc));
	HBRUSH hOldBrush;
	hOldBrush = (HBRUSH)SelectObject(hdc, pincelHead);
	RoundRect(hdc, serpiente[i].pos.x * TAMSERP, serpiente[i].pos.y * TAMSERP, 
						serpiente[i].pos.x * TAMSERP + TAMSERP, serpiente[i].pos.y * TAMSERP + TAMSERP, 5, 5);
	DeleteObject(pincelHead);
	DeleteObject(hOldBrush);
	/*CABEZA*/
	HBRUSH pincel2 = CreateSolidBrush(RGB(255, 255, 255));
	SelectObject(hdc, pincel2);
	switch(serpiente[i].dir)
	{ /*OJOS*/
		case DER:
			if(!state){ //si no esta muerto, se dibujan los ojos
				Ellipse(hdc, serpiente[i].pos.x * TAMSERP,
							serpiente[i].pos.y * TAMSERP,
							serpiente[i].pos.x * TAMSERP + TAMSERP/2,
							serpiente[i].pos.y * TAMSERP + TAMSERP/2);
							
				Ellipse(hdc, serpiente[i].pos.x * TAMSERP,
							serpiente[i].pos.y * TAMSERP + TAMSERP/2, 
							serpiente[i].pos.x * TAMSERP + TAMSERP/2,
							serpiente[i].pos.y * TAMSERP + TAMSERP);
				DeleteObject(pincel2);
			}
			else{
				DeleteObject(pincel2);
				HPEN hPen, hOldPen;
				hPen = CreatePen(PS_SOLID, 3, RGB(139, 0, 0));
				hOldPen = SelectObject(hdc, hPen);
				MoveToEx(hdc, serpiente[i].pos.x * TAMSERP, serpiente[i].pos.y * TAMSERP, NULL);
				LineTo(hdc, serpiente[i].pos.x * TAMSERP + TAMSERP/2, serpiente[i].pos.y * TAMSERP + TAMSERP/2);
				LineTo(hdc, serpiente[i].pos.x * TAMSERP, serpiente[i].pos.y * TAMSERP + TAMSERP);
				MoveToEx(hdc, serpiente[i].pos.x * TAMSERP + TAMSERP/2, serpiente[i].pos.y * TAMSERP, NULL);
				LineTo(hdc, serpiente[i].pos.x * TAMSERP, serpiente[i].pos.y * TAMSERP + TAMSERP/2);
				LineTo(hdc, serpiente[i].pos.x * TAMSERP + TAMSERP/2, serpiente[i].pos.y * TAMSERP + TAMSERP);
				SelectObject(hdc, hOldPen);
				DeleteObject(hPen);
			}
			/*LENGUA*/
			if(bod == 0){
				HBRUSH pincelLG = CreateSolidBrush(RGB(PERS.lengua[0], PERS.lengua[1], PERS.lengua[2]));
				SelectObject(hdc, pincelLG);
				lengua[0].x = serpiente[i].pos.x * TAMSERP + TAMSERP;				lengua[0].y = serpiente[i].pos.y * TAMSERP + TAMSERP/4;
				lengua[1].x = serpiente[i].pos.x * TAMSERP + TAMSERP + TAMSERP/2;	lengua[1].y = serpiente[i].pos.y * TAMSERP + TAMSERP/4;
				lengua[2].x = serpiente[i].pos.x * TAMSERP + TAMSERP + TAMSERP/4;	lengua[2].y = serpiente[i].pos.y * TAMSERP + TAMSERP/2;
				lengua[3].x = serpiente[i].pos.x * TAMSERP + TAMSERP + TAMSERP/2;	lengua[3].y = serpiente[i].pos.y * TAMSERP + TAMSERP/4 + TAMSERP/2;
				lengua[4].x = serpiente[i].pos.x * TAMSERP + TAMSERP;				lengua[4].y = serpiente[i].pos.y * TAMSERP + TAMSERP/2 +TAMSERP/4;
				Polygon(hdc,lengua,5);
				DeleteObject(pincelLG);
				bod = 1;
			}else if(bod==1){
				bod = 0;
			}
			break;
			
		case IZQ:
			if(!state){	
				Ellipse(hdc, serpiente[i].pos.x * TAMSERP + TAMSERP/2, 
							serpiente[i].pos.y * TAMSERP,
							serpiente[i].pos.x * TAMSERP + TAMSERP,
							serpiente[i].pos.y * TAMSERP + TAMSERP/2);
							
				Ellipse(hdc, serpiente[i].pos.x * TAMSERP + TAMSERP/2, 
							serpiente[i].pos.y * TAMSERP + TAMSERP/2,
							serpiente[i].pos.x * TAMSERP + TAMSERP,
							serpiente[i].pos.y * TAMSERP + TAMSERP);
				DeleteObject(pincel2);
			}
			else{
				DeleteObject(pincel2);
				HPEN hPen, hOldPen;
				hPen = CreatePen(PS_SOLID, 2, RGB(139, 0, 0));
				hOldPen = SelectObject(hdc, hPen);
				MoveToEx(hdc, serpiente[i].pos.x * TAMSERP + TAMSERP, serpiente[i].pos.y * TAMSERP, NULL);
				LineTo(hdc, serpiente[i].pos.x * TAMSERP + TAMSERP/2, serpiente[i].pos.y * TAMSERP + TAMSERP/2);
				LineTo(hdc, serpiente[i].pos.x * TAMSERP + TAMSERP, serpiente[i].pos.y * TAMSERP + TAMSERP);
				MoveToEx(hdc, serpiente[i].pos.x * TAMSERP + TAMSERP/2, serpiente[i].pos.y * TAMSERP, NULL);
				LineTo(hdc, serpiente[i].pos.x * TAMSERP + TAMSERP, serpiente[i].pos.y * TAMSERP + TAMSERP/2);
				LineTo(hdc, serpiente[i].pos.x * TAMSERP + TAMSERP/2, serpiente[i].pos.y * TAMSERP + TAMSERP);
				SelectObject(hdc, hOldPen);
				DeleteObject(hPen);	
			}
			
			if(bod == 0){
				lengua[0].x = serpiente[i].pos.x * TAMSERP;					lengua[0].y = serpiente[i].pos.y * TAMSERP + TAMSERP/4;
				lengua[1].x = serpiente[i].pos.x * TAMSERP - TAMSERP/2;		lengua[1].y = serpiente[i].pos.y * TAMSERP + TAMSERP/4;
				lengua[2].x = serpiente[i].pos.x * TAMSERP - TAMSERP/4;		lengua[2].y = serpiente[i].pos.y * TAMSERP + TAMSERP/2;
				lengua[3].x = serpiente[i].pos.x * TAMSERP - TAMSERP/2;		lengua[3].y = serpiente[i].pos.y * TAMSERP + TAMSERP/2 + TAMSERP/4;
				lengua[4].x = serpiente[i].pos.x * TAMSERP;					lengua[4].y = serpiente[i].pos.y * TAMSERP + TAMSERP/2 + TAMSERP/4;
				Polygon(hdc,lengua,5);
				bod = 1;
			}else if(bod == 1){
				bod = 0;
			}
			break;
		case ARRIBA:
			if(!state){
				Ellipse(hdc, serpiente[i].pos.x * TAMSERP, 
							serpiente[i].pos.y * TAMSERP + TAMSERP/2,
							serpiente[i].pos.x * TAMSERP + TAMSERP/2,
							serpiente[i].pos.y * TAMSERP + TAMSERP);
				Ellipse(hdc, serpiente[i].pos.x * TAMSERP + TAMSERP/2, 
							serpiente[i].pos.y * TAMSERP + TAMSERP/2,
							serpiente[i].pos.x * TAMSERP + TAMSERP,
							serpiente[i].pos.y * TAMSERP + TAMSERP);
				DeleteObject(pincel2);
			}
			else{ //si murio, dibujo X
				DeleteObject(pincel2);
				HPEN hPen, hOldPen;
				hPen = CreatePen(PS_SOLID, 2, RGB(139, 0, 0));
				hOldPen = SelectObject(hdc, hPen);
				MoveToEx(hdc, serpiente[i].pos.x * TAMSERP, serpiente[i].pos.y * TAMSERP + TAMSERP, NULL);
				LineTo(hdc, serpiente[i].pos.x * TAMSERP + TAMSERP/2, serpiente[i].pos.y * TAMSERP + TAMSERP/2);
				LineTo(hdc, serpiente[i].pos.x * TAMSERP + TAMSERP, serpiente[i].pos.y * TAMSERP + TAMSERP);
				MoveToEx(hdc, serpiente[i].pos.x * TAMSERP, serpiente[i].pos.y * TAMSERP + TAMSERP/2, NULL);
				LineTo(hdc, serpiente[i].pos.x * TAMSERP + TAMSERP/2, serpiente[i].pos.y * TAMSERP + TAMSERP);
				LineTo(hdc, serpiente[i].pos.x * TAMSERP + TAMSERP, serpiente[i].pos.y * TAMSERP + TAMSERP/2);
				SelectObject(hdc, hOldPen);
				DeleteObject(hPen);
			}
			
			if(bod == 0){
				HBRUSH pincelLG = CreateSolidBrush(RGB(PERS.lengua[0], PERS.lengua[1], PERS.lengua[2]));
				SelectObject(hdc, pincelLG);
				lengua[0].x = serpiente[i].pos.x * TAMSERP + TAMSERP/4;					lengua[0].y = serpiente[i].pos.y * TAMSERP;
				lengua[1].x = serpiente[i].pos.x * TAMSERP + TAMSERP/4;					lengua[1].y = serpiente[i].pos.y * TAMSERP - TAMSERP/2;
				lengua[2].x = serpiente[i].pos.x * TAMSERP + TAMSERP/2;					lengua[2].y = serpiente[i].pos.y * TAMSERP - TAMSERP/4;
				lengua[3].x = serpiente[i].pos.x * TAMSERP + TAMSERP/2 + TAMSERP/4;		lengua[3].y = serpiente[i].pos.y * TAMSERP - TAMSERP/2;
				lengua[4].x = serpiente[i].pos.x * TAMSERP + TAMSERP/2 + TAMSERP/4;		lengua[4].y = serpiente[i].pos.y * TAMSERP;
				Polygon(hdc,lengua,5);
				DeleteObject(pincelLG);
				bod = 1;
			}else if(bod == 1){
				bod = 0;
			}
			break;
		case ABAJO:
			if(!state){
				Ellipse(hdc, serpiente[i].pos.x * TAMSERP, 
							serpiente[i].pos.y * TAMSERP,
							serpiente[i].pos.x * TAMSERP + TAMSERP/2,
							serpiente[i].pos.y * TAMSERP + TAMSERP/2);
							
				Ellipse(hdc, serpiente[i].pos.x * TAMSERP + TAMSERP/2,
							serpiente[i].pos.y * TAMSERP,
							serpiente[i].pos.x * TAMSERP + TAMSERP,
							serpiente[i].pos.y * TAMSERP + TAMSERP/2);
				DeleteObject(pincel2);
			}
			else{ //dibujo X
				HPEN hPen, hOldPen;
				hPen = CreatePen(PS_SOLID, 2, RGB(139, 0, 0));
				hOldPen = SelectObject(hdc, hPen);
				MoveToEx(hdc, serpiente[i].pos.x * TAMSERP, serpiente[i].pos.y * TAMSERP, NULL);
				LineTo(hdc, serpiente[i].pos.x * TAMSERP + TAMSERP/2, serpiente[i].pos.y * TAMSERP + TAMSERP/2);
				LineTo(hdc, serpiente[i].pos.x * TAMSERP + TAMSERP, serpiente[i].pos.y * TAMSERP);
				MoveToEx(hdc, serpiente[i].pos.x * TAMSERP, serpiente[i].pos.y * TAMSERP + TAMSERP/2, NULL);
				LineTo(hdc, serpiente[i].pos.x * TAMSERP + TAMSERP/2, serpiente[i].pos.y * TAMSERP);
				LineTo(hdc, serpiente[i].pos.x * TAMSERP + TAMSERP, serpiente[i].pos.y * TAMSERP + TAMSERP/2);
				SelectObject(hdc, hOldPen);
				DeleteObject(hPen);
			}
			
			if(bod == 0){
				HBRUSH pincelLG = CreateSolidBrush(RGB(PERS.lengua[0], PERS.lengua[1], PERS.lengua[2]));
				SelectObject(hdc, pincelLG);
				lengua[0].x = serpiente[i].pos.x * TAMSERP + TAMSERP/4;					lengua[0].y = serpiente[i].pos.y * TAMSERP + TAMSERP;
				lengua[1].x = serpiente[i].pos.x * TAMSERP + TAMSERP/4;					lengua[1].y = serpiente[i].pos.y * TAMSERP + TAMSERP +TAMSERP/2;
				lengua[2].x = serpiente[i].pos.x * TAMSERP + TAMSERP/2;					lengua[2].y = serpiente[i].pos.y * TAMSERP + TAMSERP +TAMSERP/4;
				lengua[3].x = serpiente[i].pos.x * TAMSERP + TAMSERP/2 + TAMSERP/4;		lengua[3].y = serpiente[i].pos.y * TAMSERP + TAMSERP + TAMSERP/2;
				lengua[4].x = serpiente[i].pos.x * TAMSERP + TAMSERP/2 + TAMSERP/4;		lengua[4].y = serpiente[i].pos.y * TAMSERP + TAMSERP;
				Polygon(hdc,lengua,5);
				DeleteObject(pincelLG);
				bod = 1;
			}else if(bod == 1){
				bod = 0;
			}
			break;
			DeleteObject(pincel2);
	}
}

int MoverEnemigo(ENEMY *enemigo, int tams, RECT rect){
	int i=0,j=0;
	while(j<nivelAct){ //Cantidad de enemigos igual al nivel actual
		enemigo[j].pos.y = enemigo[j].pos.y + 1;
		if(enemigo[j].pos.y >= rect.bottom / TAMSERP)
			enemigo[j].pos.y = 0;
		if(enemigo[j].pos.x >= rect.right / TAMSERP)
			enemigo[j].pos.x = rand()% rect.right / TAMSERP;
		j++;
	}
}

int MoverSerpiente(PEDACITOS *serpiente, int dir, RECT rect, int tams, LETRA *letras, ENEMY *enemigo)
{
	int i=0;
	snakeBackup = (PEDACITOS *) realloc((void *)snakeBackup, sizeof(PEDACITOS) * tams);
	for(i=0; i<tams; i++){
		snakeBackup[i].pos.x = serpiente[i].pos.x;
		snakeBackup[i].pos.y = serpiente[i].pos.y;
		snakeBackup[i].tipo = serpiente[i].tipo;
		snakeBackup[i].dir = serpiente[i].dir;
	}
	i=0;
	
	while(serpiente[i].tipo != CABEZA){
		serpiente[i].dir = serpiente[i+1].dir;
		serpiente[i].pos = serpiente[i+1].pos;
		i++;
	}
	switch(serpiente[i].dir){
		case DER:
			if(dir!= IZQ)
				serpiente[i].dir = dir;
			break;
		case IZQ:
			if(dir != DER)
				serpiente[i].dir = dir;
			break;
		case ARRIBA:
			if(dir != ABAJO)
				serpiente[i].dir = dir;
			break;
		case ABAJO:
			if(dir != ARRIBA)
				serpiente[i].dir = dir;
			break;
	}
	switch(serpiente[i].dir){
		case DER:
			serpiente[i].pos.x = serpiente[i].pos.x + 1;
			if(serpiente[i].pos.x >= rect.right / TAMSERP)
				serpiente[i].pos.x = 0;
			break;
		case IZQ:
			serpiente[i].pos.x = serpiente[i].pos.x - 1;
			if(serpiente[i].pos.x < 0)
				serpiente[i].pos.x = rect.right/TAMSERP;
			break;
		case ARRIBA:
			serpiente[i].pos.y = serpiente[i].pos.y - 1;
			if(serpiente[i].pos.y < 0)
				serpiente[i].pos.y = rect.bottom / TAMSERP;
			break;
		case ABAJO:
			serpiente[i].pos.y = serpiente[i].pos.y + 1;
			if(serpiente[i].pos.y > rect.bottom / TAMSERP)
				serpiente[i].pos.y = 0;
			break;
	}
	
	return !Colisionar(serpiente, tams, letras, enemigo, snakeBackup);
}

int Colisionar(PEDACITOS *serpiente, int tams, const LETRA *letras, ENEMY *enemigo, PEDACITOS *snakeBackup){
	int i = 0,aux=0, j=0;

	if(com.tipo == MATA){
		if(serpiente[tams-1].pos.x == com.pos.x && serpiente[tams-1].pos.y == com.pos.y){
			stats.autocolision++;
			state = 1;
			PlaySound("Sounds\\death.wav", NULL, SND_FILENAME | SND_ASYNC);
			while(j<tams){
				serpiente[j].pos.x = snakeBackup[j].pos.x;
				serpiente[j].pos.y = snakeBackup[j].pos.y;
				serpiente[j].tipo = snakeBackup[j].tipo;
				serpiente[j].dir = snakeBackup[j].dir;
				j++;
			}
			return 1;
		}
	}

	while(serpiente[i].tipo != CABEZA){
		if(serpiente[i].pos.x == serpiente[tams-1].pos.x && serpiente[i].pos.y == serpiente[tams-1].pos.y){
			stats.autocolision++;
			state = 1;
			PlaySound("Sounds\\death.wav", NULL, SND_FILENAME | SND_ASYNC);
			while(j<tams){
				serpiente[j].pos.x = snakeBackup[j].pos.x;
				serpiente[j].pos.y = snakeBackup[j].pos.y;
				serpiente[j].tipo = snakeBackup[j].tipo;
				serpiente[j].dir = snakeBackup[j].dir;
				j++;
			}
			return 1;	
		}
		i++;
	}
	i=0; j=0;
	while(i<TAMNOM){
		if((serpiente[tams-1].pos.x == letras[i].pos.x && serpiente[tams-1].pos.y == letras[i].pos.y) || (serpiente[tams-2].pos.x == letras[i].pos.x && serpiente[tams-2].pos.y == letras[i].pos.y)){
			stats.colision++;
			state = 1;
			PlaySound("Sounds\\death.wav", NULL, SND_FILENAME | SND_ASYNC);
			while(j<tams){
				serpiente[j].pos.x = snakeBackup[j].pos.x;
				serpiente[j].pos.y = snakeBackup[j].pos.y;
				serpiente[j].tipo = snakeBackup[j].tipo;
				serpiente[j].dir = snakeBackup[j].dir;
				j++;
			}
			return 1;
		}
		i++;
	}
	aux=0;
	for(i=0; i<tams; i++){
		for(j=0; j<nivelAct; j++){
				if(serpiente[i].pos.x == enemigo[j].pos.x && serpiente[i].pos.y == enemigo[j].pos.y){
					stats.colision++;
					state=1;
					PlaySound("Sounds\\death.wav", NULL, SND_FILENAME | SND_ASYNC);
					while(aux<tams){
						serpiente[aux].pos.x = snakeBackup[aux].pos.x;
						serpiente[aux].pos.y = snakeBackup[aux].pos.y;
						serpiente[aux].tipo = snakeBackup[aux].tipo;
						serpiente[aux].dir = snakeBackup[aux].dir;
						aux++;
					}
					return 1;
				}
		} 
	}
	
	return 0;
}

PEDACITOS * AjustarSerpiente (PEDACITOS *serpiente, int *tams, int comida, RECT rect, HWND hwnd){
	int i;
	PEDACITOS cabeza = serpiente[*tams-1];
	switch(comida){
		case CRECE:
			if(*tams == 8 || *tams==11 || *tams==14 || *tams==18 || *tams==22 || *tams==25 || *tams==30 || *tams==35 || *tams==40){
				PlaySound("Sounds\\levelup.wav", NULL, SND_FILENAME | SND_ASYNC);
				levelUPDW = 1;
				nivelAct++;
				nivelValue-=10;
			}
			else if(!levelUPDW){
				PlaySound("Sounds\\eating.wav", NULL, SND_FILENAME | SND_ASYNC);
			}
			(*tams)++;
			serpiente = (PEDACITOS *) realloc((void *)serpiente, sizeof(PEDACITOS) * (*tams));
			serpiente[*tams-2].tipo = CUERPO;
			serpiente[*tams-1] = cabeza;
			i = *tams-1;
			switch(serpiente[i].dir){
				case DER:
					serpiente[i].pos.x = serpiente[i].pos.x + 1;
					if(serpiente[i].pos.x >= rect.right / TAMSERP)
						serpiente[i].pos.x = 0;
					break;
				case IZQ:
					serpiente[i].pos.x = serpiente[i].pos.x - 1;
					if(serpiente[i].pos.x < 0)
						serpiente[i].pos.x = rect.right/TAMSERP;
					break;
				case ARRIBA:
					serpiente[i].pos.y = serpiente[i].pos.y - 1;
					if(serpiente[i].pos.y < 0)
						serpiente[i].pos.y = rect.bottom / TAMSERP;
					break;
				case ABAJO:
					serpiente[i].pos.y = serpiente[i].pos.y + 1;
					if(serpiente[i].pos.y > rect.bottom / TAMSERP)
						serpiente[i].pos.y = 0;
					break;
			}
			racha++;
			if(racha > stats.racha)
				stats.racha = racha;
			if(*tams > stats.tamanio)
				stats.tamanio = *tams;
			break;
		case ACHICA:
			racha=0;
			if(*tams == 7 || *tams==10 || *tams==13 || *tams==17 || *tams==21 || *tams==24 || *tams==29 || *tams==34 || *tams==39){
				PlaySound("Sounds\\leveldown.wav", NULL, SND_FILENAME | SND_ASYNC);
				levelUPDW = 2;
				nivelAct--;
				PlaySound(NULL, NULL, 0);
				nivelValue+=5;
			}
			if(!levelUPDW){
				PlaySound("Sounds\\error.wav", NULL, SND_FILENAME | SND_ASYNC);
			}
			i=0;
			if(*tams > 2){
				while(serpiente[i].tipo != CABEZA){
					serpiente[i] = serpiente[i+1];
					i++;	
				}
				(*tams)--;
				serpiente = (PEDACITOS *) realloc ((void *) serpiente, sizeof(PEDACITOS) * (*tams));
				serpiente[*tams-1] = cabeza;
			}
			else{
				(*tams)--;
				serpiente = (PEDACITOS *) realloc ((void *) serpiente, sizeof(PEDACITOS) * (*tams));
				serpiente[*tams-1] = cabeza;
			}
			break;
		case NADA:
			break;
		case MATA:
				KillTimer(hwnd,IDT_TIMER1);
				MessageBox(NULL,"Has sido envenenado", "Chale",MB_OK);
			break;
	}
	return serpiente;
}

ENEMY * AjustarEnemigo(ENEMY *enemigo, RECT rect){
	enemigo = (ENEMY *) realloc(enemigo, sizeof(ENEMY) * nivelAct); //Crear mas enemigos
	if(enemigo == NULL){
		MessageBox(NULL,"Sin memoria","Error",MB_OK | MB_ICONERROR);
		exit(0);
	}
	if(enemigo[nivelAct-1].pos.y >= rect.bottom / TAMSERP){
		enemigo[nivelAct-1].pos.y = 0;
	}
	enemigo[nivelAct-1].pos.x = rand()% (rect.right / TAMSERP);
	enemigo[nivelAct-1].pos.y = 0;
	
	return enemigo;
}

int Comer(const PEDACITOS *serpiente, int tams)
{
	return (serpiente[tams-1].pos.x == com.pos.x && serpiente[tams-1].pos.y == com.pos.y)? 1 : 0;
}