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
			
			if(board[1] == 3){
				return true;
			}/*
			if(phase == 2){
				return true;
			}*/

			return false;
		}

		Player CounterAirState::CurrentPlayer() const{
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
				
				if((board_[13] + board_[27])%2 == 0) { 
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

			}
			return IsTerminal() ? kTerminalPlayerId : Player{0};
		}

		void CounterAirState::TakenHit(int player, int space, int evade){
			if (space == kCols){
				board_[space]--;
				board_[space+1]++;
			}else if(board_[1] == 3 && space == 7 || evade == 3){
				board_[player*kCols + 10]++;
			}else{
				if(evade == 1)
				{
					board_[player*kCols + 10]++;
					board_[space]--;
					board_[space+1]++;
				}else if(evade == 0){
					board_[player*kCols + 10]+=2;
				}else{
					if (board_[1] == 2 && evade == 5){
						board_[2]--;
						board_[3]++;
					}
					if(evade == 5 && board_[1] == 3){
						board_[5]--;
						board_[6]++;
					}
					board_[player*kCols + 10]++;
				}
			}

			if(board_[player*kCols+ 10] == 4)
				{
					if(board_[1] == 3 && space == 7){
						board_[space]--;
						attacked_low_strike--;
					}else if(evade == 0)
					{
						board_[space]--;
					}else if(evade == 5 && board_[1] == 2)
					{
						board_[2]--;
					}else if(evade == 5 && board_[1] == 3){
						board_[6]--;
					}else{
						board_[space+1]--;
					}
					board_[player*kCols+10] = 0;
					board_[player * kCols + 12]++;
				}
		}


		void CounterAirState::DoApplyAction(Action move) {
			current_player_ = CurrentPlayer();
			if(move != 100){
			
			if(board_[1] == 0) { //First phase of the game (put your pieces on the board)
				switch(current_player_) {
					case 0:
							if(board_[13] > 6){
								board_[13] = 0;	
							}
							board_[13]+=2;
							if(board_[13] < 9)
							{
								board_[board_[13]] = move;	
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
			//second phase of game, escort attacks intercept, intercept attacks escort, high strike and low strike
			else if (board_[1] == 2){
				switch(current_player_) {
					case 0:
						//Blue attacks with Armed escorts, can only attack intercepts
						if (board_[32] == false){
							board_[2]--;
							board_[3]++;
							board_[32] = true;
							board_[13]++;
						}else{
							//Blue gets board_[32]
							TakenHit(0, board_[31], move);
							board_[32] = false;
							if(board_[2] == 0){
								board_[28] = true;
							}
							if(board_[28] == true){
								board_[13]++;
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
			}/*else if(board_[1] == 3){
				switch(current_player_) {
					case 0:
						//Blue attacks with Armed escorts, can only attack intercepts
						if (attacked == false){	
							board_[5]--;
							board_[6]++;
							attacked = true;
							turn++;
							attacked_space = move;	
						}else{
							//Blue gets attacked
							TakenHit(0, attacked_space, move);
							attacked = false;
							if(board_[5] == 0){
								board_[28] = true;
							}
							if(board_[28] == true){
								turn++;
							}
						}
						break;

					case 1:
						if(attacked == false) {
							if(move == 3){
								board_[kCols + 6]--;
								board_[kCols + 7]++;
							}else{
								attacked_low_strike++;
								board_[kCols]--;
								board_[kCols + 1]++;
							}
							attacked = true;
							turn++;
							attacked_space = move;				
						}else{
								TakenHit(1,attacked_space, move);
								attacked = false;
								if((board_[kCols] < 1 || board_[7] <= attacked_low_strike) && (board_[kCols+6] < 1 || board_[3] < 1)){ //special fall
								board_[29] = true;
								}
								if(board_[29] == true){
								turn++;
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
				if(board_[5] == 0)
				{
					board_[28] = true;
				}
				//If there are no armed: escorts, high strikes or low strikes. Neither player can shoot
				if((board_[kCols] < 1 || board_[7] <= attacked_low_strike) && (board_[kCols+6] < 1 || board_[3] < 1)){
					board_[29] = true;
				}
			}else if(board_[1] == 4){
				switch(current_player_) {
					case 0:
						//Blue attacks with Armed escorts, can only attack intercepts
						if (attacked == false){	
							if(turn==0){
								board_[3]--;
								board_[4]++;
							}else{
								board_[7]--;
								board_[8]++;
							}
							attacked = true;
							turn++;
							attacked_space = move;	
						}
						break;

					case 1:
							TakenHit(1,attacked_space, move);
							attacked = false;
							turn--;
						break;

				}

				if((board_[3] < 1 || (board_[kCols+4] < 1 && board_[kCols+6] < 1 && board_[kCols+8] < 1)) && turn == 0){
					turn+=2;
				}
				
				//If red doesn't have any armed fighters in intercept, neither player can shoot
				if(turn==2 && (board_[7] < 1 || (board_[kCols+2] < 1 && board_[kCols+4] < 1 && board_[kCols+6] < 1 && board_[kCols+8] < 1))){
					board_[28] = true;
				}
			}*/
		
			}else{
				board_[30] = false;
				board_[13] = 0;
				board_[27] = 0;
				if(board_[1] == 2){
					if(board_[2] < 1){
						board_[28] = true;
						board_[13]++;
					}
					if(board_[2] < 1 && board_[4] < 1 && board_[8] < 1)
					{
						board_[29] = true;
					}

					if(board_[kCols+2] < 1){
						board_[29] = true;
						board_[28] = true;
					}
				}
			}

			if(board_[1] == 2){
				if(board_[28] == true && board_[29] == true && board_[32] != true)
				{
					if(board_[1] == 4){
						board_[1] = 1;
					}else{
						board_[1]++;
					}
					board_[13] = 0;
					board_[27] = 0;
					board_[28] = false;
					board_[29] = false;
				}
			}
			if(HasLine(CurrentPlayer())){
				outcome_ = current_player_;
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
					if(board_[13] == 6){
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
			else if (board_[1] == 2){
				if (board_[30] == true){
					moves.push_back(100);
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
			}/*else if(board_[1] == 3){ 
				//Red can attack if(A High strike>0 || A Low strike>0) and can only attack each low strike once (does not make them evading)
				//Each fighter may be targeted only once in this phase even if still armed
				//Blue can attack if(A Active SAM>0 || A AAA>0)
				//if blue doesn't have any SEADs, blue can't shoot in this phase
				if(new_phase == true){
				if (board_[5] < 1){
					board_[28] = true;
					//current_player_=1;
					turn++;
				}
				if ((board_[kCols] < 1 || board_[7] == attacked_low_strike) && (board_[kCols+6] < 1 || board_[3] < 1)){
					board_[29] = true;
				}

				if (board_[28] == true && board_[29] == true){
					moves.push_back(100);
					return moves;
				}
				new_phase = false;
				}

				switch(CurrentPlayer())
				{
					case 0:
							if(attacked == true){
								if(board_[9] == 3)
								{
									moves.push_back(1);
								}else{
									if(attacked_space == 7)
									{
										moves.push_back(1);
									}else{
										moves.push_back(0);
										moves.push_back(1);
										if(board_[5] > 0)
										{
											moves.push_back(2);
										}
									}
								}
							}else{
							if(board_[5] > 0) {
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
							if(attacked == true){
								if(board_[kCols+10] == 3)
								{
									moves.push_back(1);
								}else{
									moves.push_back(0);
									moves.push_back(1);
								}
							}else{

								if(board_[3] > 0 && board_[kCols+6] > 0) {
									moves.push_back(3);
								}
								if(board_[7] > 0 && attacked_low_strike < board_[7] && board_[kCols] > 0) {
									moves.push_back(7);
								}
							}
							break;
					}
			}else if(board_[1] == 4){
				//High strike attacks airbase and or any SAM box. Inflicts one hit
				//UAV inflicts 1 hit on any SAM box on wave 1 & 3
				//Low strike attacks any SAM or intercepts or airbase. Inflicts one hit on SAM
				//Low strike turns armed intercepts to evading in airbase and armed fighters in airbase to evading.
				if(new_phase == true){
					board_[29] = true;
				if((board_[3] < 1 || (board_[kCols+4] < 1 && board_[kCols+6] < 1 && board_[kCols+8] < 1)) && attacked == false){
					turn+=2;
				}
				
				//If red doesn't have any armed fighters in intercept, neither player can shoot
				if(turn==2 && (board_[7] < 1 || (board_[kCols+2] < 1 && board_[kCols+4] < 1 && board_[kCols+6] < 1 && board_[kCols+8] < 1)) && attacked == false){
					board_[28] = true;
				}
				
				//kolla hur vi ska göra ifall bara low får skjuta i början, måste ändra dem över


				if (board_[28] == true){moves.push_back(1);
				}
				new_phase = false;
				}

				switch(CurrentPlayer())
				{
					case 0:
						if(turn == 0)
						{
							if(board_[3] > 0) {
								if(board_[kCols+4] > 0) {
									moves.push_back(kCols+4);
								}
								if(board_[kCols+6] > 0){
									moves.push_back(kCols+6);
								}
								if(board_[kCols+8] > 0){
									moves.push_back(kCols+8);
								}
							}
						}else if(turn == 2){
							if(board_[7] > 0) {
								if(board_[kCols+2] > 0) {
								moves.push_back(kCols+2);
								}
								if(board_[kCols+4] > 0){
									moves.push_back(kCols+4);
								}
								if(board_[kCols+6] > 0){
									moves.push_back(kCols+6);
								}
								if(board_[kCols+8] > 0){
									moves.push_back(kCols+8);
								}
							}
						}

						break;
					case 1: //måste fixa airbase inte tar hits utan bara sätter till evading från low strike
					//Ändra också att vi håller koll på hur många ggr vi skjutit en pjäs även om den inte sätts till evading
						if(attacked == true){
								if(attacked_space == (kCols+8) || attacked_space == (kCols+6))
								{
									moves.push_back(3);
								}else if(attacked_space == (kCols+4) && turn == 3){ 
									moves.push_back(1); //kanske ändra eller ändra airbase
								}else if(attacked_space == (kCols+4) && turn == 1)
								{
									moves.push_back(3);
								}else if(attacked_space == (kCols+2) && turn == 3){
									moves.push_back(6);
								}
							}
						break;
				}
			}
*/
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
			board_[30]=true;
		}
 
		//vi kan lägga till namn på de olika platserna
		std::string CounterAirState::ToString() const {
			std::string str;
			absl::StrAppend(&str, "turn: " + std::to_string(board_[13]) + "\n");
			absl::StrAppend(&str, "phase: " + std::to_string(board_[1]) + "\n");
			absl::StrAppend(&str, "BFS: " + std::to_string(board_[28]) + "\n");
			absl::StrAppend(&str, "RFS: " + std::to_string(board_[29]) + "\n");
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
			absl::StrAppend(&str,"State Info: ");
			absl::StrAppend(&str,"BFS: " + std::to_string(board_[28]) + " |");
			absl::StrAppend(&str, "RFS: " + std::to_string(board_[29]) + " |");
			absl::StrAppend(&str, "NP: " + std::to_string(board_[30]) + " |");
			absl::StrAppend(&str, "AS: " + std::to_string(board_[31]) + " |");
			absl::StrAppend(&str, "AT: " + std::to_string(board_[32]) + " |");
			absl::StrAppend(&str, "\n");
			return str;
		}

		//Den har tar in om spelet är över
		bool CounterAirState::IsTerminal() const {
			return outcome_ != kInvalidPlayer;
		}

		std::vector<double> CounterAirState::Returns() const {
			if (HasLine(Player{ 0 })) {
				return { 1.0, -1.0 };
			}
			else if (HasLine(Player{ 1 })) {
				return { -1.0, 1.0 };
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
			TensorView<1> view(values, { kNumCells }, true);
			for (int cell = 0; cell < kNumCells; ++cell) {
				view[{static_cast<int>(board_[cell])}] = 1.0;
			}
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
