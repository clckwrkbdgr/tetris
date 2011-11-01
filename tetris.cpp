#include <QtDebug>

#include "tetris.h"

Tetris::Tetris(const QSize & newSize)
	: size(newSize), cup(size.width() * size.height(), F::NONE),
	score(0)
{
	figure = randomFigure();
	figure.pos = startPoint();
}

bool Tetris::isValid(const QPoint & point) const
{
	return (point.x() >= 0 && point.y() >= 0 &&
			point.x() < size.width() && point.y() < size.height());
}

F::Type Tetris::cell(const QPoint & point) const
{
	if(isValid(point)) {
		QPoint relFigurePos = point - figure.pos;
		F::Type figureValue = F::NONE;
		if(0 <= relFigurePos.x() && relFigurePos.x() < F::WIDTH)
			if(0 <= relFigurePos.y() && relFigurePos.y() < F::HEIGHT ) {
				figureValue = figure.figure[relFigurePos.x() +
					relFigurePos.y() * F::WIDTH];
			}
		if(figureValue)
			return figureValue;
		else 
			return cup[point.x() + point.y() * size.width()];
	}
	return F::NONE;
}

bool Tetris::tick()
{
	QPoint newPos = figure.pos;
	newPos.ry()++;

	if(overlapping(newPos, figure) || outOfCup(newPos, figure)) {
		addFigure(figure.pos, figure);
		removeFullRows();


		figure = randomFigure();
		figure.pos = startPoint();
		if(overlapping(figure.pos, figure))
			return false;
		return true;
	}

	figure.pos = newPos;
	return true;
}

QVector<int> Tetris::shadow() const
{
	QVector<int> result;
	for(int x = 0; x < F::WIDTH; ++x) {
		for(int y = 0; y < F::HEIGHT; ++y) {
			if(figure.figure[x + y * F::WIDTH]) {
				result << figure.pos.x() + x;
				break;
			}
		}
	}
	return result;
}

void Tetris::removeFullRows()
{
	for(int y = size.height() - 1; y >= 0; --y) {
		int blockCount = 0;
		for(int x = 0; x < size.width(); ++x) {
			if(cup[x + y * size.width()]) {
				++blockCount;
			}
		}

		if(blockCount == size.width()) {
			for(int ty = y; ty > 0; --ty) {
				for(int x = 0; x < size.width(); ++x) {
					cup[x + ty * size.width()] = 
						cup[x + (ty - 1) * size.width()];
				}
			}
			++score;
			++y;
		}
	}
}

void Tetris::rotate()
{
	F::Figure newFigure = figure.rotated();
		if(!outOfCup(newFigure.pos, newFigure) && !overlapping(newFigure.pos, newFigure)) {
			figure = newFigure;
			return;
		}
}

void Tetris::drop()
{
	while(!overlapping(figure.pos, figure) && !outOfCup(figure.pos, figure)) {
		figure.pos.ry()++;
	}
	figure.pos.ry()--;
}

void Tetris::left()
{
	QPoint newPos = figure.pos;
	newPos.rx()--;
	if(outOfCup(newPos, figure) || overlapping(newPos, figure))
		return;

	figure.pos = newPos;
}

void Tetris::right()
{
	QPoint newPos = figure.pos;
	newPos.rx()++;
	if(outOfCup(newPos, figure) || overlapping(newPos, figure))
		return;

	figure.pos = newPos;
}

QPoint Tetris::startPoint() const
{
	return QPoint(size.width() / 2 - F::WIDTH / 2, 0);
}

F::Figure Tetris::randomFigure() const
{
	return F::figures[F::Type(qrand() % (F::COUNT - 1) + 1)];
}

bool Tetris::overlapping(const QPoint & pos, const F::Figure & figure) const
{
	for(int x = 0; x < F::WIDTH; ++x) {
		for(int y = 0; y < F::HEIGHT; ++y) {
			QPoint cupPos = pos + QPoint(x, y);
			if(cup[cupPos.x() + cupPos.y() * size.width()] &&
					figure.figure[x + y * F::WIDTH]) {
				return true;
			}
		}
	}
	return false;
}

bool Tetris::outOfCup(const QPoint & pos, const F::Figure & figure) const
{
	for(int x = 0; x < F::WIDTH; ++x) {
		for(int y = 0; y < F::HEIGHT; ++y) {
			if(figure.figure[x + y * F::WIDTH] && !isValid(pos + QPoint(x, y))) {
				return true;
			}
		}
	}
	return false;
}

void Tetris::addFigure(const QPoint & pos, const F::Figure & figure)
{
	for(int x = 0; x < F::WIDTH; ++x) {
		for(int y = 0; y < F::HEIGHT; ++y) {
			QPoint cupPos = pos + QPoint(x, y);
			if(figure.figure[x + y * F::WIDTH] && isValid(pos + QPoint(x, y))) {
				cup[cupPos.x() + cupPos.y() * size.width()] =
					figure.figure[x + y * F::WIDTH];
			}
		}
	}
}

