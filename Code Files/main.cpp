/*
  Student Name: Gurkan Demir
  Stundet Number: 2015400177
  Compile Status: Compiling
  Program Status: Working
  Notes: Compile with the command "mpic++ -std=c++11 -g main.cpp"
         Then execute with the command "mpiexec --oversubscribe -n [N] ./a.out [input_file] [output_file] [beta] [pi]"
*/
#include <mpi.h>
#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>
#include <iterator>
using namespace std;

/*
  Method in order to split lines while reading input file. 
*/
template <class Container>
void split(const string& str, Container& cont)
{
    istringstream iss(str);
    copy(istream_iterator<string>(iss),
         istream_iterator<string>(),
         back_inserter(cont));
}

/*
  Main function; master, which has rank 0, reads the input file and shares them accross to slaves.
  Then slaves do some calculations to denoise image, then slaves sends it to master process. Then master merges and prints the result to output file.
*/
int main(int argc, char *argv[]){

/*
  Initializes the MPI.
*/
  MPI_Init(NULL, NULL);

  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  
/*
  Declarations of pixel size(200), number of slaves(total number of processors -1),
  slave pixel(each slaves' number of rows)
  Reading beta, pi from console and calculates gamma using pi.
  Initializes T, which is how many executions slaves must do.
*/
  int pixel_size=200;
  int num_slaves=world_size-1;
  int slave_pixel=pixel_size/num_slaves;
  double beta=stod(argv[3]);
  double pi=stod(argv[4]);
  double gamma = 0.5 * log((1.0-pi)/pi);
  int T=1000000;

/*
  If it is master process, reads the input from file. And shares them to slaves.
  Then it merges results of slaves' operations. Then prints the result to the output file.
*/    
  if(world_rank == 0){
    fstream infile(argv[1]);
    string line="";
    int input[pixel_size][pixel_size];

/*
  Reads input file.
*/
    for(int i=0;i<pixel_size;i++){
      vector<string> words;
      getline(infile, line);
      split(line, words);

      for(int j=0;j<pixel_size;j++)
        input[i][j]= stoi(words[j]);
    }

/*
  Shares input to the slaves.
*/
    for(int i=1;i<world_size; i++){      
      for(int j=0;j<slave_pixel; j++)
        MPI_Send(input[(i-1)*slave_pixel+j], pixel_size, MPI_INT, i, j+500, MPI_COMM_WORLD);
    }

/*
  Receives result of slaves' operations and merges them.
*/
    for(int i=1; i<world_size;i++){      
      for(int j=0; j<slave_pixel;j++)
        MPI_Recv(input[(i-1)*slave_pixel+j], pixel_size, MPI_INT, i, j+1000, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    ofstream output;
    output.open(argv[2]);

/*
  Prints the result to the output file.
*/
    for(int i=0;i<pixel_size;i++){
      for(int j=0;j<pixel_size;j++)
        output<<input[i][j]<<" ";

      output<<endl;
    }

    output.close();
  }

/*
  Slaves function; gets own matrix from master. Then does 1.000.000 executions to whether flip or not flip
  the pixel with communicating with other slaves. Then sends result to the master processor.
*/
  else{
/*
  Declarations of arrays.
  Upper: Contains bottom row of the previous processor.
  Lower: Contains top row of the next processor.
  X: Contains initial matrix, coming from master processor.
  Z: Contains current matrix, after each execution.
*/
    int *upper=NULL;
    int *lower=NULL;
    upper=(int *)malloc(sizeof(int)*pixel_size);
    lower=(int *)malloc(sizeof(int)*pixel_size);
    int **X=NULL;
    X=(int **)malloc(sizeof(int *)*slave_pixel);
    int **Z=NULL;
    Z=(int **)malloc(sizeof(int *)*slave_pixel);

/*
  Gets initial matrix from master processor.
*/
    for(int i=0;i<slave_pixel;i++){
      X[i] = (int*)malloc(sizeof(int)*pixel_size);
      Z[i] = (int*)malloc(sizeof(int)*pixel_size);
      MPI_Recv(X[i], pixel_size, MPI_INT, 0, i+500, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    for(int i=0;i<slave_pixel;i++)
      for(int j=0;j<pixel_size;j++)
        Z[i][j]=X[i][j];

/*
  First communication done with neighbour processors.
*/
    if(world_rank!=1)
      MPI_Send(Z[0], pixel_size, MPI_INT, world_rank-1, 2, MPI_COMM_WORLD);

    if(world_rank!=world_size-1)
      MPI_Send(Z[slave_pixel-1], pixel_size, MPI_INT, world_rank+1, 3, MPI_COMM_WORLD);

    if(world_rank!=1)
      MPI_Recv(upper, pixel_size, MPI_INT, world_rank-1 , 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    if(world_rank!=world_size-1)
      MPI_Recv(lower, pixel_size, MPI_INT, world_rank+1 , 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

/*
  Arrays in order make communication faster.
  Contains 3 elements; index0: whether we flip the pixel or not
  index1: i'th index
  index2: j'th index
*/      
    int *sendingMessage=NULL;
    int *receivedMessage=NULL;
    sendingMessage=(int *)malloc(sizeof(int)*3);
    receivedMessage=(int *)malloc(sizeof(int)*3);

    srand(time(NULL));
    int index_i, index_j, sum=0, change=0;
    double delta_E, prob;

/*
  Executes 1.000.000 times for each processor. Randomly picks index i and j and then
  calculates delta_E and flip or not flip the pixel according to randomly choosen probability.
*/
    for(int i=0;i<T;i++){
      index_i = rand() % slave_pixel;
      index_j = rand() % pixel_size;

/*
  Calculates the sum of surroundings pixels.
*/
      sum=0;
      if(index_i==0){
        if(index_j==0){
          sum = Z[0][1] + Z[1][0] + Z[1][1];
          if(world_rank!=1)
            sum+= upper[0]+upper[1];
        }
        else if(index_j==pixel_size-1){
          sum = Z[0][pixel_size-2] + Z[1][pixel_size-1] + Z[1][pixel_size-2];
          if(world_rank!=1)
            sum+=upper[pixel_size-1]+upper[pixel_size-2];
        }
        else{
          sum = Z[0][index_j-1] + Z[0][index_j+1] + Z[1][index_j-1] + Z[1][index_j] + Z[1][index_j+1];
          if(world_rank!=1)
            sum+=upper[index_j-1]+upper[index_j]+upper[index_j+1];
        }
      }
      else if(index_i==slave_pixel-1){
        if(index_j==0){
          sum = Z[index_i-1][0] + Z[index_i-1][1] + Z[index_i][1];
          if(world_rank!=world_size-1)
            sum+=lower[0]+lower[1];
        }
        else if(index_j==pixel_size-1){
          sum = Z[index_i-1][pixel_size-1] + Z[index_i-1][pixel_size-2] + Z[index_i][pixel_size-2];
          if(world_rank!=world_size-1)
            sum+=lower[pixel_size-1]+lower[pixel_size-2];
        }
        else{
          sum = Z[index_i][index_j-1] + Z[index_i][index_j+1] + Z[index_i-1][index_j-1] + Z[index_i-1][index_j] + Z[index_i-1][index_j+1];
          if(world_rank!=world_size-1)
            sum+=lower[index_j-1]+lower[index_j]+lower[index_j+1];
        }
      }
      else{
        if(index_j==0)
          sum = Z[index_i-1][0] + Z[index_i-1][1] + Z[index_i][1] + Z[index_i+1][1] + Z[index_i+1][0];
        else if(index_j==pixel_size-1)
          sum = Z[index_i-1][index_j] + Z[index_i-1][index_j-1] + Z[index_i][index_j-1] + Z[index_i+1][index_j-1] + Z[index_i+1][index_j];
        else{
          sum+= Z[index_i-1][index_j-1]+Z[index_i-1][index_j]+Z[index_i-1][index_j+1]+Z[index_i][index_j-1]+Z[index_i][index_j+1]+Z[index_i+1][index_j-1]+Z[index_i+1][index_j]+Z[index_i+1][index_j+1];
        }        
      }

/*
  Calculates delta_E and decide to flip or not flip according to randomly choosen probability.
*/
      delta_E = -2*gamma*X[index_i][index_j]*Z[index_i][index_j] -2*beta*Z[index_i][index_j]*sum;
      prob = (double)(rand()) / RAND_MAX;
      change=0;
      if(log(prob) < delta_E){
        Z[index_i][index_j]*=-1;
        change=1;
      }

/*
  After each execution, communication done with neighbour processors.
*/
      sendingMessage[0]=change;
      sendingMessage[1]=index_i;
      sendingMessage[2]=index_j;

      if(world_rank!=1)
        MPI_Send(sendingMessage, 3, MPI_INT, world_rank-1, 4, MPI_COMM_WORLD);

      if(world_rank!=world_size-1)
        MPI_Send(sendingMessage, 3, MPI_INT, world_rank+1, 5, MPI_COMM_WORLD);

      if(world_rank!=1){
        MPI_Recv(receivedMessage, 3, MPI_INT, world_rank-1 , 5, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if(receivedMessage[0])
          if(receivedMessage[1]==slave_pixel-1)
            upper[receivedMessage[2]]*=-1;
      }

      if(world_rank!=world_size-1){
        MPI_Recv(receivedMessage, 3, MPI_INT, world_rank+1 , 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if(receivedMessage[0])
          if(receivedMessage[1]==0)
            lower[receivedMessage[2]]*=-1;
      }
    }

/*
  Send the result to the master processor.
*/
    for(int i=0;i<slave_pixel;i++)
      MPI_Send(Z[i], pixel_size, MPI_INT, 0, i+1000, MPI_COMM_WORLD);
  }

/*
  Finalizes the MPI and process.
*/
  MPI_Finalize();
  return 0;
}
