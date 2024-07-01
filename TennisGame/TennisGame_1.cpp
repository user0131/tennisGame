#include "DxLib.h"
#include<stdlib.h>
#include<time.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	const int WIDTH = 960, HEIGHT = 640;

	SetWindowText("テニスゲーム");
	SetGraphMode(WIDTH, HEIGHT, 32);
	ChangeWindowMode(TRUE);
	if (DxLib_Init() == -1) return -1;
	SetBackgroundColor(0, 0, 0);
	SetDrawScreen(DX_SCREEN_BACK);

	//ボールを動かすための変数
	int ballX = 40;
	int ballY = 80;
	int ballVx = 5;
	int ballVy = 5;
	int ballR = 10;
	srand((unsigned int)time(NULL));

	//ラケットを動かすための変数
	int racketX = WIDTH / 2;
	int racketY = HEIGHT -50;
	int racketW = 120;
	int racketH = 12;

	//ゲーム進行に関する変数、スコアを代入する変数
	enum { TITLE, PLAY, OVER };
	int scene = TITLE;
	int timer = 0;
	int score = 0;
	int highScore = 1000;
	int dx, dy;

	//背景画像の読み込み
	int imgBg = LoadGraph("image/bg.png");

	//サウンドの読み込みを音量設定
	int bgm = LoadSoundMem("sound/bgm.mp3");
	int jin = LoadSoundMem("sound/gameover.mp3");
	int se = LoadSoundMem("sound/hit.mp3");
	ChangeVolumeSoundMem(128, bgm);
	ChangeVolumeSoundMem(128, jin);

	while (1)
	{
		ClearDrawScreen();
		DrawGraph(0, 0, imgBg, FALSE);
		timer++;

		switch (scene)
		{
		case TITLE:
			SetFontSize(50);
			DrawString(WIDTH / 2 - 50 / 2 * 12 / 2, HEIGHT / 3, "Tennis Game", 0x00ff00);
			if (timer % 40 < 20) { //文字を点滅表示
				SetFontSize(30);
				DrawString(WIDTH / 2 - 30 / 2 * 21 / 2, HEIGHT * 2 / 3, "Press SPACE to start", 0x00ffff);
			}
			if (CheckHitKey(KEY_INPUT_SPACE) == 1)
			{
				ballX = 40;
				ballY = 80;
				ballVx = 5;
				ballVy = 5;
				racketX = WIDTH / 2;
				racketY = HEIGHT - 50;
				score = 0;
				scene = PLAY;
				PlaySoundMem(bgm, DX_PLAYTYPE_LOOP);//BGMをループ再生
			}
			break;

		case PLAY:
			//ボールを動かす
			ballX = ballX + ballVx;
			if (ballX < ballR || ballX > WIDTH - ballR) ballVx = -ballVx;
			ballY = ballY + ballVy;
			if (ballY < ballR || ballY < 0) ballVy = -ballVy;
			if (ballY > HEIGHT)
			{
				scene = OVER;
				timer = 0;
				StopSoundMem(bgm);
				PlaySoundMem(jin, DX_PLAYTYPE_BACK);//ジングルを出力
				break;
			}
			DrawCircle(ballX, ballY, ballR, 0xff0000, TRUE);
			DrawCircle(ballX - ballR / 4, ballY - ballR / 4, ballR / 2, 0xffa0a0, TRUE);
			DrawCircle(ballX - ballR / 4, ballY - ballR / 4, ballR / 4, 0xffffff, TRUE);

			//ラケットを動かす
			if (CheckHitKey(KEY_INPUT_LEFT) == 1)
			{
				racketX = racketX - 10;
				if (racketX < racketW / 2) racketX = racketW / 2;
			}
			if (CheckHitKey(KEY_INPUT_RIGHT) == 1)
			{
				racketX = racketX + 10;
				if (racketX > WIDTH - racketW / 2) racketX = WIDTH - racketW / 2;
			}
			DrawBox(racketX - racketW / 2 -2, racketY - racketH / 2 -2, racketX + racketW / 2, racketY + racketH / 2, 0x40c0ff, TRUE);
			DrawBox(racketX - racketW / 2, racketY - racketH / 2, racketX + racketW / 2 +2 , racketY + racketH / 2 +2, 0x0080ff, TRUE);
			DrawBox(racketX - racketW / 2, racketY - racketH / 2, racketX + racketW / 2, racketY + racketH / 2, 0x0080ff, TRUE);

			//ヒットチェック
		    dx = ballX - racketX;
			dy = ballY - racketY;
			if (-racketW / 2 - 10 < dx && dx < racketW / 2 + 10 && -20 < dy && dy < 0)
			{
				ballVy = -5 - rand() % 5;
				score = score + 100;
				if (score > highScore) highScore = score;
				PlaySoundMem(se, DX_PLAYTYPE_BACK);
			}
			break;

		case OVER:
			SetFontSize(40);
			DrawString(WIDTH / 2 - 40 / 2 * 9 / 2, HEIGHT / 3, "GAME OVER", 0xff0000);
			if (timer > 60 * 2) scene = TITLE;
			break;
		}

		SetFontSize(30);
		DrawFormatString(10, 10, 0xffff00, "SCORE %d", score);
		DrawFormatString(WIDTH - 200, 10, 0xffff00, "HI-SC %d", highScore);

		ScreenFlip();
		WaitTimer(16);
		if (ProcessMessage() == -1) break;
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) break;
	}
	
	DxLib_End();
	return 0;
}