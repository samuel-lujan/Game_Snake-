#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>

/*DEFINE THE SIDES OF THE MOVE*/
	#define UP 1
	#define LEFT 2
	#define DOWN 3
	#define RIGHT 4

//DEFINE THE MAX SIZE OF THE SNAKE
	#define MAX_LENGTH 50

//GLOBAL VARIABLES
	int width = 35, height = 35;
	int x, y, foodX, foodY, large, flag, vel=350, loseX, loseY;
	bool gameover = false, generatede=false;

/*
*STRUCT THAT DEFINE THE SNAKE
*/
	struct snake{
		int headX, headY;
		int tailX[MAX_LENGTH], tailY[MAX_LENGTH];
		int score;
		bool gameover;
	};


/*
*SNAKES CHECKLIST
*/
	void create_snake(struct snake *s);
	bool is_null(struct snake *s);
	bool is_full(struct snake *s);
	int get_last_x(struct snake *s);
	int get_last_y(struct snake *s);
	void add_tail(struct snake *s);
	void drop_tail(struct snake *s);
	void walk(struct snake *s, int side);

/*
*GAME FUNCTIONS
*/
	void food_generator(void);
	bool is_valid(int *x, int *y);
	void food(void);
	void lose_generator(void);
	void lose();
	void setup(struct snake *s);
	void draw(struct snake *s);
	int kbhit(void);
	void input(struct snake *s);
	void update(struct snake *s);

/*
*MAIN
*/
int main(void){
	struct snake *snake_game = (struct snake *) malloc(sizeof(struct snake));
	setup(snake_game);
	draw(snake_game);
	while(snake_game->gameover==false){	
		usleep(vel*1000);
		if(kbhit()){
			input(snake_game);
		}

		system("clear");
		draw(snake_game);
		update(snake_game);
	}
	return 0;
}

/*
*DOING THE SNAKES CHECKLIST
*/
	void create_snake(struct snake *s){//CREATE AND SETUP ALL THE INFOS ABOUT SNAKE
		s->headX = width/2;
		s->headY = height/2;
		s->tailX[s->score] = -1;
		s->tailY[s->score] = -1;
		s->score = 0;
		s->gameover = false;
	}

	bool is_null(struct snake *s){//CHECKS IF THE SNAKE IS NULL (LINE)
		if(((s->score) == 0)){
			return true;
		}else{
			return false;
		}
	}

	bool is_full(struct snake *s){//CHECKS IF THE SNAKE IS FULL (LINE)
		if(((s->score) == MAX_LENGTH-1)){
			return true;
		}else{
			return false;
		}
	}

	int get_last_x(struct snake *s){//RETURNS THE LAST POINT X OF THE SNAKE
		if(is_null(s)){
			return s->headX;
		}
		else if(!is_full(s)){
			return s->tailX[s->score]; 
		}
	}

	int get_last_y(struct snake *s){//RETURNS THE LAST POINT Y OF THE SNAKE
		if(is_null(s)){
			return s->headY;
		}
		else if(!is_full(s)){
			return s->tailY[s->score];
		}	
	}

	void add_tail(struct snake *s){//ADD A POINT TO THE TAIL (LINE)
		s->score++;
		/*VERIFY WHERE THE TAIL IS GOING TO*/
		switch(flag){
				case UP:{
					s->tailX[s->score] = get_last_x(s);
					s->tailY[s->score] = get_last_y(s)+1;
					break;
				}
				case DOWN:{
					s->tailX[s->score] = get_last_x(s);
					s->tailY[s->score] = get_last_y(s)-1;
					break;
				}
				case LEFT:{
					s->tailX[s->score] = get_last_x(s)+1;
					s->tailY[s->score] = get_last_y(s);
					break;
				}
				case RIGHT:{
					s->tailX[s->score] = get_last_x(s)-1;
					s->tailY[s->score] = get_last_y(s);
					break;
				}
				default:{break;}

			}
	}

	void drop_tail(struct snake *s){//DROPS THE FIRST DOT OF THE SNAKE (LINE)
		if(!is_null(s)){
			for(int i =0; i<s->score; i++){
				s->tailX[i++] = s->tailX[i];
				s->tailY[i++] = s->tailY[i];
			}
			s->score--;
		}else{
			s->gameover = true;
		}
	}
	
	void snake_walk(struct snake *s){//WALK WITH THE HEAD OF THE SNAKE
		int vetPrevX = s->tailX[0];
		int vetPrevY = s->tailY[0];
		int vet2PrevX, vet2PrevY;
		s->tailX[0] = s->headX; 
		s->tailY[0] = s->headY;


		for(int i =1; i<(s->score) ;i++){
			vet2PrevX = s->tailX[i];
			vet2PrevY = s->tailY[i];
			s->tailX[i] = vetPrevX;
			s->tailY[i] = vetPrevY; 
			vetPrevX = vet2PrevX;
			vetPrevY = vet2PrevY;
		}
		switch(flag){
				case UP:{
					s->headY--;
					break;
				}
				case DOWN:{
					s->headY++;
					break;
				}
				case LEFT:{
					s->headX--;
					break;
				}
				case RIGHT:{
					s->headX++;
					break;
				}
				default:{break;}
			}
	}

/*GAME FUNCTIONS*/
	void food_generator(){//GENERATE THE FOOD IN A RANDOM PLACE
		foodX = rand() % width-1;
		foodY = rand() % height-1;
	}

	bool is_valid(int *x, int *y){//CHECK IF THE POINT OF FOOD OR LOSS IS VALID
		if(*x <= 1 || *x>=height-1 || *y<=1 || *y>=width-1 || foodX==loseX || foodY==loseY){
				return false;
		}
		else{
			return true;
		}
	}

	void food(){//FUNCTION THAT CREAT AND RECREATE THE FOOD UNTILL GET A VALID POINT
		food_generator();
		while(is_valid(&(foodX), &(foodY))==false){
			food_generator();
		}
		return;
	}

	void lose_generator(){//GENERATE THE LOSS IN A RANDOM PLACE
		loseX = rand() % width-1;
		loseY = rand() % height-1;
	}

	void lose(){//FUNCTION THAT CREAT AND RECREATE THE LOSS UNTIL GET A VALID POINT
		lose_generator();
		while(is_valid( &(loseX), &(loseY))){
			lose_generator();
		}
		generatede = true;
		return;
	}

	void setup(struct snake *s){//SETTUP THE GAME
		create_snake(s);
		food();
		return;
	}

	void draw(struct snake *s){//DRAW THE FULL GAME
		if(s->gameover == true){
			printf("GAM& 0V&ER :) \n\n\a");
		}else{
			printf("             SCORE: %d\n", s->score);
			for(int i=0;i<width;i++){//ROWS
				for(int j=0; j<height;j++){
					if( (i==0 && j==0)  || (i==(height-1) && j==(width-1)) || (i==0 && j==(height-1)) || (i==(height-1) && j==0)) {//DRAW THE CORNERS
						printf("*");
					}
					else if( (i==0 && j>=1 && j<(width-2)) || (i==height-1 && j>=1 && j<(width-2)) ){//DRAW THE TOP AND BOTTOM SIDE
						printf("-");
					}
					else if(( j==0 || j==(width-1)) && (i>0 && i<(height-1))){//DRAW THE RIGHT AND THE LEFT SIDE
						printf("|");
					}
					else if(i==s->headY && j==s->headX){ //DRAW THE HEAD OF THE SNAKE
						printf("@");
					}
					else if(j==foodY && i==foodX){//DRAW THE FOOD
						printf(".");
					}
					else if(j==loseY && i==loseX){//DRAW THE LOSS
						printf("!");
					}
					else{//DRAW THE TAIL AND THE SPACES
						int ch =0;
						for(int k=0; k<s->score; k++){
							if(j==s->tailX[k] && i==s->tailY[k]){
								printf("#");
								ch=1;				
							}						
						}		
						if(ch==0){
							printf(" ");
						}
					}
				}
				printf("\n");
			}
		}
	}

	int kbhit(){//A EXTERN FUNCTION THAT READS THE INTERRUPTION IN THE KEYBOARD
	  struct termios oldt, newt;
	  int ch;
	  int oldf;
	 
	  tcgetattr(STDIN_FILENO, &oldt);
	  newt = oldt;
	  newt.c_lflag &= ~(ICANON | ECHO);
	  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
	 
	  ch = getchar();
	 
	  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	  fcntl(STDIN_FILENO, F_SETFL, oldf);
	 
	  if(ch != EOF)
	  {
	    ungetc(ch, stdin);
	    return 1;
	  }
	 
	  return 0;
	}

	void input(struct snake *s){//CHECK THE INPUT OF THE USER AND SET THE WAY OF THE SNAKE
		switch(getchar()){
			case 'a':{
				if(flag!= RIGHT){
					flag = LEFT;
				}
				break;
			}
			case'A':{
				if(flag!=RIGHT){
					flag = LEFT;
				}
				break;
			}
			case 'd':{
				if(flag!=LEFT){
					flag = RIGHT;
				}
				break;
			}
			case 'D':{
				if(flag!=LEFT){
					flag = RIGHT;
				}
				break;
			}
			case 's':{
				if(flag!=UP){
					flag = DOWN;
				}
				break;
			}
			case 'S':{
				if(flag!=UP){
					flag = DOWN;
				}
				break;
			}
			case 'w':{
				if(flag!=DOWN){
					flag = UP;
				}
				break;
			}case 'W':{
				if(flag!=DOWN){
					flag = UP;
				}
				break;
			}
			case 'o':{
				s->gameover =1;
				break;
			}
			case 'O':{
				s->gameover =1;
				break;
			}
			default:{break;}
		}
	}

	void update(struct snake *s){//DO ALL THE CHECKS TO SEE IF EAT A FOOD OR A LOSS OR IF IT IS IN THE WALL
		if( (s->headX == foodY) && (s->headY==foodX)){//THE SNAKE EATING THE FOOD
			printf("\a");
			food();
			if(s->score%5==0 && vel>50){
				vel=vel-50;
			}
			if(s->score%5==0 && vel==50){
				vel=vel-25;
			}
			
			add_tail(s);
		}
		
		if(s->score%2==0 && s->score>5 && generatede == false){
				lose();
		}
		if( (s->headX == loseY) && (s->headY==loseX)){
			drop_tail(s);
			generatede = false;
			lose();
			if(s->score%2==0 && s->score>5 && generatede==false){
				lose();
			}else{
				generatede=false;
			}
		}
		if( (s->headX==0) || (s->headX==width-1) || (s->headY==0) || (s->headY==height-1)){//THE SNAKE HAS CRASHED
			s->gameover = true;
			printf("GAM& 0V&ER :'(\a\n\n");
			
		}
		snake_walk(s);
	}
