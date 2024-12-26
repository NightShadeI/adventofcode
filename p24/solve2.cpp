#include "support/filereader.hpp"
#include "support/strprocessing.hpp"

#include <algorithm>
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

[[nodiscard]] constexpr std::int64_t generateCode(std::string_view aNodeValue)
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

[[nodiscard]] bool isSequence(std::int64_t aCode, char aPrefix)
{
    const std::int64_t myCodePrefix{aCode / (128 * 128)};
    const char myCharPrefix{static_cast<char>(myCodePrefix)};
    return myCharPrefix == aPrefix;
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

    static constexpr std::array SWAPS{
        std::make_pair(generateCode(std::string_view{"z06"}), generateCode(std::string_view{"fkp"})),
        std::make_pair(generateCode(std::string_view{"fkp"}), generateCode(std::string_view{"z06"})),
        std::make_pair(generateCode(std::string_view{"z11"}), generateCode(std::string_view{"ngr"})),
        std::make_pair(generateCode(std::string_view{"ngr"}), generateCode(std::string_view{"z11"})),
        std::make_pair(generateCode(std::string_view{"z31"}), generateCode(std::string_view{"mfm"})),
        std::make_pair(generateCode(std::string_view{"mfm"}), generateCode(std::string_view{"z31"})),
        std::make_pair(generateCode(std::string_view{"krj"}), generateCode(std::string_view{"bpt"})),
        std::make_pair(generateCode(std::string_view{"bpt"}), generateCode(std::string_view{"krj"}))
    };

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
            const auto myIt{std::ranges::find_if(SWAPS, [&](const auto& aVal){
                const auto& [mySeed, myReplace] = aVal;
                return mySeed == myOutputCode;
            })};

            const auto myRealOutputCode{myIt == SWAPS.cend() ? myOutputCode : myIt->second};

            const std::int64_t myInputLeftCode{generateCode(myParts[0])};
            const std::int64_t myInputRightCode{generateCode(myParts[2])};
            const Operation myOperation{generateOperator(myParts[1])};

            Node& myNode{myGraph[myRealOutputCode]};
            myNode.theOutput.theCode = myRealOutputCode;
            myNode.theLeftInput = myInputLeftCode;
            myNode.theRightInput = myInputRightCode;
            myNode.theOperation = myOperation;
        }
    });

    std::int64_t myLhs{0};
    std::int64_t myRhs{0};
    std::int64_t myResult{0};

    for (const auto& [myNodeKey, myNode] : myGraph)
    {
        const std::int64_t myNodeSuffix{myNodeKey % (128 * 128)};
        const std::int64_t myFirstChar{myNodeSuffix/128};
        const std::int64_t mySecondChar{myNodeSuffix%128};
        const std::int64_t mySequenceId{10 * (myFirstChar - '0') + (mySecondChar - '0')};

        if (isSequence(myNodeKey, 'x') && myNode.theOutput.theValue)
        {
            myLhs |= (1ull << mySequenceId);
        }
        else if (isSequence(myNodeKey, 'y') && myNode.theOutput.theValue)
        {
            myRhs |= (1ull << mySequenceId);
        }
        else if (isSequence(myNodeKey, 'z'))
        {
            if (const bool myComputedResult{computeResult(myGraph, myNodeKey)}; myComputedResult)
            {
                myResult |= (1ull << mySequenceId);
            }
        
            const auto& myLeftNode{myGraph[myNode.theLeftInput]};
            const auto& myRightNode{myGraph[myNode.theRightInput]};
            std::cout << serialize(myNodeKey) << " " << serialize(myNode.theOperation) << '\n';
            std::cout << serialize(myLeftNode.theOutput.theCode) << " " << serialize(myLeftNode.theOperation) << '\n';
            std::cout << serialize(myRightNode.theOutput.theCode) << " " << serialize(myRightNode.theOperation) << '\n';

            std::cout << "OR:\n";
            if (myLeftNode.theOperation == Operation::OR)
            {
                std::cout << serialize(myGraph[myLeftNode.theLeftInput].theOutput.theCode) << " " << serialize(myGraph[myLeftNode.theLeftInput].theOperation) << '\n';
                std::cout << serialize(myGraph[myLeftNode.theRightInput].theOutput.theCode) << " " << serialize(myGraph[myLeftNode.theRightInput].theOperation) << '\n';
            }
            else if (myRightNode.theOperation == Operation::OR)
            {
                std::cout << serialize(myGraph[myRightNode.theLeftInput].theOutput.theCode) << " " << serialize(myGraph[myRightNode.theLeftInput].theOperation) << '\n';
                std::cout << serialize(myGraph[myRightNode.theRightInput].theOutput.theCode) << " " << serialize(myGraph[myRightNode.theRightInput].theOperation) << '\n';
            }
            else
            {
                std::cout << "No OR gate's were found!\n";
            }

            std::cout << "XOR:\n";
            if (myLeftNode.theOperation == Operation::XOR)
            {
                std::cout << serialize(myGraph[myLeftNode.theLeftInput].theOutput.theCode) << " " << serialize(myGraph[myLeftNode.theLeftInput].theOperation) << '\n';
                std::cout << serialize(myGraph[myLeftNode.theRightInput].theOutput.theCode) << " " << serialize(myGraph[myLeftNode.theRightInput].theOperation) << '\n';
            }
            else if (myRightNode.theOperation == Operation::XOR)
            {
                std::cout << serialize(myGraph[myRightNode.theLeftInput].theOutput.theCode) << " " << serialize(myGraph[myRightNode.theLeftInput].theOperation) << '\n';
                std::cout << serialize(myGraph[myRightNode.theRightInput].theOutput.theCode) << " " << serialize(myGraph[myRightNode.theRightInput].theOperation) << '\n';
            }
            else
            {
                std::cout << "No OR gate's were found!\n";
            }

            std::cout << '\n';
        }
    }

    std::cout << myLhs << " + " << myRhs << " = " << myLhs + myRhs << " got: " << myResult << '\n';

    std::vector<std::int64_t> mySolution;
    for (const auto& [mySeed, myReplace] : SWAPS)
    {
        mySolution.emplace_back(mySeed);
    }
    std::ranges::sort(mySolution);

    std::cout << "Solution: ";
    for (std::int64_t myIdx{0}; myIdx < std::ssize(mySolution)-1; ++myIdx)
    {
        std::cout << serialize(mySolution[myIdx]) << ',';
    }
    std::cout << serialize(mySolution.back()) << '\n';
}

// What seems to be wrong:

// Fine:
// z00 is OKAY -> its a simple XOR only
// z01 is OKAY -> it should lead to an AND
// z45 is OKAY -> it is only the carry out since there is no x45 or y45 
// 
// Not Fine:
// z06 should be XOR (was or) <-- TODO: maybe something else going on here?, was: bpp(AND), ghf(AND)
// z07 should be OR, XOR, was: fkp(XOR)
//
// so z06 should be fkp
// fkp should be z06 
// DONE
//
// z11 should be XOR, was: AND
// z12 carry IN should be AND, AND, was: ngr(XOR)
//
// ngr should be z11
// z11 should be ngr
// DONE
//
// z31 should be XOR (was and), XOR (x2!), but instead it links DIRECTLY to x31 and y31
// z32 carry IN should be AND, AND, was: mfm(XOR)
//
// mfm should be z31
// z31 should be mfm
//
// z38 should be XOR, OR but was AND(bpt)
// z39 carry IN should be AND, AND, was: krj(XOR)
//
// krj should be bpt
// bpt should be krj

// As a result, I want the inputs of:
// static constexpr std::array DESIRED_SEQS{"mgq", "jpp", "jgw"}

// Results:

// For z39:
// krj connects to x38 and y38 but they should connect to AND
//
// For z32:
// mfm contains someone elses XOR(mgq)
// mgq is x31 and y31
//
// For z31:
// x31 and y31 should connect to an XOR
//
// For z12:
// ngr contains someone elses XOR(jpp)
// jpp is x11 and y11
//
// For z07:
// fkp connects to someone elses XOR(jgw)
// jgw is x06 and y06
//
// For z06:
// OR must go into an XOR or OR
// some XOR must come into ME