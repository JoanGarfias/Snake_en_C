//Estructuras necesarias para el juego

struct pos{
	int x;
	int y;
};
typedef struct pos POS;

struct PedacitoS{ //SERPIENTE
	POS pos;
	int tipo;
	int dir;
};
typedef struct PedacitoS PEDACITOS;

struct comida{
	POS pos;
	int tipo;
};
typedef struct comida COMIDA;

struct letra{
	POS pos;
};
typedef struct letra LETRA;

struct enemy{
	POS pos;
};
typedef struct enemy ENEMY;

struct personalizacion{ //Perfil del jugador
	int temaId;
	int tema[3];
	int cuerpoId;
	int cuerpo[3];
	int colaId;
	int cola[3];
	int lenguaId;
	int lengua[3];
	int enemigoId;
	int enemigo[3];
	int cabezaId;
	int cabeza[3];
};
typedef struct personalizacion PERSZ;


struct estadisticas{
	int racha;
	int jugadas;
	int tamanio;
	int autocolision;
	int colision;
	int nivel;
};
typedef struct estadisticas STATS;
