/*
 * N-Queens + Knight's Tour Solver — C++ Backend
 * MIT-WPU DSA Unit 4: Backtracking
 *
 * Compiles with:
 *   g++ -O2 -std=c++17 -o nqueens nqueens.cpp
 *
 * ── N-Queens Usage ───────────────────────────────────────
 *   ./nqueens [rows] [cols] [maxSolutions]
 *   ./nqueens              -> 8x8, up to 200 solutions
 *   ./nqueens 6 6          -> 6x6 board
 *   ./nqueens 10 10 50     -> 10x10, stop at 50
 *
 * ── Knight's Tour Usage ──────────────────────────────────
 *   ./nqueens knight [rows] [cols]
 *   ./nqueens knight       -> 8x8
 *   ./nqueens knight 5 5   -> 5x5 board
 *   ./nqueens knight 6 8   -> 6x8 rectangular board
 *
 *   Strategy : Pure backtracking (educational, no heuristics)
 *   Tour type : Closed-first — for each starting square the solver
 *               first tries to find a closed tour (knight returns
 *               to its starting square on the final move).
 *               If no closed tour exists from that square it falls
 *               back and searches for any open tour instead.
 *
 * ── Output files ─────────────────────────────────────────
 *   solutions.json        <- N-Queens results  (read by index.html)
 *   knight_tour.json      <- Knight's Tour results (read by index.html)
 *   nqueens_NxM_log.txt   <- human-readable N-Queens ASCII log
 *   knight_NxM_log.txt    <- human-readable Knight's Tour ASCII log
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <sstream>
#include <algorithm>
#include <cmath>

// ═══════════════════════════════════════════════════════════════════
//  SHARED — knight move offsets
// ═══════════════════════════════════════════════════════════════════

static const int KDR[8] = {-2,-2,-1,-1, 1, 1, 2, 2};
static const int KDC[8] = {-1, 1,-2, 2,-2, 2,-1, 1};

inline bool inBounds(int r, int c, int rows, int cols) {
    return r >= 0 && r < rows && c >= 0 && c < cols;
}

// ═══════════════════════════════════════════════════════════════════
//  N-QUEENS
// ═══════════════════════════════════════════════════════════════════

struct QueenSolution {
    std::vector<int> queens; // queens[col] = row  (0-indexed)
};

bool queenIsSafe(const std::vector<int>& queens, int col, int row) {
    for (int c = 0; c < col; ++c) {
        if (queens[c] == row)                     return false;
        if (std::abs(queens[c] - row) == col - c) return false;
    }
    return true;
}

long long gQueenCalls = 0;

void queenSolve(std::vector<int>& queens, int col,
                int rows, int cols,
                std::vector<QueenSolution>& out, int maxSols)
{
    ++gQueenCalls;
    if ((int)out.size() >= maxSols) return;
    if (col == cols) { out.push_back({queens}); return; }
    for (int row = 0; row < rows; ++row) {
        if (queenIsSafe(queens, col, row)) {
            queens[col] = row;
            queenSolve(queens, col + 1, rows, cols, out, maxSols);
            queens[col] = -1;
        }
    }
}

std::string renderQueenBoard(const QueenSolution& s, int rows, int cols) {
    std::ostringstream o;
    o << "  ";
    for (int c = 0; c < cols; ++c) o << (char)('a'+c) << " ";
    o << "\n  +";
    for (int c = 0; c < cols; ++c) o << "--";
    o << "+\n";
    for (int r = 0; r < rows; ++r) {
        o << (rows-r) << " |";
        for (int c = 0; c < cols; ++c)
            o << (s.queens[c] == r ? 'Q' : ((r+c)%2==0 ? '.' : ' ')) << "|";
        o << " " << (rows-r) << "\n  +";
        for (int c = 0; c < cols; ++c) o << "--";
        o << "+\n";
    }
    o << "  ";
    for (int c = 0; c < cols; ++c) o << (char)('a'+c) << " ";
    o << "\n";
    return o.str();
}

bool writeQueenJSON(const std::string& fname, int rows, int cols,
                    const std::vector<QueenSolution>& solutions, long long ms)
{
    std::ofstream f(fname);
    if (!f) { std::cerr << "ERROR: cannot write " << fname << "\n"; return false; }
    f << "{\n";
    f << "  \"type\": \"nqueens\",\n";
    f << "  \"rows\": " << rows << ",\n";
    f << "  \"cols\": " << cols << ",\n";
    f << "  \"totalSolutions\": " << solutions.size() << ",\n";
    f << "  \"timeMs\": " << ms << ",\n";
    f << "  \"recursiveCalls\": " << gQueenCalls << ",\n";
    f << "  \"solutions\": [\n";
    for (int i = 0; i < (int)solutions.size(); ++i) {
        const auto& q = solutions[i].queens;
        f << "    {\"id\": " << (i+1) << ", \"queens\": [";
        for (int c = 0; c < cols; ++c) { f << q[c]; if (c < cols-1) f << ", "; }
        f << "], \"squares\": [";
        for (int c = 0; c < cols; ++c) {
            f << "\"" << (char)('a'+c) << (q[c]+1) << "\"";
            if (c < cols-1) f << ", ";
        }
        f << "]}";
        if (i < (int)solutions.size()-1) f << ",";
        f << "\n";
    }
    f << "  ]\n}\n";
    return true;
}

void writeQueenLog(const std::string& fname, int rows, int cols,
                   const std::vector<QueenSolution>& solutions, long long ms)
{
    std::ofstream f(fname);
    if (!f) return;
    int total = (int)solutions.size();
    f << "========================================================\n";
    f << "  N-Queens | MIT-WPU DSA | Unit 4 Backtracking\n";
    f << "========================================================\n";
    f << "  Board: " << rows << "x" << cols
      << "  |  Solutions: " << total
      << "  |  Time: " << ms << " ms"
      << "  |  Calls: " << gQueenCalls << "\n";
    f << "========================================================\n\n";
    f << "--- QUEEN POSITIONS ---\n";
    for (int i = 0; i < total; ++i) {
        f << "Sol " << (i+1) << ": ";
        for (int c = 0; c < cols; ++c)
            f << (char)('a'+c) << (solutions[i].queens[c]+1)
              << (c < cols-1 ? "  " : "\n");
    }
    f << "\n--- ASCII BOARDS (first 10) ---\n";
    int show = std::min(total, 10);
    for (int i = 0; i < show; ++i)
        f << "\nSolution " << (i+1) << ":\n"
          << renderQueenBoard(solutions[i], rows, cols);
    if (total > 10) f << "\n... (" << total-10 << " more in solutions.json)\n";
    f << "\n========================================================\n";
    f << "  Open index.html in a browser to view all solutions\n";
    f << "========================================================\n";
}

// ═══════════════════════════════════════════════════════════════════
//  KNIGHT'S TOUR  — pure backtracking, closed-first strategy
// ═══════════════════════════════════════════════════════════════════

/*
 *  board[r][c] holds the move number (1-based) at which the knight
 *  visited that cell, or 0 if not yet visited.
 *
 *  Closed-first strategy (per starting square)
 *  ────────────────────────────────────────────
 *  Pass 1 — closed tour:
 *    When placing the very last move (move == total) we additionally
 *    require that the candidate cell is a knight-jump away from the
 *    starting square.  This ensures the tour is a closed (Hamiltonian)
 *    cycle without a separate verification step.
 *
 *  Pass 2 — open tour (only if pass 1 failed):
 *    No extra constraint; any cell may be the last landing.
 *
 *  Pure backtracking — moves are tried in the fixed KDR/KDC order.
 *  No Warnsdorff or other pruning heuristic is applied, keeping the
 *  code transparent for educational review.
 *
 *  Practical note: boards up to 6×6 finish in reasonable time.
 *  8×8 with pure backtracking can take minutes for some starting
 *  squares — use smaller boards for demos or compile with -O3.
 */

struct KnightTour {
    int startRow, startCol;              // 0-indexed start
    std::vector<std::vector<int>> board; // move number at each cell
    bool closed;                         // true if tour is a cycle
};

long long gKnightCalls = 0;

// Is (r,c) a knight-jump from (sr,sc)?
bool canReachStart(int r, int c, int sr, int sc) {
    for (int k = 0; k < 8; ++k)
        if (r + KDR[k] == sr && c + KDC[k] == sc) return true;
    return false;
}

// ── Pass 1: closed-tour backtracking ─────────────────────────────
bool knightSolveClosed(std::vector<std::vector<int>>& board,
                        int r, int c, int move, int total,
                        int rows, int cols, int sr, int sc)
{
    ++gKnightCalls;
    if (move > total)
        // All squares filled — is the final cell adjacent to start?
        return canReachStart(r, c, sr, sc);

    for (int k = 0; k < 8; ++k) {
        int nr = r + KDR[k], nc = c + KDC[k];
        if (!inBounds(nr, nc, rows, cols) || board[nr][nc] != 0) continue;
        // On the last move, require closability to start
        if (move == total && !canReachStart(nr, nc, sr, sc)) continue;
        board[nr][nc] = move;
        if (knightSolveClosed(board, nr, nc, move+1, total,
                               rows, cols, sr, sc))
            return true;
        board[nr][nc] = 0;
    }
    return false;
}

// ── Pass 2: open-tour backtracking ───────────────────────────────
bool knightSolveOpen(std::vector<std::vector<int>>& board,
                      int r, int c, int move, int total,
                      int rows, int cols)
{
    ++gKnightCalls;
    if (move > total) return true;

    for (int k = 0; k < 8; ++k) {
        int nr = r + KDR[k], nc = c + KDC[k];
        if (!inBounds(nr, nc, rows, cols) || board[nr][nc] != 0) continue;
        board[nr][nc] = move;
        if (knightSolveOpen(board, nr, nc, move+1, total, rows, cols))
            return true;
        board[nr][nc] = 0;
    }
    return false;
}

// ── Try one starting square — closed first, then open ────────────
bool trySingleStart(int sr, int sc, int rows, int cols, KnightTour& result)
{
    int total = rows * cols;
    std::vector<std::vector<int>> board(rows, std::vector<int>(cols, 0));
    board[sr][sc] = 1;

    // Pass 1: closed tour
    if (knightSolveClosed(board, sr, sc, 2, total, rows, cols, sr, sc)) {
        result = {sr, sc, board, true};
        return true;
    }

    // Reset board, try open tour
    for (auto& row : board) std::fill(row.begin(), row.end(), 0);
    board[sr][sc] = 1;

    if (knightSolveOpen(board, sr, sc, 2, total, rows, cols)) {
        result = {sr, sc, board, false};
        return true;
    }

    return false; // No tour from this square
}

// ── Solve every starting square ───────────────────────────────────
std::vector<KnightTour> solveAllStartingSquares(int rows, int cols)
{
    std::vector<KnightTour> results;
    int total = rows * cols;
    std::cout << " Progress: ";
    int done = 0;
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            KnightTour kt;
            if (trySingleStart(r, c, rows, cols, kt))
                results.push_back(kt);
            ++done;
            // Print a dot every square so the user sees progress
            std::cout << "." << std::flush;
        }
    }
    std::cout << "\n";
    return results;
}

// ── ASCII board for log ───────────────────────────────────────────
std::string renderKnightBoard(const KnightTour& kt, int rows, int cols)
{
    int w = (int)std::to_string(rows * cols).size();
    std::ostringstream o;
    // column header
    o << "    ";
    for (int c = 0; c < cols; ++c) {
        o << (char)('a'+c);
        for (int i = 0; i < w; ++i) o << ' ';
    }
    o << "\n    +";
    for (int c = 0; c < cols; ++c) {
        for (int i = 0; i <= w; ++i) o << '-';
        o << '+';
    }
    o << "\n";
    for (int r = 0; r < rows; ++r) {
        o << (rows-r) << "  |";
        for (int c = 0; c < cols; ++c) {
            std::string cell = std::to_string(kt.board[r][c]);
            while ((int)cell.size() < w) cell = " " + cell;
            o << cell << "|";
        }
        o << "  " << (rows-r) << "\n    +";
        for (int c = 0; c < cols; ++c) {
            for (int i = 0; i <= w; ++i) o << '-';
            o << '+';
        }
        o << "\n";
    }
    o << "    ";
    for (int c = 0; c < cols; ++c) {
        o << (char)('a'+c);
        for (int i = 0; i < w; ++i) o << ' ';
    }
    o << "\n";
    return o.str();
}

// ── Write knight_tour.json ────────────────────────────────────────
bool writeKnightJSON(const std::string& fname, int rows, int cols,
                     const std::vector<KnightTour>& tours,
                     long long ms, long long calls)
{
    std::ofstream f(fname);
    if (!f) { std::cerr << "ERROR: cannot write " << fname << "\n"; return false; }

    f << "{\n";
    f << "  \"type\": \"knight\",\n";
    f << "  \"rows\": " << rows << ",\n";
    f << "  \"cols\": " << cols << ",\n";
    f << "  \"totalTours\": " << tours.size() << ",\n";
    f << "  \"timeMs\": " << ms << ",\n";
    f << "  \"recursiveCalls\": " << calls << ",\n";
    f << "  \"tours\": [\n";

    for (int i = 0; i < (int)tours.size(); ++i) {
        const auto& kt = tours[i];

        // Build move-order sequence: sequence[move-1] = "e4"-style square
        std::vector<std::string> seq(rows * cols);
        for (int r = 0; r < rows; ++r)
            for (int c = 0; c < cols; ++c)
                seq[kt.board[r][c] - 1] =
                    std::string(1, (char)('a'+c)) + std::to_string(rows-r);

        f << "    {\n";
        f << "      \"id\": " << (i+1) << ",\n";
        f << "      \"startSquare\": \""
          << (char)('a'+kt.startCol) << (rows-kt.startRow) << "\",\n";
        f << "      \"closed\": " << (kt.closed ? "true" : "false") << ",\n";
        // Flat board array [row0col0, row0col1, ..., rowN-1colM-1]
        f << "      \"board\": [";
        for (int r = 0; r < rows; ++r)
            for (int c = 0; c < cols; ++c) {
                f << kt.board[r][c];
                if (!(r == rows-1 && c == cols-1)) f << ", ";
            }
        f << "],\n";
        // Ordered list of squares
        f << "      \"sequence\": [";
        for (int s = 0; s < (int)seq.size(); ++s) {
            f << "\"" << seq[s] << "\"";
            if (s < (int)seq.size()-1) f << ", ";
        }
        f << "]\n";
        f << "    }";
        if (i < (int)tours.size()-1) f << ",";
        f << "\n";
    }

    f << "  ]\n}\n";
    return true;
}

// ── Write knight_NxM_log.txt ──────────────────────────────────────
void writeKnightLog(const std::string& fname, int rows, int cols,
                    const std::vector<KnightTour>& tours,
                    long long ms, long long calls)
{
    std::ofstream f(fname);
    if (!f) return;
    int total = (int)tours.size();

    f << "========================================================\n";
    f << "  Knight's Tour | MIT-WPU DSA | Unit 4 Backtracking\n";
    f << "========================================================\n";
    f << "  Board: " << rows << "x" << cols
      << "  |  Tours found: " << total
      << "  |  Time: " << ms << " ms"
      << "  |  Calls: " << calls << "\n";
    f << "  Strategy : Pure backtracking (no heuristics)\n";
    f << "  Mode     : Closed-first, fallback to open\n";
    f << "========================================================\n\n";

    f << "--- TOUR SUMMARY ---\n";
    for (int i = 0; i < total; ++i) {
        const auto& kt = tours[i];
        f << "Tour " << (i+1) << ": start="
          << (char)('a'+kt.startCol) << (rows-kt.startRow)
          << "  [" << (kt.closed ? "CLOSED" : "open") << "]\n";
    }

    f << "\n--- ASCII BOARDS (first 5) ---\n";
    int show = std::min(total, 5);
    for (int i = 0; i < show; ++i) {
        const auto& kt = tours[i];
        f << "\nTour " << (i+1) << " — start: "
          << (char)('a'+kt.startCol) << (rows-kt.startRow)
          << "  [" << (kt.closed ? "CLOSED" : "open") << "]\n";
        f << renderKnightBoard(kt, rows, cols);
    }
    if (total > 5) f << "\n... (" << total-5 << " more in knight_tour.json)\n";

    f << "\n========================================================\n";
    f << "  Open index.html in a browser to visualise all tours\n";
    f << "========================================================\n";
}

// ═══════════════════════════════════════════════════════════════════
//  MAIN
// ═══════════════════════════════════════════════════════════════════

int main(int argc, char* argv[])
{
    // ── Knight's Tour mode ────────────────────────────────────────
    if (argc >= 2 && std::string(argv[1]) == "knight") {
        int rows = 8, cols = 8;
        if (argc >= 3) rows = std::stoi(argv[2]);
        if (argc >= 4) cols = std::stoi(argv[3]);

        std::cout << "======================================\n";
        std::cout << " Knight's Tour | MIT-WPU DSA Unit 4\n";
        std::cout << "======================================\n";
        std::cout << " Board    : " << rows << " x " << cols << "\n";
        std::cout << " Strategy : Pure backtracking\n";
        std::cout << " Mode     : Closed-first, fallback open\n";
        std::cout << " Probing all " << rows*cols << " starting squares...\n\n";

        gKnightCalls = 0;
        auto t0 = std::chrono::high_resolution_clock::now();
        auto tours = solveAllStartingSquares(rows, cols);
        auto t1 = std::chrono::high_resolution_clock::now();
        long long ms = std::chrono::duration_cast<
                           std::chrono::milliseconds>(t1-t0).count();

        int nClosed = 0, nOpen = 0;
        for (auto& t : tours) t.closed ? ++nClosed : ++nOpen;

        std::cout << "\n Found    : " << tours.size() << " tour(s)\n";
        std::cout << "            " << nClosed << " closed  |  "
                  << nOpen << " open\n";
        std::cout << " Time     : " << ms << " ms"
                  << "  |  Calls: " << gKnightCalls << "\n\n";

        if (!tours.empty()) {
            const auto& kt = tours[0];
            std::cout << "First tour — start "
                      << (char)('a'+kt.startCol) << (rows-kt.startRow)
                      << "  [" << (kt.closed ? "CLOSED" : "open") << "]\n";
            std::cout << renderKnightBoard(kt, rows, cols) << "\n";
        }

        std::string jf = "knight_tour.json";
        if (writeKnightJSON(jf, rows, cols, tours, ms, gKnightCalls))
            std::cout << "Written -> " << jf << "\n";

        std::string lf = "knight_"
            + std::to_string(rows) + "x" + std::to_string(cols) + "_log.txt";
        writeKnightLog(lf, rows, cols, tours, ms, gKnightCalls);
        std::cout << "Written -> " << lf << "\n";
        std::cout << "\nOpen index.html in your browser to visualise all tours.\n";
        return 0;
    }

    // ── N-Queens mode (default) ───────────────────────────────────
    int rows = 8, cols = 8, maxSols = 200;
    if (argc >= 2) rows    = std::stoi(argv[1]);
    if (argc >= 3) cols    = std::stoi(argv[2]);
    if (argc >= 4) maxSols = std::stoi(argv[3]);

    std::cout << "======================================\n";
    std::cout << " N-Queens | MIT-WPU DSA Unit 4\n";
    std::cout << "======================================\n";
    std::cout << " Board : " << rows << " x " << cols << "\n";
    std::cout << " Max   : " << maxSols << " solutions\n";
    std::cout << " Solving...\n\n";

    std::vector<int> queens(cols, -1);
    std::vector<QueenSolution> solutions;
    gQueenCalls = 0;

    auto t0 = std::chrono::high_resolution_clock::now();
    queenSolve(queens, 0, rows, cols, solutions, maxSols);
    auto t1 = std::chrono::high_resolution_clock::now();
    long long ms = std::chrono::duration_cast<
                       std::chrono::milliseconds>(t1-t0).count();

    int total = (int)solutions.size();
    std::cout << " Found  : " << total << " solution(s)\n";
    std::cout << " Time   : " << ms << " ms  |  Calls: " << gQueenCalls << "\n\n";

    int preview = std::min(total, 2);
    for (int i = 0; i < preview; ++i)
        std::cout << "Solution " << (i+1) << ":\n"
                  << renderQueenBoard(solutions[i], rows, cols) << "\n";
    if (total > preview)
        std::cout << "... (" << total-preview << " more in output files)\n\n";

    if (writeQueenJSON("solutions.json", rows, cols, solutions, ms))
        std::cout << "Written -> solutions.json\n";

    std::string logFile = "nqueens_"
        + std::to_string(rows) + "x" + std::to_string(cols) + "_log.txt";
    writeQueenLog(logFile, rows, cols, solutions, ms);
    std::cout << "Written -> " << logFile << "\n";
    std::cout << "\nOpen index.html in your browser to view all solutions.\n";
    return 0;
}
