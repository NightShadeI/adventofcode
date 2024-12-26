#include "support/filereader.hpp"
#include "support/strprocessing.hpp"

#include <iostream>
#include <vector>
#include <unordered_map>
#include <utility>

enum class State
{
    SIGNALS,
    OPERATIONS
};

enum class Operation
{
    AND,
    OR,
    XOR
};

struct NodeDetail
{
    std::int64_t theCode{0};
    bool theValue;
};

struct Node
{
    NodeDetail theOutput;
    std::int64_t theLeftInput{0};
    std::int64_t theRightInput{0};
    Operation theOperation{Operation::AND};
    bool theComputed{false};
};

using GraphT = std::unordered_map<std::int64_t, Node>;

[[nodiscard]] Operation generateOperator(std::string_view anOperand)
{
    if (anOperand == "AND")
    {
        return Operation::AND;
    }
    else if (anOperand == "OR")
    {
        return Operation::OR;
    }
    else if (anOperand == "XOR")
    {
        return Operation::XOR;
    }
    std::unreachable();
}

[[nodiscard]] std::int64_t generateCode(std::string_view aNodeValue)
{
    static constexpr std::int64_t MAX_VAL{128};
    std::int64_t myResult{0};
    for (const char myChar : aNodeValue)
    {
        myResult *= MAX_VAL;
        myResult += myChar;
    }
    return myResult;
}

[[nodiscard]] bool isOutputSequence(std::int64_t aCode)
{
    const std::int64_t myCodePrefix{aCode / (128 * 128)};
    const char myCharPrefix{static_cast<char>(myCodePrefix)};
    return myCharPrefix == 'z';
}

[[nodiscard]] bool operateOn(Operation aOperation, bool aLhs, bool aRhs)
{
    switch(aOperation)
    {
        case Operation::AND:
            return aLhs & aRhs;
        case Operation::OR:
            return aLhs | aRhs;
        case Operation::XOR:
            return aLhs ^ aRhs;
    }

    std::unreachable();    
}

[[nodiscard]] std::string serialize(std::int64_t aCode)
{
    std::string myRes;
    myRes += static_cast<char>(aCode / (128 * 128));
    myRes += static_cast<char>((aCode % (128 * 128)) / 128);
    myRes += static_cast<char>(aCode % 128);
    return myRes;
}

[[nodiscard]] std::string serialize(Operation aOperation)
{
    switch(aOperation)
    {
        case Operation::AND:
            return "AND";
        case Operation::OR:
            return "OR";
        case Operation::XOR:
            return "XOR";
    }

    std::unreachable();   
}

[[nodiscard]] bool computeResult(GraphT& aGraph, std::int64_t aCode)
{
    Node& myNode{aGraph[aCode]};
    if (myNode.theComputed || myNode.theLeftInput == 0 || myNode.theRightInput == 0)
    {
        return myNode.theOutput.theValue;
    }

    const bool myLeftInput{computeResult(aGraph, myNode.theLeftInput)};
    const bool myRightInput{computeResult(aGraph, myNode.theRightInput)};

    const bool myResult{operateOn(myNode.theOperation, myLeftInput, myRightInput)};

    // std::cout << serialize(myNode.theLeftInput) << '(' << (int)myLeftInput << ") " << serialize(myNode.theOperation) << " " << serialize(myNode.theRightInput) << '(' << (int)myRightInput << 
    // ')' << " = "  << serialize(aCode) << '(' << (int)myResult << ")\n";

    myNode.theComputed = true;
    myNode.theOutput.theValue = myResult;

    return myResult;
}

int main()
{
    GraphT myGraph;
    FileReader::processLines(NonCopyablePath{"p24/input.txt"}, [&, myState = State::SIGNALS](const std::string& aLine) mutable {
        if (aLine.empty())
        {
            myState = State::OPERATIONS;
            return;
        }

        if (myState == State::SIGNALS)
        {
            static constexpr std::string_view DELIM{": "};
            const auto myParts{splitBy(aLine, DELIM)};
            const std::int64_t myOutputCode{generateCode(myParts[0])};
            myGraph.emplace(myOutputCode, Node{
                .theOutput = NodeDetail{myOutputCode, myParts[1] == "1"}
            });
        }
        else
        {
            static constexpr std::string_view DELIM{" "};
            const auto myParts{splitBy(aLine, DELIM)};
            
            const std::int64_t myOutputCode{generateCode(myParts[4])};
            const std::int64_t myInputLeftCode{generateCode(myParts[0])};
            const std::int64_t myInputRightCode{generateCode(myParts[2])};
            const Operation myOperation{generateOperator(myParts[1])};

            Node& myNode{myGraph[myOutputCode]};
            myNode.theOutput.theCode = myOutputCode;
            myNode.theLeftInput = myInputLeftCode;
            myNode.theRightInput = myInputRightCode;
            myNode.theOperation = myOperation;
        }
    });

    std::int64_t myResult{0};

    for (const auto& [myNodeKey, myNode] : myGraph)
    {
        if (isOutputSequence(myNodeKey))
        {
            const std::int64_t myNodeSuffix{myNodeKey % (128 * 128)};
            const std::int64_t myFirstChar{myNodeSuffix/128};
            const std::int64_t mySecondChar{myNodeSuffix%128};
            const std::int64_t mySequenceId{10 * (myFirstChar - '0') + (mySecondChar - '0')};
            
            if (const bool myComputedResult{computeResult(myGraph, myNodeKey)}; myComputedResult)
            {
                myResult |= (1ull << mySequenceId);
            }
        }
    }

    std::cout << "Solution: " << myResult << '\n';
}