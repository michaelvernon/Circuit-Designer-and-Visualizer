import networkx as nx
import matplotlib.pyplot as plt
import sys

assert(1 != len(sys.argv)), "Data file name needed..."
filename=sys.argv[1]

edges=[]
f=open(filename,'r')
battery=f.readline().split()
battery=map(lambda x:int(x),battery)


for line in f:
    edges.append(line.split())


G = nx.DiGraph()
G.add_edges_from([(int(u),int(v),{'weight': float(w)}) for (u,v,w) in edges])

MAX=float(max([w for (u,v,w) in edges]))


pos = nx.circular_layout(G)
nx.draw_networkx_nodes(G, pos, cmap=plt.get_cmap('jet'), node_color = 'k', node_size = 400)
nx.draw_networkx_labels(G, pos, font_color="w")
nx.draw_networkx_edges(G, pos, G.edges(), edge_color=['r' if set(edge[0:2]) == set(battery) else 'k' for edge in G.edges() ],arrows=True,width=[4*w/MAX for (u,v,w) in G.edges.data('weight')])

plt.show()
