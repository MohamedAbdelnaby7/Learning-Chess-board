/*   This is the code of arduino Mega in Chess Project -HV team
 *   Black Rock is at row 0 and column 0


 */

// Castling and En passant were not added
 
#include <FastLED.h>
#include <LiquidCrystal.h>

#define LED_PIN     18
#define NUM_LEDS    64

CRGB leds[NUM_LEDS];
LiquidCrystal lcdwhite ( 19, 20, 21, 22, 23, 24);
LiquidCrystal lcdblack ( 25, 26, 27, 28, 29, 30);

#define TX1       18                   // Transmission pin in Mega
#define RX1       19                   // Recieving pin in Mega
#define King      1                    // Until now giving each piece its own code isn't necessary but
#define Queen     2                    // for clarity of the code for a human reader.
#define Bishop    3
#define Knight    4
#define Rook      5
#define Pawn      6
#define Free      7                    // This represent a free spot on the board
#define black     1                   
#define white     0 
#define buzzer_1  31                   // BUZZER anode and cathode
#define buzzer_2  32                   // NOT DONE YET (define pins of buzzer)              


int changes = 0, place0 = -1, place1 = -1;                 // number of changes captured by the sensors,places of the changes on the board
int ones , code, from_row, from_col, to_row, to_col;                                                  // counts number of ones on the board, i.e. : number of pieces (according to the circuit, a reading of 1 is a piece on the block and a reading of 0 is a free spot). 
int killed_pieces = 0;                                     // number of killed_pieces
int i,j;                                                   // loop counter (better make it local to each)
int y,x;                                                   // loop counter also
int led_array[64];                                         // contains the leds you want to light from 0 to 63  // note : isn't important now since max is deleted
int turn = 0;                                              // White always start
int white_king_loc = 60;                                   // we will change this as the game goes on
int black_king_loc = 4;

int board_inti[64] =    { 5, 4, 3, 2, 1, 3, 4, 5,          // This represent the board at the beginning of the game with a black rock in row 0, col 0
                          6, 6, 6, 6, 6, 6, 6, 6,
                          7, 7, 7, 7, 7, 7, 7, 7,
                          7, 7, 7, 7, 7, 7, 7, 7,
                          7, 7, 7, 7, 7, 7, 7, 7,
                          7, 7, 7, 7, 7, 7, 7, 7,
                          6, 6, 6, 6, 6, 6, 6, 6,
                          5, 4, 3, 2, 1, 3, 4, 5  };
                                        
int board[64] ;          // This represent the board while the game is on. Changes when a move happens
                          

int color_inti[64]  =   { 1, 1, 1, 1, 1, 1, 1, 1,          // This represent the colors of the pieces at the beginning of the game
                          1, 1, 1, 1, 1, 1, 1, 1,
                          7, 7, 7, 7, 7, 7, 7, 7,               
                          7, 7, 7, 7, 7, 7, 7, 7,             
                          7, 7, 7, 7, 7, 7, 7, 7,
                          7, 7, 7, 7, 7, 7, 7, 7,
                          0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0  };

int color[64] ;         // This represent the colors of the pieces while the game is on. Changes when a move happens
                       

int detect_inti[64] =   { 1, 1, 1, 1, 1, 1, 1, 1,         // This represent the reading of the blocks at the beginning of the game
                          1, 1, 1, 1, 1, 1, 1, 1,
                          0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0,
                          1, 1, 1, 1, 1, 1, 1, 1,
                          1, 1, 1, 1, 1, 1, 1, 1  };

int detect_0[64] ;

int detect_1[64] ;           // This represent the reading of the blocks while the game is on. Changes when a move happens
                         


//functions
void Lcd_Erorr(void);
void Intialize_board(void);                                 // This function intializes all arrays at the start of the game to the default values.
void Detect_me(void);                                       // This function is resposible for detection of movement on board

void Compare_me(void);                                      // Compares detect_0 and detect_1, calculates differences(changes) betwwen the 2 arrays.
                                                            //  2 if only a move is detected or a piece is eaten and 1 if a piece is lifted up but not placed.

void Count_my_ones(void);                                   // counts ones inside detect_1[], i.e. : number of pieces

int Get_row(int led);                                       // takes the led number from 0 to 63 and returns the row where it is located from 2 to 9 
                                                            //as leds are connected in a matrix where rows start from pin2 to pin 9  and columns start from pin10 to pin 17 


void Calculate_case(int changes, int ones, int place0,int place1); 
void Decision_maker(void);
void Rearrange_me(int,int,int,int);                         // Changes the suitable values inside board array,color arrays. takes (killer,killer_color,from_place,to_place)
void Swap_0_and_1(void);                                    // swaps detect_0 and detect_1
void Buzzer(void);

int Get_row_col(int, int *);                                //takes led number from 0 to 63 and returns the 
                                                            // pin number where the row of the led is connected and the pin number
                                                            // where the col of the led is connected

void Intialize_led_array(void);                             // intialize the matrix to values of -1 to know when to stop because we
                                                            // don't need to illuminate all leds each time
void Start_lights(int l);
void Light_leds_on(void);                                   // Turn on the leds inside Led_indices
void Light_leds_off(void);                                  // Turn off the leds inside Led_indicesint  Length_array(void);                                    // gets the length of led_matricies to stop using max (didn't try strlen YET)
void Pawn_moves(int row,int col);                           // get moves for black or white pawns
void Rook_moves(int row,int col);                           // get moves for rocks
void Knight_moves(int row,int col);                         // get moves for knights
void Bishop_moves(int row ,int col);                        // get moves for bishops
void Queen_moves(int row ,int col);                         // get moves for queen basically its (bishop moves+rock moves)
void King_moves(int row, int col);                          // get moves for king
bool Is_King_in_check(int row,int col);                     // checks if the king was in check
bool Is_allowed_to_king(int from_row, int from_col, int to_row, int to_col);                  
                                                            // filters the movements of the king
bool Is_allowed_relative_to_king(int from_row, int from_col, int to_row, int to_col);          
                                                            // checks if the movement will leave the king in check or not
bool Is_legal_move(int piece, int from_row, int from_col, int to_row, int to_col);
                                                            // check the movements of pieces is legal or not

                                                            

void setup() {

  
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(255);
  // intialize all arrays at the start
  Intialize_board();

//-----------------for detedtion----------------------------
  // Configuring Output and Input pins for detection
  for(int i = 2 ;i < 10 ; i++){
    pinMode(i,OUTPUT); 
  }

  // Configuring pins for Serial communication for debugging
  //Serial.begin(9600);

  
  // Buzzer Configuration and music at the start
  pinMode(buzzer_1,OUTPUT);
  pinMode(buzzer_2,OUTPUT);

  
  // Lcd configuration
  lcdwhite.begin(16,2);
  lcdblack.begin(16,2);
  lcdwhite.print("  Hello,White!  ");
  lcdblack.print("  Hello,Black!  ");
  Start_lights(2);

  // make sure that all pieces on default position
  Retry : 
  Detect_me();
  Compare_me();
  if( changes > 0){
 
    lcdwhite.print("Error Detection!");
    lcdblack.print("Error Detection!");
    Start_lights(1);
    goto Retry ;
  }
  lcdwhite.clear();
  lcdblack.clear();
  Light_leds_off();
}

void loop() {

  Detect_me();
  Compare_me();
  Calculate_case(changes,ones,place0,place1);
  Swap_0_and_1();
  Decision_maker();
  
}

void Intialize_board(void){
                                                                   // loop counter
  for (int i = 0 ; i <64; i++){
    board[i]    = board_inti[i] ;
    color[i]    = color_inti[i] ;
    detect_0[i] = detect_inti[i];
  }
}

void Detect_me(void){
  // Detection function depends on  scrolling on blocks one by one,
  // while reading results are stored inside an array " detect_1[] " then(using
  // Compare_me() fn, we start comparing the reading with
  // the previous one " detect_0[] " until a change is noticed and also
  // we are calculating the number of pieces on the board in order to 
  // know if a piece is eaten . 
                                                         
  byte first_reading = -1, second_reading = -1, third_reading = -1;          // to make sure the reading is right,we read it three times
  byte count_out = 0;                                                        // to control where to write in the detect_1 
  for( i = 2; i < 10; i++){                                                  // for all the rows
        for ( j =2 ; j < 10 ; j++){                                          // for making only one row HIGH and the rest are LOW
          if ( i == j ){
            digitalWrite(j,HIGH);
          }else{
            digitalWrite(j,LOW);
          }
        }
        
        for( j = 10; j < 17 ;j++){                                           // This reads all the columns and the reading belongs to the only row which is HIGH
                                                                             // signal interference is prevented by putting a diode after every single key in the matrix
          reread :
          first_reading = (digitalRead(j));
          delay(1);
          second_reading = (digitalRead(j));
          delay(1);
          third_reading = (digitalRead(j));
          
          if ( (first_reading == second_reading) && (first_reading == third_reading) ){  // I read the same input pin 3 times with a tiny delay just to make sure the signal is correct
              detect_1[count_out++] = first_reading;                            // detect1[ ((i-2)*8) + (j-10)] but I have free memory :)
          }else {
            goto reread;
          }
        }
  }
  Count_my_ones();
}


void Compare_me(void){
                                                            
 //changes refers to number of differences between detect_0[] and detect_1[], 
 //changes can't exceed 2 changes which is the case of a piece eating another.
  changes =0;                                                                        
  int index[4];                                                             // storage for the indicies where a change is noticed, [4] for avoiding unexpected behavior if something happened
  int index_counter = 0;
  for ( i = 0 ; i < 64; i++){
          if (detect_0[i] != detect_1[i]){                                  // count differences between the 2 arrays
              changes++;
              index[index_counter++] = i;
          }
      }
      
      if (changes == 1){
        if (detect_1[index[0]] == 0 && ones ==(32 - killed_pieces -1)){
          place0 = index[0]; 
        }
        else if (detect_1[index[0]] == 1 && ones == (32 - killed_pieces)){
          place1 = index[0];
          
        }
               
      }else if (changes == 2){
        // we need to know from where it came and to where it moved
        // the place where it moved will hold a one but remember that
        if(detect_0[index[0]] == 1 && detect_1[index[0]] == 0){
            place0 = index[0];                                             //moved from index[0] to index[1]
            place1 = index[1];
        }else{
            place0 = index[1];                                             //moved from index[1] to index[0]
            place1 = index[0];
        }
      }else{
        // There shouldn't be more than 2 changes between the 2 arrays and an error message must appear if so
        // We will disregard the last move 
        for(int i = 0 ; i< 64;i++){
          detect_1[i] = detect_0[i];
        }
         Lcd_Erorr();
      }

}

void Lcd_Erorr(void){
  
  for (int i =10 ; i >0 ; i--){
        lcdwhite.clear(); 
        lcdwhite.setCursor(0,0);
        lcdwhite.print("Error!  ");
        lcdwhite.print(i);
        lcdblack.clear();
        lcdblack.setCursor(0,0);
        lcdblack.print("Error!  ");
        lcdblack.print(i);
        delay(1000);
        }
}

void Count_my_ones(void){
 ones = 0;
  for(int i = 0; i <64 ; i++)
    if( detect_1[i] == 1 ) ones++;
    
}


int Get_row(int led){
  
 if (led <= 7){
  return 0;  
 }else if (led <= 15){
  return 1;
 }else if (led <= 23){
  return 2;
 }else if (led <= 31){
  return 3;
 }else if (led <= 39){
  return 4;
 }else if (led <= 47){
  return 5;
 }else if (led <= 55){
  return 6;
 }else if (led <= 63){
  return 7;
 }

}

void Calculate_case(int changes, int ones, int place0,int place1){
  if (changes == 1){
    // a piece is lifted but not placed --> case 1 (look documentation beside Send_uno() function).
    // DONE but not sure :::: we should send something to let the board know whether the piece is lifted or put.
    // this will be done by through the code variable which is sent first to the UNO.
    // if it's 0 then the place is empty and it should light the leds.
    // if it's 1 then the place isn't empty and it should turn off the leds.
      if (detect_1[place0] == 0 && ones == (32 - killed_pieces -1)){
          code =0;
          from_row = Get_row(place0);
          from_col = (place0%8);
      }else if (detect_1[place1] == 1 && ones == (32 - killed_pieces)){
          if ( place1 == place0 ){
            code = 1;
            from_row = Get_row(place0);
            from_col = (place0%8);
          }else{
            code = 3;
            from_row = Get_row(place0);
            from_col = (place0%8);
            to_row = Get_row(place1);
            to_col = (place1%8);

          }
          
      }
  }else if (changes == 2){
    // either a piece is moved or killed
  if ( ones < (32 - killed_pieces)){
      // this means that a piece is eaten --> case 2 ( look documentation beside Send_uno() function )
      // we will change board,color arrays with the suitable values Rearrange_me() does this
      killed_pieces++;                                                     // because we are certain that a piece is eaten
      code = 2;
      from_row = Get_row(place0);
      from_col = (place0%8);
      to_row = Get_row(place1);
      to_col = (place1%8);

    }
    if ( ones == (32 - killed_pieces)){
      // this means that a piece moved form a place to another but no killing --> case 3 ( look documentation beside Send_uno() function )
      // we will change board,color arrays with the suitable values
      code = 3;
      from_row = Get_row(place0);
      from_col = (place0%8);
      to_row = Get_row(place1);
      to_col = (place1%8);

    }
  }
}
  // killer==move_piece >>>> takes (1,2,3,4,5,6).... killer_color==move_piece_color >>>> takes(0,1) 7 refrers to free place.
void Rearrange_me(int killer, int killer_color, int from_place, int to_place){
    board[from_place] = 7;
    board[to_place]   = killer;
    color[from_place] = 7;
    color[to_place]   = killer_color;
}



void Swap_0_and_1(void){
  for(int i = 0 ; i < 64 ;i++){
    detect_0[i] = detect_1[i];
  }
}

void Intialize_led_array(void){
  
  for ( int counter = 0 ; counter < 64 ; counter++)
    led_array[counter] = -1;                          
  
}

int Get_row_col(int led, int *colu){

  *colu = (led % 8) ;
  int row;
 if (led <= 7){
  return 0;  
 }else if(led <=15){
  return 1;
 }else if(led <= 23){
  return 2;
 }else if (led <= 31){
  return 3;
 }else if ( led <= 39){
  return 4;
 }else if ( led <= 47){
  return 5;
 }else if ( led <= 55){
  return 6;
 }else if ( led <= 63){
  return 7;
 }
}

void Start_lights(int l){
  for( i = 0; i < l; i++){
    
   for (int i = 0; i <= 63; i++) {
    leds[i] = CRGB::Blue;
    FastLED.show();
    delay(50);
   }
   
   for (int i = 63; i >= 0; i--) {
    leds[i] = CRGB::Green;
    FastLED.show();
    delay(50);
   }
   
   for(int i = 0; i<= 63; i++) {
    leds[i] = CRGB::Red;
    FastLED.show();
    delay(50);
   }
   
  }
}


int Length_array(void){
  // length = sizeof(led_array)/sizeof(int);  
  // NOTE: this method returns number of elements in the array but we need number of leds (all elements in ssray except -1)                                              
  int counter = 0;
    while ( led_array[counter] != -1){
        counter++;
    }
   return counter ;
}

void Light_leds_on(void){
  // This is the loop which lights the suitable leds after being put inside Led_arrays
  // Each led has 2 indices the first is refers to row & the second refers to column
  // Even idexes(led_indicies[i]) refer to row's numbers & Odd indexes(led_indicies[i+1]) refer to column's numbers
  i = 0;
  while(led_array[i] != -1){

    if ( color[led_array[i]] == turn^1 ){
      
      leds[led_array[i]] = CRGB::Red;

      
    } else {
      
      leds[led_array[i]] = CRGB::Blue;

     
    }
    i++;
  }
  FastLED.show(); 
}

void Light_leds_off(void){

  FastLED.clear();
  byte r,c,x;
  for ( r = 1; r < 8; r+=2) {
    for ( c = 0, x = r; c <= r; x--, c++) {
      leds[ x*8 + c] = CRGB :: Green;
    }
  }
  
  for ( r = 2; r < 7; r+=2) {
    for ( c = 7, x = r; c >= r; x++, c--) {
      leds[ x*8 + c] = CRGB :: Green;
    }
  }
  FastLED.show();

}


void Pawn_moves(int row,int col){
   x =0 ;                                                                  // counter for the array indexes
  int place_on_board = (row*8)+col;                                        // BECAREFUL  rows(0.....7) col(0.....7)
  int color1 = color[place_on_board];  
  // pawn, the board always starts with a black rock at 0,0 and a white rock at 7,7
  // white pawn
  if(color1 == white){
     if ( row == 0){
      // PROMOTION HERE 
      // Promotion assume that the player will only upgrade to queen
      Get_action(0, color1);
      board[col] = Queen;
      // We should calculate Pawn_moves as Queen_moves
      Queen_moves(row, col);
      goto Rest;
    }
  if (row < 7 && row > 0 && board[(place_on_board-8)] ==Free && Is_allowed_relative_to_king(row, col, row-1, col)){
      // board[(place_on_board-8)] == 7 , this condition ensures that the block ahead of the pawn is free
      led_array[x++] = place_on_board - 8 ;
    if (row == 6 && board[(place_on_board-16)] == Free && Is_allowed_relative_to_king(row, col, row-2, col)){
       led_array[x++] = place_on_board - 16 ;
    }
   
    // Attacking moves must be added diagonally if colour[row -1][col -1] == 1 means a black piece exist diagonally to the left
    if(color[(place_on_board-7)] == (color1^1) && col < 7 && Is_allowed_relative_to_king(row, col, row-1, col+1)){
     // This means an opposite color piece exists at the upper right block
    led_array[x++] = place_on_board - 7;
    }
    if (color[(place_on_board - 9)] == (color1^1) && col > 0 && Is_allowed_relative_to_king(row, col, row-1, col-1)){
      // This means an opposite color piece exists at the upper left block
      led_array[x++] = place_on_board - 9;
    }
  }
}else if (color1 == black){
    if (row == 7 ){
        Get_action(0, color1);                                              // Promotion
        board[row*8+col] = Queen;
        // We should calculate pawn_moves as Queen_moves
        Queen_moves(row, col);
        goto Rest;                                             
    }
    if ( row > 0 && row < 7 && board[(place_on_board + 8)] == Free && Is_allowed_relative_to_king(row, col, row+1, col)){       // Black pawn
    led_array[x++] = place_on_board + 8;
    if (row == 1 && board[(place_on_board + 16)] == Free && Is_allowed_relative_to_king(row, col, row+2, col)){
      led_array[x++] = place_on_board + 16;
    }

    // Attacking moves must be added diagonally 
    if ( color[(place_on_board + 9)] == (color1^1) && col < 7 && Is_allowed_relative_to_king(row, col, row+1, col+1)){
      // This means an opposite color piece exists at the lower right block
      led_array[x++] = place_on_board + 9;
    }
    if ((color[(place_on_board + 7)] == (color1^1)) && col > 0 && Is_allowed_relative_to_king(row, col, row+1, col-1)){
      // This means an opposite color piece exists at the lower left block
      led_array[x++] = place_on_board + 7;
    }
  }
}
Rest:;
}

void Rook_moves(int row,int col){ 
    //int place_on_board = row*8 + col;                                    // BECAREFUL  rows(0.....7) col(0.....7)
    int color1 = color[row*8+col];
     x = 0;                                                                // x for the array index increment

    // The next 2 loops will generate the allowable moves in the column of the Rock
    for(i = row - 1; i >= 0; i--){
          if( (board[((i*8)+col)] == Free || ( board[((i*8)+col)] != Free && color[((i*8)+col)] == color1^1)) && Is_allowed_relative_to_king(row,col,i,col)){
              // this condition ensures the place is empty or have an opposit color piece and don't make king in threat.                             
              led_array[x++] = i*8 + col;
            }else{
              // if a block contains a piece, it will block all the moves on the Rock
              break;
            }
       }
    for(i= row+1; i < 8 ;i++){
          if( (board[((i*8)+col)] == Free || (board[((i*8)+col)] != Free && color[((i*8)+col)] == color1^1))  && Is_allowed_relative_to_king(row,col,i,col)){
            // this condition ensures the place is empty or have an opposit color piece and don't make king in threat.                             
            led_array[x++] = i*8 + col;
            }else{
              // if a block contains a piece, it will block all the moves on the Rock
              break;
            }
       }
       
    // The next 2 loops will generate the moves allowed in the row of the Rock
    for(i = col - 1; i >= 0; i--){
          if( (board[((row*8)+i)] == Free || (board[((row*8)+i)] != Free && color[((row*8)+i)] == color1^1)) && Is_allowed_relative_to_king(row,col,row,i)){
            // this condition ensures the place is empty or have an opposit color piece and don't make king in threat.                             
            led_array[x++] = row*8 + i;
            }else{
              // if a block contains a piece, it will block all the moves on the Rock
              break;
            }
       }
    for(i= col + 1; i < 8 ; i++){
          if( (board[((row*8)+i)] == Free || (board[((row*8)+i)] != Free && color[((row*8)+i)] == color1^1)) && Is_allowed_relative_to_king(row,col,row,i)){
            // this condition ensures the place is empty or have an opposit color piece and don't make king in threat.                             
            led_array[x++] = row*8 + i;
            }else{
              // if a block contains a piece, it will block all the moves on the Rock
              break;
            }
       }
    
  }


void Knight_moves(int row,int col){
   x = 0;                                                                  // array index increment
                                                                           // BECAREFUL  rows(0.....7) col(0.....7)
                                                                           // This function willn't need filteration
  //int place_on_board = row*8 + col;
  int color1 = color[(row*8)+ col];
  
  if ((row -2) >= 0){                                                      // the 2 moves upwards
    // inside the board
    // the upper right move
    if ( (col < 7) && ( (board[((row-2)*8)+ (col+1)] == Free || (board[((row-2)*8)+ (col+1)] != Free && color[((row-2)*8)+ (col+1)] == color1^1)) && Is_allowed_relative_to_king(row,col,row-2,col+1)) ){
    // This condition ensures that the place on board is empty
    led_array[x++] = (row-2)*8 + (col+1);
    }
    
    // the upper left move
    if ( (col > 0) && ( (board[((row-2)*8)+ (col-1)] == Free || (board[((row-2)*8)+ (col-1)] != Free && color[((row-2)*8)+ (col-1)] == color1^1)) && Is_allowed_relative_to_king(row,col,row-2,col-1)) ){
    // This condition ensures that the place on board is empty
    led_array[x++] = (row-2)*8 + (col-1);
    }
    
  }
  
  if((row +2) <= 7){                                                       // the 2 moves downwards
    // inside the board
    // the lower right move 
    if ( (col < 7) && ( (board[((row+2)*8)+ (col+1)] == Free || (board[((row+2)*8)+ (col+1)] != Free && color[((row+2)*8)+ (col+1)] == color1^1)) && Is_allowed_relative_to_king(row,col,row+2,col+1)) ){
    // This condition ensures that the place on board is empty
    led_array[x++] = (row+2)*8 + (col+1);
    }
    
    // the lower left move
    if ( (col > 0) && ( (board[((row+2)*8)+ (col-1)] == Free || (board[((row+2)*8)+ (col-1)] != Free && color[((row+2)*8)+ (col-1)] == color1^1)) && Is_allowed_relative_to_king(row,col,row+2,col-1)) ){
    // This condition ensures that the place on board is empty
    led_array[x++] = (row+2)*8 + (col-1);
    }
    
  }
  
  if ((col -2) >= 0){                                                      // the 2 moves to the left
    // the left lower move
    if ( ( row < 7) && ( (board[((row+1)*8)+ (col-2)] == Free || (board[((row+1)*8)+ (col-2)] != Free && color[((row+1)*8)+ (col-2)] == color1^1)) && Is_allowed_relative_to_king(row,col,row+1,col-2)) ){
    // This condition ensures that the place on board is empty
    led_array[x++] = (row+1)*8 + (col-2);
    }
    
    // the left uper move
    if ( ( row > 0) && ( (board[((row-1)*8)+ (col-2)] == Free || (board[((row-1)*8)+ (col-2)] != Free && color[((row-1)*8)+ (col-2)] == color1^1)) && Is_allowed_relative_to_king(row,col,row-1,col-2)) ){
    // This condition ensures that the place on board is empty
    led_array[x++] = (row-1)*8 + (col-2);
    }
    
  }
  
  if((col +2) <= 7){                                                       // the 2 moves to the right
    // the right upper move
    if ( ( row > 0) && ( (board[((row-1)*8)+ (col+2)] == Free || (board[((row-1)*8)+ (col+2)] != Free && color[((row-1)*8)+ (col+2)] == color1^1)) && Is_allowed_relative_to_king(row,col,row-1,col+2)) ){
    // This condition ensures that the place on board is empty
    led_array[x++] = (row-1)*8 + (col+2);
    }
    
    // the right lower move
    if ( ( row < 7) && ( (board[((row+1)*8)+ (col+2)] == Free || (board[((row+1)*8)+ (col+2)] != Free && color[((row+1)*8)+ (col+2)] == color1^1)) && Is_allowed_relative_to_king(row,col,row+1,col+2)) ){
    // This condition ensures that the place on board is empty
    led_array[x++] = (row+1)*8 + (col+2);
    }
    
  }
}

void Bishop_moves(int row ,int col){
  int x = 0;                                                               // array index increment
  //int place_on_board = row*8 + col;
  int color1 = color[row*8+col];
  int r;                                                                   // rows counter
  int c;                                                                   // columns counter
  bool index_0 = 0;                                                        // This will let you decide wether to complete the diagonal line to the left
  bool index_1 = 0;                                                        // This will let you decide wether to complete the diagonal line to the right

  for ( r = row -1, c = 1; r >= 0; r--,c++){
    // the upper portion of the board
    //*   *
    // * *
    //  *
    if(index_0 == 0){
      if ((board[((r*8)+(col -c))] == Free || (board[((r*8)+(col -c))] != Free && color[((r*8)+(col -c))] == color1^1)) && c <= col && Is_allowed_relative_to_king(row,col,r*8,col-c)){
          // allowable spot to the left 
          led_array[x++] = ((r*8)+(col -c));
        }else{
          index_0 =1;
        }
    }
    if (index_1 == 0){                                                                                                  // col + c <= 7
      if ((board[((r*8)+(col +c))] == Free || (board[((r*8)+(col +c))] != Free && color[((r*8)+(col +c))] == color1^1)) && c <= (7 - col) && Is_allowed_relative_to_king(row,col,r*8,col+c)){
        // allowable spot to the right
        led_array[x++] = ((r*8)+(col +c));
      }else{
        index_1 =1;
      }
    }
  }
  for ( r = row + 1, c = 1; r < 8; r++, c++){
    // the lower portion of the board
    //  *
    // * *
    //*   *  
    if(index_0 == 1){
    if ((board[((r*8)+(col -c))] == Free || (board[((r*8)+(col -c))] != Free && color[((r*8)+(col -c))] == color1^1)) && c <= col && Is_allowed_relative_to_king(row,col,r*8,col-c)){
        // allowable spot to the left
        led_array[x++] = ((r*8)+(col -c));
      }else{
        index_0 =0;
      }
    }
    if(index_1 == 1){                                                                                                 // col + c <= 7
    if ((board[((r*8)+(col +c))] == Free || (board[((r*8)+(col +c))] != Free && color[((r*8)+(col +c))] == color1^1)) && c <= (7 - col) && Is_allowed_relative_to_king(row,col,r*8,col+c)){
        // allowable spot to the right
        led_array[x++] = ((r*8)+(col +c));
      }else{
        index_1 =0;
      }
    }
  }
}

void Queen_moves(int row ,int col){
  Rook_moves(row,col);
  Bishop_moves(row,col);
}

void King_moves(int row, int col){
  int x = 0;                                                               // array index increment
  int place_on_board = row*8 +col;                                         // BECAREFUL  rows(0.....7) col(0.....7)
  int c ;                                                                  // column counter
  int color_king = color[row*8+col];
                                                                           // This one needs FILTERING (Is_allowed_to_king() fn)
                                                                           // a suggested algorithm is to send each position to a function which finds the nearest piece in every direction of the 
                                                                           // surrounding 7 directions (excluding king) and excluding also the ones of the same color ,checking if the nearest piece
                                                                           // can attack the king if he was in such position then returning 1 or 0 to add the Led in Led_array or not. 
                                                                           // also we will need another function to check for castling ( NOT DONE YET )

  if ( row < 7){
    // The lower 3 moves
    for(int i= 0, c = col - 1; i< 3;i++,c++){
        if((board[(row+1)*8+c] == Free || (board[(row+1)*8+c] != Free && color[(row+1)*8+c] == color_king^1))&& c >= 0 && c <= 7 && Is_allowed_to_king(row,col,row+1,c)){
          // allowable spot and the second condition prevents negative column number
          led_array[x++] = (row+1)*8+c;
        }
    }
  }
  if ( row > 0){
    // the upper 3 moves
    for(int i= 0,  c = col -1; i< 3;i++,c++){
        if((board[(row-1)*8+c] == Free || (board[(row-1)*8+c] != Free && color[(row-1)*8+c] == color_king^1)) && c >= 0 && c <= 7 && Is_allowed_to_king(row,col,row-1,c)){
          // allowable spot and the second condition prevents negative column number or outside of scope
          led_array[x++] = (row-1)*8+c;
        }
   }
  }
   if ( col > 0){
    if ( (board[(row*8)+(col-1)] == Free || (board[(row*8)+(col-1)] != Free && color[(row*8)+(col-1)] == color_king^1)) && Is_allowed_to_king(row,col,row,col-1)){
      // allowable spot to the left
      led_array[x++] = (row*8)+(col-1);
    }
   }
   if ( col < 7){
      if ((board[(row*8)+(col+1)] == Free || (board[(row*8)+(col+1)] != Free && color[(row*8)+(col+1)] == color_king^1)) && Is_allowed_to_king(row,col,row,col+1)){
          // free spot to the right
          led_array[x++] = (row*8)+(col+1);
      }
   }
}
  



bool Is_king_in_check(int row,int col){
  // This will check if a piece existing in a path where it can attack the king
  // This will be done piece by piece, returning 1 if king is in check and 0 if not.
  // First we check vertical then horizontal, and find the first piece we meet
  // then we check for its color to know if it belongs to the oppposite side
  // For vertical and horizontal,if the piece is a queen or a rook,this means a check
  // For diagonal,if the piece is a bishop,pawn,queen,king
  // For the Knight,special treatment.
  int place_on_board = row*8 + col;
  int color_king = color[place_on_board];
  
  // Find first near piece verically upwards
  if ( row > 0 ){
    // Avoiding going outside the board
      for( i = row -1 ; i >= 0;i--){
          if(board[((i*8)+col)] != Free){
            // There is a piece above the king (^^)
            // Now we check its color and type 
            if ( color[(i*8)+col] == (color_king ^1) && (board[(i*8)+col] == Queen || board[(i*8)+col] == Rook)){
                return 1;
            }
          }
      }
  }
  
  // Find first near place vertically downwards
  if ( row < 7){
    // Avoiding going outside the board
    for( i = row +1 ; i <=7; i++){
        if(board[((i*8)+col)] != Free){
           // There is a piece under the king (vv)
           // Now we check its color and type
            if ( color[(i*8)+col] == (color_king ^1) && (board[(i*8)+col] == Queen || board[(i*8)+col] == Rook)){
            return 1;
            }
         }
      }
  }
  
  // Find first near piece to the left
  if ( col > 0){
    // Avoiding going outside the board 
      for( i = col-1 ; i >= 0 ; i--){
          if(board[((row*8)+i)] != Free){
              // There is a piece left the king (<<)
              // Now we check its color and type 
              if ( color[(row*8)+i] == (color_king ^1) && (board[(row*8)+i] == Queen || board[(col*8)+i] == Rook)){
                  return 1;
              }
          }
        }
  }
  
  // Find first near piece to the right
  if ( col < 7){
    // Avoiding going outside the board 
        for( i = col+1 ; i <=7 ; i++){
            if(board[((row*8)+i)] != Free){
                // There is a piece right the king (>>)
                // Now we check its color and type 
                if ( color[(row*8)+i] == (color_king ^1) && (board[(row*8)+i] == Queen || board[(col*8)+i] == Rook)){
                    return 1;
                }
            }
        }
  }
  
  int c ;                                                                  // to control the diagonal flow like stairs
  // Find first near piece diagonally upwards to the right                  
  if ( row > 0 && col < 7){
    // Avoiding going outside the board 
      for ( i = row -1, c = col +1 ; i >= 0 ; i--,c++){
           if ( board[(i*8)+c] != Free){
              // There is a piece
              // Now we check its colour and type it could be a Pawn but it will have to be near or a queen or a bishop which doesn't have to be near
              if ( i == row -1 && color[(i*8)+c] == black && color_king == white && (board[(i*8)+c] == Pawn || board[(i*8)+c] == King)){
                // That is the first upper right block to the King, could be a black Pawn or King
                return 1;
              }
              else if( color[(i*8)+c] == (color_king ^ 1) && (board[(i*8)+c] == Queen || board[(i*8)+c] == Bishop)){
                   // There is a Bishop or a Queen making the king in check
                   return 1;
              }
           }
          }
  }
  
  // Find first near piece diagonally upwards to the left
  if ( row > 0 && col > 0){
    // Avoiding going outside the board 
      for ( i = row -1, c = col -1 ; i >= 0 ; i--, c--){
           if ( board[(i*8)+c] != Free){
              // There is a piece, now check if it is from the opposite color
              // Now it could be a Pawn but it will have to be near or a queen or a bishop which doesn't have to be near
              if ( i == row -1 && color[(i*8)+c] == black && color_king == white && (board[(i*8)+c] == Pawn || board[(i*8)+c] == King)){
                // That is the first upper left block to the king, could be a black Pawn or King
                return 1;
              }
              else if( color[(i*8)+c] == (color_king ^ 1) && (board[(i*8)+c] == Queen || board[(i*8)+c] == Bishop)){
                   // There is a Bishop or a Queen making the king in check
                   return 1;
              }
           }
          }
  }
  
  // Find first near diagonally downwards to the right
  if ( row < 7 && col < 7 ){
    // Avoiding going outside the board 
      for ( i = row + 1, c = col+1 ; i <=7 ; i++, c++){
           if ( board[(i*8)+c] != Free){
              // There is a piece, now check if it is from the opposite color
              // Now it could be a Pawn but it will have to be near or a queen or a bishop which doesn't have to be near
               if ( i == row +1 && color[(i*8)+c] == white && color_king == black && (board[(i*8)+c] == Pawn || board[(i*8)+c] == King)){
                // That is the first lower right block to the king, could be a black Pawn or King
                return 1;
              }
              else if( color[(i*8)+c] == (color_king ^ 1) && (board[(i*8)+c] == Queen || board[(i*8)+c] == Bishop)){
                   // There is a Bishop or a Queen making the king in check
                   return 1;
              }
           }
          }
  }
  
  // Find first near diagonally downwards to the left
  if ( row < 7 && col > 0 ){
    // Avoiding going outside the board               
      for ( i = row + 1, c = col -1 ; i < 8 ; i++, c--){
           if (board[(i*8)+c] != Free){
            // There is a piece, now check if it is from the opposite color
            // Now it could be a Pawn but it will have to be near or a queen or a bishop which doesn't have to be near
             if ( i == row +1 && color[(i*8)+c] == white && color_king == black && (board[(i*8)+c] == Pawn || board[(i*8)+c] == King)){
                // That is the first lower right block to the king, could be a black Pawn or King
                return 1;
              }
              else if( color[(i*8)+c] == (color_king ^ 1) && (board[(i*8)+c] == Queen || board[(i*8)+c] == Bishop)){
                   // There is a Bishop or a Queen making the king in check
                   return 1;
              }
           }
           }
      }
      
  // By using algorithim of Knight_moves()fn   
  // 2 upper spots
  if ((row -2) >= 0){
    // Inside the board
    if( col < 7){
    if ( board[((row-2)*8)+ (col+1)] == Knight && color[((row-2)*8)+ (col+1)] == (color_king ^ 1)){
    // There is a Knight in upper right
    return 1;
    }
    }
    if( col >0){
    if ( board[((row-2)*8)+ (col-1)] == Knight && color[((row-2)*8)+ (col-1)] == (color_king ^ 1)){
    // There is a Knight in upper left
    return 1;
    }
    }
  }
  if((row +2) <= 7){
    // Inside the board
    if(col < 7){
    if ( board[((row+2)*8)+ (col+1)] == Knight && color[((row+2)*8)+ (col+1)] == (color_king ^ 1)){
    // There is a Knight in lower right
    return 1;
    }
    }
    if ( col > 0){
    if ( board[((row+2)*8)+ (col-1)] == Knight && color[((row+2)*8)+ (col-1)] == (color_king ^ 1)){
    // There is a Knight in lower left
    return 1;
    }
    }
  }
  if ((col -2) >= 0){
    // Inside the board
    if ( row < 7){
    if ( board[((row+1)*8)+ (col-2)] == Knight && color[((row+1)*8)+ (col-2)] == (color_king ^ 1)){
    // There is a Knight in lower left
    return 1;
    }
    }
    if ( row>0){
    if ( board[((row-1)*8)+ (col-2)] == Knight && color[((row-1)*8)+ (col-2)] == (color_king ^ 1)){
    // There is a Knight in upper left
    return 1;
    }
    }
  }
  if((col +2) <= 7){
    // Inside the board
    if (row < 7){
    if ( board[((row+1)*8)+ (col+2)] == Knight && color[((row+1)*8)+ (col+2)] == (color_king ^ 1)){
    // There is a Knight in lower right
    return 1;
    }
    }
    if ( row > 0){
    if ( board[((row-1)*8)+ (col+2)] == Knight && color[((row-1)*8)+ (col+2)] == (color_king ^ 1)){
    // There is a Knight in upper right
    return 1;
    }
    }
  }
  // If the king is in safe return 0
  return 0;
}



bool Is_allowed_to_king(int from_row, int from_col, int to_row, int to_col){
  // This will check if the king went to the new position ,the king would be in danger
  // Returning 1 if allowable and 0 if not.  
    int to_piece = board[to_row*8+to_col];
    board[to_row*8+to_col] = board[from_row*8 + from_col];                 // send the king to the position that you want to check
    board[from_row*8 + from_col] = Free;                                   // free the king 's position
    bool check = Is_king_in_check(to_row,to_col);                          // find if he is in danger
    board[from_row*8 + from_col] = board[to_row*8+to_col];                 // return the king to where he was
    board[to_row*8+to_col] = to_piece;                                     // retun the piece to its position again
    return check^1;
}

bool Is_allowed_relative_to_king(int from_row, int from_col, int to_row, int to_col){
  // This will check if the any piece went to new position ,the king would be in danger
  // Returning 1 if allowable and 0 if not.  
    int to_piece = board[to_row*8+to_col];
    board[to_row*8+to_col] = board[from_row*8 + from_col];                 // send the piece to the position that you want to check
    board[from_row*8 + from_col] = Free;                                   // free the position position
    int king_row, king_col;                                                // the same color king location
    if (color[from_row*8+from_col] == white){                              // white piece
      king_row = Get_row_col(white_king_loc,&king_col);
    }else{                                                                 // black piece
      king_row = Get_row_col(black_king_loc,&king_col);
    }
    bool check = Is_king_in_check(king_row,king_col);                      // find if king is in danger
    board[from_row*8 + from_col] = board[to_row*8+to_col];                 // return the piece to where it was
    board[to_row*8+to_col] = to_piece;                                     // free the position again
    return check^1;
}

bool Is_legal_move(int piece, int from_row, int from_col, int to_row, int to_col){
  // This fun. checks Was the piece moved to legal spot?
  // Return 1 if legal and 0 if illegal
  
  int to_place = (8 * to_row) + to_col ;
  
  Intialize_led_array();
 

  if ( piece == King){        
   King_moves(from_row,from_col);                                        
  }else if ( piece == Queen){
   Queen_moves(from_row,from_col);
  }else if ( piece == Bishop){
   Bishop_moves(from_row,from_col);
  }else if ( piece == Knight){
   Knight_moves(from_row,from_col);
  }else if ( piece == Rook){
   Rook_moves(from_row,from_col);
  }else if ( piece == Pawn){
   Pawn_moves(from_row,from_col);
  }
  for(i = 0; i < Length_array(); i++){
    if(to_place == led_array[i]){
      return 1;
    }
  }
  
  return 0;
}

void Get_action(int act, int color){

    // IT must know if a promotion (act = 0) happened or a check ( act =1 )or a move wasn't legal either wrong turn or wrong place


    if (act == 0){                                                          // Promotion
        if (color == white){
          
          lcdwhite.setCursor(0,0);
          lcdwhite.print("  Promote pawn  ");
          lcdwhite.setCursor(0,1);
          lcdwhite.print("    to Queen!    ");
          
          lcdblack.setCursor(0,0);
          lcdblack.print("  Your ennemey  ");
          lcdblack.setCursor(0,1);
          lcdblack.print(" has promotion! ");
          Buzzer();
          
        }else{
          lcdblack.setCursor(0,0);
          lcdblack.print("  Promote pawn  ");
          lcdblack.setCursor(0,1);
          lcdblack.print("    to Queen!    ");
          
          lcdwhite.setCursor(0,0);
          lcdwhite.print("  Your ennemey  ");
          lcdwhite.setCursor(0,1);
          lcdwhite.print(" has promotion! ");
          Buzzer();
          
        }
    }else if (act == 1){                                                 //check
        if (color == 0){
          lcdwhite.setCursor(0,1);
          lcdwhite.print("     Check!     ");
          Buzzer();
        }else{
          lcdblack.setCursor(0,1);
          lcdblack.print("     Check!     ");
          Buzzer();
        }
    }else if (act == 2){
        if (color == 0){
          lcdwhite.setCursor(0,1);
          lcdwhite.print("WRONG TURN!");
          lcdblack.setCursor(0,1);
          lcdblack.print("ALERT! YOUR TURN");
          Buzzer();
        }else{
          lcdwhite.setCursor(0,1);
          lcdwhite.print("ALERT! YOUR TURN");
          lcdblack.setCursor(0,1);
          lcdblack.print(" WRONG   TURN ! ");
          Buzzer();
        }
    }else if(act == 3){
        if (color == 0){
          lcdwhite.setCursor(0,1);
          lcdwhite.print(" ALERT! ILLEGAL ");
          lcdblack.setCursor(0,1);
          lcdblack.print(" ALERT! ILLEGAL ");
          Buzzer();
        }else{
          lcdwhite.setCursor(0,1);
          lcdwhite.print(" ALERT! ILLEGAL ");
          lcdblack.setCursor(0,1);
          lcdblack.print(" ALERT! ILLEGAL ");
          Buzzer();
        }
    }
  
}
void Buzzer(void){
          digitalWrite(buzzer_1,HIGH);
          digitalWrite(buzzer_2,HIGH);
          delay(100);
          digitalWrite(buzzer_1,LOW);
          digitalWrite(buzzer_2,LOW);
          delay(100);
}

void Decision_maker(void){
  int piece_loop = board[place0];
        if (color[place0] == turn){                           // first move is always White
          if(code == 0){
            Intialize_led_array();
                // This code means a piece is lifted and it is still in the air. We will find which piece exists in such place and light its leds. Note : When the Mega sends a row, it starts from 0. while the Uno starts from 2 on sending   
                if ( piece_loop == King){                                     // if a king moves , king_loc must be changed
                  King_moves(from_row,from_col);                                        
                }else if ( piece_loop == Queen){
                  Queen_moves(from_row,from_col);
                }else if ( piece_loop == Bishop){
                  Bishop_moves(from_row,from_col);
                }else if ( piece_loop == Knight){
                  Knight_moves(from_row,from_col);
                }else if ( piece_loop == Rook){
                  Rook_moves(from_row,from_col);
                }else if ( piece_loop == Pawn){
                  Pawn_moves(from_row,from_col);
                }
                Light_leds_off();
                Light_leds_on();
                
        }else if (code == 1){
                // This means a Piece is placed down and we need to do nothing
                Light_leds_off;
        }else if (code == 3){
                // This means a piece is moved but no eating happened and we need to update our arrays
                Light_leds_off;
                int move_piece = -1;
                int move_piece_color = -1;
                move_piece = board[place0];
                move_piece_color = color[place0];  
                Rearrange_me(move_piece,move_piece_color,place0,place1);
                                                                                    // a move is made so the turn must be switched to the other color
                // if the king move we should change his position
                if( piece_loop == King ){
                  if(color[to_row*8+to_col] == 0){
                    white_king_loc = to_row*8+to_col;
                  }
                  else{
                    black_king_loc = to_row*8+to_col;
                  }
                }

                // Check the movement is legal or NOT
                if(Is_legal_move(piece_loop, from_row, from_col, to_row, to_col) ^1){
                  
                  Get_action(3, color[to_row*8+to_col]);
                  while(detect_1[to_row *8+to_col] == 1 && detect_1[from_row *8+from_col] == 0){
                    leds[to_row *8+to_col] = CRGB::DeepPink;
                    FastLED.show();
                    Detect_me();
                  }
                  Light_leds_off();

                  // Return the piece to its previous right block 
                  board[from_row*8+from_col] = piece_loop;                                      
                  board[to_row*8+to_col] = Free;                                              
                  color[from_row*8+from_col] = color[from_row*8+from_col];                         
                  color[to_row*8+to_col] = Free;
                  // We don't need to check the king enemy in danger as the game undo
                  goto BREAK ;
                }
                // Check the king enemy in danger
                int king_row, king_col;                                                                     
                if(color[to_row*8+to_col] == 0){               // white piece
                  king_row = Get_row_col(black_king_loc,&king_col);
                }else{                                              // black piece
                   king_row = Get_row_col(white_king_loc,&king_col);
                }
                if (Is_king_in_check(king_row,king_col)) Get_action(1,color[to_row*8+to_col] ^= 1);


                turn ^=1;
        }else if (code == 2){
                // This means a piece is moved and also a piece is eaten
                int killer = -1 ;
                int killer_color = -1;
                killer = board[place0];
                killer_color = color[place0];
                // we may need to check legallity before the next line   (Uno must send a feedback on Legality)  (Lcd notification if not legal)
                Rearrange_me(killer,killer_color,place0,place1);
                
                // if the king move we should change his position
                if( piece_loop == 1 ){
                  if(color[to_row*8+to_col] == 0){
                    white_king_loc = to_row*8+to_col;
                  }
                  else{
                    black_king_loc = to_row*8+to_col;
                  }
                }

                // Check the movement is legal or NOT
                if(Is_legal_move(piece_loop, from_row, from_col, to_row, to_col) ^1){
                  
                  Get_action(3, color[to_row*8+to_col]);
                  while(detect_1[to_row *8+to_col] == 1 && detect_1[from_row *8+from_col] == 0){
                    leds[to_row *8+to_col] = CRGB::DeepPink;
                    FastLED.show();
                    Detect_me();

                  }
                  Light_leds_off();
                    

                  // Return the piece to its previous right block 
                  board[from_row*8+from_col] = piece_loop;                                      
                  board[to_row*8+to_col] = Free;                                              
                  color[from_row*8+from_col] = color[from_row*8+from_col];                         
                  color[to_row*8+to_col] = Free;
                  // We don't need to check the king enemy in danger as the game undo
                  goto BREAK ;
                }
            
                //Check the king enemy in danger
                int king_row, king_col;                                                                     
                if(color[to_row*8+to_col] == 0){               // white piece
                  king_row = Get_row_col(black_king_loc,&king_col);
                }else{                                              // black piece
                   king_row = Get_row_col(white_king_loc,&king_col);
                }
                if (Is_king_in_check(king_row,king_col)) Get_action(1,color[to_row*8+to_col] ^= 1) ;
                
                turn ^=1;
        
        }
     }else{
        Get_action(2, color[place0]);                                                                        // Wrong turn warning , This will give the players a 10-sec period to retreat the move 
     }
     BREAK:;
}
   
