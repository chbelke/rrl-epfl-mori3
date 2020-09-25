

def BFS(self, start, end, connections):
    visited = []
    queue = [[start]]

    if start == end:
        return [start]

    while queue:
        path = queue.pop(0)
        node = path[-1]
        if node not in visited:
            visited.append(node)
            neighbours = connections[node]
            for neighbour in neighbours:
                if neighbour != 0:
                    new_path = list(path)
                    new_path.append(neighbour)
                    queue.append(new_path)                      
                    if neighbour == end:
                        # print("Fin!")
                        # print(new_path)
                        return new_path
    return [False]