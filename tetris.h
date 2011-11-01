#pragma once

#include <map>
#include <QtCore/QVector>
#include <QtCore/QMap>
#include <QtCore/QSize>
#include <QtCore/QPoint>
#include <QtDebug>

namespace F {
	enum { WIDTH = 4, HEIGHT = 4, SIZE = WIDTH * HEIGHT };
	enum Type { NONE = 0, I, J, L, O, S, T, Z, COUNT };

	struct Figure {
		QVector<Type> figure;
		QVector<int> rotateMask;
		Type type;
		QPoint pos;
		
		Figure() : figure(SIZE, NONE), rotateMask(SIZE, 0), type(NONE) {}
		template<std::size_t z>
		Figure(int (&f)[z], int (&mask)[z], Type fType)
			: figure(SIZE, NONE),
			rotateMask(QVector<int>::fromStdVector(std::vector<int>(mask, mask + SIZE))), type(fType)
		{
			figure.resize(SIZE);
			for(int i = 0; i < SIZE; ++i) {
				if(f[i])
					figure[i] = type;
			}
		}
		Figure rotated() {
			Figure newFigure = *this;
			for(int i = 0; i < SIZE; ++i) {
				newFigure.figure[i] = figure[rotateMask[i]];
			}
			return newFigure;
		}
	};

	static int I_MASK[SIZE] = {
		0, 4, 0,  0,
		1, 5, 9, 13,
		0, 6, 0,  0,
		0, 7, 0,  0
	};
	static int J_MASK[SIZE] = {
		 2,  6, 10, 15,
		 1,  5,  9, 15,
		 0,  4,  8, 15,
		15, 15, 15, 15
	};
	static int O_MASK[SIZE] = {
		 0,  1,  2,  3,
		 4,  5,  6,  7,
		 8,  9, 10, 11,
		12, 13, 14, 15
	};

	static int I_F[SIZE] = {
		0, 1, 0, 0,
		0, 1, 0, 0,
		0, 1, 0, 0,
		0, 1, 0, 0
	};
	static int J_F[SIZE] = {
		1, 0, 0, 0,
		1, 1, 1, 0,
		0, 0, 0, 0,
		0, 0, 0, 0
	};
	static int L_F[SIZE] = {
		0, 0, 1, 0,
		1, 1, 1, 0,
		0, 0, 0, 0,
		0, 0, 0, 0
	};
	static int O_F[SIZE] = {
		0, 0, 0, 0,
		0, 1, 1, 0,
		0, 1, 1, 0,
		0, 0, 0, 0
	};
	static int S_F[SIZE] = {
		0, 1, 1, 0,
		1, 1, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0
	};
	static int T_F[SIZE] = {
		0, 1, 0, 0,
		1, 1, 1, 0,
		0, 0, 0, 0,
		0, 0, 0, 0
	};
	static int Z_F[SIZE] = {
		1, 1, 0, 0,
		0, 1, 1, 0,
		0, 0, 0, 0,
		0, 0, 0, 0
	};

	typedef std::map<Type, Figure> map_type;
	typedef map_type::value_type pair_type;
	template<typename T, std::size_t z> std::size_t size(T const (&)[z]) { return z; }
	static pair_type initializers[] = {
		pair_type(I, Figure(I_F, I_MASK, I)),
		pair_type(J, Figure(J_F, J_MASK, J)),
		pair_type(L, Figure(L_F, J_MASK, L)),
		pair_type(O, Figure(O_F, O_MASK, O)),
		pair_type(S, Figure(S_F, J_MASK, S)),
		pair_type(T, Figure(T_F, J_MASK, T)),
		pair_type(Z, Figure(Z_F, J_MASK, Z))
	};
	static QMap<Type, Figure> figures(map_type(initializers, initializers + size(initializers)));
}

class Tetris {
public:
	enum { ROWS_FOR_LEVEL = 10 };
	typedef QVector<F::Type> Cup;

	Tetris(const QSize & newSize);
	virtual ~Tetris() {}

	const QSize & cupSize() const { return size; }
	bool isValid(const QPoint & point) const;
	F::Type cell(const QPoint & point) const;

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
	F::Figure figure;
	int score;

	QPoint startPoint() const;
	F::Figure randomFigure() const;
	bool overlapping(const QPoint & pos, const F::Figure & figure) const;
	bool outOfCup(const QPoint & pos, const F::Figure & figure) const;

	void addFigure(const QPoint & pos, const F::Figure & figure);
	void removeFullRows();
};

