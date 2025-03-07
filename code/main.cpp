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

static std::vector<std::string> GetWordList()
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

static std::vector<std::string> GetCurseWords()
{
    return
    {
        "shit",
        "fuck",
        "bitch",
    };
}

static void ThreadDoWork()
{
    std::println("Starting ThreadDoWork...");

    bool atLeastOneWordRemaining = false;
    int thisWordIndex;
    {
        std::scoped_lock lock(g_WordCountCalculatorMutex);
        thisWordIndex = g_CurrentWordIndex;
        g_CurrentWordIndex++;
    }

    if (thisWordIndex < g_WordList.size())
    {
        atLeastOneWordRemaining = true;
    }

    while (atLeastOneWordRemaining)
    {
        const std::string thisWord = g_WordList[thisWordIndex];

        if (std::find(g_CurseWords.begin(), g_CurseWords.end(), thisWord) != g_CurseWords.end())
        {
            std::println("Curse word detected!");
        }

        {
            std::scoped_lock lock(g_WordCountCalculatorMutex);

            bool firstOccurrenceOfWord = !g_WordCountDict.contains(thisWord);
            if (firstOccurrenceOfWord)
            {
                g_WordCountDict[thisWord] = 1;
            }
            else
            {
                g_WordCountDict[thisWord]++;
            }

            thisWordIndex = g_CurrentWordIndex;
            g_CurrentWordIndex++;

            if (thisWordIndex >= g_WordList.size())
            {
                atLeastOneWordRemaining = false;
            }
        }
    }
}

static void CalculateWordCounts()
{
    g_WordList = GetWordList();
    g_CurseWords = GetCurseWords();

    std::thread threadA(ThreadDoWork);
    std::thread threadB(ThreadDoWork);
    std::thread threadC(ThreadDoWork);
    std::thread threadD(ThreadDoWork);

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
