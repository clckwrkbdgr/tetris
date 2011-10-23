#include <QtDebug>

#include "tetris.h"

namespace Figures {
	int patternCount(Type type)
	{
		switch(type) {
			case _I: return 2;
			case _J: return 2;
			case _L: return 2;
			case _O: return 2;
			case _S: return 2;
			case _T: return 2;
			case _Z: return 2;
			default: return 0;
		}
	}
	QVector<int> fromPattern(const int * pattern)
	{
		QVector<int> figure(WIDTH * HEIGHT);
		for(int i = 0; i < WIDTH * HEIGHT; ++i) {
			figure[i] = pattern[i];
		}
		return figure;
	}
	QList<QVector<int> > pattern(Type type)
	{
		static const int I0[WIDTH * HEIGHT] = {
			0, 1, 0, 0,
			0, 1, 0, 0,
			0, 1, 0, 0,
			0, 1, 0, 0
		};
		static const int I1[WIDTH * HEIGHT] = {
			0, 0, 0, 0,
			1, 1, 1, 1,
			0, 0, 0, 0,
			0, 0, 0, 0
		};

		static const int J0[WIDTH * HEIGHT] = {
			0, 1, 0, 0,
			0, 1, 0, 0,
			1, 1, 0, 0,
			0, 0, 0, 0
		};
		static const int J1[WIDTH * HEIGHT] = {
			0, 0, 0, 0,
			1, 1, 1, 0,
			0, 0, 1, 0,
			0, 0, 0, 0
		};
		static const int J2[WIDTH * HEIGHT] = {
			0, 1, 1, 0,
			0, 1, 0, 0,
			0, 1, 0, 0,
			0, 0, 0, 0
		};
		static const int J3[WIDTH * HEIGHT] = {
			1, 0, 0, 0,
			1, 1, 1, 0,
			0, 0, 0, 0,
			0, 0, 0, 0
		};

		static const int L0[WIDTH * HEIGHT] = {
			0, 1, 0, 0,
			0, 1, 0, 0,
			0, 1, 1, 0,
			0, 0, 0, 0
		};
		static const int L1[WIDTH * HEIGHT] = {
			0, 0, 1, 0,
			1, 1, 1, 0,
			0, 0, 0, 0,
			0, 0, 0, 0
		};
		static const int L2[WIDTH * HEIGHT] = {
			1, 1, 0, 0,
			0, 1, 0, 0,
			0, 1, 0, 0,
			0, 0, 0, 0
		};
		static const int L3[WIDTH * HEIGHT] = {
			0, 0, 0, 0,
			1, 1, 1, 0,
			1, 0, 0, 0,
			0, 0, 0, 0
		};

		static const int O0[WIDTH * HEIGHT] = {
			0, 0, 0, 0,
			0, 1, 1, 0,
			0, 1, 1, 0,
			0, 0, 0, 0
		};

		static const int S0[WIDTH * HEIGHT] = {
			1, 0, 0, 0,
			1, 1, 0, 0,
			0, 1, 0, 0,
			0, 0, 0, 0
		};
		static const int S1[WIDTH * HEIGHT] = {
			0, 1, 1, 0,
			1, 1, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0
		};

		static const int T0[WIDTH * HEIGHT] = {
			0, 1, 0, 0,
			1, 1, 0, 0,
			0, 1, 0, 0,
			0, 0, 0, 0
		};
		static const int T1[WIDTH * HEIGHT] = {
			0, 0, 0, 0,
			1, 1, 1, 0,
			0, 1, 0, 0,
			0, 0, 0, 0
		};
		static const int T2[WIDTH * HEIGHT] = {
			0, 1, 0, 0,
			0, 1, 1, 0,
			0, 1, 0, 0,
			0, 0, 0, 0
		};
		static const int T3[WIDTH * HEIGHT] = {
			0, 1, 0, 0,
			1, 1, 1, 0,
			0, 0, 0, 0,
			0, 0, 0, 0
		};

		static const int Z0[WIDTH * HEIGHT] = {
			1, 1, 0, 0,
			0, 1, 1, 0,
			0, 0, 0, 0,
			0, 0, 0, 0
		};
		static const int Z1[WIDTH * HEIGHT] = {
			0, 1, 0, 0,
			1, 1, 0, 0,
			1, 0, 0, 0,
			0, 0, 0, 0
		};

		static const QList<QVector<int> > I = QList<QVector<int> >() <<
			fromPattern(I0) << fromPattern(I1);
		static const QList<QVector<int> > J = QList<QVector<int> >() <<
			fromPattern(J0) << fromPattern(J1) <<
			fromPattern(J2) << fromPattern(J3);
		static const QList<QVector<int> > L = QList<QVector<int> >() <<
			fromPattern(L0) << fromPattern(L1) <<
			fromPattern(L2) << fromPattern(L3);
		static const QList<QVector<int> > O = QList<QVector<int> >() <<
			fromPattern(O0);
		static const QList<QVector<int> > S = QList<QVector<int> >() <<
			fromPattern(S0) << fromPattern(S1);
		static const QList<QVector<int> > T = QList<QVector<int> >() <<
			fromPattern(T0) << fromPattern(T1) <<
			fromPattern(T2) << fromPattern(T3);
		static const QList<QVector<int> > Z = QList<QVector<int> >() <<
			fromPattern(Z0) << fromPattern(Z1);
		switch(type) {
			case _I: return I;
			case _J: return J;
			case _L: return L;
			case _O: return O;
			case _S: return S;
			case _T: return T;
			case _Z: return Z;
			default:break;
		}
		return QList<QVector<int> >();
	}
}

Tetris::Tetris(const QSize & newSize)
	: size(newSize), cup(size.width() * size.height(), 0),
	figurePos(startPoint()), currentFigure(randomFigure()), score(0)
{
}

bool Tetris::isValid(const QPoint & point) const
{
	return (point.x() >= 0 && point.y() >= 0 &&
			point.x() < size.width() && point.y() < size.height());
}

int Tetris::cell(const QPoint & point) const
{
	if(isValid(point)) {
		QPoint relFigurePos = point - figurePos;
		int figureValue = 0;
		if(0 <= relFigurePos.x() && relFigurePos.x() < Figures::WIDTH)
			if(0 <= relFigurePos.y() && relFigurePos.y() < Figures::HEIGHT ) {
				figureValue = currentFigure.front()[relFigurePos.x() +
					relFigurePos.y() * Figures::WIDTH];
			}
		return figureValue || cup[point.x() + point.y() * size.width()];
	}
	return 0;
}

bool Tetris::tick()
{
	QPoint newPos = figurePos;
	newPos.ry()++;

	if(overlapping(newPos, currentFigure.front())) {
		addFigure(figurePos, currentFigure.front());
		removeFullRows();

		figurePos = startPoint();

		currentFigure = randomFigure();
		if(overlapping(figurePos, currentFigure.front()))
			return false;
		return true;
	}

	figurePos = newPos;
	return true;
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
	if(currentFigure.size()) {
		currentFigure.append(currentFigure.takeFirst());
		if(outOfCup(figurePos, currentFigure.first()) || overlapping(figurePos, currentFigure.first())) {
			currentFigure.prepend(currentFigure.takeLast());
			return;
		}
	}
}

void Tetris::drop()
{
	while(!overlapping(figurePos, currentFigure.front())) {
		figurePos.ry()++;
	}
	figurePos.ry()--;
}

void Tetris::left()
{
	QPoint newPos = figurePos;
	newPos.rx()--;
	if(outOfCup(newPos, currentFigure.front()) || overlapping(newPos, currentFigure.front()))
		return;

	figurePos = newPos;
}

void Tetris::right()
{
	QPoint newPos = figurePos;
	newPos.rx()++;
	if(outOfCup(newPos, currentFigure.front()) || overlapping(newPos, currentFigure.front()))
		return;

	figurePos = newPos;
}

QPoint Tetris::startPoint() const
{
	return QPoint(size.width() / 2 - Figures::WIDTH / 2, 0);
}

Tetris::FigureList Tetris::randomFigure() const
{
	switch(qrand() % Figures::COUNT) {
		case Figures::_I: return Figures::pattern(Figures::_I);
		case Figures::_J: return Figures::pattern(Figures::_J);
		case Figures::_L: return Figures::pattern(Figures::_L);
		case Figures::_O: return Figures::pattern(Figures::_O);
		case Figures::_S: return Figures::pattern(Figures::_S);
		case Figures::_T: return Figures::pattern(Figures::_T);
		case Figures::_Z: return Figures::pattern(Figures::_Z);
		default: return Figures::pattern(Figures::COUNT);
	}
}

Tetris::Tetris::Figure Tetris::rotate(const Figure & figure) const
{
	const int swap[Figures::WIDTH * Figures::HEIGHT] = {
		 8, 4, 0, 12,
		 9, 5, 1, 13,
		10, 6, 2, 14,
		11, 7, 3, 15
	};
	Figure result = Figure(Figures::WIDTH * Figures::HEIGHT);
	for(int i = 0; i < result.size(); ++i) {
		result[swap[i]] = figure[i];
	}
	return result;
}

bool Tetris::overlapping(const QPoint & pos, const Tetris::Figure & figure) const
{
	for(int x = 0; x < Figures::WIDTH; ++x) {
		for(int y = 0; y < Figures::HEIGHT; ++y) {
			QPoint cupPos = pos + QPoint(x, y);
			if(cup[cupPos.x() + cupPos.y() * size.width()] &&
					figure[x + y * Figures::WIDTH]) {
				return true;
			}
		}
	}
	return false;
}

bool Tetris::outOfCup(const QPoint & pos, const Tetris::Figure & figure) const
{
	for(int x = 0; x < Figures::WIDTH; ++x) {
		for(int y = 0; y < Figures::HEIGHT; ++y) {
			if(figure[x + y * Figures::WIDTH] && !isValid(pos + QPoint(x, y))) {
				return true;
			}
		}
	}
	return false;
}

void Tetris::addFigure(const QPoint & pos, const Tetris::Figure & figure)
{
	for(int x = 0; x < Figures::WIDTH; ++x) {
		for(int y = 0; y < Figures::HEIGHT; ++y) {
			QPoint cupPos = pos + QPoint(x, y);
			if(figure[x + y * Figures::WIDTH] && isValid(pos + QPoint(x, y))) {
				cup[cupPos.x() + cupPos.y() * size.width()] =
					figure[x + y * Figures::WIDTH];
			}
		}
	}
}

