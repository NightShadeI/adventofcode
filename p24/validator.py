mySeen = {}
myNum = ["0" for i in range(64)] 

# y07(0) XOR x07(0) = pkh(0)

def getPartValue(aInput): return aInput[:3], int(aInput[4])

for line in open("output.txt"):
    myParts = line.split()
    myLhs, myOp, myRhs, _, myRes = myParts
    myLhsName, myLhsBit = getPartValue(myLhs)
    myRhsName, myRhsBit = getPartValue(myRhs)
    myResName, myResBit = getPartValue(myRes)
    print(myLhsName, myLhsBit, myOp, myRhsName, myRhsBit, myResName, myResBit)
    if (myLhsName[0] not in ('x', 'y') and mySeen[myLhsName] != myLhsBit):
        print("UNSET VALUE SEEN")
    if (myRhsName[0] not in ('x', 'y') and mySeen[myRhsName] != myRhsBit):
        print("UNSET VALUE SEEN")
    mySeen[myResName] = myResBit
    if (myOp == "AND"):
        if (myLhsBit & myRhsBit != myResBit):
            print("RESULT INVALID", line)
    elif (myOp == "OR"):
        if (myLhsBit | myRhsBit != myResBit):
            print("RESULT INVALID", line)
    elif (myOp == "XOR"):
        if (myLhsBit ^ myRhsBit != myResBit):
            print("RESULT INVALID", line)
    else:
        print("!!!!!!!!!!!!!!!!!")

    if (myResName.startswith('z')):
        myNum[int(myResName[1:])] = str(myResBit)

print(int(''.join(myNum), base=2))
    

