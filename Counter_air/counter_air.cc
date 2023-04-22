#include "open_spiel/games/counter_air.h"

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>


#include "open_spiel/spiel_utils.h"
#include "open_spiel/utils/tensor_view.h"

namespace open_spiel {
	namespace counter_air {
		namespace {
			const GameType kGameType{
				/*short_name=*/"counter_air",
				/*long_name=*/"Counter Air",
				GameType::Dynamics::kSequential,  //turn based
				GameType::ChanceMode::kDeterministic, //no chance
				GameType::Information::kPerfectInformation, //Everyone knows all information about the game
				GameType::Utility::kZeroSum,                //os�kra
				GameType::RewardModel::kTerminal,           //V�ljer n�r rewards ska ges
				/*max_num_players=*/2,
				/*min_num_players=*/2,
				/*provides_information_state_string=*/true, //
				/*provides_information_state_tensor=*/false, //
				/*provides_observation_string=*/true,
				/*provides_observation_tensor=*/true,
				/*parameter_specification=*/{}  // no parameters
			};

			std::shared_ptr<const Game> Factory(const GameParameters& params) {
				return std::shared_ptr<const Game>(new CounterAirGame(params));
			}

			REGISTER_SPIEL_GAME(kGameType, Factory);

		}

		//SLUTET
		bool BoardHasLine(const std::array<int, kNumCells>& board,
			const Player player) {
			
			/*if(board[1] == 2 && board[0] == 2){
				return true;
			}*/
			
			if(board[1] == 1 && board[0] == 6 && board[35] == 3){
				return true;
			}
			if(board[35] == player){
				return true;
			}else{
				return false;
			}
			
			/*
			if(phase == 2){
				return true;
			}*/

			return false;
		}

		Player CounterAirState::CurrentPlayer() const{
			if(board_[35] == 3)
			{
				if(board_[1] == 0)
			{
				if(blue_pieces > (board_[2] + board_[4] + board_[6] + board_[8]))
				{
					return IsTerminal() ? kTerminalPlayerId : Player{0};
				}
				else
				{
					if((red_sams+red_fighters) > (board_[kCols+2] + board_[kCols+4] + board_[kCols+6] + board_[kCols+8])){
						return IsTerminal() ? kTerminalPlayerId : Player{1};
					}
					else
					{
						return IsTerminal() ? kTerminalPlayerId : Player{0};
					}
				}
			}
			else if (board_[1] == 2 || board_[1] == 3 || board_[1] == 4) {
				
				if((board_[27])%2 == 0) { 
					if(board_[28] == true && board_[29] == false && board_[32] != true){
						return IsTerminal() ? kTerminalPlayerId : Player{1};
					}
					return IsTerminal() ? kTerminalPlayerId : Player{0};
				}else{
					if(board_[28] == false && board_[29] == true && board_[32] != true)
					{
						return IsTerminal() ? kTerminalPlayerId : Player{0};
					}
					return IsTerminal() ? kTerminalPlayerId : Player{1};
				}

			}else if(board_[1] == 1){

				if(board_[28] != true)
				{
					return IsTerminal() ? kTerminalPlayerId : Player{0};
				}
				else
				{
					if((red_sams+red_fighters) > (board_[kCols+2] + board_[kCols+4] + board_[kCols+6] + board_[kCols+8])){
						return IsTerminal() ? kTerminalPlayerId : Player{1};
					}
					else
					{
						return IsTerminal() ? kTerminalPlayerId : Player{0};
					}
				}
			}
			return IsTerminal() ? kTerminalPlayerId : Player{0};
			}else{
				if(board_[1] == 0)
			{
				if(blue_pieces > (board_[2] + board_[4] + board_[6] + board_[8]))
				{
					return IsTerminal() ? kTerminalPlayerId : Player{0};
				}
				else
				{
					if((red_sams+red_fighters) > (board_[kCols+2] + board_[kCols+4] + board_[kCols+6] + board_[kCols+8])){
						return IsTerminal() ? kTerminalPlayerId : Player{1};
					}
					else
					{
						return IsTerminal() ? kTerminalPlayerId : Player{0};
					}
				}
			}
			else if (board_[1] == 2 || board_[1] == 3 || board_[1] == 4) {
				
				if((board_[27])%2 == 0) { 
					if(board_[28] == true && board_[29] == false && board_[32] != true){
						return IsTerminal() ? kTerminalPlayerId : Player{1};
					}
					return IsTerminal() ? kTerminalPlayerId : Player{0};
				}else{
					if(board_[28] == false && board_[29] == true && board_[32] != true)
					{
						return IsTerminal() ? kTerminalPlayerId : Player{0};
					}
					return IsTerminal() ? kTerminalPlayerId : Player{1};
				}

			}else if(board_[1] == 1){

				if(board_[28] != true)
				{
					return IsTerminal() ? kTerminalPlayerId : Player{0};
				}
				else
				{
					if(board_[29] != true){
						return IsTerminal() ? kTerminalPlayerId : Player{1};
					}
					else
					{
						return IsTerminal() ? kTerminalPlayerId : Player{0};
					}
				}
			}
			return IsTerminal() ? kTerminalPlayerId : Player{board_[35]};
			}
			
		}

		void CounterAirState::TakenHit(int player, int space, int evade){
			if (space == kCols){
				board_[space]--;
				board_[space+1]++;
			}else if(board_[1] == 3 && space == 8 || evade == 3){
				board_[player*kCols + 10]++;
			}else{
				if(evade == 1)
				{
					board_[player*kCols + 10]++;
					board_[space]--;
					board_[space+1]++;
				}else if(evade == 0){
					board_[player*kCols + 10]+=2;
				}else if(evade == 6 && board_[1] == 4)
				{
					if(board_[kCols+2] != 0)
					{
						board_[kCols+2]--;
					}else{
						board_[kCols+3]--;
					}

					board_[kCols+5]++;
				}else{
					if (board_[1] == 2 && evade == 2){
						board_[2]--;
						board_[3]++;
					}
					if(evade == 2 && board_[1] == 3){
						board_[6]--;
						board_[7]++;
					}
					board_[player*kCols + 10]++;
				}
			}

			if(board_[player*kCols+ 10] == 4)
				{
					if(board_[1] == 3 && space == 8){
						board_[space]--;
						board_[33]--;
					}else if(evade == 0)
					{
						board_[space]--;
					}else if(evade == 2 && board_[1] == 2)
					{
						board_[3]--;
					}else if(evade == 2 && board_[1] == 3){
						board_[7]--;
					}else if(evade == 6 && board_[1] == 4){
						board_[kCols+5]--;
					}else if(evade == 3 && board_[1] == 4){
						if(space == kCols+8){
							board_[kCols+8]--;
						}
					}else{
						board_[space+1]--;
					}
					board_[player*kCols+10] = 0;
					board_[player * kCols + 12]++;
				}
		}


		void CounterAirState::DoApplyAction(Action move) {
			current_player_ = CurrentPlayer();
			if(move != 30){
			
			if(board_[1] == 0) { //First phase of the game (put your pieces on the board)
				switch(current_player_) {
					case 0:
							if(board_[27] > 6){
								board_[27] = 0;	
							}
							board_[27]+=2;
							if(board_[27] < 9)
							{
								board_[board_[27]] = move;	
							}
						break;
					case 1:
						if(move > 4){
							board_[kCols+6] = move%5;
							board_[kCols+8] = red_sams - move%5;
							board_[1] = 2;
						}else{
							board_[kCols+2] = move;
							board_[kCols+4] = red_fighters - move;
						}
						break;
				}
			}
			else if(board_[1] == 1)
			{
				if (board_[0] == 3){
					board_[34] = 1;
				}
				switch(current_player_) {
					case 0:
							/*if(board_[27] > 6){
								board_[27] = 0;	
								//Nästa turn
							}*/
							board_[27]+=2;
							if(board_[27] < 9)
							{
								board_[board_[27]] = move;
								board_[13] = board_[13]-move;	
							}

							if(board_[13] == 0){
								board_[28] = true;
								if(board_[kCols+2] != 0 || board_[kCols + 4]!= 0){
									board_[13] = board_[kCols+2] + board_[kCols+4];
									board_[kCols+2] = 0;
									board_[kCols+4] = 0;
									board_[27]=0;
								}else if(board_[kCols+6] != 0 || board_[kCols+8] != 0){
									board_[13] = board_[kCols+6] + board_[kCols+8];
									board_[kCols+6] = 0;
									board_[kCols+8] = 0;
									board_[27]=1;
								}else{
									board_[kCols]+=board_[kCols+1]; board_[kCols+1]=0;
									board_[kCols+2]+=board_[kCols+3]; board_[kCols+3]=0;
									board_[kCols+4]+=board_[kCols+5]; board_[kCols+5]=0;
									board_[kCols+6]+=board_[kCols+7]; board_[kCols+7]=0;
									board_[kCols+8]+=board_[kCols+9]; board_[kCols+9]=0;
									board_[29] = true;
								}
								
							}
						break;
					case 1:
						if(move > 4){
							board_[kCols+6] = move%5;
							board_[kCols+8] = board_[13] - move%5;
							board_[13] = 0;

							board_[kCols]+=board_[kCols+1]; board_[kCols+1]=0;
							board_[kCols+2]+=board_[kCols+3]; board_[kCols+3]=0;
							board_[kCols+4]+=board_[kCols+5]; board_[kCols+5]=0;
							board_[kCols+6]+=board_[kCols+7]; board_[kCols+7]=0;
							board_[kCols+8]+=board_[kCols+9]; board_[kCols+9]=0;

							board_[29] = true;
							board_[kCols] = 4;
							board_[kCols+1] = 0;
						}else{
							board_[kCols+2] = move;
							board_[kCols+4] = board_[13] - move;
							board_[13] = 0;

							if(board_[kCols+6] != 0 || board_[kCols+8] != 0){
								board_[13] = board_[kCols+6] + board_[kCols+8];
								board_[kCols+6] = 0;
								board_[kCols+8] = 0;
								board_[27]++;
							}else{
								board_[kCols]+=board_[kCols+1]; board_[kCols+1]=0;
								board_[kCols+2]+=board_[kCols+3]; board_[kCols+3]=0;
								board_[kCols+4]+=board_[kCols+5]; board_[kCols+5]=0;
								board_[kCols+6]+=board_[kCols+7]; board_[kCols+7]=0;
								board_[kCols+8]+=board_[kCols+9]; board_[kCols+9]=0;
								board_[29] = true;
							}
						}
						break;
				}

			}
			//second phase of game, escort attacks intercept, intercept attacks escort, high strike and low strike
			else if (board_[1] == 2){
				switch(current_player_) {
					case 0:
						//Blue attacks with Armed escorts, can only attack intercepts
						if (board_[32] == false){
							board_[2]--;
							board_[3]++;
							board_[32] = true;
							board_[27]++;
						}else{
							//Blue gets board_[32]
							TakenHit(0, board_[31], move);
							board_[32] = false;
							if(board_[2] == 0){
								board_[28] = true;
							}
							if(board_[28] == true){
								board_[27]++;
							}
						}
						break;

					case 1:
						if(board_[32] == false) {
							board_[kCols + 2]--;
							board_[kCols + 3]++;
							board_[32] = true;
							board_[27]++;
							board_[31] = move;		
						}else{
								TakenHit(1,kCols+2, move);
								board_[32] = false;
								if(board_[kCols+2] == 0){
								board_[29] = true;
								}
								if(board_[29] == true){
								board_[27]++;
								}
						}
						break;
				}
				//If red doesn't have any armed fighters in intercept, neither player can shoot
				if(board_[kCols+2] == 0)
				{
					board_[29] = true;
					board_[28] = true;
				}
				//If blue doesn't have any armed escorts, blue can no longer shoot
				if(board_[1] == 0)
				{
					board_[28] = true;
				}
				//If there are no armed: escorts, high strikes or low strikes. Neither player can shoot
				if((board_[2] == 0) && (board_[4] == 0) && (board_[8] == 0)){
					board_[29] = true;
					board_[28] = true;
				}
			}else if(board_[1] == 3){
				switch(current_player_) {
					case 0:
						//Blue attacks with Armed escorts, can only attack intercepts
						if (board_[32] == false){	
							board_[6]--;
							board_[7]++;
							board_[32] = true;
							board_[27]++;
							board_[31] = move;	
						}else{
							//Blue gets board_[32]
							TakenHit(0, board_[31], move);
							board_[32] = false;
							if(board_[6] == 0){
								board_[28] = true;
							}
							if(board_[28] == true){
								board_[27]++;
							}
						}
						break;

					case 1:
						if(board_[32] == false) {
							if(move == 4){
								board_[kCols + 6]--;
								board_[kCols + 7]++;
							}else{
								board_[33]++;
								board_[kCols]--;
								board_[kCols + 1]++;
							}
							board_[32] = true;
							board_[27]++;
							board_[31] = move;				
						}else{
								TakenHit(1,board_[31], move);
								board_[32] = false;
								if((board_[kCols] < 1 || board_[8] <= board_[33]) && (board_[kCols+6] < 1 || board_[4] < 1)){ //special fall
								board_[29] = true;
								}
								if(board_[29] == true){
								board_[27]++;
								}
						}
						break;

				}
				
				//If red doesn't have any armed fighters in intercept, neither player can shoot
				if(board_[kCols] == 0 && board_[kCols+6] == 0)
				{
					board_[29] = true;
					board_[28] = true;
				}
				//If blue doesn't have any armed escorts, blue can no longer shoot
				if(board_[6] == 0)
				{
					board_[28] = true;
				}
				//If there are no armed: escorts, high strikes or low strikes. Neither player can shoot
				if((board_[kCols] < 1 || board_[8] <= board_[33]) && (board_[kCols+6] < 1 || board_[4] < 1)){
					board_[29] = true;
				}

				
			}else if(board_[1] == 4){
				switch(current_player_) {
					case 0:
						//Blue attacks with HighStrike & LowStrike, eventually UAV
						if (board_[32] == false){	
							if (board_[4] < 1 && board_[34] == 1){
								board_[34] = 0;
							}
							else if(board_[27]==0){
								board_[4]--;
								board_[5]++;
							}else{
								board_[8]--;
								board_[9]++;
							}
							board_[32] = true;
							board_[27]++;
							board_[31] = move;	
						}
						if(move == kCols+4 || move == kCols+5){
							board_[36]++;
						}else if (move == kCols+6 || move == kCols+7){
							board_[37]++;
						}else if(move == kCols+8 || move == kCols+9){
							board_[38]++;
						}
						break;

					case 1:
							TakenHit(1,board_[31], move);
							board_[32] = false;
							board_[27]--;
						break;

				}

				if((board_[4] < 1 || (board_[kCols+4] < 1 && board_[kCols+6] < 1 && board_[kCols+8] < 1) || (board_[kCols+4] <= board_[36] && board_[kCols+6] <= board_[37] && board_[kCols+8] <= board_[38])) && board_[27] == 0){
					board_[27]+=2;
				}
				if (((board_[kCols+6] < 1 && board_[kCols+8] < 1) || (board_[kCols+6] <= board_[37] && board_[kCols+8] <= board_[38])) && board_[34] == 1){
					board_[34] = 0;
				}
				//
				if(board_[27]==2 && ((board_[8] < 1) || (board_[kCols+2] < 1 && board_[kCols+4] < 1 && board_[kCols+6] < 1 && board_[kCols+8] < 1) || (board_[kCols+4] <= board_[36] && board_[kCols+6] <= board_[37] && board_[kCols+8] <= board_[38]))){
					if (board_[34] == 1 && (board_[0] == 1 || board_[0] == 3)){
						
					}else{
						board_[28] = true;
					}
				}
			}
			}else{
				board_[30] = false;
				board_[27] = 0;

				if(board_[1] == 1){

					for(int i = 2; i<10; i+=2){
						board_[i] = 0;
						board_[i+1] = 0;
					}
					if (board_[0] == 3){
						board_[11]=1;
					}
					board_[13] = blue_pieces-board_[12]-board_[11];
				}

				if(board_[1] == 2){
					if(board_[2] < 1){
						board_[28] = true;
						board_[27]++;
					}
					if(board_[2] < 1 && board_[4] < 1 && board_[8] < 1)
					{
						board_[29] = true;
					}

					if(board_[kCols+2] < 1){
						board_[29] = true;
						board_[28] = true;
					}
				}else if(board_[1] == 3){
					if (board_[6] < 1 || (board_[kCols] < 1 && board_[kCols+6] < 1)){ //ändrat
					board_[28] = true;
					board_[27]++;
					}
					if ((board_[kCols] < 1 || board_[8] == board_[33]) && (board_[kCols+6] < 1 || board_[4] < 1)){
						board_[29] = true;
					}

				}else if(board_[1] == 4)
				{
				board_[29] = true;

				if((board_[4] < 1 || (board_[kCols+4] < 1 && board_[kCols+6] < 1 && board_[kCols+8] < 1)) && board_[32] == false){
					board_[27]+=2;
				}
				if ((board_[kCols+6] < 1 && board_[kCols+8] < 1) && board_[34] == 1){
					board_[34] = 0;
				}
				
				if(board_[27]==2 && ((board_[8] < 1 && board_[34] == 0) || (board_[kCols+2] < 1 && board_[kCols+4] < 1 && board_[kCols+6] < 1 && board_[kCols+8] < 1)) && board_[32] == false){
					board_[28] = true;
				}
				}
			}

			if(board_[1] == 2 ||board_[1] == 3 || board_[1] == 4){
				if(board_[28] == true && board_[29] == true && board_[32] != true)
				{
					if(board_[1] == 4){
						board_[1] = 1;
						board_[30]=true;
						board_[36] = 0;
						board_[37] = 0;
						board_[38] = 0;
						board_[0]++;
					}else if(board_[1] == 3)
					{
						board_[30]=true;
						board_[1]++;
						board_[33] = 0;
					}
					else {
						board_[30]=true;
						board_[1]++;
					}
					board_[27] = 0;
					board_[28] = false;
					board_[29] = false;
				}
			}
			else if(board_[1] == 1){
				if(board_[28] == true && board_[29] == true)
				{
					board_[27] = 0;
					board_[28] = false;
					board_[29] = false;
					board_[1] = 2;
					board_[30] = true;
				}
			}
			if(HasLine(CurrentPlayer())){
				if((board_[12]+2) == board_[kCols+12]){
					if(board_[10] > board_[kCols+10])
					{
						board_[35] = 1;
					}else{
						board_[35] = 2;
					}
				}
				if((board_[12]+2) > (board_[kCols+12])){
					board_[35] = 1;
				}else{
					board_[35] = 0;
				}
				outcome_ = board_[35];
			}
			
		}

		//Ändrat 0
		std::vector<Action> CounterAirState::LegalActions() const {    //Klassen är counterAirState och metod är LegalActions
			if (IsTerminal()) return {};    //Om spelet har vunnits return annars hoppa över
			std::vector<Action> moves;    //Skapar en vector med alla legal moves
			//current_player_ = CurrentPlayer();
			if(board_[1] == 0)
			{
				switch(CurrentPlayer())
				{
				case 0:
					if(board_[27] == 6){
						moves.push_back((blue_pieces - (board_[2] + board_[4] + board_[6] + board_[8])));
					}else{
						for(int amount = 0; amount <= (blue_pieces - (board_[2] + board_[4] + board_[6] + board_[8])); amount++) {
							moves.push_back(amount);
						}
					}
					break;
				case 1:
					if ((board_[kCols+2] + board_[kCols+4]) < red_fighters){
						for (int amount = 0; amount <= (red_fighters-(board_[kCols+2]+board_[kCols+4])); amount++){
							moves.push_back(amount);
						}
					}
					else{
						for (int amount = 5; amount <= 5+(red_sams-(board_[kCols+6]+board_[kCols+8])); amount++){
							moves.push_back(amount);
						}
					}
					break;
				}
			}
			else if(board_[1] == 1){

				if (board_[30] == true){
					moves.push_back(30);
					return moves;
				}

				switch(CurrentPlayer())
				{
					case 0:
					if(board_[27] == 6){
						moves.push_back((board_[13]));
					}else{
						for(int amount = 0; amount <= (board_[13]); amount++) {
							moves.push_back(amount);
						}
					}
					break;
				case 1:
					if (board_[27]==0){
						for (int amount = 0; amount <= (board_[13]); amount++){
							moves.push_back(amount);
						}
					}
					else{
						for (int amount = 5; amount <= 5+(board_[13]); amount++){
							moves.push_back(amount);
						}
					}
					break;
				}


			}
			else if (board_[1] == 2){
				if (board_[30] == true){
					moves.push_back(30);
					return moves;
				}
				switch(CurrentPlayer())
				{
				case 0:
					if(board_[32] == true){
						if(board_[10] == 3){
							moves.push_back(1);
							if (board_[2] > 0){
								moves.push_back(2);//Escort evades for hs/ls while blue has 3 hits
							}
						}else{
							moves.push_back(0);
							moves.push_back(1);
							if (board_[2] > 0 && board_[31] != 2){
								moves.push_back(2); //Escort evades for hs/ls
							}
						}
					}else{
						if(board_[2] > 0) {
							if(board_[kCols + 2] > 0) {
								moves.push_back(kCols+2);
							}
						}
					}
					break;
				case 1:
					if(board_[32] == true){
						if(board_[kCols+10] == 3)
						{
							moves.push_back(1);
						}else{
							moves.push_back(0);
							moves.push_back(1);
						}
					}else{
						if(board_[kCols+2] > 0) {
							if(board_[2] > 0) {
								moves.push_back(2);
							}
							if(board_[4] > 0) {
								moves.push_back(4);
							}
							if(board_[8] > 0) {
								moves.push_back(8);
							}
						}
					}
					break;
				}
			}else if(board_[1] == 3){ 
				//Red can attack if(A High strike>0 || A Low strike>0) and can only attack each low strike once (does not make them evading)
				//Each fighter may be targeted only once in this phase even if still armed
				//Blue can attack if(A Active SAM>0 || A AAA>0)
				//if blue doesn't have any SEADs, blue can't shoot in this phase
				if(board_[30] == true){
				
					moves.push_back(30);
					return moves;
				}

				switch(CurrentPlayer())
				{
					case 0:
						if(board_[32] == true){
							if(board_[10] == 3)
							{
								moves.push_back(1);
								if (board_[6] > 0){
									moves.push_back(2);
								}
							}else{
								if(board_[31] == 8)
								{
									moves.push_back(1);
								}else{
									moves.push_back(0);
									moves.push_back(1);
									if(board_[6] > 0)
									{
										moves.push_back(2);
									}
								}
							}
						}else{
						if(board_[6] > 0) {
							if(board_[kCols] > 0) {
								moves.push_back(kCols);
							}
							if(board_[kCols+6] > 0){
								moves.push_back(kCols+6);
							}
						}
						}
						break;
					case 1:
							if(board_[32] == true){
								moves.push_back(1);
							}else{

								if(board_[4] > 0 && board_[kCols+6] > 0) {
									moves.push_back(4);
								}
								if(board_[8] > 0 && board_[33] < board_[8] && board_[kCols] > 0) {
									moves.push_back(8);
								}
							}
							break;
					}
			}else if(board_[1] == 4){
				//High strike attacks airbase and or any SAM box. Inflicts one hit
				//UAV inflicts 1 hit on any SAM box on wave 1 & 3
				//Low strike attacks any SAM or intercepts or airbase. Inflicts one hit on SAM
				//Low strike turns armed intercepts to evading in airbase and armed fighters in airbase to evading.
				if(board_[30] == true){
				moves.push_back(30);
				return moves;
				}

				switch(CurrentPlayer())
				{
					case 0:
						if(board_[27] == 0)
						{
							if(board_[4] > 0) {
								if(board_[kCols+4] > 0 && (board_[kCols+4] > board_[36])) {
									moves.push_back(kCols+4);
								}else if(board_[kCols+5] > 0 && ((board_[kCols+4]+board_[kCols+5]) > board_[36])){
									moves.push_back(kCols+5);
								}

								if(board_[kCols+6] > 0 && (board_[kCols+6] > board_[37])){
									moves.push_back(kCols+6);
								}else if(board_[kCols+7] > 0 && ((board_[kCols+6]+board_[kCols+7]) > board_[37])){
									moves.push_back(kCols+7);
								}

								if(board_[kCols+8] > 0 && (board_[kCols+8] > board_[38])){
									moves.push_back(kCols+8);
								}else if(board_[kCols+9] > 0 && ((board_[kCols+8]+board_[kCols+9]) > board_[38])){
									moves.push_back(kCols+9);
								}
							}
						}else if(board_[27] == 2){
							if (board_[34] == 1 && (board_[0] == 1 || board_[0] == 3)){
								if (board_[kCols + 6] > 0 && (board_[kCols+6] > board_[37])){
									moves.push_back(kCols+6);
								}else if(board_[kCols + 7] > 0 && ((board_[kCols+6]+board_[kCols+7]) > board_[37])){
									moves.push_back(kCols+7);
								}

								if (board_[kCols+8] > 0 && (board_[kCols+8] > board_[38])){
									moves.push_back(kCols+8);
								}else if(board_[kCols + 9] > 0 && ((board_[kCols+8]+board_[kCols+9]) > board_[38])){
									moves.push_back(kCols+9);
								}
							}
							else if(board_[8] > 0) {
								if(board_[kCols+2] > 0 || board_[kCols+3] > 0) {
								moves.push_back(kCols+2);
								}
								if(board_[kCols+4] > 0 && (board_[kCols+4] > board_[36])){
									moves.push_back(kCols+4);
								}else if(board_[kCols+5] > 0 && ((board_[kCols+4]+board_[kCols+5]) > board_[36])){
									moves.push_back(kCols+5);
								}

								if(board_[kCols+6] > 0 && (board_[kCols+6] > board_[37])){
									moves.push_back(kCols+6);
								}else if(board_[kCols+7] > 0 && ((board_[kCols+6]+board_[kCols+7]) > board_[37])){
									moves.push_back(kCols+7);
								}

								if(board_[kCols+8] > 0 && (board_[kCols+8] > board_[38])){
									moves.push_back(kCols+8);
								}else if(board_[kCols+9] > 0 && ((board_[kCols+8]+board_[kCols+9]) > board_[38])){
									moves.push_back(kCols+9);
								}

							}
						}

						break;
					case 1: //måste fixa airbase inte tar hits utan bara sätter till evading från low strike
					//Ändra också att vi håller koll på hur många ggr vi skjutit en pjäs även om den inte sätts till evading FIXED
						if(board_[32] == true){
								if(board_[31] == (kCols+8) || board_[31] == (kCols+6) || board_[31] == (kCols+9) || board_[31] == (kCols+7))
								{
									moves.push_back(3);
								}else if((board_[31] == (kCols+4) || board_[31] == (kCols+5)) && board_[27] == 3){ 
									moves.push_back(1); //kanske ändra eller ändra airbase
								}else if((board_[31] == (kCols+4) || board_[31] == (kCols+5)) && board_[27] == 1)
								{
									moves.push_back(3);
								}else if((board_[31] == (kCols+2) || board_[31] == (kCols+3)) && board_[27] == 3){
									moves.push_back(6);
								}
							}
						break;
				}
			}

			return moves;
		}

		

		std::string CounterAirState::ActionToString(Player player,
			Action action_id) const {
			return game_->ActionToString(player, action_id);                      //Game_ är game som skapade vårt state
		}

		//Returnerar dem olika sätten att vinna
		bool CounterAirState::HasLine(Player player) const {
			return BoardHasLine(board_, player);
		}

		//konstruktor
		CounterAirState::CounterAirState(std::shared_ptr<const Game> game) : State(game) {
			std::fill(begin(board_), end(board_), 0);
			board_[0] = 1;
			board_[14] = 4;
			board_[30] = true;
			board_[34] = 1;
			board_[35] = 3;
		}
 
		//vi kan lägga till namn på de olika platserna
		std::string CounterAirState::ToString() const {
			std::string str;
			for (int r = 0; r < kRows; ++r) {
				for (int c = 0; c < kCols; ++c) {
					if(c == 0){
						if(r==0){
							absl::StrAppend(&str, "B: ");
						}else{
							absl::StrAppend(&str, "R: ");
						}					
					}
					absl::StrAppend(&str, cell_names[r*kCols + c] + ": " + std::to_string(board_[r*kCols+c]) + " |");
				}
				if (r < (kRows - 1)) {
					absl::StrAppend(&str, "\n");
				}
			}
			absl::StrAppend(&str, "\n");
			absl::StrAppend(&str, "UAV: " + std::to_string(board_[34]) + " |");
			absl::StrAppend(&str,"State Info: ");
			absl::StrAppend(&str,"Turn: " + std::to_string(board_[27]) + " |");
			absl::StrAppend(&str,"BFS: " + std::to_string(board_[28]) + " |");
			absl::StrAppend(&str, "RFS: " + std::to_string(board_[29]) + " |");
			absl::StrAppend(&str, "NP: " + std::to_string(board_[30]) + " |");
			absl::StrAppend(&str, "AS: " + std::to_string(board_[31]) + " |");
			absl::StrAppend(&str, "AT: " + std::to_string(board_[32]) + " |");
			absl::StrAppend(&str, "ATLowStrike: " + std::to_string(board_[33]) + " |");
			absl::StrAppend(&str, "HW: " + std::to_string(board_[35]) + " |");
			absl::StrAppend(&str, "ATBase: " + std::to_string(board_[36]) + " |");
			absl::StrAppend(&str, "ATActive: " + std::to_string(board_[37]) + " |");
			absl::StrAppend(&str, "ATPassive: " + std::to_string(board_[38]) + " |");
			absl::StrAppend(&str, "\n");
			absl::StrAppend(&str, "______________________________________________________________________________________________");
			absl::StrAppend(&str, "\n");
			return str;
		}

		//Den har tar in om spelet är över
		bool CounterAirState::IsTerminal() const {
			return outcome_ != kInvalidPlayer;
		}

		std::vector<double> CounterAirState::Returns() const {
			double point = 0.0;
			double hits1 = ((board_[12]+2)*4+board_[10]);
			double hits2 = (board_[kCols+12]*4+board_[kCols+10]);

			if (HasLine(Player{ 0 })) {
				point = (hits2 - hits1)/hits2;
				return { point, -point };
			}
			else if (HasLine(Player{ 1 })) {
				point = (hits1-hits2)/hits1;
				return { -point, point };
			}
			else {
				return { 0.0, 0.0 };
			}
		}

		std::string CounterAirState::InformationStateString(Player player) const {
			SPIEL_CHECK_GE(player, 0);
			SPIEL_CHECK_LT(player, num_players_);
			return HistoryString();
		}

		std::string CounterAirState::ObservationString(Player player) const {
			SPIEL_CHECK_GE(player, 0);
			SPIEL_CHECK_LT(player, num_players_);
			return ToString();
		}

		void CounterAirState::ObservationTensor(Player player,
			absl::Span<float> values) const {
			SPIEL_CHECK_GE(player, 0);
			SPIEL_CHECK_LT(player, num_players_);
			// Treat `values` as a 2-d tensor.
			std::fill(values.begin(), values.end(), 0.);
			SPIEL_CHECK_EQ(values.size(), kNumCells);
			//Wave, uses 6 places (last index 5)
			values[board_[0]-1] = 1.0f;
			//Phase, uses 5 places (last index 10)
			values[6+board_[1]] = 1.0f;
			//A Escort -> E Low strike
			for (int i = 2; i <= 9; i++){
				//How many of each with one-hot encoding, eg: |0 0 0 1 0 0 0 0 0 0 0| represents 3 out of a maximum of 10 of something and minimum of 0
				values[(i-1)*11 + board_[i]] = 1.0f;
			}
			//Hits (blue)
			values[99+board_[10]] = 1.0f;
			//Maintenance (blue)
			values[104+board_[11]] = 1.0f;
			//Graveyard (blue)
			values[106+board_[12]] = 1.0f;
			//tmpieces
			values[117+board_[13]] = 1.0f;
			//A AAA -> E Passive SAM
			for (int i = 0; i <= 9; i++){
				//How many of each with one-hot encoding, eg: |1 0 0 0 0| represents 0 out of a maximum of 4 of something
				values[128+(5*i) + board_[kCols+i]] = 1.0f;
			}
			//Hits (red)
			values[178+board_[kCols+10]] = 1.0f;
			//Graveyard (red)
			values[183+board_[kCols+12]] = 1.0f;
			//turn
			values[192+board_[27]] = 1.0f;
			//UAV
			values[212+board_[34]] = 1.0f;
			//BFS
			values[214+board_[28]] = 1.0f;
			//RFS
			values[216+board_[29]] = 1.0f;
			//NP
			values[218+board_[30]] = 1.0f;
			//AS
			values[220+board_[31]] = 1.0f;
			//AT
			values[243+board_[32]] = 1.0f;
			//ATLowStrike
			values[245+board_[33]] = 1.0f;
			//HW
			values[256+board_[35]] = 1.0f;
			//ATBase
			values[260+board_[36]] = 1.0f;
			//ATActive
			values[265+board_[37]] = 1.0f;
			//ATPassive
			values[270+board_[38]] = 1.0f;

			/*for (int i = 0; i <= 255; i++){
				std::cout << values[i] <<  " | " << std::endl;
			}*/
		}

		/*//kanske ej för mcts hoppas pray my lord
		void CounterAirState::UndoAction(Player player, Action move) {
			board_[move] = CellState::kEmpty;
			current_player_ = player;
			outcome_ = kInvalidPlayer;
			num_moves_ -= 1;
			history_.pop_back();
			--move_number_;
		}*/

		std::unique_ptr<State> CounterAirState::Clone() const {
			return std::unique_ptr<State>(new CounterAirState(*this));
		}

		std::string CounterAirGame::ActionToString(Player player,
			Action action_id) const {
			return absl::StrCat(action_id);
		}

		CounterAirGame::CounterAirGame(const GameParameters& params)
			: Game(kGameType, params) {}

	}  // namespace counter_air
}  // namespace open_spiel

