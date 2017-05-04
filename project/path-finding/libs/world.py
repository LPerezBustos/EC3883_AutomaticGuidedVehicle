from robot import robot
from SimpleGraph import SimpleGraph
from PriorityQueue import PriorityQueue
from Grid import Grid

class world(object):
	def __init__(self):
		# inicializa la clase de mundo
		self.graph = SimpleGraph()
		self.graph.edges = {'1' : ['2','8'], 
							'2' : ['1','3','7'], 
							'3' : ['2','4','6'], 
							'4' : ['3','5'], 
							'5' : ['4','6','12'],
							'6' : ['3','5','7','11'], 
							'7' : ['2','6', '8', '10'], 
							'8' : ['1','7', '9'], 
							'9' : ['8','10'], 
							'10' : ['7','9', '11'], 
							'11' : ['6','10', '12'], 
							'12' : ['5','11']} 
		self.posiciones = {'1' : (1,1), 
							'2' : (1,2), 
							'3' : (1,3), 
							'4' : (1,4), 
							'5' : (2,1),
							'6' : (2,2), 
							'7' : (2,3), 
							'8' : (2,4), 
							'9' : (3,1), 
							'10' : (3,2), 
							'11' : (3,3), 
							'12' : (3,4)}
		self.goal = '5'
		self.start = '1'	
		self.plan = []
	
	def getNode(self, pt):
		# retorna el indice del nodo donde se ubica el vehiculo
		pass

	def cost(self, a, b):
		# define una heuristica para la busqueda informada
		(x1, y1) = a
		(x2, y2) = b
		return abs(x1 - x2) + abs(y1 - y2)

	def a_star(self):
		# Genera camino usando el algoritmo A*
		frontier = PriorityQueue()
		frontier.put(self.start, 0)
		came_from = {}
		cost_so_far = {}
		came_from[self.start] = None
		cost_so_far[self.start] = 0
		
		# comienza la busqueda
		while not frontier.empty():
			current = frontier.get()
			# termina si llega a la meta
			if current == self.goal:
				break
			# busca en sus vecinos
			for next in self.graph.neighbors(current):
				new_cost = cost_so_far[current] + self.graph.cost(current, next)
				if next not in cost_so_far or new_cost < cost_so_far[next]:
					cost_so_far[next] = new_cost
					priority = new_cost + self.cost(self.posiciones[self.goal], self.posiciones[next])
					frontier.put(next, priority)
					came_from[next] = current
		# ordena el plan
		current = self.goal
		path = [current]
		while current != self.start:
			current = came_from[current]
			path.append(current)
		# invierte el camino para empezar al inicio
		path.reverse()
		self.plan = path