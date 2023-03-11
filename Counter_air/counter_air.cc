
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
				GameType::Utility::kZeroSum,                //osäkra
				GameType::RewardModel::kTerminal,           //Väljer när rewards ska ges
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

		CellState PlayerToState(Player player) {
			switch (player) {
			case 0:
				return CellState::kBlue;
			case 1:
				return CellState::kRed;
			default:
				SpielFatalError(absl::StrCat("Invalid player id ", player));
				return CellState::kEmpty;
			}
		}

		std::string StateToString(CellState state) {
			switch (state) {
			case CellState::kEmpty:
				return ".";
			case CellState::kRed:
				return "r";
			case CellState::kBlue:
				return "b";
			default:
				SpielFatalError("Unknown state.");
			}
		}

		int CheckPoints(const std::array<CellState, kNumCells>& board,
			CellState c, int cell, int Points, int direction = 0) {
			int ActiveCol = cell % 7;
			bool LastRow = false;
			if (cell > 34)
			{
				LastRow = true;
			}
			if (Points < 3)
			{
				if (cell < 42) {
					if (direction == 0)
					{
						switch (ActiveCol)
						{
						case 0:
							if (board[cell + 1] == c) { if (CheckPoints(board, c, cell + 1, Points + 1, 1)) { return true; } }
							if (LastRow == false)
							{
								if (board[cell + 7] == c) { if (CheckPoints(board, c, cell + 7, Points + 1, 7)) { return true; } }
								if (board[cell + 8] == c) { if (CheckPoints(board, c, cell + 8, Points + 1, 8)) { return true; } }
							}
							break;
						case 6:
							if (LastRow == false)
							{
								if (board[cell + 7] == c)
								{
									CheckPoints(board, c, cell + 7, Points + 1, 7);
								}
								if (board[cell + 6] == c)
								{
									CheckPoints(board, c, cell + 6, Points + 1, 6);
								}
							}
							break;

						default:
							if (LastRow == false)
							{
								if (board[cell + 7] == c)
								{
									if ((CheckPoints(board, c, cell + 7, Points + 1, 7))) { return true; }
								}
								if (board[cell + 6] == c)
								{
									if (CheckPoints(board, c, cell + 6, Points + 1, 6)) { return true; }
								}
								if (board[cell + 8] == c)
								{
									if (CheckPoints(board, c, cell + 8, Points + 1, 8)) { return true; }
								}
							}
							if (board[cell + 1] == c)
							{
								if (CheckPoints(board, c, cell + 1, Points + 1, 1)) { return true; }
							}

						}
					}
					else { //när vi har diagonalt vid en kant blir det knas
						if (LastRow == false)
						{
							switch (ActiveCol)
							{
							case 0:
								if (direction != 7)
								{
									if (board[cell + direction] == c)
									{
										if (CheckPoints(board, c, cell + direction, Points + 1, direction)) { return true; }
									}
								}
								break;
							case 6:
								if (direction != 1 && direction != 8)
								{
									if (board[cell + direction] == c)
									{
										if (CheckPoints(board, c, cell + direction, Points + 1, direction)) { return true; }
									}
								}
								break;
							default:
								if (board[cell + direction] == c)
								{
									if (CheckPoints(board, c, cell + direction, Points + 1, direction)) { return true; }
								}
							
							}
						}
						else if(direction == 1 && ActiveCol != 6){
							if (board[cell + direction] == c) {
								if (CheckPoints(board, c, cell + direction, Points + 1, direction)) { return true; }
							}
						}
					}
				}
			}
			else {
				return true;
			}
			return false;
		}

		//SLUTET
		bool BoardHasLine(const std::array<CellState, kNumCells>& board,
			const Player player) {
			CellState c = PlayerToState(player);
			bool HasWon = false;
			int Points = 0;

			for (int cell = 0; cell < kNumCells; ++cell)
			{

				if (board[cell] == c)
				{
					if (CheckPoints(board, c, cell, Points))
					{
						HasWon = true;
					}
				}

			}

			return HasWon;
		}

		void CounterAirState::DoApplyAction(Action move) {
			SPIEL_CHECK_EQ(board_[move], CellState::kEmpty); //Kollar om platsen är tom
			board_[move] = PlayerToState(CurrentPlayer());  //Sätter ut spelarens symbol på vald plats på brädet

			if (HasLine(current_player_)) {   //Om current player har vunnit
				outcome_ = current_player_;
			}
			current_player_ = 1 - current_player_; //Uppdaterar vilken spelare som kör
			num_moves_ += 1;  //håller koll på hur många moves som har körts
		}

		std::vector<Action> CounterAirState::LegalActions() const {    //Klassen är counterAirState och metod är LegalActions
			if (IsTerminal()) return {};    //Om spelet har vunnits return annars hoppa över
			// Can move in any empty cell.
			std::vector<Action> moves;    //Skapar en array med action antal element
			for (int cell = 0; cell < kNumCells; ++cell) {
				if (board_[cell] == CellState::kEmpty) {
					if (cell >= 35)
					{
						moves.push_back(cell);
					}
					else if (board_[cell + 7] != CellState::kEmpty)
					{
						moves.push_back(cell);
					}
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

		bool CounterAirState::IsFull() const { return num_moves_ == kNumCells; }

		CounterAirState::CounterAirState(std::shared_ptr<const Game> game) : State(game) {
			std::fill(begin(board_), end(board_), CellState::kEmpty);
		}

		std::string CounterAirState::ToString() const {
			std::string str;
			for (int r = 0; r < kNumRows; ++r) {
				for (int c = 0; c < kNumCols; ++c) {
					absl::StrAppend(&str, StateToString(BoardAt(r, c)));
				}
				if (r < (kNumRows - 1)) {
					absl::StrAppend(&str, "\n");
				}
			}
			return str;
		}

		bool CounterAirState::IsTerminal() const {
			return outcome_ != kInvalidPlayer || IsFull();
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

		void CounterAirState::UndoAction(Player player, Action move) {
			board_[move] = CellState::kEmpty;
			current_player_ = player;
			outcome_ = kInvalidPlayer;
			num_moves_ -= 1;
			history_.pop_back();
			--move_number_;
		}

		std::unique_ptr<State> CounterAirState::Clone() const {
			return std::unique_ptr<State>(new CounterAirState(*this));
		}

		std::string CounterAirGame::ActionToString(Player player,
			Action action_id) const {
			return absl::StrCat(StateToString(PlayerToState(player)), "(",
				action_id / kNumCols, ",", action_id % kNumCols, ")");
		}

		CounterAirGame::CounterAirGame(const GameParameters& params)
			: Game(kGameType, params) {}

	}  // namespace counter_air
}  // namespace open_spiel

