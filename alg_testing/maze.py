
# coding: utf-8

# #### Python algorithm testing: for easy POC + debugging ###

# In[2]:


from queue import Queue


# In[3]:


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


# In[4]:


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


# In[5]:


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


# In[6]:


def search(row,col):
    return physical_maze[col*16+row]


# In[7]:


# Cell weight tracker
class Cell:
    def __init__(self,north=None, south=None, east=None, west=None):
        self.north = north
        self.west = west
    
    def __repr__(self):
        return 'Cell:' + 'N: ' + str(self.north) + 'W: ' + str(self.west) + '   '


# In[8]:


class StopRecursion(BaseException):
    pass


# In[9]:


neighbor_pattern = [(0,1),(0,-1),(1,0),(-1,0)]


# In[10]:


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
        self.old_memory = [[0]*16 for i in range(16)]
        self.goal_found = False
        self.path = []
        self.relative_path = []
        self.merged = False
        self.visited = []
            
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
        self.visited = []
        
        q = Queue()
        # row, col, edge, dist
        q.put((7,7,NORTH,0))
        while not q.empty():
            cur = q.get()
            if cur[0] < 0 or cur[0] > 15 or cur[1] < 0 or cur[1] > 15                 or ((cur[0],cur[1],cur[2]) in self.visited)                 or self.memory[cur[0]][cur[1]] == 0                 or (self.memory[cur[0]][cur[1]] & cur[2]):
                continue
            printMaze(self.memory,Mouse(cur[0],cur[1],cur[2]),self.n_weights,self.w_weights)
            self.visited.append((cur[0],cur[1],cur[2]))
            if cur[2] in (NORTH,WEST):
                self.set_weight(cur[0],cur[1],cur[2],cur[3])
            print(cur[0],cur[1],cur[2],cur[3])

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
        try:
            self.map(0,self.edge)
        except StopRecursion:
            pass
    #edge here is the edge we start at
    def map(self,dist,edge):
    # TODO DELETE DIST
        if self.at_goal() and not self.goal_found:
            self.goal_found = True
#             self.old_n = list(self.n_weights)
#             self.old_w = list(self.w_weights)
            #reset memory to find home
            self.n_weights = [[99999]*16 for i in range(16)]
#             self.w_weights = [[99999]*16 for i in range(16)]
            self.old_memory = list([list(i) for i in self.memory])
            self.memory = [[0]*16 for i in range(16)]
            self.map(0,edge)
            return
        #print(self.row,self.col)
        
        if self.goal_found and (self.row == 0 and self.col == 0):
#                                 #or (edge != NORTH and self.row-1 == 0 and self.col == 0)
#                                 #or (edge != WEST and self.row == 0 and self.col+1 == 0)):
#             #There's a potential optimization here: when we're heading back home we should always 
#             #go towards the bottom right corner of the maze: 0,0. This means that below in the recursive part,
#             #the ordering could change...
            if self.merged:
                return            
            for i in range(16):
                for j in range(16):
                    self.memory[i][j] = max(self.memory[i][j],self.old_memory[i][j])
#             # merge memories for solving
#             # this is the real maximum because the other one is 99999, aka infinite weighting
#             maximum = min(self.n_weights[0][0],self.w_weights[0][0])
#             printMaze(physical_maze,self,self.n_weights,self.w_weights)        

#             for i in range(16):
#                 for j in range(16):
#                     if self.n_weights[i][j] != 99999 and maximum >= self.n_weights[i][j] :
#                         self.n_weights[i][j] = min(maximum - self.n_weights[i][j], self.old_n[i][j])
#                     else:
#                         self.n_weights[i][j] = 99999
#                     if self.w_weights[i][j] != 99999 and maximum >= self.w_weights[i][j]:
#                         self.w_weights[i][j] = min(maximum - self.w_weights[i][j], self.old_w[i][j])
#                     else:
#                         self.n_weights[i][j] = 99999
            self.merged = True
            raise StopRecursion()
            
        if self.memory[self.row][self.col] & CHECKED:
#             if not self.memory[self.row][self.col] & WEST:
#                 self.set_weight(self.row,self.col,WEST,dist+.7)
#             if not self.memory[self.row][self.col] & NORTH:
#                 self.set_weight(self.row,self.col,NORTH,dist+.7)
            return
        self.memory[self.row][self.col] = search(self.row,self.col) + CHECKED
        printMaze(self.memory,self,self.n_weights,self.w_weights)        

        
        for d in [NORTH,SOUTH,EAST,WEST]:
            if self.merged:
                return
            # if there is not a wall                
            if not self.memory[self.row][self.col] & d:
                #print(self.memory[self.row][self.col],d)
                # have to reverse the direction: we're going from one cell to the next and thus the relative 
                # edge changes as well
                if (d in (NORTH,SOUTH) and edge in (EAST,WEST)) or (edge in (NORTH,SOUTH) and d in (EAST,WEST)):
                    if d == NORTH and self.row < 15:
                        self.row += 1
                        self.map(dist+.7,NORTH)
                        if not self.merged:
                            self.row -= 1
                    elif d == SOUTH and self.row > 0:
                        self.row -= 1
                        self.map(dist+.7,SOUTH)
                        if not self.merged:
                            self.row += 1
                    elif d == EAST and self.col < 15:
                        self.col +=1 
                        self.map(dist+.7,EAST)
                        if not self.merged:
                            self.col -= 1
                    elif d == WEST and self.col > 0:
                        self.col -= 1
                        self.map(dist+.7,WEST)
                        if not self.merged:
                            self.col += 1
                else: 
                    if d == NORTH and self.row < 15:
                        self.row += 1
                        self.map(dist+1,NORTH)
                        if not self.merged:
                            self.row -= 1
                    elif d == SOUTH and self.row > 0:
                        self.row -= 1
                        self.map(dist+1,SOUTH)
                        if not self.merged:
                            self.row += 1
                    elif d == EAST and self.col < 15:
                        self.col += 1
                        self.map(dist+1,EAST)
                        if not self.merged:
                            self.col -= 1
                    elif d == WEST and self.col > 0:
                        self.col -= 1
                        self.map(dist+1,WEST)
                        if not self.merged:
                            self.col += 1


# In[11]:


open('mazeiters','w+').close()
m = Mouse(edge=NORTH)
m.map_helper()
printMaze(m.memory,m,m.n_weights,m.w_weights)  



# In[12]:


#we might need a special case for where the mouse starts
m.floodfill()
printMaze(m.memory,m,m.n_weights,m.w_weights)  

