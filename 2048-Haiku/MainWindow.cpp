#include "MainWindow.h"

static const int TILE_SIZE = 64;
static const int TILE_MARGIN = 16;

enum
{
	M_NEW_GAME = 'ng',
	M_SETTINGS = 'prms',
	M_SAVE_GAME = 'sg',
	M_LOAD_GAME = 'lg',
	M_QUIT = 'qg',
	M_ABOUT = 'abt'	
};

MainWindow::MainWindow(void)
	:	BWindow(BRect(100,100,340,400),"2048-Haiku",B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS)
{
	BView *view = new BView(BRect(0,0,340,400), "GameView", B_FOLLOW_ALL, B_WILL_DRAW);
	BRect rect(Bounds());
	BMenu *gameMenu = new BMenu("Game");
	BMenu *helpMenu = new BMenu("Help");
	
	rect.bottom = 20;
	
	BMenuBar *menuBar = new BMenuBar(rect, "MainMenu");
	AddChild(menuBar);
	
	gameMenu->AddItem(new BMenuItem("New Game", new BMessage(M_NEW_GAME), 'N'));
	gameMenu->AddItem(new BMenuItem("Settings", new BMessage(M_SETTINGS), 'P'));
	gameMenu->AddItem(new BMenuItem("Save Game", new BMessage(M_SAVE_GAME), 'S'));
	gameMenu->AddItem(new BMenuItem("Load Game", new BMessage(M_LOAD_GAME), 'L'));
	gameMenu->AddItem(new BMenuItem("Quit", new BMessage(M_QUIT), 'Q'));
	menuBar->AddItem(gameMenu);
	
	helpMenu->AddItem(new BMenuItem("About", new BMessage(M_ABOUT), 'A'));
	menuBar->AddItem(helpMenu);
	
	AddChild(view);
	view->SetViewColor(187, 173, 160);
}

inline int 
MainWindow::offsetCoords(int coord) 
{ 
	return coord * (TILE_MARGIN + TILE_SIZE) + TILE_MARGIN; 
}

void
MainWindow::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		default:
		{
			BWindow::MessageReceived(msg);
			break;
		}
	}
}


bool
MainWindow::QuitRequested(void)
{
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}
