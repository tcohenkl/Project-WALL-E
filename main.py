import osmnx as ox
import json

# Step 1: Save Graph of Waterloo with radius of 400m of paths that are walkable
G = ox.graph_from_point([43.469455, -80.522522], dist = 400, network_type='walk', custom_filter = '["highway"~"footway|path"]', simplify=True)
ox.io.save_graphml(G, "./maps/walkable_waterloo.osm")


# Step 2: Load graph and optionally plot it
G = ox.io.load_graphml("./maps/walkable_waterloo.osm")
# ox.plot.plot_graph(G)


cleaned_nodes = []
cleaned_edges = []

# Step 3: Only get edges that have geometry with their from and to node info
for from_id, to_id, data in G.edges(data=True):
        if "geometry" in data:
            coords = list(data["geometry"].coords)
            cleaned_edges.append({
                "from": from_id,
                "to": to_id,
                "geometry": coords  # list of (lon, lat) pairs
            })

# Step 4: Get the positions of nodes and whether or not there is a traffic signal
for node_id, data in G.nodes(data = True):
    cleaned_nodes.append({
        "id": node_id,
        "x": data["x"],
        "y": data["y"],
        "is_traffic_signal": data.get("highway") == "traffic_signals"
    })

# Step 5: Remove disconnected nodes (nodes that don't have an edge)
initial_number_of_nodes = len(cleaned_nodes)
connected_ids = set()

for edge in cleaned_edges:
    connected_ids.add(edge["from"])
    connected_ids.add(edge["to"])


cleaned_nodes = [node for node in cleaned_nodes if node["id"] in connected_ids]

# Step 6: Save nodes and edges as jsons
with open("./graphs/nodes.json", "w") as f:
    json.dump(cleaned_nodes, f, indent=2)

with open("./graphs/edges.json", "w") as f:
    json.dump(cleaned_edges, f, indent=2)

print(f"Pruned {initial_number_of_nodes - len(cleaned_nodes)} disconnected nodes.")