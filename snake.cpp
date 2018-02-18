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

/* Ckecks if keyboard was hit */
int kbhit()
{
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

/* Gets char from keyboard */
int getch()
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

#define WIDTH 25
#define HEIGHT 15

struct node
{
	int x;
	int y;
	node *next;
	node *before;
};

/* Returns an integer in range [min, max] */
int rand_range (int min, int max)
{
	srand(clock());
	return rand()*1.0/(RAND_MAX-1) * (max+1-min) + min;
}

bool prob (float x)
{
	srand(clock());
	return x > (double)rand() / ((double)RAND_MAX + 1);
}

/* Returns time now */
double time_now()
{
	return double (clock())/CLOCKS_PER_SEC;
}

/* Returns maximum number between x and y */
double max (double x, double y) {
	if (x > y) return x;
	return y;
}

/* Creates and returns pointer to a node in the list after *last */
node *createNode (node *last, int pos_x, int pos_y)
{
	node *new_node = new(node);
	new_node->x = pos_x;
	new_node->y = pos_y;
	
	new_node->before = last;
	new_node->next = last->next;
	
	last->next->before = new_node;	
	last->next = new_node;
	
	return new_node;
}

/* Checks if pair [i,j] belongs to list *first */
bool belongsList (node *first, int size, int i, int j)
{
	int k;
	node *p;
	
	for (k = 0, p = first; k < size; p = p->next, k++)
		if (p->x == i && p->y == j) return true;
	return false;
}

/* Sets new position to fruit. New position can't be in the snake */
void resetFruit (node *first, int size, int *x, int *y)
{
	while (belongsList (first, size, *x, *y)){
		*x = rand_range(1, WIDTH-2);
		*y = rand_range(1, HEIGHT-2);
	}
}

/* Prints out list */
void printList (node *first, int size) {
	int i;
	node *p;
	
	cout << "lista:" << endl;
	for (i = 0, p = first; i < size; p = p->next, i++)
		cout << "[" << p->x << ", " << p->y << "]" << endl;
}

bool hitsTail (node *first, int size, int x, int y)
{
	if (belongsList(first->next, size-1, x, y))
		return true;
	return false;
}

int main ()
{
	bool gameOver = false;
	int score = 0;
	
	char input;
	double t1 = time_now();
	double t2 = t1;
	double DELAY = 0.15;
	double remTimeF;
	
	int fx = rand_range(1, WIDTH-2);
	int fy = rand_range(1, HEIGHT-2);
	
	int SPx = rand_range(1, WIDTH-2);
	int SPy = rand_range(1, HEIGHT-2);
	bool SPfruit = false;
	
	int x_before, y_before;
	int size = 1;
	
	node head, first, second, third;
	node *last = &first;
	
	first.x = WIDTH/2;
	first.y = HEIGHT/2;
	first.next = &first;
	first.before = &first;
	
	while (!gameOver) {
		/* SETUP TERMINAL */
		set_conio_terminal_mode();
		
		//DELAY is set to change, giving proression in dificulty
		DELAY = max(0.1 - 0.0003*(double)(score/10), 0.03);
		
		while (!kbhit() && t1-t2 < DELAY) t1 = time_now();
		if (t1-t2 < DELAY) input = getch();
		t2 = t1;
		
		reset_terminal_mode();
		system ("clear");
		/*----------------*/

		/* INPUT */		
		x_before = first.x;
		y_before = first.y;
		
		switch (input) {
			case 'w':
			case 'W':
				first.y--;
				break;
			case 's':
			case 'S':
				first.y++;
				break;
			case 'a':
			case 'A':
				first.x--;
				break;
			case 'd':
			case 'D':
				first.x++;
				break;
			case 'q':
			case 'Q':
				gameOver = true;
				break;
		}
		/*-------*/
		
		/* LOGIC */
		//Hits Fruit
		if (fx == first.x && fy == first.y) {
			score += 10;
			size++;
			last = createNode (last, fx, fy);
			resetFruit (&first, size, &fx, &fy);
			if (!SPfruit && prob (0.25)) {
				SPfruit = true;
				resetFruit (&first, size, &SPx, &SPy);
				remTimeF = 3.5;
			}
		}
		
		if (SPfruit) remTimeF -= DELAY;
		if (remTimeF <= 0) SPfruit = false;
		
		//Hits Special Fruit
		if (SPfruit && SPx == first.x && SPy == first.y) {
				score += 30;
				size += 3;
				for (int i = 0; i < 3; i++) last = createNode (last, fx, fy);
				resetFruit (&first, size, &SPx, &SPy);
				SPfruit = false;
		}
		
		//Move
		if (size > 1){	
			last->x = x_before;
			last->y = y_before;
			last = last->before;
		}
		
		//Hits Tail
		if (size > 1)
			if (hitsTail (&first, size, first.x, first.y)) gameOver = true;
		
		//Hits Wall
		if (first.x == WIDTH || first.x == -1 || first.y == HEIGHT || first.y == -1) gameOver = true;
		/*-------*/
		
		/* DRAW */
		cout << "Score: " << score << endl;
		for (int i = 0; i <= WIDTH+1; i++) cout << '#';
		cout << endl;
		
		for (int y = 0; y < HEIGHT; y++) {
		cout << '#';
			for (int x = 0; x < WIDTH; x++) {
				if (belongsList (&first, size, x, y))		cout << 'O';
				else if (x == fx && y == fy)				cout << 'x';
				else if (SPfruit && x == SPx && y == SPy)	cout << '@';
				else 										cout << ' ';
			}
		cout << '#' << endl;
		}
		
		for (int i = 0; i <= WIDTH+1; i++) cout << '#';
		cout << endl;
		if (SPfruit) cout << "Remaining Time: " << remTimeF << endl;
		/*------*/
	}
	
	system ("clear");
	cout << endl;
	cout << " =================== " << endl;
	cout << " ==== GAME OVER ==== " << endl;
	cout << " =================== " << endl << endl;
	cout << "Score: " << score << endl << endl;
	
	
	return 0;
}



















