#include <QtDebug>

#include "tetris.h"

Tetris::Tetris(const QSize & newSize)
	: size(newSize), cup(size.width() * size.height(), F::NONE),
	score(0)
{
	figure = randomFigure(startPoint());
	nextFigure = randomFigure(startPoint());
}

bool Tetris::isValid(const QPoint & point) const
{
	return (point.x() >= 0 && point.y() >= 0 &&
			point.x() < size.width() && point.y() < size.height());
}

F::Type Tetris::cell(const QPoint & point) const
{
	if(!isValid(point)) return F::NONE;
	QPoint relFigurePos = point - figure.pos;
	if(figure.isValid(relFigurePos) && !at(point)) {
		return figure.at(relFigurePos);
	}
	return at(point);
}

bool Tetris::tick()
{
	figure.pos.ry()++;
	if(!overlapping(figure)) return true;
	figure.pos.ry()--;

	addFigure(figure);

	figure = nextFigure;
	nextFigure = randomFigure(startPoint());
	return !overlapping(figure);
}

QVector<int> Tetris::shadow() const
{
	QVector<int> result;
	foreach(QPoint pos, range(F::WIDTH, F::HEIGHT)) {
		if(figure.at(pos)) {
			result << figure.pos.x() + pos.x();
		}
	}
	return result;
}

void Tetris::removeFullRows()
{
	QPoint pos(0, size.height() - 1);
	for(; pos.y() >= 0; --pos.ry()) {
		int blockCount = 0;
		for(pos.setX(0); pos.x() < size.width(); ++pos.rx()) {
			if(at(pos)) {
				++blockCount;
			}
		}

		if(blockCount == size.width()) {
			QPoint tp(0, pos.y());
			for(; tp.y() > 0; --tp.ry()) {
				for(tp.setX(0); tp.x() < size.width(); ++tp.rx()) {
					at(tp) = at(tp - QPoint(0, 1));
				}
			}
			++score;
			++pos.ry();
		}
	}
}

void Tetris::rotate()
{
	F::Figure newFigure = figure.rotated();
	if(!overlapping(newFigure)) {
		figure = newFigure;
	}
}

void Tetris::drop()
{
	while(!overlapping(figure)) {
		figure.pos.ry()++;
	}
	figure.pos.ry()--;

	addFigure(figure);
	figure = nextFigure;
	nextFigure = randomFigure(startPoint());
}

void Tetris::left()
{
	figure.pos.rx()--;
	if(overlapping(figure)) {
		figure.pos.rx()++;
	}
}

void Tetris::right()
{
	figure.pos.rx()++;
	if(overlapping(figure)) {
		figure.pos.rx()--;
	}
}

QPoint Tetris::startPoint() const
{
	return QPoint(size.width() / 2 - F::WIDTH / 2, 0);
}

F::Figure Tetris::randomFigure(const QPoint & pos) const
{
	F::Figure f = F::figures.values().at(qrand() % F::figures.size());
	f.pos = pos;
	return f;
}

bool Tetris::overlapping(const F::Figure & figure) const
{
	foreach(QPoint pos, range(F::WIDTH, F::HEIGHT)) {
		if(figure.at(pos)) {
			QPoint cupPos = figure.pos + pos;
			if(!isValid(cupPos) || at(cupPos)) {
				return true;
			}
		}
	}
	return false;
}

void Tetris::addFigure(const F::Figure & figure)
{
	foreach(QPoint pos, range(F::WIDTH, F::HEIGHT)) {
		QPoint cupPos = figure.pos + pos;
		if(figure.at(pos) && isValid(cupPos)) {
			at(cupPos) = figure.at(pos);
		}
	}
	removeFullRows();
}

