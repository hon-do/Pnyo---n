#include<stdio.h> //printf使える
#include<stdlib.h> //描画前に画面クリア
#include<string.h> //文字列コピーしたりできる
#include<time.h> //リアルタイム処理使うため時間取得
#include<conio.h>

#define FIELD_WIDTH 8
#define FIELD_HEIGHT 14 //上と下ブロック12+2

#define PUYO_START_X 3
#define PUYO_START_Y 1

#define PUYO_COLOR_MAX 4

//フィールドの情報を定義
enum {
	CELL_NONE,
	CELL_WALL,
	CELL_PUYO_0,
	CELL_PUYO_1,
	CELL_PUYO_2,
	CELL_PUYO_3,
	CELL_MAX
};

enum {
	PUYO_ANGLE_0,
	PUYO_ANGLE_90,
	PUYO_ANGLE_180,
	PUYO_ANGLE_270,
	PUYO_ANGLE_MAX
};
//それぞれの回転の時Subはどこにいるのか相対座標
int puyoSubPositions[][2] = {
	{0,-1},//PUYO_ANGLE_0,
	{-1,0},//PUYO_ANGLE_90,
	{0,1},//PUYO_ANGLE_180,
	{1,0}//PUYO_ANGLE_270,
};

int cells[FIELD_HEIGHT][FIELD_WIDTH];
//描画用bufferにフィールドを書き込み その上に組みぷよを書き込む→合成されたbufferを描画する
int displayBuffer[FIELD_HEIGHT][FIELD_WIDTH];

//探索済みを重複しないようにフラグ立てる
int checked[FIELD_HEIGHT][FIELD_WIDTH];

//全角2byte + null 1
char cellNames[][2 + 1] = {
	"・",// CELL_NONE,
	"■",// CELL_WALL,
	"○",// CELL_PUYO_0,
	"△",// CELL_PUYO_1,
	"□",// CELL_PUYO_2,
	"☆",// CELL_PUYO_3,
};


int puyoX = PUYO_START_X,
puyoY = PUYO_START_Y;
int puyoColor;
int puyoAngle;

//ぷよが消えた時ロックがかかる
bool lock = false;


//描画は複数のところで使うので関数にしちゃう
void display() {
	system("cls"); //コンソールクリア
		//displayBufferにcellsのデータをsizeぶんコピーする
	memcpy(displayBuffer, cells, sizeof cells);

	if (!lock) {
		//Sub位置定義
		int subX = puyoX + puyoSubPositions[puyoAngle][0];
		int subY = puyoY + puyoSubPositions[puyoAngle][1];
		//ぷよ描画
		displayBuffer[puyoY][puyoX] =
			displayBuffer[subY][subX] = CELL_PUYO_0 + puyoColor;
	}
	for (int y = 1; y < FIELD_HEIGHT; y++) {
		for (int x = 0; x < FIELD_WIDTH; x++)
			printf("%s", cellNames[displayBuffer[y][x]]);
		printf("\n");
		
	}
}

//ぷよと壁の当たり判定 仮の座標で判定し、当たらなければ移動または回転可能
bool intersectPuyoToField(int _puyoX, int _puyoY, int _puyoAngle) {
	if (cells[_puyoY][_puyoX] != CELL_NONE)
		return true;

	int subX = _puyoX + puyoSubPositions[_puyoAngle][0];
	int subY = _puyoY + puyoSubPositions[_puyoAngle][1];

	if (cells[subY][subX] != CELL_NONE)
		return true;

	return false;
}

//この中で必要なら_count加算して
int getPuyoConnectedCount(int _x, int _y, int _cell, int _count) {
	if (
		checked[_y][_x]
		|| (cells[_y][_x] != _cell)
		)
		return _count;

	_count++;
	checked[_y][_x] = true;

	for (int i = 0; i < PUYO_ANGLE_MAX; i++) {
		int x = _x + puyoSubPositions[i][0];
		int y = _y + puyoSubPositions[i][1];
		_count = getPuyoConnectedCount(x, y, _cell, _count);
	}

	return _count;
	
}

void erasePuyo(int _x, int _y, int _cell) {
	if (cells[_y][_x] != _cell)
		return;

	cells[_y][_x] = CELL_NONE;

	for (int i = 0; i < PUYO_ANGLE_MAX; i++) {
		int x = _x + puyoSubPositions[i][0];
		int y = _y + puyoSubPositions[i][1];
		erasePuyo(x, y, _cell);
	}

}

int main() {
	//キャスとしてから渡す
	srand((unsigned int)time(NULL));

	for (int y = 0; y < FIELD_HEIGHT; y++)
		//左端と右端ブロック 
		cells[y][0] =
		cells[y][FIELD_WIDTH - 1] = CELL_WALL;

	//下端ブロック
	for (int x = 0; x < FIELD_WIDTH; x++)
		cells[FIELD_HEIGHT - 1][x] = CELL_WALL;

	puyoColor = rand() % PUYO_ANGLE_MAX;



	time_t t = 0;
	while (1) {
		if (t < time(NULL)) {
			t = time(NULL);
			if (!lock) {
				if (!intersectPuyoToField(puyoX, puyoY + 1, puyoAngle)) {

					//時間経過したらぷよ落下
					puyoY++;
				}
				else {
					int subX = puyoX + puyoSubPositions[puyoAngle][0];
					int subY = puyoY + puyoSubPositions[puyoAngle][1];

					cells[puyoY][puyoX] =
						cells[subY][subX] = CELL_PUYO_0 + puyoColor;

					//
					puyoX = PUYO_START_X;
					puyoY = PUYO_START_Y;
					puyoAngle = PUYO_ANGLE_0;
					puyoColor = rand() % PUYO_ANGLE_MAX;

					lock = true;
				}
			}

			if (lock) {
				lock = false;
				for (int y = FIELD_HEIGHT - 3; y >= 0; y--)
					for (int x = 1; x < FIELD_WIDTH - 1; x++)
						if (
							(cells[y][x] != CELL_NONE)
							&& (cells[y + 1][x] == CELL_NONE)
							) {
							cells[y + 1][x] = cells[y][x];
							cells[y][x] = CELL_NONE;

							lock = true;
						}

				if (!lock) {
					memset(checked, 0, sizeof checked);
					for (int y = 0; y < FIELD_HEIGHT - 1; y++)
						for (int x = 1; x < FIELD_WIDTH - 1; x++) {
							if(cells[y][x]!=CELL_NONE)
								if (getPuyoConnectedCount(x, y, cells[y][x], 0) >= 4) {
									erasePuyo(x, y, cells[y][x]);
									lock = true;
								}
						}
				}
			}

			display();

		}


		//キーボード入力なければ止まらないように　入力あった時だけ受け付ける
		if (_kbhit()) {
			if (lock)
				_getch();
			else {
				//puyoの影武者を作る
				int x = puyoX;
				int y = puyoY;
				int angle = puyoAngle;
				//キーボード入力をswitchに戻り値として渡す
				switch (_getch()) {
				case 's':y++; break;
				case 'a':x--; break;
				case 'd':x++; break;
				case ' ':angle = (++angle) % PUYO_ANGLE_MAX;
				}
				if (!intersectPuyoToField(x, y, angle)) {
					puyoX = x;
					puyoY = y;
					puyoAngle = angle;
				}

				display();
			}
		}
	}
}