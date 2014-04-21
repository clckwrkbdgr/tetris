/** tetris.py - Standard simple tetris game.
 * author antifin 2012
 * version 1.0.0
 * license WTFPLv2
 */

#include <SDL2/SDL.h>
#include <chthon2/map.h>
#include <chthon2/pixmap.h>
#include <chthon2/log.h>
#include <map>
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

typedef Chthon::Map<char> Face;
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

int main()
{
	srand(time(nullptr));
	std::vector<Figure> figures = make_figures();

	Cup cup(CUP_WIDTH, CUP_HEIGHT);
	Chthon::Point start_pos = {(CUP_WIDTH - Figure::W) / 2, 0};
	Figure current = figures[rand() % figures.size()];
	Figure next_figure = figures[rand() % figures.size()];

	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window * window = SDL_CreateWindow(
			"Catris",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			0, 0,
			SDL_WINDOW_FULLSCREEN_DESKTOP
			);
	SDL_ShowCursor(0);
	SDL_Rect screen_size;
	screen_size.x = 0;
	screen_size.y = 0;
	SDL_GetWindowSize(window, &screen_size.w, &screen_size.h);
	int cell_width = std::min(screen_size.w / CUP_WIDTH, screen_size.h / CUP_HEIGHT);

	SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	// make surfaces
	SDL_Surface * s = SDL_CreateRGBSurface(SDL_SWSURFACE, cell_width * CUP_WIDTH, cell_width * CUP_HEIGHT, 32, 0, 0, 0, 0); //Creating the surface.
	SDL_FillRect(s, NULL, (unsigned)Color::BACK);
	SDL_Texture * cup_surface = SDL_CreateTextureFromSurface(renderer, s);
	SDL_Rect cup_rect = {(screen_size.w - s->w) / 2, (screen_size.h - s->h) / 2, s->w, s->h};

	s = SDL_CreateRGBSurface(SDL_SWSURFACE, cell_width, cell_width * CUP_HEIGHT, 32, 0, 0, 0, 0); //Creating the surface.
	SDL_FillRect(s, NULL, (unsigned)Color::SHADOW);
	SDL_Texture * shadow = SDL_CreateTextureFromSurface(renderer, s);

	std::map<Color, SDL_Texture*> bricks;
	std::vector<Color> colors = { Color::I, Color::J, Color::L, Color::O, Color::S, Color::T, Color::Z, Color::BACK, Color::FORE, Color::SHADOW };
	for(Color color : colors) {
		s = SDL_CreateRGBSurface(SDL_SWSURFACE, cell_width, cell_width, 32, 0, 0, 0, 0); //Creating the surface.
		SDL_FillRect(s, NULL, (unsigned)Color::BACK);
		SDL_Rect r = {1, 1, s->w - 1, s->h - 1};
		SDL_FillRect(s, &r, (unsigned)color);
		bricks[color] = SDL_CreateTextureFromSurface(renderer, s);
	}
	s = SDL_CreateRGBSurface(SDL_SWSURFACE, cell_width * Figure::W, cell_width * Figure::H, 32, 0, 0, 0, 0); //Creating the surface.
	SDL_Texture * next_view = SDL_CreateTextureFromSurface(renderer, s);

	bool playing = true;
	bool quit = false;
	bool paused = false;
	int speed = 1;
	int score = 0;
	Chthon::Point figure_pos = start_pos;
	Uint32 previous_time = SDL_GetTicks();
	SDL_Event event;
	while(!quit) {
		// Drawing.
		SDL_SetRenderDrawColor(renderer, Chthon::get_red(Chthon::Color(Color::BACK)), Chthon::get_green(Chthon::Color(Color::BACK)), Chthon::get_blue(Chthon::Color(Color::BACK)), 255);
		SDL_RenderClear(renderer);
		std::set<int> shadows = get_shadow(cup, figure_pos, current);
		for(int col : shadows) {
			SDL_Rect shadow_rect = {cell_width * col, cup_rect.y, cell_width, cup_rect.h};
			shadow_rect.x += cup_rect.x;
			shadow_rect.y += cup_rect.y;
			SDL_RenderCopy(renderer, shadow, NULL, &shadow_rect);
		}
		// If playing and not paused: colors; else: draw gray content and figure.
		for(int row = 0; row < CUP_HEIGHT; ++row) {
			for(int col = 0; col < CUP_WIDTH; ++col) {
				if((unsigned)cup.cell(col, row)) {
					SDL_Rect cell_rect = {col * cell_width, row * cell_width, cell_width, cell_width};
					cell_rect.x += cup_rect.x;
					cell_rect.y += cup_rect.y;
					if(!playing) {
						SDL_RenderCopy(renderer, bricks[Color::SHADOW], NULL, &cell_rect);
					} else if(paused){
						SDL_RenderCopy(renderer, bricks[Color::FORE], NULL, &cell_rect);
					} else {
						SDL_RenderCopy(renderer, bricks[cup.cell(col, row)], NULL, &cell_rect);
					}
				}
			}
		}
		for(int row = 0; row < Figure::H; ++row) {
			for(int col = 0; col < Figure::W; ++col) {
				if(current.face().cell(col, row)) {
					SDL_Rect cell_rect = {(figure_pos.x + col) * cell_width, (figure_pos.y + row) * cell_width, cell_width, cell_width};
					cell_rect.x += cup_rect.x;
					cell_rect.y += cup_rect.y;
					if(!playing) {
						SDL_RenderCopy(renderer, bricks[Color::SHADOW], NULL, &cell_rect);
					} else if(paused){
						SDL_RenderCopy(renderer, bricks[Color::FORE], NULL, &cell_rect);
					} else {
						SDL_RenderCopy(renderer, bricks[current.color], NULL, &cell_rect);
					}
				}
			}
		}
		SDL_Rect next_view_rect = {cup_rect.x + cup_rect.w + cell_width, cup_rect.y, cell_width, cell_width};
		SDL_RenderFillRect(renderer, &next_view_rect);
		for(int row = 0; row < Figure::H; ++row) {
			for(int col = 0; col < Figure::W; ++col) {
				if(next_figure.face().cell(col, row)) {
					SDL_Rect cell_rect = {col * cell_width, row * cell_width, cell_width, cell_width};
					cell_rect.x += next_view_rect.x;
					cell_rect.y += next_view_rect.y;
					if(!playing) {
						SDL_RenderCopy(renderer, bricks[Color::SHADOW], NULL, &cell_rect);
					} else if(paused){
						SDL_RenderCopy(renderer, bricks[Color::FORE], NULL, &cell_rect);
					} else {
						SDL_RenderCopy(renderer, bricks[next_figure.color], NULL, &cell_rect);
					}
				}
			}
		}
		SDL_SetRenderDrawColor(renderer, Chthon::get_red(Chthon::Color(Color::FORE)), Chthon::get_green(Chthon::Color(Color::FORE)), Chthon::get_blue(Chthon::Color(Color::FORE)), 255);
		SDL_RenderDrawRect(renderer, &cup_rect);

		SDL_RenderPresent(renderer);

		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_KEYDOWN) {
				int event_key = event.key.keysym.sym;
				if(event_key == SDLK_q) {
					quit = true;
				} else if(playing) {
					if(event_key == SDLK_UP) {
						Figure rotated = current;
						rotated.rotate();
						if(valid_pos(cup, figure_pos, rotated)) {
							current = rotated;
						}
					} else if(event_key == SDLK_DOWN) {
						while(valid_pos(cup, figure_pos, current)) {
							figure_pos = shifted(figure_pos, Chthon::Point(0, 1));
						}
						figure_pos = shifted(figure_pos, Chthon::Point(0, -1));
					} else if(event_key == SDLK_LEFT) {
						if(valid_pos(cup, shifted(figure_pos, Chthon::Point(-1, 0)), current)) {
							figure_pos = shifted(figure_pos, Chthon::Point(-1, 0));
						}
					} else if(event_key == SDLK_RIGHT) {
						if(valid_pos(cup, shifted(figure_pos, Chthon::Point(1, 0)), current)) {
							figure_pos = shifted(figure_pos, Chthon::Point(1, 0));
						}
					} else if(event_key == SDLK_p) {
						paused = !paused;
					}
				}
			} else if(event.type == SDL_QUIT) {
				quit = true;
			}
		}

		if(playing && !paused) {
			Uint32 current_ticks = SDL_GetTicks();
			if(current_ticks - previous_time > (START_SPEED - SPEED_MODIFIER * speed)) {
				previous_time = current_ticks;
				if(valid_pos(cup, shifted(figure_pos, Chthon::Point(0, 1)), current)) {
					figure_pos = shifted(figure_pos, Chthon::Point(0, 1));
				}
			}

			if(may_be_fixated(cup, figure_pos, current)) {
				fixate(current, figure_pos, cup);
				int lines_cleared = clear_filled_rows(cup);
				score += lines_cleared;
				if(score >= SCORE_LEVEL) {
					speed += 1;
					score = 0;
				}

				current = next_figure;
				figure_pos = start_pos;
				if(!valid_pos(cup, figure_pos, current)) {
					playing = false;
				}
				next_figure = figures[rand() % figures.size()];
			}
		}
	}
	return 0;
}
