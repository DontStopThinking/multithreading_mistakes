#include <print>
#include <thread>
#include <vector>
#include <unordered_map>
#include <string>
#include <mutex>
#include <algorithm>

// NOTE: Following this article: https://benbowen.blog/post/cmmics_ii/

static std::vector<std::string> g_WordList = {};
static std::vector<std::string> g_CurseWords = {};
static int g_CurrentWordIndex = 0;
std::unordered_map<std::string, int> g_WordCountDict = {};
static std::mutex g_WordCountCalculatorMutex = {};

// trim from both ends (in place)
static inline void Trim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// Trim s, make it uppercase, remove hyphens and single-quotes.
static void RemoveSpecialSymbols(std::string& s)
{
    Trim(s);
    // Make string uppercase
    for (char& c : s)
    {
        c = std::toupper(c);
    }
    std::erase(s, '-');
    std::erase(s, '\'');
}

static inline std::vector<std::string> GetWordList()
{
    return
    {
        "word1",
        "fuck",
        "word2",
        "word3",
        "shit",
        "word4",
        "word5",
        "word6",
        "word7",
        "word8",
        "word9",
        "word10",
        "word11",
        "word12",
    };
}

static inline std::vector<std::string> GetCurseWords()
{
    return
    {
        "shit",
        "fuck",
        "bitch",
    };
}

struct ThreadWorkBlock
{
public:
    size_t m_StartingIndex;
    size_t m_Count;

    ThreadWorkBlock(const size_t startingIndex, const size_t count)
        : m_StartingIndex(startingIndex)
        , m_Count(count)
    {
    }
};

static void ThreadDoWork(const ThreadWorkBlock localWorkBlock)
{
    std::println("Starting ThreadDoWork...");

    std::unordered_map<std::string, int> localWordCountDict;
    localWordCountDict.reserve(localWorkBlock.m_Count);

    for (size_t i = localWorkBlock.m_StartingIndex;
         i < localWorkBlock.m_StartingIndex + localWorkBlock.m_Count;
         ++i)
    {
        std::string thisWord = g_WordList[i];

        if (std::find(g_CurseWords.begin(), g_CurseWords.end(), thisWord) != g_CurseWords.end())
        {
            std::println("Curse word detected!");
        }

        RemoveSpecialSymbols(thisWord);
        localWordCountDict[thisWord]++;
    }

    {
        std::scoped_lock lock(g_WordCountCalculatorMutex);
        for (const auto& [key, value] : localWordCountDict)
        {
            g_WordCountDict[key] = value;
        }
    }
}

static void CalculateWordCounts()
{
    g_WordList = GetWordList();
    g_CurseWords = GetCurseWords();

    const size_t numWords = g_WordList.size();
    const size_t quarterOfWords = numWords / 4;

    std::thread threadA(ThreadDoWork, ThreadWorkBlock(quarterOfWords * 0, quarterOfWords));
    std::thread threadB(ThreadDoWork, ThreadWorkBlock(quarterOfWords * 1, quarterOfWords));
    std::thread threadC(ThreadDoWork, ThreadWorkBlock(quarterOfWords * 2, quarterOfWords));
    std::thread threadD(ThreadDoWork, ThreadWorkBlock(quarterOfWords * 3, numWords - quarterOfWords * 3));

    threadA.join();
    std::println("g_WordCountDict: {}", g_WordCountDict);
    threadB.join();
    std::println("g_WordCountDict: {}", g_WordCountDict);
    threadC.join();
    std::println("g_WordCountDict: {}", g_WordCountDict);
    threadD.join();
    std::println("g_WordCountDict: {}", g_WordCountDict);
}

int main()
{
    CalculateWordCounts();

    return 0;
}
