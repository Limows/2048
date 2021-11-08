#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <Window.h>
#include <MenuBar.h>
#include <Menu.h>
#include <MenuItem.h>
#include <View.h>
#include <Application.h>

#include "2048.h"

class MainWindow : public BWindow
{
public:
						MainWindow(void);
			void		MessageReceived(BMessage *msg);
			bool		QuitRequested(void);
			
private:
	inline	int			offsetCoords(int coord);
};

#endif
