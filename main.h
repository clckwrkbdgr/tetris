#pragma once

#include <QtGui/QWidget>
#include <QtCore/QTimer>

#include "tetris.h"

namespace Settings {
	const QSize SIZE(10, 22);
	enum { START_SPEED = 500 };
	const double SPEED_MOD = 0.9;
	const QColor BACKGROUND = Qt::black;
	const QColor SHADOW = qRgb(10, 10, 10);
	const QColor FOREGROUND = Qt::white;
}

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


