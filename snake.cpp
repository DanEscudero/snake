#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <ctime>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>

using namespace std;

#ifdef _WIN32

#elif __linux__

struct termios orig_termios;

void reset_terminal_mode()  //Retorna para o terminal original
{
    tcsetattr(0, TCSANOW, &orig_termios);
}

void set_conio_terminal_mode()  //Configura terminal especial
{
    struct termios new_termios;

    /* take two copies - one for now, one for later */
    tcgetattr(0, &orig_termios);
    memcpy(&new_termios, &orig_termios, sizeof(new_termios));

    /* register cleanup handler, and set the new terminal mode */
    atexit(reset_terminal_mode);
    cfmakeraw(&new_termios);
    tcsetattr(0, TCSANOW, &new_termios);
}

int kbhit() //Retorna 0 se nada foi teclado
{
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

int getch() //Lê o que foi teclado
{
    int r;
    unsigned char c;
    if ((r = read(0, &c, sizeof(c))) < 0) {
        return r;
    } else {
        return c;
    }
}

#endif

#define ALTURA 9
#define LARGURA 9
#define DELAY 0.3

struct node
{
	int x;
	int y;
	node * prox;
};

struct pos
{
	int x;
	int y;
};

int rand_range (int min, int max)
{
	srand(clock());
	return rand()*1.0/(RAND_MAX-1) * (max+1-min) + min;
}

double time_n()
{
	return double (clock())/CLOCKS_PER_SEC;
}

node * createNode (int posx, int posy, node *p)
{
	node *novo;
	novo = new(node);
	novo->x = posx;
	novo->y = posy;
	novo->prox = p->prox;
	p->prox = novo;
}

bool belongsList (node *lst, int pos_x, int pos_y) {
	node *i;
	for (i = lst; i->prox != NULL; i = i->prox)
		if ((i->x)%LARGURA == pos_x && (i->y)%ALTURA == pos_y) return true;
	return false;
}

void reset_fruit (node *lst, int *x, int *y)
{
	while (belongsLista(lst, int *x, int *y)) {
		*x = rand_range(0, LARGURA-1);
		*y = rand_range(0, ALTURA-1);
	}
}

void printLista (node *lst) {
	cout << "lista:" << endl;
	for (node* i = lst; i->prox!=NULL; i = i->prox) {
		cout << "[" << i->x << ", " << i->y << "]" << endl;
	}
}

int main () {
	/* SETUP */	
	bool gameOver = false;
	int score = 0;
	int nSnake = 1;
	
	char input;
	double tnow = time_n();
	double tbefore = tnow;
	
	int fx = rand_range (0, LARGURA-1);
	int fy = rand_range (0, ALTURA-1);
	
	node head;
	head.x = LARGURA/2;
	head.y = ALTURA/2;
	head.prox = NULL;
	node *last = &head;
	
	/*-------*/
	
	while (!gameOver) {
		/* SETUP TERMINAL */
		set_conio_terminal_mode();
		while (kbhit() == 0 && tnow - tbefore < DELAY) tnow = time_n();
		if(tnow - tbefore < DELAY) input = getch();
		tbefore = tnow;
		reset_terminal_mode();
		system("clear");
		/*----------------*/
		
		/* INPUT */
		/* W A S D INPUT TYPE */
		switch (input) {
			case 'w':
			case 'W':
				head.x--;
				break;
			case 's':
			case 'S':
				head.x++;
				break;
			case 'a':
			case 'A':
				head.y--;
				break;
			case 'd':
			case 'D':
				head.y++;
				break;
			case 'q':
			case 'Q':
				gameOver = true;
				break;
		}
		/* ARROW KEYS INPUT TYPE*/
		/*switch (input) {
			case 'A':
				head.y--;
				break;
			case 'B':
				head.y++;
				break;
			case 'C':
				head.x++;
				break;
			case 'D':
				head.x--;
				break;
			case 'q':
			case 'Q':
				gameOver = true;
				break;
		}
		/*-------*/
		
		/* LOGIC */
		head.y = head.y%ALTURA;
		head.x = head.x%LARGURA;
		if (head.x < 0) head.x += LARGURA;
		if (head.y < 0) head.y += ALTURA;
		
		if (head.y%ALTURA == fy && head.x%LARGURA == fx) {
			score += 10;
			reset_fruit(&fx, &fy);
			nSnake++;
			createNode(head.x, head.y, last);
			last = last->prox;			
		}
		
		for (node *p = &head; p->prox !=  NULL; p = p->prox) {
			p->x = p->prox->x;
			p->y = p->prox->y;
		}
		/*-------*/
		
		/* DRAW */			
		cout << input << endl;
		cout << "score:   " << score << endl;
		cout << "head.x   " << head.x%LARGURA << endl;
		cout << "head.y   " << head.y%ALTURA << endl;
		cout << "[fx, fy] " << fx << "," << fy << endl;
		
		for (int i = 0; i < ALTURA; i++) {
			for (int j = 0; j < LARGURA; j++) {
				//if (i == head.y && j == head.x) cout << "0";
				if (belongsList(&head, i, j)) cout << "0";
				else if (j == fy && i == fx) cout << "x";
				else cout << ".";
			}
			cout << endl;
		}
		cout << endl;
		/*------*/
		printLista(&head);
	}
	return 0;
}


















