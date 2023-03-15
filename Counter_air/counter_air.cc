
#include "open_spiel/games/counter_air.h"

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#define 

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
		CellState PieceToState(Player player, int piece) {
			switch (player) {
			//Blue player is player 0
			case 0:
				switch (piece){
					case 0:
					return CellState::kUAV
					case 1:
					return CellState::kAFighter
					case 2:
					return CellState::kEFighter
				}
			//Red player is player 1
			case 1:
				switch (piece){
					case 0:
					return CellState::kAFighter
					case 1:
					return CellState::kEFighter
					case 2:
					return CellState::kASam
					case 3:
					return CellState::kESam
					case 4:
					return CellState::kAAAA
					case 5:
					return CellState::kEAAA
					default:
					SpielFatalError(absl::StrCat("Invalid player id ", player));
					return CellState::kEmpty;

				}
			default:
				SpielFatalError(absl::StrCat("Invalid player id ", player));
				return CellState::kEmpty;
			}
		}
		//Ändrat
		std::string StateToString(CellState state) {
			switch (state) {
			case CellState::kEmpty:
				return ".";
			case CellState::kAFigher:
				return "1";
			case CellState::kEFigher:
				return "0";	
			case CellState::kAUAV:
				return "1";
			case CellState::kEUAV:
				return "0";
			case CellState::kASam:
				return "1";
			case CellState::kESam:
				return "0";
			case CellState::kAAAA:
				return "1";
			case CellState::kEAAA:
				return "0";
			
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

		Player CounterAirState::CurrentPlayer() const {
			if(phase == 0)
			{
				if(blue_pieces != 0)
				{
					return Player{0};
				}
				else
				{
					if(red_sams != 0)
					{
						return Player {1};
					}
					else
					{
						phase = 2;
						return Player{0};
					}
				}
			}

		}

		void CounterAirState::DoApplyAction(Action move) {
			SPIEL_CHECK_EQ(board_[move], CellState::kEmpty); //Kollar om platsen �r tom
			board_[move] = PlayerToState(CurrentPlayer());  //S�tter ut spelarens symbol p� vald plats p� br�det

			if (HasLine(current_player_)) {   //Om current player har vunnit
				outcome_ = current_player_;
			}
			current_player_ = 1 - current_player_; //Uppdaterar vilken spelare som k�r
			num_moves_ += 1;  //h�ller koll p� hur m�nga moves som har k�rts
		}

		//Ändrar
		std::vector<Action> CounterAirState::LegalActions() const {    //Klassen �r counterAirState och metod �r LegalActions
			if (IsTerminal()) return {};    //Om spelet har vunnits return annars hoppa �ver
			// Can move in any empty cell.
			std::vector<Action> moves;    //Skapar en array med action antal element
			
			if(phase = 0)
			{
				switch(CurrentPlayer())
				{
				case 0:
					for(int amount = 0; amount <= blue_pieces; amount++) {
						moves.push_back(amount);
					}
				case 1:
					for(int amount = 0; amount <= red_sams amount++) {
							moves.push_back(amount);
						}
				}
			}

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

