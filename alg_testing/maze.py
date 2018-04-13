
# coding: utf-8

# #### Python algorithm testing: for easy POC + debugging ###

# In[66]:


from queue import Queue


# In[67]:


# 16x16 grid representing walls, 2D "bytearray"
# Each cell is a binary integer: north, east, south, west represented in a bit
# 0 represents a wall
# The leftmost bit will be whether or not the cell has been checked

CHECKED = 0b10000
NORTH   = 0x1
EAST    = 0x2
SOUTH   = 0x4
WEST    = 0x8
# The size of each cell in the output
PRINT_SIZE = 13


# In[68]:


# # 16x16, mazes thanks to https://github.com/micromouseonline/micromouse-maze
# physical_maze = [
#   0x0e, 0x08, 0x0a, 0x08, 0x08, 0x0a, 0x08, 0x0a, 0x09, 0x0c, 0x09, 0x0c, 0x08, 0x0b, 0x0c, 0x09,
#   0x0c, 0x02, 0x08, 0x01, 0x05, 0x0c, 0x02, 0x0a, 0x02, 0x03, 0x06, 0x03, 0x06, 0x0a, 0x03, 0x05,
#   0x05, 0x0e, 0x03, 0x06, 0x00, 0x02, 0x0a, 0x0a, 0x08, 0x08, 0x0b, 0x0c, 0x0a, 0x08, 0x09, 0x05,
#   0x04, 0x09, 0x0c, 0x09, 0x07, 0x0c, 0x09, 0x0d, 0x05, 0x05, 0x0e, 0x01, 0x0d, 0x07, 0x05, 0x05,
#   0x05, 0x04, 0x01, 0x05, 0x0c, 0x03, 0x04, 0x01, 0x06, 0x02, 0x0b, 0x04, 0x02, 0x0a, 0x01, 0x05,
#   0x04, 0x01, 0x04, 0x01, 0x04, 0x09, 0x07, 0x06, 0x09, 0x0e, 0x0a, 0x01, 0x0e, 0x0b, 0x05, 0x05,
#   0x05, 0x04, 0x01, 0x07, 0x05, 0x06, 0x08, 0x0b, 0x06, 0x09, 0x0d, 0x04, 0x0a, 0x0a, 0x01, 0x05,
#   0x04, 0x03, 0x05, 0x0e, 0x03, 0x0c, 0x02, 0x08, 0x09, 0x04, 0x03, 0x04, 0x08, 0x0a, 0x01, 0x05,
#   0x06, 0x08, 0x02, 0x0b, 0x0c, 0x02, 0x09, 0x06, 0x03, 0x06, 0x09, 0x05, 0x07, 0x0d, 0x05, 0x05,
#   0x0e, 0x00, 0x0b, 0x0d, 0x05, 0x0d, 0x07, 0x0c, 0x0a, 0x08, 0x02, 0x02, 0x08, 0x02, 0x01, 0x05,
#   0x0e, 0x03, 0x0c, 0x01, 0x05, 0x06, 0x09, 0x05, 0x0c, 0x00, 0x09, 0x0d, 0x06, 0x09, 0x05, 0x05,
#   0x0c, 0x08, 0x01, 0x05, 0x04, 0x0b, 0x05, 0x04, 0x03, 0x05, 0x06, 0x01, 0x0d, 0x06, 0x01, 0x05,
#   0x07, 0x05, 0x06, 0x00, 0x00, 0x0b, 0x05, 0x06, 0x0a, 0x03, 0x0d, 0x06, 0x00, 0x0b, 0x05, 0x05,
#   0x0e, 0x00, 0x0b, 0x05, 0x07, 0x0c, 0x03, 0x0c, 0x0a, 0x08, 0x02, 0x08, 0x02, 0x08, 0x01, 0x05,
#   0x0e, 0x01, 0x0e, 0x03, 0x0c, 0x02, 0x08, 0x02, 0x08, 0x02, 0x09, 0x06, 0x09, 0x05, 0x07, 0x05,
#   0x0e, 0x02, 0x0a, 0x0a, 0x02, 0x0a, 0x02, 0x0a, 0x02, 0x0a, 0x02, 0x0a, 0x02, 0x02, 0x0a, 0x03
# ]

physical_maze = [0 for i in range(256)]
# raw_maze = ''
# Assumes valid maze file....
with open('japan-2011-finals.maz','rb') as f:
    ind = 0
    for line in f:
        for char in line:
            physical_maze[ind] = char
            ind += 1
print(len(physical_maze))
print(physical_maze)


# In[69]:


def printMaze(maze,robot,n_weights=None,w_weights=None):
    # maze out is a list of strings, each representing a row
    maze_out = [list((' '*PRINT_SIZE).join('+'*17))]
    maze_out += [list(' '*((PRINT_SIZE+1)*16+1))]
    maze_out += [list(' '*((PRINT_SIZE+1)*16+1))]
    for line in range(15):
        maze_out += [list('+'+(' '*(PRINT_SIZE-1)) + (('+'+' '*(PRINT_SIZE-1)).join(list(' '*16))) + '+')]
        maze_out += [list(' '*((PRINT_SIZE+1)*16+1))]
        maze_out += [list(' '*((PRINT_SIZE+1)*16+1))]
    maze_out += [list((' '*PRINT_SIZE).join('+'*17))]
    # if the maze is a 2d array, unpack it 
    if type(maze[0]) == list:
        maze = list(list(reversed(i) for i in zip(*maze[::-1])))
        m2 = []
        for i in maze:
            for j in i:
                m2.append(j)
        maze = m2
    for ind,i in enumerate(maze):
        # In the data, the mouse 
        # row is ind%16, col is ind/16 (rotated)
        # from 0,0, bottommost row, leftmost column
        # to 1,0, second to bottom row, leftmost column
        if i & NORTH:
            for s in range(1,PRINT_SIZE+1):
                maze_out[48-((ind%16)+1)*3][int(ind/16)*(PRINT_SIZE+1)+s] = '-'
        if i & EAST:
            maze_out[48-((ind%16)+1)*3 + 1][int(ind/16)*(PRINT_SIZE+1)+(PRINT_SIZE+1)] = '|'
            maze_out[48-((ind%16)+1)*3 + 2][int(ind/16)*(PRINT_SIZE+1)+(PRINT_SIZE+1)] = '|'
        if i & SOUTH:
            for s in range(1,PRINT_SIZE+1):
                maze_out[48-((ind%16))*3][int(ind/16)*(PRINT_SIZE+1)+s] = '-'
        if i & WEST:
            maze_out[48-((ind%16)+1)*3 + 1][int(ind/16)*(PRINT_SIZE+1)] = '|'
            maze_out[48-((ind%16)+1)*3 + 2][int(ind/16)*(PRINT_SIZE+1)] = '|'
    
    #mouse location
    if robot.dir & NORTH:
        maze_out[48-(robot.row*3+2)][robot.col*(PRINT_SIZE+1)+2] = '^'
    if robot.dir & EAST:
        maze_out[48-(robot.row*3+2)][robot.col*(PRINT_SIZE+1)+3] = '>'
    if robot.dir & SOUTH:
        maze_out[48-(robot.row*3+2)][robot.col*(PRINT_SIZE+1)+2] = 'v'
    if robot.dir & WEST:
        maze_out[48-(robot.row*3+2)][robot.col*(PRINT_SIZE+1)+1] = '<'
    
    
    #Prints where the weights are
    if n_weights:
        for n,i in enumerate(n_weights):
            for m,j in enumerate(i):
                if j != 99999:
                    # Ad-hoc but efficient way of guaranteeing 3 characters to print....
                    v = 'N'+str(j+.0000001)[:int(PRINT_SIZE/2-1)]
                    for ind in range(len(v)):
                        maze_out[48-(n*3+2)][m*(PRINT_SIZE+1)+ind+1+int(PRINT_SIZE/2)] = v[ind]
    if w_weights:
        for n,i in enumerate(w_weights):
            for m,j in enumerate(i):
                if j != 99999:
                    v = 'W'+str(j+.0000001)[:int(PRINT_SIZE/2-1)]
                    for ind in range(len(v)):
                        maze_out[48-(n*3+1)][m*(PRINT_SIZE+1)+ind+1] = v[ind]


    #print('\n'.join([''.join(i) for i in maze_out]))
    with open('mazeiters','a+') as f:
        f.write('\n'.join([''.join(i) for i in maze_out])+'\n'+'#'*100+'\n')


# In[70]:


def search(row,col):
    return physical_maze[col*16+row]


# In[71]:


# Cell weight tracker
class Cell:
    def __init__(self,north=None, south=None, east=None, west=None):
        self.north = north
        self.west = west
    
    def __repr__(self):
        return 'Cell:' + 'N: ' + str(self.north) + 'W: ' + str(self.west) + '   '


# In[72]:


neighbor_pattern = [(0,1),(0,-1),(1,0),(-1,0)]


# In[191]:


class Mouse:
    # This will be code to translate to arduino code
    def __init__(self,row=0,col=0,edge=SOUTH):
        self.row = row
        self.col = col
        #after searching has started, this will only be NORTH or EAST
        self.edge = edge
        self.dir = NORTH
        #Each cell in the memory is initialized to 0, or no walls.
        self.memory = [[0]*16 for i in range(16)]
        #Each node in the weights represents an edge
        self.n_weights = [[99999]*16 for i in range(16)]
        self.w_weights = [[99999]*16 for i in range(16)]
        self.old_n = [[None]*16 for i in range(16)]
        self.old_w = [[None]*16 for i in range(16)]
        self.goal_found = False
        self.path = []
        self.relative_path = []
            
    def neighbors(self,row,col):
        x = [(row+1,col), (row-1,col),(row,col+1),(row,col-1)]
        return [i for i in x if i[0]<16 and i[1]<16 and i[0]>0 and i[1]>0]
    
    def set_weight(self, r,c, direction, value):
        if r < 0 or r > 15 or c < 0 or c > 15:
            return
        if direction == NORTH and (self.n_weights[r][c] == None or self.n_weights[r][c] > value):
            self.n_weights[r][c] = value
        if direction == WEST and (self.w_weights[r][c] == None or self.w_weights[r][c] > value):
            self.w_weights[r][c] = value
            
    def at_goal(self):
        return self.row in (7,8) and self.col in (7,8)
    
    #floodfill algorithm on the mouse memory
    #based on bfs
    def floodfill(self): 
        # TODO: delete this line, it's unnecessary
        visited = []
        
        q = Queue()
        # row, col, edge, dist
        q.put((7,7,NORTH,0))
        while not q.empty():
            cur = q.get()
            if cur[0] < 0 or cur[0] > 15 or cur[1] < 0 or cur[1] > 15                 or ((cur[0],cur[1],cur[2]) in visited)                 or self.memory[cur[0]][cur[1]] == 0                 or (self.memory[cur[0]][cur[1]] & cur[2]):
                continue
            printMaze(self.memory,Mouse(cur[0],cur[1],cur[2]),self.n_weights,self.w_weights)
            visited.append((cur[0],cur[1],cur[2]))
            self.set_weight(cur[0],cur[1],cur[2],cur[3])

            if cur[2] == WEST:
                if self.memory[cur[0]][cur[1]] & WEST == 0:
                    q.put((cur[0],cur[1]-1,NORTH,cur[3]+.7))
                    q.put((cur[0],cur[1]-1,WEST,cur[3]+1))
                    q.put((cur[0]-1,cur[1]-1,NORTH,cur[3]+.7))
                q.put((cur[0],cur[1],NORTH,cur[3]+.7))
                q.put((cur[0],cur[1]+1,WEST,cur[3]+1))
                q.put((cur[0]-1,cur[1],NORTH,cur[3]+.7))
            if cur[2] == NORTH:
                if self.memory[cur[0]][cur[1]] & NORTH == 0:
                    q.put((cur[0]+1,cur[1],NORTH,cur[3]+1))
                    q.put((cur[0]+1,cur[1],WEST,cur[3]+.7))
                    q.put((cur[0]+1,cur[1]+1,WEST,cur[3]+.7))
                q.put((cur[0]-1,cur[1],NORTH,cur[3]+1))
                q.put((cur[0],cur[1],WEST,cur[3]+.7))
                q.put((cur[0],cur[1]+1,WEST,cur[3]+.7))
                
    
    
#-----------------------------------------------------------------------------------------------
    # Important to note that there's no logic for telling how many "units" we've moved in this
    # End goal: diagonal maze solving like http://ieeexplore.ieee.org/document/6852576/ 
    # So each edge has a distance, and we'll assume we start on the bottom edge...        
    
    def map_helper(self):
        self.map(self.edge)
    #edge here is the edge we start at
    def map(self,edge):
        history = []
        next_node = []
        # next node has its locations and the previous location [1]
        next_node.append(((0,0),(0,0)))
        prevlen = 0
        while len(next_node) != 0:
            cur = next_node.pop()
#             print(next_node)
#             print(history)
#             print(cur[0])
#             print(cur[1])
#             print((self.row,self.col))
#             print()
            if (self.row,self.col) != cur[1]:
                while (self.row,self.col) != cur[1]:
                    i = history.pop()

                    assert abs(self.row-i[0]) <= 1,'row failure'
                    assert abs(self.col-i[1]) <= 1,'col failure'
                    assert abs(self.row-i[0]) + abs(self.col-i[1]) < 2, 'moved diagonally or >1 square'
                    self.row = i[0]
                    self.col = i[1]
                history.append((self.row,self.col))

            self.row,self.col = cur[0]
            if self.at_goal():
                break
            self.memory[self.row][self.col] = search(*cur[0]) + CHECKED
            printMaze(self.memory,self,self.n_weights,self.w_weights)
            prevlen = len(next_node)
            
            if self.row > 7 and self.col < 7:
                dirs = [WEST,NORTH,EAST,SOUTH]
            elif self.row < 7 and self.col < 7:
                dirs = [WEST,SOUTH,EAST,NORTH]
            elif self.row < 7 and self.col > 7:
                dirs = [EAST,SOUTH,WEST,NORTH]
            else:
                dirs = [EAST,NORTH,WEST,SOUTH]
            for d in dirs:
                if self.memory[self.row][self.col] & d==0:
                    if d == WEST and self.memory[self.row][self.col-1] & CHECKED == 0:
                            next_node.append(((self.row,self.col-1),(self.row,self.col)))
                    elif d == EAST and self.memory[self.row][self.col+1] & CHECKED == 0:
                            next_node.append(((self.row,self.col+1),(self.row,self.col)))
                    elif d == SOUTH and self.memory[self.row-1][self.col] & CHECKED == 0:
                            next_node.append(((self.row-1,self.col),(self.row,self.col)))
                    elif d == NORTH and self.memory[self.row+1][self.col] & CHECKED == 0:
                            next_node.append(((self.row+1,self.col),(self.row,self.col)))

            if prevlen != len(next_node):
                history.append(cur[0])
        old_memory = list([list(i) for i in self.memory])
        self.memory = [[0]*16 for i in range(16)]
        
        history = []
        next_node = []
        # next node has its locations and the previous location
        next_node.append(((self.row,self.col),(self.row,self.col)))
        prevlen = 0
        while len(next_node) != 0:
            cur = next_node.pop() 
#             print(next_node)
#             print(history)
#             print(cur[0])
#             print(cur[1])
#             print((self.row,self.col))
#             print()
            if (self.row,self.col) != cur[1]:
                while (self.row,self.col) != cur[1]:
                    i = history.pop()

                    assert abs(self.row-i[0]) <= 1,'row failure'
                    assert abs(self.col-i[1]) <= 1,'col failure'
                    assert abs(self.row-i[0]) + abs(self.col-i[1]) < 2, 'moved diagonally or >1 square'
                    self.row = i[0]
                    self.col = i[1]
                history.append((self.row,self.col))
            self.row,self.col = cur[0]
            if self.row == 0 and self.col == 0:
                break
            self.memory[self.row][self.col] = search(*cur[0]) + CHECKED
            printMaze(self.memory,self,self.n_weights,self.w_weights)
            prevlen = len(next_node)
            for d in [NORTH,EAST,SOUTH,WEST]:
                if self.memory[self.row][self.col] & d==0:
                    if d == WEST and self.memory[self.row][self.col-1] & CHECKED == 0:
                            next_node.append(((self.row,self.col-1),(self.row,self.col)))
                    elif d == EAST and self.memory[self.row][self.col+1] & CHECKED == 0:
                            next_node.append(((self.row,self.col+1),(self.row,self.col)))
                    elif d == SOUTH and self.memory[self.row-1][self.col] & CHECKED == 0:
                            next_node.append(((self.row-1,self.col),(self.row,self.col)))
                    elif d == NORTH and self.memory[self.row+1][self.col] & CHECKED == 0:
                            next_node.append(((self.row+1,self.col),(self.row,self.col)))

            if prevlen != len(next_node):
                history.append(cur[0])

        for i in range(16):
            for j in range(16):
                self.memory[i][j] = max(self.memory[i][j],old_memory[i][j])   
                
    def get_dir_value(self,x):
        if x == NORTH:
            return 0
        elif x & NORTH and x & EAST:
            return 45
        elif x == EAST:
            return 90
        elif x & SOUTH and x & EAST:
            return 135
        elif x == SOUTH:
            return 180
        elif x & SOUTH and x & WEST:
            return -135
        elif x == WEST:
            return -90
        return -45
        
    def create_path(self):
        # can just be struct of command (turning (0,1), value (if turning, angle, if not, dist)
        q = []
        e = NORTH
        d = 0
        r = 0
        c = 0
        cur_dist = 0
        old = False
        dist_delta = 0.707
        while not (r in (7,8) and c in (7,8)):
            #Implies that a component of the direction is NORTH
            #Also implies it's not inside a wall..
        
            if e == NORTH:
                if (self.memory[r+1][c] & NORTH) == 0 and self.n_weights[r+1][c] < self.w_weights[r+1][c] and self.n_weights[r+1][c] < self.w_weights[r+1][c+1]:
                    angle = self.get_dir_value(NORTH)-d
                    d = self.get_dir_value(NORTH)
                    dist_delta = 1
                elif (self.memory[r+1][c] & EAST) == 0 and self.w_weights[r+1][c] > self.w_weights[r+1][c+1]:
                    angle = self.get_dir_value(NORTH+EAST)-d
                    dist_delta = 0.707
                    e = EAST
                    d = self.get_dir_value(NORTH+EAST)
                # this can be an else because this is the last option if floodfill worked correctly
                else:
                    angle = self.get_dir_value(NORTH+WEST)-d
                    dist_delta = 0.707
                    e = WEST
                    d = self.get_dir_value(NORTH+WEST)
                r += 1
            elif e == WEST:
                if (self.memory[r][c-1] & WEST) == 0 and self.w_weights[r][c-1] < self.n_weights[r][c-1] and self.w_weights[r][c-1] < self.n_weights[r-1][c-1]:
                    angle = self.get_dir_value(WEST)-d 
                    d = self.get_dir_value(WEST)
                    dist_delta = 1
                elif (self.memory[r][c-1] & NORTH) == 0 and self.n_weights[r][c-1] < self.n_weights[r-1][c-1]:
                    angle = self.get_dir_value(NORTH+WEST)-d
                    dist_delta = 0.707
                    e = NORTH
                    d = self.get_dir_value(NORTH+WEST)
                else:
                    angle = self.get_dir_value(SOUTH+WEST)-d
                    dist_delta = 0.707
                    e = SOUTH
                    d = self.get_dir_value(SOUTH+WEST)
                c -= 1
            elif e == SOUTH:
                if (self.memory[r-1][c] & SOUTH) == 0 and self.n_weights[r-2][c] < self.w_weights[r-1][c] and self.n_weights[r-2][c] < self.w_weights[r-1][c+1]:
                    angle = self.get_dir_value(SOUTH) -d
                    d = self.get_dir_value(SOUTH)
                    dist_delta = 1
                elif (self.memory[r-1][c] & EAST) == 0 and self.w_weights[r-1][c+1] < self.w_weights[r-1][c]:
                    angle = self.get_dir_value(SOUTH+EAST)-d
                    dist_delta = 0.707
                    e = EAST
                    d = self.get_dir_value(SOUTH+EAST)
                else:
                    angle = self.get_dir_value(SOUTH+WEST)-d
                    dist_delta = 0.707
                    e = WEST
                    d = self.get_dir_value(SOUTH+WEST)
                r -= 1
            elif e == EAST:
                if (self.memory[r][c+1] & EAST) == 0 and self.w_weights[r][c+2] < self.n_weights[r][c+1] and self.w_weights[r][c+2] < self.n_weights[r-1][c+1]:
                    angle = self.get_dir_value(EAST) -d
                    d = self.get_dir_value(EAST)
                    dist_delta = 1
                elif (self.memory[r][c+1] & NORTH) == 0 and self.n_weights[r][c+1] < self.n_weights[r-1][c+1]:
                    angle = self.get_dir_value(NORTH+EAST)-d
                    dist_delta = 0.707
                    e = NORTH
                    d = self.get_dir_value(EAST+NORTH)
                else:
                    angle = self.get_dir_value(SOUTH+EAST)-d
                    dist_delta = 0.707
                    e = SOUTH
                    d = self.get_dir_value(SOUTH+EAST)
                c += 1
                
                
            cur_dist += dist_delta
            if angle != 0:
                q.append((1,angle))
            q.append((0, cur_dist))
            cur_dist = 0
            print(q)
                
            


# In[192]:


open('mazeiters','w+').close()
m = Mouse(edge=NORTH)
m.map_helper()
printMaze(m.memory,m,m.n_weights,m.w_weights)  



# In[193]:


#we might need a special case for where the mouse starts
m.floodfill()
printMaze(m.memory,m,m.n_weights,m.w_weights)  


# In[194]:


for i in m.memory:
    for j in i:
        print(str(j)+' ',end='')
    print()


# In[195]:


m.create_path()

