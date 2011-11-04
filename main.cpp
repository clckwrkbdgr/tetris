#include <QtCore/QTimer>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtGui/QApplication>
#include <QtGui/QMessageBox>
#include <QtGui/QWidget>
#include <QtDebug>
#include <time.h>

#include "main.h"
#include "tetris.h"

MainWindow::MainWindow(QWidget * parent)
	: QWidget(parent), tetris(Settings::SIZE)
{
	oldLevel = tetris.level();
	connect(&timer, SIGNAL(timeout()), this, SLOT(onTimer()));
	timer.start(Settings::START_SPEED);
}


void MainWindow::onTimer()
{
	if(!tetris.tick()) {
		timer.stop();
		QMessageBox::information(this, tr("Fail"), tr("Game over"));
		close();
	}

	if(tetris.level() != oldLevel) {
		timer.setInterval(Settings::SPEED_MOD * timer.interval());
	}
	oldLevel = tetris.level();

	update();
}

void MainWindow::keyPressEvent(QKeyEvent * event)
{
	switch(event->key()) {
		case Qt::Key_Up   : tetris.rotate(); break;
		case Qt::Key_Down : tetris.drop();   break;
		case Qt::Key_Left : tetris.left();   break;
		case Qt::Key_Right: tetris.right();  break;
		case Qt::Key_Q    : close();         break;
		case Qt::Key_P: {
			if(timer.isActive()) {
				timer.stop();
			} else {
				timer.start();
			}
			break;
		}
		default: QWidget::keyPressEvent(event);
	}
	update();
}

void MainWindow::paintEvent(QPaintEvent *)
{
	QMap<F::Type, QColor> blockColors;
	blockColors[F::I] = Qt::cyan;
	blockColors[F::J] = Qt::blue;
	blockColors[F::L] = qRgb(255, 128, 0);
	blockColors[F::O] = Qt::yellow;
	blockColors[F::S] = Qt::green;
	blockColors[F::T] = Qt::darkMagenta;
	blockColors[F::Z] = Qt::red;
	if(!timer.isActive()) {
		foreach(F::Type type, blockColors.keys()) {
			blockColors[type] = blockColors[type].darker(300);
		}
	}

	int cellWidth = qMin(width() / tetris.cupSize().width(), height() / tetris.cupSize().height());
	QSize cupSize = cellWidth * tetris.cupSize();
	QSize startSize = (size() - cupSize) / 2;
	QRect cupRect = QRect(QPoint(startSize.width(), startSize.height()), cupSize);
	QPoint statusPos = QPoint(cupRect.right() + cellWidth, cellWidth);
	QRect nextFigureRect = QRect(cupRect.right() + cellWidth, cellWidth * 2, F::WIDTH * cellWidth, F::HEIGHT * cellWidth);

	QPainter painter(this);
	painter.fillRect(rect(), Settings::BACKGROUND);

	QRect shadowRect = QRect(cupRect.topLeft(), QSize(cellWidth, cupSize.height()));
	foreach(int i, tetris.shadow()) {
		painter.fillRect(shadowRect.translated(cellWidth * i, 0), Settings::SHADOW);
	}

	painter.setPen(Settings::FOREGROUND);
	painter.setBrush(Qt::NoBrush);
	painter.drawRect(cupRect);

	painter.setPen(Settings::BACKGROUND);
	QRect cell(0, 0, cellWidth, cellWidth);
	foreach(QPoint pos, range(tetris.cupSize())) {
		cell.moveTopLeft(cupRect.topLeft() + pos * cellWidth);
		F::Type block = tetris.cell(pos);
		if(block) {
			painter.setBrush(blockColors[block]);
			painter.drawRect(cell);
		}
	}

	painter.setPen(Settings::FOREGROUND);
	painter.setBrush(Settings::BACKGROUND);
	painter.drawText(cupRect.right() + cell.width(), cell.height(),
			tr("Level: %1.").arg(tetris.level()));

	painter.setPen(Settings::BACKGROUND);
	cell = QRect(nextFigureRect.topLeft(), cell.size());
	foreach(QPoint pos, range(F::WIDTH, F::HEIGHT)) {
		cell.moveTopLeft(nextFigureRect.topLeft() + pos * cellWidth);
		F::Type block = tetris.next().at(pos);
		if(block) {
			painter.setBrush(blockColors[block]);
			painter.drawRect(cell);
		}
	}
}



int main(int argc, char ** argv)
{
	qsrand(time(NULL));
	QApplication app(argc, argv);
	MainWindow wnd;
	wnd.showFullScreen();
	return app.exec();
}
