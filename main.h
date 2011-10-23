#pragma once

#include <QtGui/QWidget>
#include <QtCore/QTimer>

#include "tetris.h"

class MainWindow : public QWidget {
	Q_OBJECT;
	Q_DISABLE_COPY(MainWindow);
public:
	MainWindow(QWidget * parent = 0);
	virtual ~MainWindow() {}
protected:
	virtual void paintEvent(QPaintEvent *);
	virtual void keyPressEvent(QKeyEvent * event);
public slots:
	void onTimer();
private:
	QTimer timer;
	Tetris tetris;
};


