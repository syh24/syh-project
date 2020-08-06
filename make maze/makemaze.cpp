#include <iostream>
#include <fstream>
#include <ctime>

#define GO_LEFT 0
#define GO_RIGHT 1
#define GO_UP 2
#define GO_DOWN 3
using namespace std;
int flag = 0;

ofstream file("maze.maz");

void HAK(int, int, int**, char**,int ,int );

void main()

{
	int M, N;
	cin >> N >> M;
	int sm = 2 * M + 1;
	int sn = 2 * N + 1;
	int** ar;
	char** st;
	ar = new int*[M+2];
	for (int i = 0; i < M+2; i++)
	{
		ar[i] = new int[N+2];
	}
	for (int i = 0; i < M + 2; i++) {
		ar[i][0] = 1;
		ar[i][N + 1] = 1;
	}
	for (int i = 0; i < N + 2; i++)
	{
		ar[0][i] = 1;
		ar[M + 1][i] = 1;
	}
	st = new char*[sm];

	for (int i = 0; i < sm; i++)
	{
		st[i] = new char[sn];
	}
	for (int i = 0; i < sm; i++)
	{
		for (int j = 0; j < sn; j++)
		{
			if (i % 2 == 0 && j % 2 == 0) st[i][j] = '+';
			if (i % 2 == 0 && j % 2 == 1) st[i][j] = '-';
			if (i % 2 == 1 && j % 2 == 0) st[i][j] = '|';
			if (i % 2 == 1 && j % 2 == 1) st[i][j] = ' ';
		}
	}
	HAK(N, M, ar, st,0,0);
	for (int i = 0; i < sm; i++)
	{
		for (int j = 0; j < sn; j++)
		{
			file << st[i][j];
		}
		file << endl;
	}
	for (int i = 0; i < sm; i++)
	{
		for (int j = 0; j < sn; j++)
		{
			cout << st[i][j];
		}
		cout << endl;
	}

	delete st;
	delete ar;
}
void HAK(int N, int M, int** ar, char** st,int x,int y)
{
	srand((unsigned int)time(NULL));
	int rn, k, rm;
	if (!flag) {
		rn = rand() % N;
		rm = rand() % M;
		ar[rm + 1][rn + 1] = 1;
	}
	if (flag==1)
	{
		rn = x - 1;
		rm = y - 1;
		ar[y][x] = 1;
		st[rm * 2 + 1][rn * 2] = ' ';
	}
	if (flag == 2)
	{
		rn = x - 1;
		rm = y - 1;
		ar[y][x] = 1;
		st[rm * 2 + 1][(rn + 1) * 2] = ' ';
	}
	if (flag == 3)
	{
		rn = x - 1;
		rm = y - 1;
		ar[y][x] = 1;
		st[(rm + 1) * 2][rn * 2 + 1] = ' ';
	}
	if (flag == 4)
	{
		rn = x - 1;
		rm = y - 1;
		ar[y][x] = 1;
		st[rm * 2][rn * 2 + 1] = ' ';
	}
	while (1) {
		k = rand() % 4;
		if (ar[rm + 2][rn + 1] == 1 && ar[rm][rn + 1] == 1 && ar[rm + 1][rn + 2] == 1 && ar[rm + 1][rn] == 1) break;

		if (ar[rm + 2][rn + 1] == 1 && k == GO_DOWN) continue;
		if (ar[rm][rn + 1] == 1 && k == GO_UP) continue;
		if (ar[rm + 1][rn + 2] == 1 && k == GO_RIGHT) continue;
		if (ar[rm + 1][rn] == 1 && k == GO_LEFT) continue;

		switch (k) 
		{
			case GO_LEFT:
				st[rm * 2 + 1][rn * 2] = ' ';
				rn --; 
				break;
			case GO_RIGHT:
				st[rm * 2 + 1][(rn + 1) * 2] = ' ';
				rn ++;
				break;
			case GO_UP:
				st[rm * 2][rn * 2 + 1] = ' ';
				rm --;
				break;
			case GO_DOWN:
				st[(rm + 1) * 2][rn * 2 + 1] = ' ';
				rm ++;
				break;
		}
		ar[rm + 1][rn + 1] = 1;
	}
	int cnt = 0;
	for (int i = 1; i < M+1; i++)
	{
		for (int j = 1; j < N+1; j++)
		{
			if (ar[i][j] == 1) cnt++;
			else {
				if (j!=1 && ar[i][j-1] == 1)
				{
					flag = 1;
					HAK(N, M, ar, st, j, i);
				}
				else if (j!=N && ar[i][j + 1] == 1)
				{
					flag = 2;
					HAK(N, M, ar, st, j, i);
				}
				else if (i!=M && ar[i + 1][j]==1)
				{
					flag = 3;
					HAK(N, M, ar, st, j, i);
				}
				else if (i !=1 && ar[i - 1][j]==1)
				{
					flag = 4;
					HAK(N, M, ar, st, j, i);
				}
			}

		}
	}
	if (cnt == N * M) return;
}

