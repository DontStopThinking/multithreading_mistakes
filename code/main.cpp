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
        "word13",
        "word14",
        "word15",
        "word16",
        "word17",
        "word18",
        "word19",
        "word20",
        "word21",
        "word22",
        "word23",
        "word24",
        "word25",
        "word26",
        "word27",
        "word28",
        "word29",
        "bitch",
        "fuck",
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

// A "section" here is a smaller chunk of work within a larger work block that is assigned to a
// thread.
constexpr inline size_t NUM_SECTIONS_PER_THREAD = 3;

static void ThreadDoWork(const ThreadWorkBlock localWorkBlock)
{
    std::println("Starting ThreadDoWork...");

    std::unordered_map<std::string, int> localWordCountDict;
    localWordCountDict.reserve(localWorkBlock.m_Count);

    const size_t countPerSection = localWorkBlock.m_Count / NUM_SECTIONS_PER_THREAD;
    for (size_t section = 0;
        section < NUM_SECTIONS_PER_THREAD;
        section++)
    {
        const size_t startingIndex = localWorkBlock.m_StartingIndex + countPerSection * section;
        const size_t count = section < NUM_SECTIONS_PER_THREAD - 1
            ? countPerSection
            : localWorkBlock.m_Count - countPerSection * (NUM_SECTIONS_PER_THREAD - 1);
        const size_t endingIndex = startingIndex + count;

        localWordCountDict.clear();

        for (size_t i = startingIndex;
            i < endingIndex;
            i++)
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
