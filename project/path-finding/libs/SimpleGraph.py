class SimpleGraph:
	def __init__(self):
		self.edges = {}
    
	def neighbors(self, id):
		return self.edges[id]

	def cost(self, a, b):
		return 1