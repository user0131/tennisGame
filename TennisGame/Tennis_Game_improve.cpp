#include "DxLib.h"
#include<stdlib.h>
#include<time.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	const int WIDTH = 960, HEIGHT = 640;

	SetWindowText("荷物運びゲーム");
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
	double speed = 1.0;
	srand((unsigned int)time(NULL));

	//ラケットを動かすための変数
	int racketX = WIDTH / 2;
	int racketY = HEIGHT -200;
	int racketW = 120;
	int racketH = 12;

	//ゲーム進行に関する変数、スコアを代入する変数
	enum { TITLE, PLAY, OVER, MENU, CLEAR };
	int scene = TITLE;
	int timer = 0;
	int score = 0;
	int highScore = 1000;
	int dx, dy;

	//背景画像の読み込み
	int imgBg = LoadGraph("image/bg.png");
	int imgSanta = LoadGraph("image/christmas_dance_santa.png");
	int imgPresent = LoadGraph("image/ballon_present.png");
	int imgLastBg = LoadGraph("image/LastBg.png");
	int imgDog[4]{
		LoadGraph("image/dog0.png"),
		LoadGraph("image/dog1.png"),
		LoadGraph("image/dog2.png"),
		LoadGraph("image/dog3.png"),
	};
	double dogX = 122-35;
	int dogY = HEIGHT - 70;

	//サウンドの読み込みを音量設定
	int bgm = LoadSoundMem("sound/bgm.mp3");
	int jin = LoadSoundMem("sound/gameover.mp3");
	int se = LoadSoundMem("sound/hit.mp3");
	int finish = LoadSoundMem("sound/finish.mp3");
	ChangeVolumeSoundMem(128, bgm);
	ChangeVolumeSoundMem(128, jin);

	while (1)
	{
		ClearDrawScreen();
		DrawExtendGraph(0, 0, 960, 640, imgBg, FALSE);
		timer++;

		switch (scene)
		{
		case TITLE:
			SetFontSize(50);
			DrawString(WIDTH / 2 - 50 / 2 * 13 , HEIGHT / 3, "落とさないで、サンタさん！", 0x00ff00);
			if (timer % 40 < 20) { //文字を点滅表示
				SetFontSize(30);
				DrawString(WIDTH / 2 - 30 / 2 * 13 , HEIGHT * 2 / 3, "スペースを押せば始まるよ", 0x00ffff);
			}
			if (CheckHitKey(KEY_INPUT_SPACE) == 1)
			{
				ballX = 40;
				ballY = 80;
				ballVx = 5;
				ballVy = 5;
				racketX = WIDTH / 2;
				racketY = HEIGHT - 200;
				score = 0;
				scene = PLAY;
				PlaySoundMem(bgm, DX_PLAYTYPE_LOOP);//BGMをループ再生
			}
			break;

		case PLAY:
			dogX = dogX +0.3;
			if (dogX > 378)
			{
				StopSoundMem(bgm);
				PlaySoundMem(finish, DX_PLAYTYPE_BACK);
				scene = CLEAR;
			}
			DrawExtendGraph(dogX, dogY,dogX+70 ,dogY+ 60, imgDog[(timer / 10) % 4], TRUE);
			
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
			DrawExtendGraph(racketX - racketW*2/3 , racketY - racketH-100 , racketX + racketW*2/3 , racketY + racketH+100 , imgSanta, TRUE);
			DrawBox(racketX - racketW / 2 - 2, racketY - racketH / 2 - 2, racketX + racketW / 2, racketY + racketH / 2, 0x40c0ff, TRUE);
			DrawBox(racketX - racketW / 2, racketY - racketH / 2, racketX + racketW / 2 + 2, racketY + racketH / 2 + 2, 0x0080ff, TRUE);
			DrawBox(racketX - racketW / 2, racketY - racketH / 2, racketX + racketW / 2, racketY + racketH / 2, 0x0080ff, TRUE);


			//ボールを動かす
			ballX = ballX + ballVx;
			if (ballX < ballR || ballX > WIDTH - ballR) ballVx = -ballVx;
			ballY = ballY + ballVy;
			if (ballY < ballR || ballY < 0) ballVy = -ballVy;
			if (CheckHitKey(KEY_INPUT_M) == 1) scene = MENU;
			if (ballY > HEIGHT)
			{
				scene = OVER;
				timer = 0;
				StopSoundMem(bgm);
				PlaySoundMem(jin, DX_PLAYTYPE_BACK);//ジングルを出力
				break;
			}
			DrawExtendGraph(ballX - 80, ballY - 120, ballX + 80, ballY+20 , imgPresent, TRUE);

			//ヒットチェック
			dx = ballX - racketX;
			dy = ballY - racketY;
			if (-racketW / 2 - 10 < dx && dx < racketW / 2 + 10 && -20 < dy && dy < 0)
			{
				ballVy = ( - 5 + -rand() % 5)*speed;
				ballVx = ballVx -rand() % 5;
				score = score + 100;
				if (score > highScore) highScore = score;
				if ((score % 500) == 0) speed = speed + 0.3;
				PlaySoundMem(se, DX_PLAYTYPE_BACK);
			}

			DrawBox(WIDTH / 2 - 102, 5, WIDTH / 2 + 100, 43, 0x40c0ff, TRUE);
			DrawBox(WIDTH / 2 - 100, 7, WIDTH / 2 + 100, 43, 0xff33ff, TRUE);
			DrawFormatString(WIDTH / 2 - 85, 10, 0xffff00, "Mで一時中止");

			break;

		case MENU:
			DrawBox(80, 40, 510, 210, 0x40c0ff, TRUE);
			SetFontSize(50);
			DrawString(100, 50, "メニュー画面", GetColor(255,255,255));
			SetFontSize(20);
			DrawString(100, 150, "Rキーでゲームに戻る", GetColor(255, 255, 255));
			DrawString(100, 180, "Nキーでタイトル画面に戻る", GetColor(255, 255, 255));
			if (CheckHitKey(KEY_INPUT_R) == 1) scene = PLAY;
			if (CheckHitKey(KEY_INPUT_N) == 1)
			{
				StopSoundMem(bgm);
				scene = TITLE;
			}
			break;

		case OVER:
			SetFontSize(40);
			DrawString(WIDTH / 2 - 40 / 2 * 9 / 2, HEIGHT / 3, "GAME OVER", 0xff0000);
			if (timer > 60 * 2) scene = TITLE;
			speed = 1.0;
			score = 0;
			break;
		
		case CLEAR:
			DrawExtendGraph(0,0,WIDTH,HEIGHT ,imgLastBg, FALSE);
			DrawString(100, 150, "Rキーでゲームに戻る", GetColor(255, 255, 255));
			if (CheckHitKey(KEY_INPUT_R) == 1)
			{
				scene = PLAY;
				scene = TITLE;
				score = 0;
				dogX = 122 - 35;
				break;
			}			
			break;
		}


		SetFontSize(30);
		DrawFormatString(10, 10, 0xffff00, "SCORE %d", score);
		DrawFormatString(WIDTH - 200, 10, 0xffff00, "HI-SC %d", highScore);
		DrawBox(17, HEIGHT - 42,17 + 85, HEIGHT-8, 0xffffff, TRUE);
		DrawBox(398, HEIGHT - 42, 398 + 70, HEIGHT - 8, 0xffffff, TRUE);
		DrawBox(122, HEIGHT - 15, 378, HEIGHT - 8, 0xffffff, TRUE);
		DrawString(20, HEIGHT - 40, "Start", 0x000000);
		DrawString(400, HEIGHT - 40, "Goal", 0x000000);

		ScreenFlip();
		WaitTimer(16);
		if (ProcessMessage() == -1) break;
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) break;
	}

	DxLib_End();
	return 0;
}