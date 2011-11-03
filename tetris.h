#pragma once

#include <map>
#include <QtCore/QVector>
#include <QtCore/QMap>
#include <QtCore/QSize>
#include <QtCore/QPoint>
#include <QtDebug>

template<class T, std::size_t z>
QVector<T> fromArray(const T (&array)[z])
{
	return QVector<T>::fromStdVector(std::vector<T>(array, array + z));
}

static QVector<QPoint> range(int width, int height) {
	QVector<QPoint> result;
	result.reserve(width * height);
	QPoint pos;
	for(; pos.x() < width; ++pos.rx()) {
		for(pos.setY(0); pos.y() < height; ++pos.ry()) {
			result << pos;
		}
	}
	return result;
}

namespace F {
	enum { WIDTH = 4, HEIGHT = 4, SIZE = WIDTH * HEIGHT };
	enum Type { NONE = 0, _ = 0, I, J, L, O, S, T, Z, COUNT };

	struct Figure {
		QVector<Type> blocks;
		QVector<int> rotateMask;
		QPoint pos;
		
		Figure() : blocks(SIZE, NONE), rotateMask(SIZE, 0) {}
		Figure(Type (&f)[SIZE], int (&mask)[SIZE])
			: blocks(fromArray(f)), rotateMask(fromArray(mask)) {}

		bool isValid(const QPoint & p) const {
			return (p.x() >= 0 && p.y() >= 0 && p.x() < WIDTH && p.y() < HEIGHT);
		}
		Type & at(const QPoint & p) { return blocks[p.x() + p.y() * F::WIDTH]; }
		const Type & at(const QPoint & p) const { return blocks[p.x() + p.y() * F::WIDTH]; }
		Figure rotated() {
			Figure newFigure = *this;
			for(int i = 0; i < SIZE; ++i) {
				newFigure.blocks[i] = blocks[rotateMask[i]];
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

	static Type I_F[SIZE] = {
		_, I, _, _,
		_, I, _, _,
		_, I, _, _,
		_, I, _, _
	};
	static Type J_F[SIZE] = {
		J, _, _, _,
		J, J, J, _,
		_, _, _, _,
		_, _, _, _
	};
	static Type L_F[SIZE] = {
		_, _, L, _,
		L, L, L, _,
		_, _, _, _,
		_, _, _, _
	};
	static Type O_F[SIZE] = {
		_, _, _, _,
		_, O, O, _,
		_, O, O, _,
		_, _, _, _
	};
	static Type S_F[SIZE] = {
		_, S, S, _,
		S, S, _, _,
		_, _, _, _,
		_, _, _, _
	};
	static Type T_F[SIZE] = {
		_, T, _, _,
		T, T, T, _,
		_, _, _, _,
		_, _, _, _
	};
	static Type Z_F[SIZE] = {
		Z, Z, _, _,
		_, Z, Z, _,
		_, _, _, _,
		_, _, _, _
	};

	typedef std::map<Type, Figure> map_type;
	typedef map_type::value_type pair_type;
	template<typename T, std::size_t z> std::size_t size(T const (&)[z]) { return z; }
	static pair_type initializers[] = {
		pair_type(I, Figure(I_F, I_MASK)),
		pair_type(J, Figure(J_F, J_MASK)),
		pair_type(L, Figure(L_F, J_MASK)),
		pair_type(O, Figure(O_F, O_MASK)),
		pair_type(S, Figure(S_F, J_MASK)),
		pair_type(T, Figure(T_F, J_MASK)),
		pair_type(Z, Figure(Z_F, J_MASK))
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

	F::Type & at(const QPoint & p) { return cup[p.x() + p.y() * size.width()]; }
	const F::Type & at(const QPoint & p) const { return cup[p.x() + p.y() * size.width()]; }

	QPoint startPoint() const;
	F::Figure randomFigure(const QPoint & pos) const;
	bool overlapping(const F::Figure & figure) const;

	void addFigure(const F::Figure & figure);
	void removeFullRows();
};

