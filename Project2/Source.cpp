#include<stdio.h> //printf�g����
#include<stdlib.h> //�`��O�ɉ�ʃN���A
#include<string.h> //������R�s�[������ł���
#include<time.h> //���A���^�C�������g�����ߎ��Ԏ擾
#include<conio.h>

#define FIELD_WIDTH 8
#define FIELD_HEIGHT 14 //��Ɖ��u���b�N12+2

#define PUYO_START_X 3
#define PUYO_START_Y 1

#define PUYO_COLOR_MAX 4

//�t�B�[���h�̏����`
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
//���ꂼ��̉�]�̎�Sub�͂ǂ��ɂ���̂����΍��W
int puyoSubPositions[][2] = {
	{0,-1},//PUYO_ANGLE_0,
	{-1,0},//PUYO_ANGLE_90,
	{0,1},//PUYO_ANGLE_180,
	{1,0}//PUYO_ANGLE_270,
};

int cells[FIELD_HEIGHT][FIELD_WIDTH];
//�`��pbuffer�Ƀt�B�[���h���������� ���̏�ɑg�݂Ղ���������ށ��������ꂽbuffer��`�悷��
int displayBuffer[FIELD_HEIGHT][FIELD_WIDTH];

//�T���ς݂��d�����Ȃ��悤�Ƀt���O���Ă�
int checked[FIELD_HEIGHT][FIELD_WIDTH];

//�S�p2byte + null 1
char cellNames[][2 + 1] = {
	"�E",// CELL_NONE,
	"��",// CELL_WALL,
	"��",// CELL_PUYO_0,
	"��",// CELL_PUYO_1,
	"��",// CELL_PUYO_2,
	"��",// CELL_PUYO_3,
};


int puyoX = PUYO_START_X,
puyoY = PUYO_START_Y;
int puyoColor;
int puyoAngle;

//�Ղ悪�����������b�N��������
bool lock = false;


//�`��͕����̂Ƃ���Ŏg���̂Ŋ֐��ɂ����Ⴄ
void display() {
	system("cls"); //�R���\�[���N���A
		//displayBuffer��cells�̃f�[�^��size�Ԃ�R�s�[����
	memcpy(displayBuffer, cells, sizeof cells);

	if (!lock) {
		//Sub�ʒu��`
		int subX = puyoX + puyoSubPositions[puyoAngle][0];
		int subY = puyoY + puyoSubPositions[puyoAngle][1];
		//�Ղ�`��
		displayBuffer[puyoY][puyoX] =
			displayBuffer[subY][subX] = CELL_PUYO_0 + puyoColor;
	}
	for (int y = 1; y < FIELD_HEIGHT; y++) {
		for (int x = 0; x < FIELD_WIDTH; x++)
			printf("%s", cellNames[displayBuffer[y][x]]);
		printf("\n");
		
	}
}

//�Ղ�ƕǂ̓����蔻�� ���̍��W�Ŕ��肵�A������Ȃ���Έړ��܂��͉�]�\
bool intersectPuyoToField(int _puyoX, int _puyoY, int _puyoAngle) {
	if (cells[_puyoY][_puyoX] != CELL_NONE)
		return true;

	int subX = _puyoX + puyoSubPositions[_puyoAngle][0];
	int subY = _puyoY + puyoSubPositions[_puyoAngle][1];

	if (cells[subY][subX] != CELL_NONE)
		return true;

	return false;
}

//���̒��ŕK�v�Ȃ�_count���Z����
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
	//�L���X�Ƃ��Ă���n��
	srand((unsigned int)time(NULL));

	for (int y = 0; y < FIELD_HEIGHT; y++)
		//���[�ƉE�[�u���b�N 
		cells[y][0] =
		cells[y][FIELD_WIDTH - 1] = CELL_WALL;

	//���[�u���b�N
	for (int x = 0; x < FIELD_WIDTH; x++)
		cells[FIELD_HEIGHT - 1][x] = CELL_WALL;

	puyoColor = rand() % PUYO_ANGLE_MAX;



	time_t t = 0;
	while (1) {
		if (t < time(NULL)) {
			t = time(NULL);
			if (!lock) {
				if (!intersectPuyoToField(puyoX, puyoY + 1, puyoAngle)) {

					//���Ԍo�߂�����Ղ旎��
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


		//�L�[�{�[�h���͂Ȃ���Ύ~�܂�Ȃ��悤�Ɂ@���͂������������󂯕t����
		if (_kbhit()) {
			if (lock)
				_getch();
			else {
				//puyo�̉e���҂����
				int x = puyoX;
				int y = puyoY;
				int angle = puyoAngle;
				//�L�[�{�[�h���͂�switch�ɖ߂�l�Ƃ��ēn��
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