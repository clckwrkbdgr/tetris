/** tetris.py - Standard simple tetris game.
 * author antifin 2012
 * version 1.0.0
 * license WTFPLv2
 */

#include <SDL2/SDL.h>
#include <chthon2/map.h>
#include <set>
#include <vector>

enum class Color {
	BACK = 0,
	FORE = 0xffffff,
	SHADOW = 0x101010,
	I = 0x00ffff,
	J = 0x0000ff,
	L = 0xff8000,
	O = 0xffff00,
	S = 0x00ff00,
	T = 0xff00ff,
	Z = 0xff0000
};

enum {
	CUP_WIDTH = 10,
	CUP_HEIGHT = 22,
	SCORE_LEVEL = 10, // Rows for one speed level.
	START_SPEED = 500,
	SPEED_MODIFIER = 10
};

typedef Chthon::Map<bool> Face;
typedef std::vector<Face> Array;

class Figure {
public:
	enum { W = 4, H = 4 };
	Color color;
	Array array;

	Figure(Color figure_color, const Array & figure_array);
	const Face & face() const;
	void rotate();
	Figure clone() const;
private:
	int current;
};

Figure::Figure(Color figure_color, const Array & figure_array)
	: color(figure_color), array(figure_array), current(0)
{
}

const Face & Figure::face() const
{
	return array[current];
}

void Figure::rotate()
{
	if(++current >= array.size()) {
		current = 0;
	}
}

Figure Figure::clone() const
{
	return Figure(color, array);
}


Array from_points(const std::vector<std::vector<Chthon::Point>> & figure_as_points)
{
	Array array;
	for(const std::vector<Chthon::Point> & rotational_pose : figure_as_points) {
		Face rows(Figure::W, Figure::H, false);
		for(const Chthon::Point & pos : rotational_pose) {
			rows.cell(pos) = true;
		}
		array.push_back(rows);
	}
	return array;
}

std::vector<Figure> make_figures()
{
	std::vector<Figure> result;
	result.push_back(Figure(Color::O, from_points({ {{1,1},{2,2},{1,2},{2,1}} })));
	result.push_back(Figure(Color::I, from_points({ {{1,0},{1,1},{1,2},{1,3}}, {{0,1},{1,1},{2,1},{3,1}} })));
	result.push_back(Figure(Color::J, from_points({ {{0,1},{1,1},{2,1},{2,2}}, {{1,0},{1,1},{1,2},{2,0}}, {{0,1},{1,1},{2,1},{0,0}}, {{1,0},{1,1},{1,2},{0,2}} })));
	result.push_back(Figure(Color::L, from_points({ {{0,1},{1,1},{2,1},{0,2}}, {{1,0},{1,1},{1,2},{2,2}}, {{0,1},{1,1},{2,1},{2,0}}, {{1,0},{1,1},{1,2},{0,0}} })));
	result.push_back(Figure(Color::S, from_points({ {{0,1},{1,1},{1,0},{2,0}}, {{0,0},{0,1},{1,1},{1,2}} })));
	result.push_back(Figure(Color::T, from_points({ {{0,1},{1,1},{2,1},{1,2}}, {{1,0},{1,1},{1,2},{2,1}}, {{0,1},{1,1},{2,1},{1,0}}, {{1,0},{1,1},{1,2},{0,1}} })));
	result.push_back(Figure(Color::Z, from_points({ {{0,0},{1,1},{1,0},{2,1}}, {{1,0},{0,1},{1,1},{0,2}} })));
	return result;
}

Chthon::Point shifted(const Chthon::Point & pos, const Chthon::Point & shift)
{
	return pos + shift;
}

typedef Chthon::Map<Color> Cup;

bool valid_pos(const Cup & cup, const Chthon::Point & pos, const Figure & figure)
{
	for(int col = 0; col < Figure::W; ++col) {
		for(int row = 0; row < Figure::H; ++row) {
			if(figure.face().cell(col, row)) {
				if(row + pos.y < 0 or row + pos.y >= CUP_HEIGHT) {
					return false;
				}
				if(col + pos.x < 0 or col + pos.x >= CUP_WIDTH) {
					return false;
				}
				if(cup.cell(col + pos.x, row + pos.y) != Color::BACK) {
					return false;
				}
			}
		}
	}
	return true;
}

bool may_be_fixated(const Cup & cup, const Chthon::Point & pos, const Figure & figure)
{
	return !valid_pos(cup, shifted(pos, {0, 1}), figure);
}

void fixate(const Figure & figure, const Chthon::Point & pos, Cup & cup)
{
	for(int col = 0; col < Figure::W; ++col) {
		for(int row = 0; row < Figure::H; ++row) {
			if(figure.face().cell(col, row)) {
				cup.cell(col + pos.x, row + pos.y) = figure.color;
			}
		}
	}
}

std::set<int> get_shadow(const Cup & cup, const Chthon::Point & pos, const Figure & figure)
{
	std::set<int> shadow;
	for(int col = 0; col < Figure::W; ++col) {
		for(int row = 0; row < Figure::H; ++row) {
			if(figure.face().cell(col, row)) {
				shadow.insert(pos.x + col);
				break;
			}
		}
	}
	return shadow;
}

int clear_filled_rows(Cup & cup)
{
	int lines_cleared = 0;
	Cup new_cup(cup.width(), cup.height(), Color::BACK);
	int new_cup_row = CUP_HEIGHT - 1;
	for(int row = CUP_HEIGHT - 1; row >= 0; --row) {
		bool has_gap = false;
		for(int col = 0; col < CUP_WIDTH; ++col) {
			if(cup.cell(col, row) == Color::BACK) {
				has_gap = true;
				break;
			}
		}
		if(has_gap) {
			for(int col = 0; col < CUP_WIDTH; ++col) {
				new_cup.cell(col, new_cup_row) = cup.cell(col, row);
			}
			++new_cup_row;
		} else {
			++lines_cleared;
		}
	}
	return lines_cleared;
}

def main():
	screen = init_sdl()
	figures = make_figures()

	cup = [[None for x in range(CUP_WIDTH)] for y in range(CUP_HEIGHT)]
	start_pos = [(CUP_WIDTH - Figure.W) / 2, 0]
	current = random.choice(figures).clone()
	next_figure = random.choice(figures).clone()

	// make surfaces
	colors = ["I", "J", "L", "O", "S", "T", "Z", "FORE", "SHADOW"]
	screen_size = screen.get_size()
	cell_width = min(screen_size[0] / CUP_WIDTH, screen_size[1] / CUP_HEIGHT)
	cell_size = (cell_width, cell_width)

	cup_surface = pygame.surface.Surface( (cell_width * CUP_WIDTH, cell_width * CUP_HEIGHT) )
	cup_surface.fill(COLORS["BACK"])
	cup_rect = cup_surface.get_rect()
	cup_rect.center = screen.get_rect().center

	shadow = pygame.surface.Surface( (cell_width, cell_width * CUP_HEIGHT) )
	shadow.fill(COLORS["SHADOW"])

	bricks = {}
	for color in colors:
		brick = pygame.surface.Surface(cell_size)
		brick.fill(COLORS[color])
		pygame.draw.rect(brick, COLORS["BACK"], brick.get_rect(), 1)
		bricks[color] = brick


	next_view = pygame.surface.Surface( (cell_width * Figure.W, cell_width * Figure.H) )

	playing = True
	paused = False
	speed = 1
	score = 0
	figure_pos = start_pos
	previous_time = pygame.time.get_ticks()
	while True:
		for event in pygame.event.get():
			if event.type == QUIT:
				return
			elif event.type == KEYDOWN:
				if event.key == K_q:
					return
				elif playing:
					if event.key == K_UP:
						rotated = current.clone()
						rotated.rotate()
						if valid_pos(cup, figure_pos, rotated):
							current = rotated
					elif event.key == K_DOWN:
						while valid_pos(cup, figure_pos, current):
							figure_pos = shifted(figure_pos, (0, 1))
						figure_pos = shifted(figure_pos, (0, -1))
					elif event.key == K_LEFT:
						if valid_pos(cup, shifted(figure_pos, (-1, 0)), current):
							figure_pos = shifted(figure_pos, (-1, 0))
					elif event.key == K_RIGHT:
						if valid_pos(cup, shifted(figure_pos, (1, 0)), current):
							figure_pos = shifted(figure_pos, (1, 0))
					elif event.key == K_p:
						paused = not paused

		if playing and not paused:
			if pygame.time.get_ticks() - previous_time > (START_SPEED - SPEED_MODIFIER * speed):
				previous_time = pygame.time.get_ticks()
				if valid_pos(cup, shifted(figure_pos, (0, 1)), current):
					figure_pos = shifted(figure_pos, (0, 1))

			if may_be_fixated(cup, figure_pos, current):
				fixate(current, figure_pos, cup)
				cup, lines_cleared = clear_filled_rows(cup)
				score += lines_cleared
				if score >= SCORE_LEVEL:
					speed += 1
					score = 0

				current, figure_pos = next_figure, start_pos
				if not valid_pos(cup, figure_pos, current):
					playing = False
				next_figure = random.choice(figures).clone()

		// Drawing.
		screen.fill(COLORS["BACK"])
		screen.blit(cup_surface, cup_rect)
		shadows = get_shadow(cup, figure_pos, current)
		for col in shadows:
			shadow_rect = pygame.Rect(cell_size[0] * col, cup_rect.top, cell_size[0], cup_rect.height)
			shadow_rect.move_ip(cup_rect.topleft)
			screen.blit(shadow, shadow_rect)
		//if playing and not paused: colors; else: draw gray content and figure
		for row in range(CUP_HEIGHT):
			for col in range(CUP_WIDTH):
				if cup[row][col]:
					cell_rect = pygame.Rect((col * cell_size[0], row * cell_size[1]), cell_size)
					cell_rect.move_ip(cup_rect.topleft)
					if not playing:
						screen.blit(bricks["SHADOW"], cell_rect)
					elif paused:
						screen.blit(bricks["FORE"], cell_rect)
					else:
						screen.blit(bricks[cup[row][col]], cell_rect)
		for row in range(Figure.H):
			for col in range(Figure.W):
				if current.face()[row][col]:
					cell_rect = pygame.Rect(((figure_pos[0] + col) * cell_size[0], (figure_pos[1] + row) * cell_size[1]), cell_size)
					cell_rect.move_ip(cup_rect.topleft)
					if not playing:
						screen.blit(bricks["SHADOW"], cell_rect)
					elif paused:
						screen.blit(bricks["FORE"], cell_rect)
					else:
						screen.blit(bricks[current.color], cell_rect)
		next_view.fill(COLORS["BACK"])
		for row in range(Figure.H):
			for col in range(Figure.W):
				if next_figure.face()[row][col]:
					cell_rect = pygame.Rect((col * cell_size[0], row * cell_size[1]), cell_size)
					if not playing:
						next_view.blit(bricks["SHADOW"], cell_rect)
					elif paused:
						next_view.blit(bricks["FORE"], cell_rect)
					else:
						next_view.blit(bricks[next_figure.color], cell_rect)
		pygame.draw.rect(screen, COLORS["FORE"], cup_rect, 1)
		screen.blit(next_view, (cup_rect.topright, cell_size))

		pygame.display.flip()

