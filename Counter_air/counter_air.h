#pragma once
// Copyright 2019 DeepMind Technologies Limited
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef OPEN_SPIEL_GAMES_COUNTER_AIR_H_
#define OPEN_SPIEL_GAMES_COUNTER_AIR_H_

#include <array>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "open_spiel/spiel.h"

// Simple game of Noughts and Crosses:
// https://en.wikipedia.org/wiki/Tic-tac-toe
//
// Parameters: none

namespace open_spiel {
    namespace counter_air {

        // Constants.
        inline constexpr int kNumPlayers = 2;
        inline constexpr int kRows = 2;  //ÄNDRAT
        inline constexpr int kCols = 12;  //ÄNDRAT
        inline constexpr int kNumCells = kRows * kCols;
        inline constexpr int kCellStates = 1 + kNumPlayers;  // empty, 'x', and 'o'. ändra sen

        //phase 0
        int num_moves_ = 0;
        int blue_pieces = 10; //Hur många pieces blå har kvar att lägga
        int red_fighters = 4; //Hur många fighters röd har kvar att lägga
        int red_sams = 4;   //Hur många sams röd har kvar att lägga
        int phase = 0;  //Vilken phase vi är på
        int turn = -1;   //Vems tur det är och för phase 0 vilken ruta man är på
        int count = 1;

        bool was_blue = true;
        bool initilazation = true;

        //phase 2
        int attacked_space = 0;

        //phase 3
        int attacked_low_strike = 0;

        bool blue_finished_shooting = false;
        bool red_finished_shooting = false;
        bool attacked = false;

        std::array<std::string, kNumCells> cell_names = {"Wave", "A Escort", "E Escort", "A High Strike", "E High Strike", "A Sead", "E sead", "A Low Strike", "E Low Strike", "Hits", "Maintenance", "Graveyard",
        "A AAA", "E AAA", "A Intercept", "E Intercept", "A Active Sam", "E Active Sam", "A Passive Sam", "E Passive Sam", "Airbase", "Hits", "Mainenance", "Graveyard"};

        //Board representation:
        //Row 1: wave, escort, high strike, sead, low strike, hits, maintenance, graveyard #ex: w2 {1, 0, 1, 1} {1,0} {1,0,1} 2 0 {"aaa", "bFighter", "rFighter"}
        //Row 2: aaa, intercept, active sam, passive sam, airbase, hits, maintenance, graveyard

        // https://math.stackexchange.com/questions/485752/tictactoe-state-space-choose-calculation/485852
        inline constexpr int kNumberStates = 5478;

        // State of an in-play game.
        class CounterAirState : public State {
        public:
            CounterAirState(std::shared_ptr<const Game> game);

            CounterAirState(const CounterAirState&) = default;
            CounterAirState& operator=(const CounterAirState&) = default;

            Player CurrentPlayer() const override;
            std::string ActionToString(Player player, Action action_id) const override;
            std::string ToString() const override;
            bool IsTerminal() const override;
            std::vector<double> Returns() const override;
            std::string InformationStateString(Player player) const override;
            std::string ObservationString(Player player) const override;
            void ObservationTensor(Player player,
                absl::Span<float> values) const override;
            std::unique_ptr<State> Clone() const override;
            /*void UndoAction(Player player, Action move) override;*/
            std::vector<Action> LegalActions() const override;
            /*CellState BoardAt(int cell) const { return board_[cell]; }
            CellState BoardAt(int row, int column) const {
                return board_[row * kCols + column];
            }*/
            Player outcome() const { return outcome_; }

            // Only used by Ultimate Tic-Tac-Toe.
            void SetCurrentPlayer(Player player){ current_player_ = player; }

        protected:
            std::array<int, kNumCells> board_;
            void DoApplyAction(Action move) override;
            void TakenHit(int player, int space, int evade);

        private:
            bool HasLine(Player player) const;  // Does this player have a line? 
            Player current_player_ = 0;         // Player zero goes first
            Player outcome_ = kInvalidPlayer;
        };

        // Game object.
        class CounterAirGame : public Game {
        public:
            explicit CounterAirGame(const GameParameters& params);
            int NumDistinctActions() const override { return kNumCells; }
            std::unique_ptr<State> NewInitialState() const override {
                return std::unique_ptr<State>(new CounterAirState(shared_from_this()));
            }
            int NumPlayers() const override { return kNumPlayers; }
            double MinUtility() const override { return -2; }
   	    absl::optional<double> UtilitySum() const override { return 0; }
            double MaxUtility() const override { return 2; }
            std::vector<int> ObservationTensorShape() const override {
                return { kCellStates, kRows, kCols };
            }
            int MaxGameLength() const override { return kNumCells; }
            std::string ActionToString(Player player, Action action_id) const override;
        };

        // Does this player have a line?
        bool BoardHasLine(const std::array<int, kNumCells>& board,
            const Player player);

        inline std::ostream& operator<<(std::ostream& stream, const int& state) {
            return stream << std::to_string(state);
        }

    }  // namespace counter_air
}  // namespace open_spiel

#endif  // OPEN_SPIEL_GAMES_COUNTER_AIR_H_
