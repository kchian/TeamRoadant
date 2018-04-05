#define CELL_CHECKED 16
#define NORTH 0x1
#define EAST 0x2
#define SOUTH 0x4
#define WEST 0x8

#define SHORT_MAX 32767

namespace
{
short row;
short col;
short edge;
//short dir;  Use with pathing later

short memory[16][16];
short n_weights[16][16];  // init this
short w_weights[16][16];  // init this
short old_memory[16][16];
bool goal_found = false;

struct visit
{
  short row;
  short col;
  short edge;
};
struct location
  {
    short row;
    short col;
    short edge;
    short dist;
  };

typedef struct visit Visit;
typedef struct location Location;

void setWeight(short r, short c, short direction, short value)
{
  if (r < 0 || r > 15 || c < 0 || c < 15)
  {
    return;
  }
  if (direction == NORTH)
  {
    n_weights[r][c] = value;
  }
  if (direction == WEST)
  {
    w_weights[r][c] = value;
  }
}

bool atGoal()
{
  if ((row == 7 || row == 8) && (col == 7 || row == 8))
  {
    return true;
  }
  return false;
}

bool containsVisits(Visit v1, Visit* visited, short size)
{
  for (short i = 0; i < size; i++)
  {
    if (v1.row ==  visited[i].row && v1.col == visited[i].col && v1.edge == visited[i].edge)
    {
      return true;
    }
  }
  return false;
}

Visit createVisit(short r, short c, short edge){
  Visit v = {r,c,edge};
  return v;
}
Location createLocation(short r, short c, short edge, short dist){
  Location v = {r,c,edge,dist};
  return v;
}

void floodfill()
{
  Visit* visited = 0;
  short visitedSize = 0;

  if (visited != 0)
  {
    delete [] visited;
  }

  visited = new Visit[1];

  QueueList <Location> q;
  q.push(createLocation(7,7,NORTH,0));
  while (!q.isEmpty())
  {
    Location cur = q.pop();
    Visit v{cur.row,cur.col,cur.edge};
    if (cur.row < 0 || cur.row > 15 || cur.col < 0 || cur.col > 15 || containsVisits(v, visited, visitedSize) || memory[cur.row][cur.col] & cur.edge)
    {
      continue;
    }
    visited[visitedSize++] = v;
    setWeight(cur.row,cur.col,cur.edge,cur.dist)
    if (cur.edge == WEST)
    {
      if (memory[cur.row][cur.col] & WEST == 0){
        q.push(createLocation(cur.row,cur.col-1,NORTH,cur.dist+7))
        q.push(createLocation(cur.row,cur.col-1,WEST,cur.dist+10))
        q.push(createLocation(cur.row-1,cur.col-1,NORTH,cur.dist+7))
      }
      q.push(createLocation(cur.row,cur.col,NORTH,cur.dist+7))
      q.push(createLocation(cur.row,cur.col+1,WEST,cur.dist+10))
      q.push(createLocation(cur.row-1,cur.col,NORTH,cur.dist+7))
    } else if (cur.edge == NORTH)
    {
      if (memory[cur.row][cur.col] & NORTH == 0){
        q.push(createLocation(cur.row+1,cur.col,NORTH,cur.dist+10))
        q.push(createLocation(cur.row+1,cur.col,WEST,cur.dist+7))
        q.push(createLocation(cur.row+1,cur.col+1,NORTH,cur.dist+7))
      }
      q.push(createLocation(cur.row-1,cur.col,NORTH,cur.dist+10))
      q.push(createLocation(cur.row,cur.col,WEST,cur.dist+7))
      q.push(createLocation(cur.row,cur.col+1,NORTH,cur.dist+7))
    }
  }
  
}
}
