import pygame
import random
from pygame.locals import *

def init_sdl():
	pygame.init()
	pygame.mouse.set_visible(False)
	screen = pygame.display.set_mode((640, 480)) #, FULLSCREEN)
	background = pygame.surface.Surface(screen.get_size())
	background.fill( BACKGROUND )

class Figure:
	def __init__(self):
		self.color = (0, 0, 0)
		self.array = []
		self.

def make_figures():
	create figures (array, rotation point, color)
	Figure.I : (0, 255, 255),
	Figure.J : (0, 0, 255),
	Figure.L : (255, 128, 0),
	Figure.O : (255, 255, 0),
	Figure.S : (0, 255, 0),
	Figure.T : (255, 0, 255),
	Figure.Z : (255, 0, 0)

def main():
	screen = init_sdl(colors)
	figures = make_figures()
	current = random.choice(figures)
	next_figure = random.choice(figures)

	# TODO create 10x22 cup (list of rows)
	start game
		if controls:
			q: quit
			p: pause game
			if not paused and not stopped:
				left/right: move figure, if can (overlapping, cup borders)
				down: move on the lowest possible row
				up: rotate figure using its center
		if enough time passed and not paused and not stopped:
			move current figure 1 row down, if can(overlapping, cup borders)
		if may be fixated:
			fixate figure,
			try to remove full rows and increase score, if score > some value, increase speed.
			make current = next, if overlapping, then stop game.
			make new next,

		draw cup, shadow,
		if not paused and not stopped: draw its content, draw current figure,
		else: draw gray content and figure
		draw next figure in the left-top corner (minimized).

START_SPEED = 500
SPEED_MOD = 0.9
BACKGROUND = (0, 0, 0)
SHADOW = (10, 10, 10)
FOREGROUND = (255, 255, 255)

def main():


	tetris = Tetris(SIZE)
	old_level = tetris.level()
	speed = START_SPEED
	active, playing = True, True
	while True:
		for event in pygame.event.get():
			if event.type == QUIT:
				return
			elif event.type == KEYDOWN:
				if event.key == K_q:
					return
				elif playing:
					if event.key == K_UP:
						tetris.rotate()
					elif event.key == K_DOWN:
						tetris.drop()
					elif event.key == K_LEFT:
						tetris.left()
					elif event.key == K_RIGHT:
						tetris.right()
					elif event.key == K_p:
						active = not active

		if playing and active:
			if not tetris.tick():
				playing = False

			if tetris.level() != old_level:
				speed = SPEED_MOD * speed
			old_level = tetris.level()
			
			drawing(screen, tetris, active, background)

		pygame.time.delay(speed)



class Figure:
	WIDTH = 4
	HEIGHT = 4
	SIZE = WIDTH * HEIGHT
	NONE = 0
	_ = 0
	I = 1
	J = 2
	L = 3
	O = 4
	S = 5
	T = 6
	Z = 7
	COUNT = 8

	I_MASK = [
			0, 4, 0,  0,
			1, 5, 9, 13,
			0, 6, 0,  0,
			0, 7, 0,  0
			]
	J_MASK = [
			2,  6, 10, 15,
			1,  5,  9, 15,
			0,  4,  8, 15,
			15, 15, 15, 15
			]
	O_MASK = [
			0,  1,  2,  3,
			4,  5,  6,  7,
			8,  9, 10, 11,
			12, 13, 14, 15
			]

	I_F = [
			_, I, _, _,
			_, I, _, _,
			_, I, _, _,
			_, I, _, _
			]
	J_F = [
			J, _, _, _,
			J, J, J, _,
			_, _, _, _,
			_, _, _, _
			]
	L_F = [
			_, _, L, _,
			L, L, L, _,
			_, _, _, _,
			_, _, _, _
			]
	O_F = [
			_, _, _, _,
			_, O, O, _,
			_, O, O, _,
			_, _, _, _
			]
	S_F = [
			_, S, S, _,
			S, S, _, _,
			_, _, _, _,
			_, _, _, _
			]
	T_F = [
			_, T, _, _,
			T, T, T, _,
			_, _, _, _,
			_, _, _, _
			]
	Z_F = [
			Z, Z, _, _,
			_, Z, Z, _,
			_, _, _, _,
			_, _, _, _
			]

	def __init__(self, blocks, mask):
		self.blocks = blocks[:]
		self.rotateMask = mask[:]
		self.pos = [0, 0]
	
	def is_valid(self, p):
		return (p[0] >= 0 & p[1] >= 0 & p[0] < Figure.WIDTH & p[1] < Figure.HEIGHT);

	def at(self, p):
		return self.blocks[p[0] + p[1] * Figure.WIDTH]

	def clone(self):
		new = Figure(self.blocks, self.rotateMask)
		new.pos = self.pos
		return new

	def rotated():
		new_figure = self.clone()
		new_figure.blocks = [new_figure.blocks[new_figure.rotateMask[i]] for i in range(Figure.SIZE)]
		return new_figure

FIGURES = {
		Figure.I : Figure(Figure.I_F, Figure.I_MASK),
		Figure.J : Figure(Figure.J_F, Figure.J_MASK),
		Figure.L : Figure(Figure.L_F, Figure.J_MASK),
		Figure.O : Figure(Figure.O_F, Figure.O_MASK),
		Figure.S : Figure(Figure.S_F, Figure.J_MASK),
		Figure.T : Figure(Figure.T_F, Figure.J_MASK),
		Figure.Z : Figure(Figure.Z_F, Figure.J_MASK)
		}


class Tetris:
	ROWS_FOR_LEVEL = 10

	def __init__(self, size):
		self.size = size
		self.cup = [Figure.NONE for i in range(size[0] * size[1])]
		self.figure = self._random_figure(self._start_point())
		self.next_figure = self._random_figure(self._start_point())
		self.score = 0
	
	def is_valid(self, p):
		return (p[0] >= 0 & p[1] >= 0 & p[0] < self.size[0] & p[1] < self.size[1])

	def cell(self, p):
		if not self.is_valid(p):
			return Figure.NONE
		rel = (p[0] - self.figure.pos[0], p[1] - self.figure.pos[1])
		if self.figure.is_valid(rel) and not self._at(p):
			return self.figure.at(rel)
		return self._at(p)

	def shadow(self):
		result = []
		for x in range(Figure.WIDTH):
			for y in range(Figure.HEIGHT):
				if self.figure.at([x, y]):
					result += [self.figure.pos[0] + x]
		return set(result)

	def next(self):
		return self.next_figure

	def level(self):
		return self.score / Tetris.ROWS_FOR_LEVEL;

	def tick(self):
		self.figure.pos[1] += 1
		if not self._overlapping(self.figure):
			return True
		self.figure.pos[1] -= 1

		self._add_figure(self.figure)

		self.figure = self.next_figure.clone()
		self.next_figure = self._random_figure(self._start_point())
		return not self._overlapping(self.figure)

	def drop(self):
		while not self._overlapping(self.figure):
			self.figure.pos[1] += 1
		self.figure.pos[1] -= 1

		self._add_figure(self.figure);
		self.figure = self.next_figure;
		self.next_figure = self._random_figure(self._start_point());

	def left(self):
		self.figure.pos[0] -= 1
		if self._overlapping(self.figure):
			self.figure.pos[0] += 1

	def right(self):
		self.figure.pos[0] += 1
		if self._overlapping(self.figure):
			self.figure.pos[0] -= 1

	def rotate(self):
		new_figure = self.figure.rotated()
		if not self._overlapping(new_figure):
			self.figure = new_figure

	def _at(self, p):
		return self.cup[p[0] + p[1] * self.size[0]]

	def _start_point(self):
		return [self.size[0] / 2 - Figure.WIDTH / 2, 0]

	def _random_figure(self, pos):
		f = FIGURES[random.choice(FIGURES.keys())]
		f.pos = pos
		return f

	def _overlapping(self, figure):
		for x in range(Figure.WIDTH):
			for y in range(Figure.HEIGHT):
				if self.figure.at([x, y]):
					cupPos = [self.figure.pos[0] + x, self.figure.pos[1] + y]
					if not self.is_valid(cupPos) or self._at(cupPos):
						return True
		return False;

	def _add_figure(self, figure):
		for x in range(Figure.WIDTH):
			for y in range(Figure.HEIGHT):
				cupPos = [self.figure.pos[0] + x, self.figure.pos[1] + y]
				if self.figure.at([x, y]) and self.is_valid(cupPos):
					self.cup[cupPos[0] + cupPos[1] * self.size[0]] = self.figure.at([x, y])
		self._remove_full_rows()

	def _remove_full_rows(self):
		y = self.size[1] - 1
		while y >= 0:
			blockCount = 0;
			for x in range(self.size[0]):
				if self._at([x, y]):
					blockCount += 1

			if blockCount == self.size[0]:
				for ty in reversed(range(y + 1)):
					for tx in range(self.size[0]):
						self.cup[tx + ty * self.size[0]] = self.cup[tx + (ty - 1) * self.size[0]]
				self.score += 1;
				y += 1
			y -= 1

SIZE = (10, 22)
START_SPEED = 500
SPEED_MOD = 0.9
BACKGROUND = (0, 0, 0)
SHADOW = (10, 10, 10)
FOREGROUND = (255, 255, 255)

def drawing(screen, tetris, active, background):
	cellwidth = min([screen.get_size()[0] / tetris.size[0], screen.get_size()[1] / tetris.size[1]])
	cupsize = [cellwidth * tetris.size[0], cellwidth * tetris.size[1]]
	startsize = map(lambda x, y: (x - y) / 2, screen.get_size(), cupsize)
	cellsize = (cellwidth, cellwidth)
	cuprect = pygame.Rect(startsize, cupsize)
	statuspos = [cuprect.right + cellwidth, cellwidth]
	nextfigurerect = pygame.Rect(cuprect.right + cellwidth, cellwidth * 2, Figure.WIDTH * cellwidth, Figure.HEIGHT * cellwidth)

	blocks = {
			Figure.I : (0, 255, 255),
			Figure.J : (0, 0, 255),
			Figure.L : (255, 128, 0),
			Figure.O : (255, 255, 0),
			Figure.S : (0, 255, 0),
			Figure.T : (255, 0, 255),
			Figure.Z : (255, 0, 0)
			}
	for figure in blocks:
		surface = pygame.surface.Surface(cellsize)
		surface.fill(blocks[figure])
		pygame.draw.rect(surface, BACKGROUND, surface.get_rect(), 1)
		blocks[figure] = surface

	if not active:
		darkblocks = {}
		for figure in blocks:
			darkblocks[figure] = tuple([x/4 for x in figure[blocks]])
		blocks = darkblocks

	screen.blit(background, background.get_rect())

	shadow = pygame.surface.Surface( (cellwidth, cupsize[1]) ) # cuprect.topleft
	shadow.fill(SHADOW)
	for col in tetris.shadow():
		screen.blit(shadow, (cuprect.left + cellwidth * col, cuprect.top, cellwidth, cupsize[1]))

	pygame.draw.rect(screen, FOREGROUND, cuprect, 1)

	for x in range(tetris.size[0]):
		for y in range(tetris.size[1]):
			block = tetris.cell([x, y])
			if block:
				screen.blit(blocks[block], (cuprect.left + x * cellwidth, cuprect.top + y * cellwidth, cellwidth, cellwidth))

	"""
	painter.setPen(Settings::FOREGROUND);
	painter.setBrush(Settings::BACKGROUND);
	painter.drawText(cupRect.right() + cell.width(), cell.height(),
			tr("Level: %1.").arg(tetris.level()));
	"""

	for x in range(Figure.WIDTH):
		for y in range(Figure.HEIGHT):
			block = tetris.next().at([x, y])
			if block:
				screen.blit(blocks[block], (nextfigurerect.left + x * cellwidth, nextfigurerect.top + y * cellwidth, cellwidth, cellwidth))

	pygame.display.flip()

"""
def main():
	pygame.init()
	pygame.mouse.set_visible(False)
	screen = pygame.display.set_mode((640, 480)) #, FULLSCREEN)
	background = pygame.surface.Surface(screen.get_size())
	background.fill( BACKGROUND )


	tetris = Tetris(SIZE)
	old_level = tetris.level()
	speed = START_SPEED
	active, playing = True, True
	while True:
		for event in pygame.event.get():
			if event.type == QUIT:
				return
			elif event.type == KEYDOWN:
				if event.key == K_q:
					return
				elif playing:
					if event.key == K_UP:
						tetris.rotate()
					elif event.key == K_DOWN:
						tetris.drop()
					elif event.key == K_LEFT:
						tetris.left()
					elif event.key == K_RIGHT:
						tetris.right()
					elif event.key == K_p:
						active = not active

		if playing and active:
			if not tetris.tick():
				playing = False

			if tetris.level() != old_level:
				speed = SPEED_MOD * speed
			old_level = tetris.level()
			
			drawing(screen, tetris, active, background)

		pygame.time.delay(speed)
		"""

if __name__ == "__main__":
	main()

