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
#define HEIGHT 25

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

/* Checks if pair [i,j] belongs to list *first */
bool belongsList (node *first, int i, int j)
{
	for (node *p = first; p->next != first; p = p->next)
		if (p->x == i && p->y == j) return true;
	return false;
}

/* Sets new position to fruit. New position can't be in the snake */
void resetFruit (node *first, int *fx, int *fy)
{
	while (belongsList (first, *fx, *fy)){
		*fx = rand_range(1, WIDTH-2);
		*fy = rand_range(1, HEIGHT-2);
	}
}

/* Creates and returns pointer to a node in the list after *last */
node *createNode (node *last, int pos_x, int pos_y)
{
	node *new_node = new(node);
	new_node->x = pos_x;
	new_node->y = pos_y;
	
	new_node->next = last->next;
	new_node->before = last;
	
	last->next->before = new_node;	
	last->next = new_node;
	
	return new_node;
}

/* Prints out list */
void printList (node *first) {
	cout << "lista:" << endl;
	for (node* p = first; p->next != first; p = p->next)
		cout << "[" << p->x << ", " << p->y << "]" << endl;
}

/* Checks if head hits body */
bool hitsBody (node *first, int x, int y)
{
	for (node *p = first->next; p->next->next != first; p = p->next)
		if (first->x == p->x && first->y == p->y) return true;
	return false;
}

int main ()
{
	bool gameOver = false;
	int score = 0;
	
	char input;
	double t1 = time_now();
	double t2 = t1;
	double DELAY = 0.1;
	
	int fx = rand_range(1, WIDTH-2);
	int fy = rand_range(1, HEIGHT-2);
	
	int x_before, y_before;
	
	node head, first, second, third;
	node *last = NULL;
	
	first.x = WIDTH/2;
	first.y = HEIGHT/2;
	first.next = &head;
	first.before = &head;
	
	head.x = -1;
	head.y = -1;
	head.next = &first;
	head.before = &first;
	
	while (!gameOver) {
		/* SETUP TERMINAL */
		set_conio_terminal_mode();
		
		//DELAY is set to change, giving proression in dificulty
		DELAY = max(0.1 - 0.002*(double)(score/10), 0.03);
		
		while (!kbhit() && t1-t2 < DELAY) t1 = time_now();
		if (t1-t2 < DELAY) input = getch();
		t2 = t1;
		
		reset_terminal_mode();
		system ("clear");
		/*----------------*/
		
		/* INPUT */		
		int x_before = first.x;
		int y_before = first.y;
		
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
		//MISSING: SPECIAL FRUIT
		//Hits Wall
		if (first.x == WIDTH || first.x == -1 || first.y == HEIGHT || first.y == -1) gameOver = true;
		
		//Hits Tail
		if (hitsBody (&first, first.x, first.y)) gameOver = true;
		
		//Hits Fruit
		if (fx == first.x && fy == first.y) {
			score += 10;
			last = createNode (last ? last : &first, fx, fy);
			resetFruit (&first, &fx, &fy);
		}
		
		//Tail Follows
		if (last != NULL) {
			last->x = x_before;
			last->y = y_before;
			if (last->before == &head)
				last = last->before->before;
			else
				last = last->before;
		}
		/*-------*/
		
		/* DRAW */
		cout << "Score: " << score << endl;
		for (int i = 0; i <= WIDTH+1; i++) cout << '#';
		cout << endl;
		
		for (int y = 0; y < HEIGHT; y++) {
		cout << '#';
			for (int x = 0; x < WIDTH; x++) {
				if (belongsList (&first, x, y))	cout << 'O';
				else if (x == fx && y == fy)	cout << 'x';
				else 							cout << ' ';
			}
		cout << '#' << endl;
		}
		
		for (int i = 0; i <= WIDTH+1; i++) cout << '#';
		cout << endl;
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



















