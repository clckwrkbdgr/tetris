#pragma once

#include <QtCore/QVector>
#include <QtCore/QSize>
#include <QtCore/QPoint>

namespace Figures {
	enum { WIDTH = 4, HEIGHT = 4 };
	enum Type { _I, _J, _L, _O, _S, _T, _Z, COUNT };


	int patternCount(Type type);
	QVector<int> fromPattern(const int * pattern);
	QList<QVector<int> > pattern(Type type);
}

class Tetris {
public:
	enum { ROWS_FOR_LEVEL = 10 };
	typedef QVector<int> Figure;
	typedef QList<QVector<int> > FigureList;
	typedef QVector<int> Cup;

	Tetris(const QSize & newSize);
	virtual ~Tetris() {}

	const QSize & cupSize() const { return size; }
	bool isValid(const QPoint & point) const;
	int cell(const QPoint & point) const;

	QVector<int> shadow() const;

	bool tick();

	void drop();
	void left();
	void right();
	void rotate();

	int level() const { return score / ROWS_FOR_LEVEL; }
private:
	Tetris(Tetris&) {}
	Tetris & operator=(Tetris&) {return *this;}

	QSize size;
	Cup cup;
	QPoint figurePos;
	FigureList currentFigure;
	int score;

	QPoint startPoint() const;
	FigureList randomFigure() const;
	Figure fromPattern(const int * pattern) const;
	Figure rotate(const Figure & figure) const;
	bool overlapping(const QPoint & pos, const Figure & figure) const;
	bool outOfCup(const QPoint & pos, const Figure & figure) const;

	void addFigure(const QPoint & pos, const Figure & figure);
	void removeFullRows();
};

