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


		int CheckPoints(const std::array<int, kNumCells>& board,
			int c, int cell, int Points, int direction = 0) {
			int ActiveCol = cell % 7;
			bool LastRow = false;
		}

		//SLUTET
		bool BoardHasLine(const std::array<int, kNumCells>& board,
			const Player player) {
			
			if(blue_finished_shooting == true && red_finished_shooting == true && attacked == false && phase == 3){
				return true;
			}/*
			if(phase == 2){
				return true;
			}*/

			return false;
		}

		Player CounterAirState::CurrentPlayer() const {
			if(phase == 0)
			{
				if(blue_pieces != 0)
				{
					return Player{0};
				}
				else
				{
					if(was_blue == true){
						turn = -1;
					}
					if(red_sams != 0)
					{
						was_blue = false;
						return Player {1};
					}
					else
					{
						turn = 0;
						phase = 2;
						return Player{0};
					}
				}
			}
			else if (phase == 2 || phase == 3) {
				if(blue_finished_shooting == true && red_finished_shooting == true && attacked != true)
				{
					phase++;
					turn = 0;
					blue_finished_shooting = false;
					red_finished_shooting = false;
					return Player{0};
				}

				if(turn%2 == 0) {
					if(blue_finished_shooting == true && red_finished_shooting == false && attacked != true){
						return Player{1};
					}
					return Player{0};
				}else{
					if(blue_finished_shooting == false && red_finished_shooting == true && attacked != true)
					{
						return Player{0};
					}
					return Player{1};
			}

			}
		}

		void CounterAirState::TakenHit(int player, int space, int evade){
			if (space == kCols){
				board_[space]--;
				board_[space+1]++;
			}else if(phase == 3 && space == 7){
				board_[player*kCols + 9]++;
			}else{
				if(evade == 1)
				{
					board_[player*kCols + 9]++;
					board_[space]--;
					board_[space+1]++;
				}else if(evade == 0){
					board_[player*kCols + 9]+=2;
				}else{
					board_[space+evade]--;
					board_[space+evade+1]++;
				}
			}

			if(board_[player*kCols+9] == 4)
				{
					if(evade == 0)
					{
						board_[space]--;
					}else{
						board_[space+1]--;
					}
					board_[player*kCols+9] = 0;
					board_[player * kCols + 11]++;
				}
		}


		void CounterAirState::DoApplyAction(Action move) {
			if(initilazation == true){
				board_[0] = 1;
				board_[12] = 4;
				board_[14] = 4;
				if(count == 3){
					initilazation = false;
				}
			}
			//First phase of the game (put your pieces on the board)
			if(phase == 0) {
				switch(CurrentPlayer()) {
					case 0:
							board_[turn] = move;
								blue_pieces = blue_pieces - move;

						break;
					case 1:
							//move = move + std::stoi(StateToString(BoardAt(kCols + 3 + turn)));
							board_[kCols + 3 + turn] = move;
								red_sams = red_sams - move;

							
						break;
				}
				
			}
			//second phase of game, escort attacks intercept, intercept attacks escort, high strike and low strike
			else if (phase == 2){
				switch(CurrentPlayer()) {
					case 0:
						//Blue attacks with Armed escorts, can only attack intercepts
						if (attacked == false){
							
							board_[1]--;
							board_[2]++;
							attacked = true;
							turn++;
						}else{
							//Blue gets attacked
							TakenHit(0, attacked_space, move);
							attacked = false;
						}
						break;

					case 1:
						if(attacked == false) {
							board_[kCols + 2]--;
							board_[kCols + 3]++;
							attacked = true;
							turn++;
							attacked_space = move;		
						}else{
								TakenHit(1,kCols+2, move);
								attacked = false;
						}
						break;

				}
				
				//If red doesn't have any armed fighters in intercept, neither player can shoot
				if(board_[kCols+2] == 0)
				{
					red_finished_shooting = true;
					blue_finished_shooting = true;
				}
				//If blue doesn't have any armed escorts, blue can no longer shoot
				if(board_[1] == 0)
				{
					blue_finished_shooting = true;
				}
				//If there are no armed: escorts, high strikes or low strikes. Neither player can shoot
				if((board_[1] == 0) && (board_[3] == 0) && (board_[7] == 0)){
					red_finished_shooting = true;
					blue_finished_shooting = true;
				}
			}else if(phase == 3){
				switch(CurrentPlayer()) {
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
						}
						break;

					case 1:
						if(attacked == false) {
							if(move == 3){
								board_[kCols + 4]--;
								board_[kCols + 5]++;
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
						}
						break;

				}
				
				//If red doesn't have any armed fighters in intercept, neither player can shoot
				if(board_[kCols] == 0 && board_[kCols+4] == 0)
				{
					red_finished_shooting = true;
					blue_finished_shooting = true;
				}
				//If blue doesn't have any armed escorts, blue can no longer shoot
				if(board_[5] == 0)
				{
					blue_finished_shooting = true;
				}
				//If there are no armed: escorts, high strikes or low strikes. Neither player can shoot
				if((board_[kCols] == 0 || board_[7] == 0) && (board_[kCols+4] == 0 || board_[3]==0)){
					red_finished_shooting = true;
				}
			}

			count++;
//SENASTE ÄNDRINGEN
			//HasLine(CurrentPlayer());
			cout
		}

		//Ändrat 0
		std::vector<Action> CounterAirState::LegalActions() const {    //Klassen är counterAirState och metod är LegalActions
			if (IsTerminal()) return {};    //Om spelet har vunnits return annars hoppa över
			// Can move in any empty cell.
			std::vector<Action> moves;    //Skapar en vector med alla legal moves
			if(phase == 0)
			{
				switch(CurrentPlayer())
				{
				case 0:
					turn = turn + 2;
					if(turn == 7){
						moves.push_back(blue_pieces);
					}else{
						for(int amount = 0; amount <= blue_pieces; amount++) {
							moves.push_back(amount);
						}
					}
					break;
				case 1:
					turn = turn + 2;
					if(turn == 3){
						moves.push_back(red_sams);
					}else{
						for(int amount = 0; amount <= red_sams; amount++) {
							moves.push_back(amount);
						}
					}
					break;
				}
			}
			else if (phase == 2){
				switch(CurrentPlayer())
				{
				case 0:
						if(attacked == true){
							if(board_[9] == 3)
							{
								moves.push_back(1);
							}else{
								moves.push_back(0);
								moves.push_back(1);
							}
						}else{
						if(board_[1] > 0) {
							if(board_[kCols + 2] > 0) {
								moves.push_back(kCols+2);
							}else{
								red_finished_shooting = true;
							}
						}
						}
					break;
				case 1:
						if(attacked == true){
							if(board_[kCols+9] == 3)
							{
								moves.push_back(1);
							}else{
								moves.push_back(0);
								moves.push_back(1);
							}
						}else{
							if(board_[kCols+2] > 0) {
								if(board_[1] > 0) {
									moves.push_back(1);
								}else{
									blue_finished_shooting = true;
								}
								if(board_[3] > 0) {
									moves.push_back(3);
								}
								if(board_[7] > 0) {
									moves.push_back(7);
								}
							}
						}
						break;
				}
			}else if(phase == 3){
				//Red can attack if(A High strike>0 || A Low strike>0) and can only attack each low strike once (does not make them evading)
				//Each fighter may be targeted only once in this phase even if still armed
				//Blue can attack if(A Active SAM>0 || A AAA>0)
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
								if(board_[kCols+4] > 0){
									moves.push_back(kCols+4);
								}
							}
							}
						break;
					case 1:
							if(attacked == true){
								if(board_[kCols+9] == 3)
								{
									moves.push_back(1);
								}else{
									moves.push_back(0);
									moves.push_back(1);
								}
							}else{

								if(board_[3] > 0 && board_[kCols+4] > 0) {
									moves.push_back(3);
								}
								if(board_[7] > 0 && attacked_low_strike < board_[7] && board_[kCols] > 0) {
									moves.push_back(7);
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

		}
 
		//vi kan lägga till namn på de olika platserna
		std::string CounterAirState::ToString() const {
			std::string str;
			absl::StrAppend(&str, "phase: " + std::to_string(phase) + "\n");
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
			TensorView<2> view(values, { kCellStates, kNumCells }, true);
			for (int cell = 0; cell < kNumCells; ++cell) {
				view[{static_cast<int>(board_[cell]), cell}] = 1.0;
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
