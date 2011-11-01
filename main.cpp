#include <QtCore/QTimer>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtGui/QApplication>
#include <QtGui/QMessageBox>
#include <QtGui/QWidget>
#include <QtDebug>

#include "main.h"
#include "tetris.h"

MainWindow::MainWindow(QWidget * parent)
	: QWidget(parent), tetris(Settings::SIZE)
{
	connect(&timer, SIGNAL(timeout()), this, SLOT(onTimer()));
	timer.start(Settings::START_SPEED);
}


void MainWindow::onTimer()
{
	int oldLevel = tetris.level();

	if(!tetris.tick()) {
		timer.stop();
		QMessageBox::information(this, tr("Fail"), tr("Game over"));
		close();
	}

	if(tetris.level() != oldLevel) {
		timer.setInterval(Settings::SPEED_MOD * timer.interval());
	}

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
	int cellWidth = qMin(width() / tetris.cupSize().width(),
			height() / tetris.cupSize().height());
	QSize cupSize = cellWidth * tetris.cupSize();
	QPoint startPos = QPoint((width() - cupSize.width()) / 2, (height() - cupSize.height()) / 2);

	QPainter painter(this);
	painter.fillRect(rect(), Settings::BACKGROUND);

	foreach(int i, tetris.shadow()) {
		painter.fillRect(QRect(startPos + QPoint(cellWidth * i, 0), QSize(cellWidth, cupSize.height())), Settings::SHADOW);
	}

	painter.setPen(Settings::FOREGROUND);
	painter.setBrush(Qt::NoBrush);
	painter.drawRect(QRect(startPos, cupSize));

	painter.setPen(Settings::BACKGROUND);
	//painter.setBrush(Settings::FOREGROUND);
	QRect cell(0, 0, cellWidth, cellWidth);
	for(int x = 0; x < tetris.cupSize().width(); ++x) {
		for(int y = 0; y < tetris.cupSize().height(); ++y) {
			cell.moveTopLeft(startPos + QPoint(x * cellWidth, y * cellWidth));
			if(tetris.cell(QPoint(x, y))) {
				QColor color = Settings::FOREGROUND;
				switch(tetris.cell(QPoint(x, y))) {
					case F::I: color = Qt::cyan; break;
					case F::J: color = Qt::blue; break;
					case F::L: color = qRgb(255, 128, 0); break;
					case F::O: color = Qt::yellow; break;
					case F::S: color = Qt::green; break;
					case F::T: color = Qt::darkMagenta; break;
					case F::Z: color = Qt::red; break;
					default: break;
				}
				painter.setBrush(color);
				painter.drawRect(cell);
			}
		}
	}

	painter.setPen(Settings::FOREGROUND);
	painter.setBrush(Settings::BACKGROUND);

	painter.drawText(cell.right() + cell.width(), cell.height(),
			tr("Level: %1").arg(tetris.level()));

	if(!timer.isActive()) {
		painter.drawText(cell.right() + cell.width(), cell.height() * 2, tr("Paused"));
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
