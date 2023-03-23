
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
		//Ändrat
		CellState PieceToState(int amount) {
			/*switch (player) {
			//Blue player is player 0
			case 0:
				switch (piece){
					case 0:
					return CellState::kAUAV;
					case 1:
					return CellState::kAFighter;
					case 2:
					return CellState::kEFighter;
				}
			//Red player is player 1
			case 1:
				switch (piece){
					case 0:
					return CellState::kAFighter;
					case 1:
					return CellState::kEFighter;
					case 2:
					return CellState::kASam;
					case 3:
					return CellState::kESam;
					case 4:
					return CellState::kAAAA;
					case 5:
					return CellState::kEAAA;
					default:
					SpielFatalError(absl::StrCat("Invalid player id ", player));
					return CellState::kEmpty;

				}
			default:
				SpielFatalError(absl::StrCat("Invalid player id ", player));
				return CellState::kEmpty;
			}*/
			switch (amount) {
				case 0:
					return CellState::kZero;
				case 1:
					return CellState::kOne;
				case 2:
					return CellState::kTwo;
				case 3:
					return CellState::kThree;
				case 4:
					return CellState::kFour;
				case 5:
					return CellState::kFive;
				case 6:
					return CellState::kSix;
				case 7:
					return CellState::kSeven;
				case 8:
					return CellState::kEight;
				case 9:
					return CellState::kNine;
				case 10:
					return CellState::kTen;
					
				default:
					SpielFatalError(absl::StrCat("Invalid move", amount));
					return CellState::kZero;
			}
		}
		//Ändrat
		std::string StateToString(CellState state) {
			switch (state) {
			case CellState::kZero:
				return "0";
			case CellState::kOne:
				return "1";
			case CellState::kTwo:
				return "2";	
			case CellState::kThree:
				return "3";
			case CellState::kFour:
				return "4";
			case CellState::kFive:
				return "5";
			case CellState::kSix:
				return "6";
			case CellState::kSeven:
				return "7";
			case CellState::kEight:
				return "8";
			case CellState::kNine:
				return "9";
			case CellState::kTen:
				return "10";
			
			default:
				SpielFatalError("Unknown state.");
			}
		}

		int CheckPoints(const std::array<CellState, kNumCells>& board,
			CellState c, int cell, int Points, int direction = 0) {
			int ActiveCol = cell % 7;
			bool LastRow = false;
		}

		//SLUTET
		bool BoardHasLine(const std::array<CellState, kNumCells>& board,
			const Player player) {
			
			if(blue_finished_shooting == true && red_finished_shooting == true){
				return true;
			}

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
						turn = 1;
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
			else if (phase == 2) {
				if(blue_finished_shooting == true && red_finished_shooting == true)
				{
					phase = 3;
					turn = 0;
					return Player{0};
				}

				if(turn%2 == 0) {
					if(blue_finished_shooting == true && red_finished_shooting == false){
						return Player{1};
					}
					if(has_attacked == true){
						return Player{1};
					}else{
						return Player{0};
					}
				}else{
					if(blue_finished_shooting == false && red_finished_shooting == true)
					{
						return Player{0};
					}
					if(has_attacked == true){
						return Player{0};
					}else{
						return Player{1};
					}
			}

			}
		}

		void CounterAirState::TakenHit(int player, int space, int evade){
			if(evade == 1)
			{
				if(player == 0)
				 {
					board_[9] = PieceToState(std::stoi(StateToString(BoardAt(9))) + 1);
				 }else{
					board_[kCols+9] = PieceToState(std::stoi(StateToString(BoardAt(kCols+9))) + 1);
				 }
				 board_[space] = PieceToState(std::stoi(StateToString(BoardAt(space))) - 1);
				 board_[space+1] = PieceToState(std::stoi(StateToString(BoardAt(space+1))) + 1);
			}else{
				if(player == 0)
				 {
					board_[9] = PieceToState(std::stoi(StateToString(BoardAt(9))) + 2);
				 }else{
					board_[kCols+9] = PieceToState(std::stoi(StateToString(BoardAt(kCols+9))) + 2);
				 }
			}
			if(std::stoi(StateToString(BoardAt(kCols+9))) == 4)
			{
				board_[space] = PieceToState(std::stoi(StateToString(BoardAt(space))) - 1);
				if(player == 0)
				 {
					board_[9] = PieceToState(0);
					board_[11] = PieceToState(std::stoi(StateToString(BoardAt(11))) + 1);
				 }else{
					board_[kCols+9] = PieceToState(0);
					board_[kCols+11] = PieceToState(std::stoi(StateToString(BoardAt(kCols+11))) + 1);
				 }
			}
		}

		void CounterAirState::DoApplyAction(Action move) {
			//First phase of the game (put your pieces on the board)
			if(phase == 0) {
				board_[0] = PieceToState(1);
				board_[12] = PieceToState(4);
				board_[14] = PieceToState(4);
				switch(CurrentPlayer()) {
					case 0:
							board_[turn] = PieceToState(move);
							blue_pieces = blue_pieces - move;
						break;
					case 1:
							//move = move + std::stoi(StateToString(BoardAt(kCols + 3 + turn)));
							board_[kCols + 3 + turn] = PieceToState(move);
							red_sams = red_sams - move;
						break;
				}
				turn = turn + 2;
			}
			//second phase of game, escort attacks intercept, intercept attacks escort, high strike and low strike
			else if (phase == 2){
				switch(CurrentPlayer()) {
					case 0:
						//Blue attacks with Armed escorts, can only attack intercepts
						if (attacked == false){
							
							board_[2] = PieceToState(std::stoi(StateToString(BoardAt(2))) - 1);
							board_[3] = PieceToState(std::stoi(StateToString(BoardAt(3))) + 1);
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
							board_[kCols + 2] = PieceToState(std::stoi(StateToString(BoardAt(kCols+2))) - 1);
							board_[kCols + 3] = PieceToState(std::stoi(StateToString(BoardAt(kCols+3))) + 1);
							attacked = true;
							turn++;
							switch(move){
								case 1:					
									attacked_space = 1; //Escort
									break;
								case 2:
									attacked_space = 3; //High Strike
									break;
								case 3:
									attacked_space = 7; //Low Strike
									break;
							}							
						}else{
								TakenHit(1,kCols+2, move);
								attacked = false;
						}
						break;
				}
				//If red doesn't have any armed fighters in intercept, neither player can shoot
				if(std::stoi(StateToString(BoardAt(kCols+2))) == 0)
				{
					red_finished_shooting = true;
					blue_finished_shooting = true;
				}
				//If blue doesn't have any armed escorts, blue can no longer shoot
				if(std::stoi(StateToString(BoardAt(1))) == 0)
				{
					blue_finished_shooting = true;
				}
				//If there are no armed: escorts, high strikes or low strikes. Neither player can shoot
				if((std::stoi(StateToString(BoardAt(1))) == 0) && (std::stoi(StateToString(BoardAt(3))) == 0) && (std::stoi(StateToString(BoardAt(7))) == 0)){
					red_finished_shooting = true;
					blue_finished_shooting = true;
				}
			}

			if (HasLine(current_player_)) {   //Om current player har vunnit
				outcome_ = current_player_;
			}
		}

		//Ändrat 0
		std::vector<Action> CounterAirState::LegalActions() const {    //Klassen �r counterAirState och metod �r LegalActions
			if (IsTerminal()) return {};    //Om spelet har vunnits return annars hoppa �ver
			// Can move in any empty cell.
			std::vector<Action> moves;    //Skapar en vector med alla legal moves
			if(phase == 0)
			{
				switch(CurrentPlayer())
				{
				case 0:
					if(turn == 7){
						moves.push_back(blue_pieces);
					}else{
						for(int amount = 0; amount <= blue_pieces; amount++) {
							moves.push_back(amount);
						}
					}
					break;
				case 1:
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
							if(std::stoi(StateToString(BoardAt(9))) == 3)
							{
								moves.push_back(1);
							}else{
								moves.push_back(0);
								moves.push_back(1);
							}
						}else{
						if(std::stoi(StateToString(BoardAt(1))) != 0) {
							if(std::stoi(StateToString(BoardAt(kCols + 2))) != 0) {
								moves.push_back(1);
							}else{
								red_finished_shooting = true;
							}
						}else{
							blue_finished_shooting = true;
						}
						}
					break;
				case 1:
						if(attacked == true){
							if(std::stoi(StateToString(BoardAt(kCols+9))) == 3)
							{
								moves.push_back(1);
							}else{
								moves.push_back(0);
								moves.push_back(1);
							}
						}else{
						if(std::stoi(StateToString(BoardAt(kCols + 2))) != 0) {
							if(std::stoi(StateToString(BoardAt(1))) != 0) {
								moves.push_back(1);
							}else{
								blue_finished_shooting = true;
							}
							if(std::stoi(StateToString(BoardAt(3))) != 0) {
								moves.push_back(2);
							}
							if(std::stoi(StateToString(BoardAt(7))) != 0) {
								moves.push_back(3);
							}
							if(std::size(moves) == 0){
								red_finished_shooting = true; blue_finished_shooting = true;
							}
						}else{
							red_finished_shooting = true;
						}
						}
						break;
					}
				}
				/*if(red_finished_shooting == true || blue_finished_shooting == true){
					return LegalActions();
				}*/


			return moves;
		}

		std::string CounterAirState::ActionToString(Player player,
			Action action_id) const {
			return game_->ActionToString(player, action_id);                      //Game_ �r game som skapade v�rt state
		}

		//Returnerar dem olika s�tten att vinna
		bool CounterAirState::HasLine(Player player) const {
			return BoardHasLine(board_, player);
		}

		//konstruktor
		CounterAirState::CounterAirState(std::shared_ptr<const Game> game) : State(game) {
			std::fill(begin(board_), end(board_), CellState::kZero);

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
					absl::StrAppend(&str, cell_names[r*kCols + c] + ": " + StateToString(BoardAt(r,c)) + " |");
				}
				if (r < (kRows - 1)) {
					absl::StrAppend(&str, "\n");
				}
			}
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


