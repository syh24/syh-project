#include "tetris.h"

static struct sigaction act, oact;
int main(){
	int exit=0;
	initscr();
	noecho();
	keypad(stdscr, TRUE);	
	createRankList();
	srand((unsigned int)time(NULL));
	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;
		case MENU_RECOMENDED_PLAY: recommendedPlay(); break;
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}
	writeRankFile();
	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;
	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	for(i = 0;i<VISIBLE_BLOCKS;i++)
	    nextBlock[i] = rand()%7;
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();
	DrawBlock(blockY,blockX,nextBlock[0],blockRotate,' ');
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);
	move(9,WIDTH+10);
	printw("NEXT NEXT BLOCK");
	DrawBox(10,WIDTH+10,4,8);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(16,WIDTH+10);
	printw("SCORE");
	DrawBox(17,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(18,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
	for( i = 0; i < 4; i++ ){
		move(11+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[2]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
 	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}
	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

/////////////////////////첫주차 실습에서 구현해야 할 함수/////////////////////////

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (block[currentBlock][blockRotate][i][j] == 1)
			{
				if (i + blockY >= HEIGHT) return 0;
				if (j + blockX < 0) return 0;
				if (j + blockX >= WIDTH) return 0;
				if (f[i + blockY][j + blockX] == 1) return 0;
			}
		}
	}
	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	// user cod
	//1. 이전 블록 정보를 찾는다. ProcessCommand의 switch문을 참조할 것
	//2. 이전 블록 정보를 지운다. DrawBlock함수 참조할 것.
	//3. 새로운 블록 정보를 그린다. 
	int lastX = blockX;
	int lastY = blockY;
	int lastR = blockRotate;
	switch (command) {
	case KEY_UP:
		lastR = (blockRotate + 3) % 4;
		break;
	case KEY_DOWN:
		lastY -= 1;
		break;
	case KEY_RIGHT:
		lastX -= 1;
		break;
	case KEY_LEFT:
		lastX += 1;
		break;
	default:
		break;
	}
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++)
		{
		        
		        if (block[currentBlock][lastR][i][j]==1 &&i + lastY >= 0)
		        {
				move(i+lastY+1,j+lastX+1);
			printw(".");
		        }
		}
	}
	DrawField();
	DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);
	move(HEIGHT,WIDTH+10);	
}

void BlockDown(int sig){
	// user code
	//강의자료 p26-27의 플로우차트를 참고한다.

  	if(Flag == 1)
	{
	    initR();
	    modified_recommend(recRoot);
	    blockX = recommendX;
	    blockY = recommendY;
	    blockRotate = recommendR;
	}

	if (CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX)) {
		blockY++;
		DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
	}
	else{
		if (!CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX)){
			gameOver = 1;
			timed_out = 0;
			return;
		}
		else
		{
		        score += AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
			score += DeleteLine(field);
			for(int i = 0; i<VISIBLE_BLOCKS-1;i++)
			{
			    nextBlock[i] = nextBlock[i+1];
			}
			nextBlock[VISIBLE_BLOCKS-1] = rand()%7;
			DrawNextBlock(nextBlock);
			if(Flag==0){
				initR();
				modified_recommend(recRoot);
			}
			blockY = -1;
			blockX = WIDTH / 2 - 2;
			blockRotate = 0;
			PrintScore(score);
			DrawField();
		}
		
	}
        DrawBlock(blockY,blockX,nextBlock[0],blockRotate,' ');
	timed_out = 0;
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
        
	// user code
	int touched = 0;
	int k=0;
	//Block이 추가된 영역의 필드값을 바꾼다.
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++){
			if (block[currentBlock][blockRotate][i][j] == 1){
				f[blockY + i][blockX + j] = 1;
				if(i+blockY == HEIGHT - 1)
			    		touched++;
				if(f[i+blockY+1][j+blockX] == 1)
					touched++;
			}
		}
	return touched * 10;
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	// user code

	//1. 필드를 탐색하여, 꽉 찬 구간이 있는지 탐색한다.
	//2. 꽉 찬 구간이 있으면 해당 구간을 지운다. 즉, 해당 구간으로 필드값을 한칸씩 내린다.
	int i,j;
	int line = 0;
	for (i = 0; i < HEIGHT; i++) {
		for (j = 0; j < WIDTH; j++) {
			if (f[i][j] == 0)
			{
				break;
			}
		}
		if (j == WIDTH)
		{
			line++;
			for (int p = i ; p > 0; p--)
				for (int q = 0; q <WIDTH; q++)
				{
					f[p][q] = f[p-1][q];
				}
		}
	}
	int score = line * line * 100;
	return score;
}

///////////////////////////////////////////////////////////////////////////
void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate){
    DrawRecommend(recommendY,recommendX,blockID,recommendR); 
    DrawShadow(y,x,blockID,blockRotate);
    DrawBlock(y,x,blockID,blockRotate,' ');
}
void DrawShadow(int y, int x, int blockID,int blockRotate){
    while(CheckToMove(field,blockID,blockRotate,y+1,x))
    {
	y++;
    }    
    DrawBlock(y,x,blockID,blockRotate,'/');	
}
void createRankList(){
	// user code
    Node *temp;
    int score = 0;
    char name[NAMELEN];  
    Node* curr;
    FILE* fp;
    fp = fopen("rank.txt","r");
    if(fp == NULL) {
	fp = fopen("rank.txt","w");
	fprintf(fp,"0\n");
    }

    fscanf(fp,"%d",&N);
    for(int i = 0;i<N;i++)
    {
	temp = (Node*)malloc(sizeof(Node));
	fscanf(fp, "%s %d", name, &score);
	temp->score = score;
	strcpy(temp->name,name);
	if(head == NULL){
	    temp->link = head;
	    head = temp;
	}
	else
	{
	    curr = head;
	    while(curr->link !=NULL) curr = curr->link;
	    curr->link = temp;
	}
    }
    fclose(fp);
}

void rank(){
	// user code
    int X= 1, Y = N;
    int k = 0;
    int cnt = 0;
    int rank = 100;
    char rank_name[NAMELEN];
    Node* temp = head;
    Node* curr = head;
    Node* pre;
    clear();
    printw("1. list ranks from X to Y\n");
    printw("2. list ranks by a specific name\n");
    printw("3. delete a specific rank\n");
    switch(wgetch(stdscr)){
	case '1':
	    echo();
	    printw("X: ");
	    scanw("%d", &X);
	    printw("Y: ");
	    scanw("%d", &Y);

	    printw("      name       |     score        \n");
	    printw("------------------------------------\n");

	    if(X>Y || X>N || Y>N)
	    {
		printw("\nsearch failure: no rank in the list\n");
		
	    }
	    else
	    {
		for(int i = 1;i<=Y;i++){
			if(i >= X && i<= Y)
			{
			    printw("%-17s|    %d\n",curr->name,curr->score);
			}
			curr = curr->link;
		}
	    }
	    noecho();
	    break;
	case '2':
	    echo();
	    printw("input the name: ");
	    getstr(rank_name);

	    printw("      name       |     score        \n");
	    printw("------------------------------------\n");

	    while(temp != NULL)
	    {	
		if(strcmp(rank_name,temp->name) == 0)
		{
		    printw("%-17s|     %d\n",temp->name,temp->score);
		    k = 100;
		}
		temp = temp->link;
		k++;
	    }
	    if(k == N)
	    {
		printw("\nsearch failure: no rank in the list\n");
	    }
	    noecho();
	    break;
	case '3':
	    echo();
	    printw("input the rank : ");
	    scanw("%d",&rank);
	    if(rank>N || rank < 1){
		printw("\nsearch failure: no rank in the list\n");
		break;
	    }

	    if(rank == 1)
	    {
		    curr = temp;
		    temp= curr->link;
		    head =temp;
		    free(curr);
		    printw("\nresult: the rank deleted\n");
		    N--;
		    writeRankFile();
	    }
	    if(rank >=2)
	    {
		pre = head;
		for(int i = 0;i<rank-2;i++) pre = pre->link;
		curr = pre->link;
		pre->link = curr->link;
		free(curr);
		printw("\nresult: the rank deleted\n");
		N--;
		writeRankFile();
	    }
	    noecho();
	    break;
    }
    getch();
}

void writeRankFile(){
	// user code
    FILE *fp;
    Node* temp = head;
    fp = fopen("rank.txt","w");
    fprintf(fp,"%d\n",N);
    while(1)
    {
	if(temp == NULL) break;
	fprintf(fp,"%s  %d\n",temp->name, temp->score);
	temp = temp->link;
    }
    fclose(fp);
    
	
}

void newRank(int score){
	// user code
    Node *temp;
    Node *t = head;   
    temp = (Node*)malloc(sizeof(Node));
    char name[NAMELEN];
    clear(); 
    printw("your name: ");
    echo();
    getstr(temp->name);
    noecho();
    N++;
    temp->score = score;

    if(head == NULL || score > t->score)
    {
	temp->link = head;
	head = temp;
    }
    else{
   	 while(t->link != NULL)
	 {
	     if(t->link->score <= temp->score){
		 break;
	     }
	     t = t->link;
	 } 
	 temp->link = t->link;
	 t->link = temp;
    }
    writeRankFile();
}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
    while(CheckToMove(field,blockID,blockRotate,y+1,x))
    {
	y++;
    }
    DrawBlock(y,x,blockID,blockRotate,'R');
}

int recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수
	int i,j,a,b,RecY;
	int k=0;
	int L = 0;
	int rot,sibling,A;
	int fl = 1;
	// user code
	if(nextBlock[root->lv]==1 || nextBlock[root->lv]==2 || nextBlock[root->lv]==3)
	{
		rot = 4;
		sibling = 34;
	}
	else if(nextBlock[root->lv] == 4)
	{
	    rot = 1;
	    sibling = 9;
	}
	else
	{
	    rot = 2;
	    sibling = 17;
	}
	
	RecNode **c = root->c;

	c = (RecNode**)malloc(sibling*sizeof(RecNode*));
	for(i = 0;i<sibling;i++){
		c[i] = (RecNode*)malloc(sizeof(RecNode));
	}


	for(i = 0; i<rot; i++){
	    for(j = -2; j<WIDTH+3;j++)
	    {
		if(!CheckToMove(root->f,nextBlock[root->lv],i,0,j)) continue;

		c[k]->lv = root->lv +1;
		c[k]->score = root->score;

		for(a = 0 ; a<HEIGHT; a++)
		    for(b = 0; b<WIDTH; b++)
			c[k]->f[a][b] = root->f[a][b];

	    	RecY = 0;
	    	while(CheckToMove(c[k]->f,nextBlock[root->lv],i,RecY+1,j))
    	    	{
		    	RecY++;
    	    	}
		c[k]->score += AddBlockToField(c[k]->f,nextBlock[root->lv],i,RecY,j);
		c[k]->score += DeleteLine(c[k]->f);
		L = c[k]->score;
		if(root->lv ==0){
		   	if(fl == 1){
		    		A = c[0]->score;
				fl = 0;
			}
			if(A > c[k]->score) continue;
			else A = c[k]->score;
		}


		if(c[k]->lv < VISIBLE_BLOCKS)
		{
		    L = recommend(c[k]);
		}

		if(L >= max) 
		{
		    if(root->lv ==0){
	     		recommendR = i;
		    	recommendY = RecY;
		    	recommendX = j;
		    }
		    max = L;
		}
		if(root->lv == 0)
		    free(c[k]);
		k++;
	    }
	}
	return max;
}

int modified_recommend(RecNode *root)
{
    	int max=0; 
	int i,j,a,b,RecY,t,e;
	int k=0;
	int L = 0;
	int rot,sibling;

	if(nextBlock[root->lv]==1 || nextBlock[root->lv]==2 || nextBlock[root->lv]==3)
	{
		rot = 4;
		sibling = 34;
	}
	else if(nextBlock[root->lv] == 4)
	{
	    rot = 1;
	    sibling = 9;
	}
	else
	{
	    rot = 2;
	    sibling = 17;
	}
	
	RecNode **c = root->c;

	c = (RecNode**)malloc(sibling*sizeof(RecNode*));
	for(i = 0;i<sibling;i++){
		c[i] = (RecNode*)malloc(sizeof(RecNode));
	}
	for(i = 0; i<rot; i++){
	    for(j = -2; j<WIDTH+1;j++)
	    {
		if(!CheckToMove(root->f,nextBlock[root->lv],i,0,j)) continue;

		c[k]->lv = root->lv +1;
		c[k]->score = root->score;

		for(a = 0 ; a<HEIGHT; a++)
		    for(b = 0; b<WIDTH; b++)
			c[k]->f[a][b] = root->f[a][b];

	    	RecY = 0;
	    	while(CheckToMove(c[k]->f,nextBlock[root->lv],i,RecY+1,j))
    	    	{
		    	RecY++;
    	    	}
  		c[k]->score += weightBlock(c[k]->f,nextBlock[root->lv],i,RecY,j);
		c[k]->score += DeleteLine(c[k]->f);
		L = c[k]->score;
		
		if(c[k]->lv < VISIBLE_BLOCKS)
		{
		    L = modified_recommend(c[k]);
		}
		else
		{
		    L += optimize(c[k]);
		}
		if(L >= max) 
		{
		    if(root->lv ==0){
	     		recommendR = i;
		    	recommendY = RecY;
		    	recommendX = j;
		    }
		    max = L;
		}
	    }
	}
	return max;


}
void initR()
{
    recRoot = (RecNode*)malloc(sizeof(RecNode));
    recRoot->lv = 0;
    recRoot->score = 0;
    for(int i =0; i<HEIGHT;i++)
	for(int j = 0 ; j<WIDTH; j++)
	    recRoot->f[i][j] = field[i][j];
    recommendX = 0; recommendY = 0; recommendR = 0;
}
int weightBlock(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	int touched = 0;
	int k=0;
	int result;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++){
			if (block[currentBlock][blockRotate][i][j] == 1){
				f[blockY + i][blockX + j] = 1;
				if(i+blockY == HEIGHT - 1)
			    		touched++;
				if(f[i+blockY+1][j+blockX] == 1)
					touched++;
				if(blockX+j==0 || blockX+j+1==WIDTH)
					k++;
			}
		}	
	result = touched*10 + k*3;
	return result;
}

int optimize(RecNode *root)
{
    int sum = 0;
    int hole,height,first;
    int max = 0;
    for(int i =0;i<WIDTH;i++){
	hole =0 ;
	first = 0;
	height =0;
	for(int j = 0; j<HEIGHT; j++){
	    if(root->f[i][j] == 1)
	    {
		height = HEIGHT - j;
		first = 1;
	    }
	    else
	    {
		if(first ==1)
			hole++;
	    }   
	}
	sum -= 5*hole;
	if(max < height)
	    max = height;
    }
    sum -= height * 10;
    return sum;
}

void recommendedPlay(){
	// user code
    Flag = 1;
    play();
    Flag = 0;
   
}
