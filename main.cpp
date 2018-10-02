#include <iostream>
#include <cstddef> // std::size_t

#include <thread>
#include <chrono>

// Linux specific
#include <sys/ioctl.h> // input/output control
#include <unistd.h> //

#include "streamstyle.hpp"


class ProgressBar
{
public:
    explicit ProgressBar(char elapsedProgressSymbol = '|',
                        char remainingProgressSymbol = ' ',
                        double progress = 0)
                        : progress_{progress}
                        , consoleWidth_{getConsoleWidth()}
                        , oldConsoleWidth_{consoleWidth_}
                        , barWidth_{consoleWidth_ - legendWidth_}
                        , elapsedProgressSymbol_{elapsedProgressSymbol}
                        , remainingProgressSymbol_{remainingProgressSymbol}
    { }

    void update(double progress)
    {
        // Update progress
        progress_ = progress;
        int percentage = static_cast<int>(progress_ * 100);

        clear();
        if (percentage < 0 || percentage > 100) {
            std::cout << "[--- error ---] progress " << percentage << "%\r";
            std::cout.flush();
            return;
        }

        // Update console and bar dimensions (detect possible resizing)
        oldConsoleWidth_ = consoleWidth_;
        consoleWidth_ = getConsoleWidth();
        barWidth_ = consoleWidth_ - legendWidth_;

        std::string barContent(barWidth_, remainingProgressSymbol_);

        auto nElapsedSymbols = static_cast<std::size_t>(barWidth_ * progress);
        barContent.replace(barContent.begin(),
                           barContent.begin() + nElapsedSymbols,
                           nElapsedSymbols, elapsedProgressSymbol_);

        // Display bar
        std::cout << '[' << barContent << "] " << percentage << "%";
        std::cout << "\033[u"; // Restore cursor position
        std::cout.flush();
    }

    void clear()
    {
        std::cout << "\033[s"; // Save current cursor position
        std::cout << std::string(oldConsoleWidth_, ' ');
        std::cout << "\033[u"; // Restore cursor position
        std::cout.flush();
    }

private:
    // Appearance
    const char elapsedProgressSymbol_;
    const char remainingProgressSymbol_;

    // Dimensions (in characters)
    std::size_t consoleWidth_;
    std::size_t oldConsoleWidth_;
    std::size_t legendWidth_ = sizeof("[] XXX%") - 1; // Remove terminator char
    std::size_t barWidth_;

    double progress_;

    std::size_t getConsoleWidth()
    {
        static constexpr int minTerminalWidth = 50;

        winsize terminalWinSize{};

        // TIOCGWINSZ tells to fill the winsize struct 'winsize' corresponding
        // to the standard output file descriptor STDOUT_FILENO
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminalWinSize);

        if(terminalWinSize.ws_col <= minTerminalWidth) {
            // If ioctl fails or, the terminal with is too
            return minTerminalWidth;
        } else {
            return terminalWinSize.ws_col;
        }
        // TODO: Windows implementation
    }
};

int main()
{
    ProgressBar bar;

    int max = 200;
    for (int i = 0; i <= max; ++i) {
        bar.update(static_cast<double>(i)/max);
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
    bar.clear();

    std::cout << "Hello, World!\n";

    return 0;
}