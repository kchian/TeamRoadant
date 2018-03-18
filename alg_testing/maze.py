
# coding: utf-8

# #### Python algorithm testing: for easy POC + debugging ###

# In[64]:


# 16x16 grid representing walls, 2D "bytearray"
# Each cell is a binary integer: north, east, south, west represented in a bit
# 0 represents a wall
# The leftmost bit will be whether or not the cell has been checked

CHECKED = 0b10000
NORTH   = 0x1
EAST    = 0x2
SOUTH   = 0x4
WEST    = 0x8


# In[266]:


# 16x16, mazes thanks to https://github.com/micromouseonline/micromouse-maze
physical_maze = [
  0x0e, 0x08, 0x0a, 0x08, 0x08, 0x0a, 0x08, 0x0a, 0x09, 0x0c, 0x09, 0x0c, 0x08, 0x0b, 0x0c, 0x09,
  0x0c, 0x02, 0x08, 0x01, 0x05, 0x0c, 0x02, 0x0a, 0x02, 0x03, 0x06, 0x03, 0x06, 0x0a, 0x03, 0x05,
  0x05, 0x0e, 0x03, 0x06, 0x00, 0x02, 0x0a, 0x0a, 0x08, 0x08, 0x0b, 0x0c, 0x0a, 0x08, 0x09, 0x05,
  0x04, 0x09, 0x0c, 0x09, 0x07, 0x0c, 0x09, 0x0d, 0x05, 0x05, 0x0e, 0x01, 0x0d, 0x07, 0x05, 0x05,
  0x05, 0x04, 0x01, 0x05, 0x0c, 0x03, 0x04, 0x01, 0x06, 0x02, 0x0b, 0x04, 0x02, 0x0a, 0x01, 0x05,
  0x04, 0x01, 0x04, 0x01, 0x04, 0x09, 0x07, 0x06, 0x09, 0x0e, 0x0a, 0x01, 0x0e, 0x0b, 0x05, 0x05,
  0x05, 0x04, 0x01, 0x07, 0x05, 0x06, 0x08, 0x0b, 0x06, 0x09, 0x0d, 0x04, 0x0a, 0x0a, 0x01, 0x05,
  0x04, 0x03, 0x05, 0x0e, 0x03, 0x0c, 0x02, 0x08, 0x09, 0x04, 0x03, 0x04, 0x08, 0x0a, 0x01, 0x05,
  0x06, 0x08, 0x02, 0x0b, 0x0c, 0x02, 0x09, 0x06, 0x03, 0x06, 0x09, 0x05, 0x07, 0x0d, 0x05, 0x05,
  0x0e, 0x00, 0x0b, 0x0d, 0x05, 0x0d, 0x07, 0x0c, 0x0a, 0x08, 0x02, 0x02, 0x08, 0x02, 0x01, 0x05,
  0x0e, 0x03, 0x0c, 0x01, 0x05, 0x06, 0x09, 0x05, 0x0c, 0x00, 0x09, 0x0d, 0x06, 0x09, 0x05, 0x05,
  0x0c, 0x08, 0x01, 0x05, 0x04, 0x0b, 0x05, 0x04, 0x03, 0x05, 0x06, 0x01, 0x0d, 0x06, 0x01, 0x05,
  0x07, 0x05, 0x06, 0x00, 0x00, 0x0b, 0x05, 0x06, 0x0a, 0x03, 0x0d, 0x06, 0x00, 0x0b, 0x05, 0x05,
  0x0e, 0x00, 0x0b, 0x05, 0x07, 0x0c, 0x03, 0x0c, 0x0a, 0x08, 0x02, 0x08, 0x02, 0x08, 0x01, 0x05,
  0x0e, 0x01, 0x0e, 0x03, 0x0c, 0x02, 0x08, 0x02, 0x08, 0x02, 0x09, 0x06, 0x09, 0x05, 0x07, 0x05,
0x0e, 0x02, 0x0a, 0x0a, 0x02, 0x0a, 0x02, 0x0a, 0x02, 0x0a, 0x02, 0x0a, 0x02, 0x02, 0x0a, 0x03
]

# raw_maze = ''
# try:
#     with open('50.maz') as f:
#         for ind,char in enumerate(f):
#             physical_maze[ind/16][ind%16] = char


# In[272]:


def printMaze(maze,robot,weights=None,history=None):
    # maze out is a list of strings, each representing a row
    maze_out = [list('   '.join('+'*17))]
    maze_out += [list(' '*65)]
    maze_out += [list(' '*65)]
    for line in range(15):
        maze_out += [list('+  ' + ('+  '.join(list(' '*16))) + '+')]
        maze_out += [list(' '*65)]
        maze_out += [list(' '*65)]
    maze_out += [list('   '.join('+'*17))]
    # In the data, the mouse 
    for ind,i in enumerate(maze):
        # row is ind/16, col is ind%16
        # from 0,0, bottommost row, leftmost column
        # to 1,0, second to bottom row, leftmost column
        if i & NORTH:
            maze_out[48-((ind%16)+1)*3][int(ind/16)*4+1] = '-'
            maze_out[48-((ind%16)+1)*3][int(ind/16)*4+2] = '-'
            maze_out[48-((ind%16)+1)*3][int(ind/16)*4+3] = '-'
        if i & EAST:
            maze_out[48-((ind%16)+1)*3 + 1][int(ind/16)*4+4] = '|'
            maze_out[48-((ind%16)+1)*3 + 2][int(ind/16)*4+4] = '|'
        if i & SOUTH:
            maze_out[48-((ind%16))*3][int(ind/16)*4+1] = '-'
            maze_out[48-((ind%16))*3][int(ind/16)*4+2] = '-'
            maze_out[48-((ind%16))*3][int(ind/16)*4+3] = '-'
        if i & WEST:
            maze_out[48-((ind%16)+1)*3 + 1][int(ind/16)*4] = '|'
            maze_out[48-((ind%16)+1)*3 + 2][int(ind/16)*4] = '|'
    
    #mouse location
    if robot.dir & NORTH:
        maze_out[48-(robot.row*3+2)][robot.col*4+2] = '^'
    if robot.dir & EAST:
        maze_out[48-(robot.row*3+2)][robot.col*4+3] = '>'
    if robot.dir & SOUTH:
        maze_out[48-(robot.row*3+2)][robot.col*4+2] = 'v'
    if robot.dir & WEST:
        maze_out[48-(robot.row*3+2)][robot.col*4+1] = '<'
    
    
    #Prints where the weights are
    if weights:
        for n,i in enumerate(weights):
            for m,j in enumerate(i):
                if j:
                    maze_out[48-(n*3+2)][m*4+2] = '*'
        
        

    #print('\n'.join([''.join(i) for i in maze_out]))
    with open('mazeiters','a+') as f:
        f.write('\n'.join([''.join(i) for i in maze_out])+'\n'+'#'*100+'\n')


# In[273]:


def search(row,col):
    return physical_maze[col*16+row]


# In[276]:


neighbor_pattern = [(0,1),(0,-1),(1,0),(-1,0)]


# In[283]:


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
        self.weights = [[None]*16 for i in range(16)]
        self.path = []
        self.relative_path = []
            
    def neighbors(self,row,col):
        x = [(row+1,col), (row-1,col),(row,col+1),(row,col-1)]
        return [i for i in x if i[0]<16 and i[1]<16]
                
    # Important to note that there's no logic for telling how many "units" we've moved in this
    # End goal: diagonal maze solving like http://ieeexplore.ieee.org/document/6852576/ 
    # So each edge has a distance, and we'll assume we start on the bottom edge...
    def floodfill(self,r,c,dist,edge):
        if self.memory[r][c] & CHECKED:
            return
        printMaze(physical_maze,Mouse(r,c,edge),self.weights)        
        self.memory[r][c] = search(r,c) + CHECKED
        if self.weights[r][c] is None:
            self.weights[r][c] = Cell()
        if edge == NORTH:
            self.weights[r][c].north=dist
        if edge == EAST:
            self.weights[r][c].east=dist            
        if edge == SOUTH:
            self.weights[r][c].south=dist
        if edge == WEST:
            self.weights[r][c].west=dist      
        for d in [NORTH,EAST,SOUTH,WEST]:
            # if there is not a wall
            if not self.memory[r][c] & d:
                print(self.memory[r][c],d)
                # have to reverse the direction: we're going from one cell to the next and thus the relative 
                # edge changes as well
                if (d in (NORTH,SOUTH) and edge in (EAST,WEST)) or (edge in (NORTH,SOUTH) and d in (EAST,WEST)):
                    if d == NORTH and r < 16:
                        self.floodfill(r+1,c,dist+.7,SOUTH)
                    elif d == SOUTH and r > -1:
                        self.floodfill(r-1,c,dist+.7,NORTH)
                    elif d == EAST and c < 16:
                        self.floodfill(r,c+1,dist+.7,WEST)
                    elif d == WEST and c > -1:
                        self.floodfill(r,c-1,dist+.7,EAST)
                else: 
                    if d == NORTH and r < 16:
                        self.floodfill(r+1,c,dist+1,SOUTH)
                    elif d == SOUTH and r > -1:
                        self.floodfill(r-1,c,dist+1,NORTH)
                    elif d == EAST and c < 16:
                        self.floodfill(r,c+1,dist+1,WEST)
                    elif d == WEST and c > -1:
                        self.floodfill(r,c-1,dist+1,EAST)


# In[284]:


open('mazeiters','w+').close()
m = Mouse(edge=NORTH)
m.floodfill(m.row,m.col,0,m.edge)



# In[271]:


# Cell weight tracker
class Cell:
    def __init__(self,north=None, south=None, east=None, west=None):
        self.north = north
        self.south = south
        self.east = east
        self.west = west
    
    def __repr__(self):
        return 'Cell:' + 'N: ' + str(self.north) + ' E: ' + str(self.east) + ' S: ' + str(self.south) + 'W: ' + str(self.west) + '   '

