
Location createLocation(short r, short c, short edge, short dist);
void floodfill();
Node nodeCreator(short r,short c);


void setWeight(short r, short c, short direction, short value)
{
  if (r < 0 || r > 15 || c < 0 || c > 15)
  {
    return;
  }
  if (direction == NORTH)
  {
    nWeights[r][c] = value;
  }
  if (direction == WEST)
  {
    wWeights[r][c] = value;
  }
}

bool atGoal()
{
  if ((row == 7 || row == 8) && (col == 7 || col == 8))
  {
    return true;
  }
  return false;
}

Location createLocation(short r, short c, short edge, short dist) {
  Location v = {r, c, edge, dist};
  return v;
}

void moveMouse(node next_square)
{
  if ((next_square.row - row) == 1)
  {
    //north
    dirToTurn(NORTH);
    forward(FORWARD, 20);
  }
  if ((next_square.row - row) == -1)
  {
    dirToTurn(SOUTH);
    forward(FORWARD, 20);
  }
  if ((next_square.col - col) == 1)
  {
    dirToTurn(EAST);
    forward(FORWARD, 20);
  }
  if ((next_square.col - col) == -1)
  {
    dirToTurn(WEST);
    forward(FORWARD, 20);
  }
}

void floodfill()
{
  //Each north is row*16 + col, and row*16 + col +1 is west
  char visited [512];
  for (int i = 0; i < 512; i++)
  {
    visited[i] = 0;
  }
  QueueList <Location> q;
  q.push(createLocation(7, 7, NORTH, 0));
  q.push(createLocation(7, 8, NORTH, 0));
  q.push(createLocation(8, 7, NORTH, 0));
  q.push(createLocation(8, 8, NORTH, 0));
  while (!q.isEmpty())
  {
    Location cur = q.pop();
    // a bit ugly but also as pretty as it'll get
    // continues anything out of bounds, on a wall, or visited
    if (cur.row < 0 || cur.row > 15 || cur.col < 0 || cur.col > 15 || (memory[cur.row][cur.col]) == 0 ||
        (cur.edge == NORTH && visited[(cur.col*16+cur.row)*2] != 0 ) || (cur.edge == WEST && visited[(cur.col*16+cur.row)*2+1] != 0 ))
    {
      continue;
    }
    setWeight(cur.row, cur.col, cur.edge, cur.dist);
    if (cur.edge == NORTH)
    {
      visited[(cur.col*16+cur.row)*2] = 1;
    } else {
      visited[(cur.col*16+cur.row)*2+1] = 1;
    }
    if (cur.edge == WEST)
    {
      if ((memory[cur.row][cur.col] & WEST) == 0) {
        q.push(createLocation(cur.row, cur.col - 1, NORTH, cur.dist + 7));
        q.push(createLocation(cur.row, cur.col - 1, WEST, cur.dist + 10));
        q.push(createLocation(cur.row - 1, cur.col - 1, NORTH, cur.dist + 7));
      }
      q.push(createLocation(cur.row, cur.col, NORTH, cur.dist + 7));
      q.push(createLocation(cur.row, cur.col + 1, WEST, cur.dist + 10));
      q.push(createLocation(cur.row - 1, cur.col, NORTH, cur.dist + 7));
    } else if (cur.edge == NORTH)
    {
      if ((memory[cur.row][cur.col] & NORTH) == 0) {
        q.push(createLocation(cur.row + 1, cur.col, NORTH, cur.dist + 10));
        q.push(createLocation(cur.row + 1, cur.col, WEST, cur.dist + 7));
        q.push(createLocation(cur.row + 1, cur.col + 1, NORTH, cur.dist + 7));
      }
      q.push(createLocation(cur.row - 1, cur.col, NORTH, cur.dist + 10));
      q.push(createLocation(cur.row, cur.col, WEST, cur.dist + 7));
      q.push(createLocation(cur.row, cur.col + 1, NORTH, cur.dist + 7));
    }
  }
}

Node nodeCreator(short r,short c){
  Node x = {r,c};
  return x;  
}

void map() {
  StackArray <Node> history;
  StackArray <CurPrev> nextNode;

  CurPrev origin = {nodeCreator(0,0),nodeCreator(0,0)};
  nextNode.push(origin);

  short prevLen = 0;
  while (!nextNode.isEmpty())
  {
    CurPrev cur = nextNode.pop();
    if (row != cur.prev.row or col != cur.prev.col) {
      while (row != cur.prev.row or col != cur.prev.col)
      {
        Node i = history.pop();
        moveMouse(i);        
        row = i.row;
        col = i.col;
        //Actually move the mouse here
      }
      history.push(nodeCreator(row,col));
    }

    
    //Actually move the mouse here
    moveMouse(cur.current);

    row = cur.current.row;
    col = cur.current.col;
    
    memory[row][col] = getWalls() + CELL_CHECKED;

    if (atGoal()) {
      break;
    }
    prevLen = (short)nextNode.count();

    short dirs[4];
    if (row > 7 && col < 7)
    {
      dirs[0] = WEST;
      dirs[1] = NORTH;
      dirs[2] = EAST;
      dirs[3] = SOUTH;
    }
    else if (row < 7 && col < 7)
    {
      dirs[0] = WEST;
      dirs[1] = SOUTH;
      dirs[2] = EAST;
      dirs[3] = NORTH;
    }
    else if (row < 7 && col > 7)
    {
      dirs[0] = EAST;
      dirs[1] = SOUTH;
      dirs[2] = WEST;
      dirs[3] = NORTH;
    }
    else 
    {
      dirs[0] = EAST;
      dirs[1] = NORTH;
      dirs[2] = WEST;
      dirs[3] = SOUTH;
    }

    for (int i = 0; i < 4; i++)
    {
      if ((memory[row][col] & dirs[i])==0)
      {
        if ((dirs[i] == WEST) && ((memory[row][col-1] & CELL_CHECKED) == 0))
        {
          CurPrev x = {nodeCreator(row, col-1), nodeCreator(row, col)};
          nextNode.push(x);
        }
        else if ((dirs[i] == EAST) && ((memory[row][col+1] & CELL_CHECKED) == 0))
        {
          CurPrev x = {nodeCreator(row, col+1), nodeCreator(row, col)};
          nextNode.push(x);
        }
        else if ((dirs[i] == SOUTH) && ((memory[row-1][col] & CELL_CHECKED) == 0))
        {
          CurPrev x = {nodeCreator(row-1, col), nodeCreator(row, col)};
          nextNode.push(x);
        }
        else if ((dirs[i] == NORTH) && ((memory[row+1][col] & CELL_CHECKED) == 0))
        {
          CurPrev x = {nodeCreator(row+1, col), nodeCreator(row, col)};
          nextNode.push(x);
        }
      }
    }
    if (prevLen != (short)nextNode.count())
    {
      history.push(nodeCreator(row,col));
    }
  }

  short oldMemory[16][16];
  for (int i = 0; i < 16; i++)
  {
    for (int j = 0; j < 16; j++)
    {
      oldMemory[i][j] = memory[i][j];
      memory[i][j] = 0;
    }
  }

  //Is there a better way of doing this 
  while (!history.isEmpty())
  {
    history.pop();
  }
  while (!nextNode.isEmpty())
  {
    nextNode.pop();
  }

  CurPrev center = {nodeCreator(row,col),nodeCreator(row,col)};
  nextNode.push(center);

  prevLen = 0;
  while (!nextNode.isEmpty())
  {
    CurPrev cur = nextNode.pop();
    if (row != cur.prev.row or col != cur.prev.col) {
      while (row != cur.prev.row or col != cur.prev.col)
      {
        Node i = history.pop();
        moveMouse(i);
        
        row = i.row;
        col = i.col;
        //Actually move the mouse here
      }
      history.push(nodeCreator(row,col));

    }
    
    //Actually move the mouse here
    moveMouse(cur.current);

    
    row = cur.current.row;
    col = cur.current.col;
    if (row == 0 && col == 0) {
      break;
    }
    memory[row][col] = getWalls() + CELL_CHECKED;
    prevLen = (short)nextNode.count();
    short dirs[4] = {NORTH,EAST,SOUTH,WEST};
    for (int i = 0; i < 4; i++)
    {
      if ((memory[row][col] & dirs[i])==0)
      {
        if ((dirs[i] == WEST) && ((memory[row][col-1] & CELL_CHECKED) == 0))
        {
          CurPrev x = {nodeCreator(row, col-1), nodeCreator(row, col)};
          nextNode.push(x);
        }
        else if ((dirs[i] == EAST) && ((memory[row][col+1] & CELL_CHECKED) == 0))
        {
          CurPrev x = {nodeCreator(row, col+1), nodeCreator(row, col)};
          nextNode.push(x);
        }
        else if ((dirs[i] == SOUTH) && ((memory[row-1][col] & CELL_CHECKED) == 0))
        {
          CurPrev x = {nodeCreator(row-1, col), nodeCreator(row, col)};
          nextNode.push(x);
        }
        else if ((dirs[i] == NORTH) && ((memory[row+1][col] & CELL_CHECKED) == 0))
        {
          CurPrev x = {nodeCreator(row+1, col), nodeCreator(row, col)};
          nextNode.push(x);
        }
      }
    }
    if (prevLen != (short)nextNode.count())
    {
      history.push(nodeCreator(row,col));
    }
  }
  for (int i = 0; i < 16; i++)
  {
    for (int j = 0; j < 16; j++)
    {
      if (memory[i][j] < oldMemory[i][j])
      {
        memory[i][j] = oldMemory[i][j];
      }
    }
  }
}
short getRow(){
  return row;
}
short getCol(){
  return col;
}

