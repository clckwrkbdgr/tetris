import random
import copy
import pygame
from pygame.locals import *

COLORS = {
	"BACK" : (0, 0, 0),
	"FORE" : (255, 255, 255),
	"SHADOW" : (10, 10, 10),
	"I" : (0, 255, 255),
	"J" : (0, 0, 255),
	"L" : (255, 128, 0),
	"O" : (255, 255, 0),
	"S" : (0, 255, 0),
	"T" : (255, 0, 255),
	"Z" : (255, 0, 0)
	}
CUP_WIDTH = 10
CUP_HEIGHT = 22
SCORE_LEVEL = 10 # Rows for one speed level.

def init_sdl():
	pygame.init()
	pygame.mouse.set_visible(False)
	screen = pygame.display.set_mode((640, 480)) #, FULLSCREEN)
	return screen

class Figure:
	W = 4
	H = 4

	def __init__(self, color, array):
		self.color = color
		self.array = copy.deepcopy(array)

	def face(self):
		return self.array[0]

	def rotate(self):
		self.array.append(self.array.pop(0))
	
	def clone(self):
		return Figure(self.color, self.array)

def array_from_points(figure_as_points):
	array = []
	for rotational_pose in figure_as_points:
		rows = [[False for x in range(Figure.W)] for y in range(Figure.H)]
		for col, row in rotational_pose:
			rows[row][col] = True
		array.append(rows)
	return array

def make_figures():
	result = []
	# TODO figures
	result.append(Figure("Z", array_from_points([[(0, 0), (1, 1)], [(1, 1), (1, 2)]])))
	return result

def shifted(pos, shift):
	return [pos[0] + shift[0], pos[1] + shift[1]]

def valid_pos(cup, pos, figure):
	for col in range(Figure.W):
		for row in range(Figure.H):
			if figure.face()[row][col]:
				if row + pos[1] < 0 or row + pos[1] >= CUP_HEIGHT:
					return False
				if col + pos[0] < 0 or col + pos[0] >= CUP_WIDTH:
					return False
				if cup[row + pos[1]][col + pos[0]]:
					return False
	return True

def may_be_fixated(cup, pos, figure):
	return not valid_pos(cup, shifted(pos, (0, 1)), figure)

def fixate(figure, pos, cup):
	for col in range(Figure.W):
		for row in range(Figure.H):
			if figure.face()[row][col]:
				cup[row + pos[1]][col + pos[0]] = figure.color

def make_surfaces(screen, colors):
	screen_size = screen.get_size()
	cell_width = min(screen_size[0] / CUP_WIDTH, screen_size[1] / CUP_HEIGHT)
	cell_size = (cell_width, cell_width)

	cup_surface = pygame.surface.Surface( (cell_width * CUP_WIDTH, cell_width * CUP_HEIGHT) )
	cup_surface.fill(COLORS["BACK"])
	pygame.draw.rect(cup_surface, COLORS["FORE"], cup_surface.get_rect(), 1)
	cup_rect = cup_surface.get_rect()
	cup_rect.center = screen.get_rect().center

	shadow = pygame.surface.Surface( (cell_width, cell_width * CUP_HEIGHT) )

	bricks = {}
	for color in colors:
		brick = pygame.surface.Surface(cell_size)
		brick.fill(COLORS[color])
		pygame.draw.rect(brick, COLORS["BACK"], brick.get_rect(), 1)
		bricks[color] = brick

	next_view = pygame.surface.Surface( (cell_width * Figure.W, cell_width * Figure.H) )

	return cell_size, cup_surface, cup_rect, shadow, bricks, next_view

def get_shadow(cup, pos, figure):
	shadow = set()
	for col in range(Figure.W):
		for row in range(Figure.H):
			if figure.face()[row][col]:
				shadow.add(pos[0] + col)
				break
	return shadow

def clear_filled_rows(cup):
	lines_cleared = 0
	new_cup = []
	for row in reversed(range(CUP_HEIGHT)):
		if None in cup[row]:
			new_cup.insert(0, cup[row])
		else:
			lines_cleared += 1
	for row in range(CUP_HEIGHT - len(new_cup)):
		new_cup.insert(0, [None * CUP_WIDTH])
	return new_cup, lines_cleared

def main():
	screen = init_sdl()
	figures = make_figures()

	cup = [[None for x in range(CUP_WIDTH)] for y in range(CUP_HEIGHT)]
	start_pos = [(CUP_WIDTH - Figure.W) / 2, 0]
	current = random.choice(figures).clone()
	next_figure = random.choice(figures).clone()

	cell_size, cup_surface, cup_rect, shadow, bricks, next_view = make_surfaces( screen, ["Z"]) # TODO figures

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
			if pygame.time.get_ticks() - previous_time > 500:
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

		# Drawing.
		screen.fill(COLORS["BACK"])
		screen.blit(cup_surface, cup_rect)
		shadows = get_shadow(cup, figure_pos, current)
		for col in shadows:
			shadow_rect = pygame.Rect(cell_size[0] * col, cup_rect.top, cell_size[0], cup_rect.height)
			screen.blit(shadow, shadow_rect)
		#if playing and not paused: colors; else: draw gray content and figure
		for row in range(CUP_HEIGHT):
			for col in range(CUP_WIDTH):
				if cup[row][col]:
					cell_rect = pygame.Rect((col * cell_size[0], row * cell_size[1]), cell_size)
					cell_rect.move_ip(cup_rect.topleft)
					screen.blit(bricks[cup[row][col]], cell_rect)
		for row in range(Figure.H):
			for col in range(Figure.W):
				if current.face()[row][col]:
					cell_rect = pygame.Rect(((figure_pos[0] + col) * cell_size[0], (figure_pos[1] + row) * cell_size[1]), cell_size)
					cell_rect.move_ip(cup_rect.topleft)
					screen.blit(bricks[current.color], cell_rect)
		next_view.fill(COLORS["BACK"])
		for row in range(Figure.H):
			for col in range(Figure.W):
				if next_figure.face()[row][col]:
					cell_rect = pygame.Rect((col * cell_size[0], row * cell_size[1]), cell_size)
					next_view.blit(bricks[next_figure.color], cell_rect)
		screen.blit(next_view, (cup_rect.topright, cell_size))

		pygame.display.flip()

if __name__ == "__main__":
	main()

